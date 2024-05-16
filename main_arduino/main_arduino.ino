#include "time_base.h"
#define SENSOR 4
#define ACTION 5 
#define TRIGGER_PIN 2 // pin para el pulso de disparo del sensor
#define ECHO_PIN 3 // pin para la medición del eco del sensor
#define RELAY_PIN 6 // pin para controlar el relé

int contadorPlastico = 0;
int contadorMetal = 0;

unsigned long int lastTimeL= 0;
unsigned long int lastTimeP= 0;
bool boolL = false;
bool boolP = false;

//Encoder
const int encoderAPin = 7; // Pin para la lectura del encoder
const int encoderBPin = 8;

time_base_t t_low_trigger;
time_base_t t_high_trigger;
time_base_t t_send;

//Para el encoder
int Encoder_C1Last;
int paso = 0;
bool direccion = true;
float rpm = 0;
int pulsesperturn = 11;
unsigned long timeold = 0;

void setup() {

  Serial.begin(9600); 
  pinMode(SENSOR, INPUT_PULLUP); 
  pinMode(ACTION, OUTPUT); 
  pinMode(TRIGGER_PIN, OUTPUT); // Configura el pin de disparo como salida
  pinMode(ECHO_PIN, INPUT); // Configura el pin de eco como entrada
  pinMode(RELAY_PIN, OUTPUT); // Configura el pin del relé como salida
  tb_init(&t_low_trigger, 2, true);
  tb_init(&t_high_trigger, 10, true);
  tb_init(&t_send, 200000, true);


  //Encoder
  attachInterrupt(digitalPinToInterrupt(encoderAPin), calculapulso, CHANGE);
}


void loop() {

  sendDataToSerial();


  long duration, distance;
  // Genera un pulso corto en el pin de disparo
  generateTrigger(TRIGGER_PIN);
  int L = digitalRead(SENSOR); 
  delayMicroseconds(5);
  
  if (L == 1) {
    //Serial.println("All Clear");
    digitalWrite(ACTION, LOW);
  } else {
    //Serial.println("=== Obstacle detected");
    digitalWrite(ACTION, HIGH);
    // contadorMetal = contadorMetal+1;
    if (boolL == false && (micros() - lastTimeL >= 500000)) {
      boolL = true;
    }
    lastTimeL = micros();
  }

  if (boolL == true) {
    contadorMetal = contadorMetal + 1;
    boolL = false;
    // Muestra el número de botellas de metal en el monitor serial
    Serial.print("Numero de botellas de metal: ");
    Serial.println(contadorMetal);
  }


  
  // Mide el tiempo que tarda en llegar el eco
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calcula la distancia en centímetros
  distance = duration * 0.034 / 2;
  
  // Imprime la distancia en el monitor serial
  //Serial.print("Distancia: ");
  //Serial.print(distance);
  //Serial.println(" cm");
 // Si la distancia es menor que 20 cm, activa el relé
  if (distance < 8) {
    //Serial.println("¡Objeto detectado! Activando relé.");
    digitalWrite(RELAY_PIN, HIGH); // Activa el relé
    if (boolP == false && (micros() - lastTimeP >= 500000)) {
      boolP = true;
    }
    contadorPlastico = contadorPlastico+1;
    // Muestra el número de botellas de plastico en el monitor serial
    Serial.print("Numero de botellas de plastico: ");
    Serial.println(contadorPlastico);
  } else {
    digitalWrite(RELAY_PIN, LOW); // Desactiva el relé
  }

  //Encoder
  if (millis() - timeold >= 200){

  rpm = -(60 * 1000 / pulsesperturn ) / (millis() - timeold) * paso/4;
  //Serial.print("RPM: ");
  // Serial.println(rpm);
  timeold = millis();
  paso = 0;

  }

  
delay(100);
}


void generateTrigger(int trigPin){
  digitalWrite(trigPin, LOW);  
	if(tb_check(&t_low_trigger))
	  digitalWrite(trigPin, HIGH);  
    tb_next(&t_low_trigger);
	if(tb_check(&t_high_trigger))  
	  digitalWrite(trigPin, LOW);
    tb_next(&t_high_trigger);
}


//Funcion para enviar los datos en el formato que recibe LabVIEW
void sendDataToSerial(){
  if(tb_check(&t_send)){
    Serial.print("L");
    Serial.print(contadorMetal);
    Serial.print("P");
    Serial.print(contadorPlastico);
    Serial.print("R");
    Serial.print(rpm);
    Serial.println("F");
    tb_next(&t_send);
  }
}

//Encoder
void calculapulso()
{

  if ((Encoder_C1Last == LOW) && digitalRead(encoderAPin) == HIGH){

    if (digitalRead(encoderBPin) == LOW && direccion){
      direccion = false; //Reverse
    }
    else if (digitalRead(encoderBPin) == HIGH && !direccion){
      direccion = true;  //Forward
    }
  }

  Encoder_C1Last = digitalRead(encoderAPin);

  if (!direccion)  paso--;
  else  paso++;

}