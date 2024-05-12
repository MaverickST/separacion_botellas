#include "time_base.h"

// DEFINICIONES DE PINES

//Sensor inductivo
const int sensorLataPin = 4; // Pin para el sensor inductivo

//ultrasonidos
const int ultrasonidoObjetoEchoPin = 3;
const int ultrasonidoObjetoTrigPin = 2;
//const int ultrasonidoLataEchoPin = 32;
//const int ultrasonidoLataTrigPin = 36;
//const int ultrasonidoBotEchoPin = 33;
//const int ultrasonidoBotTrigPin = 39;

//Motor
const int motorPWM = A0; //Pin PWM para controlar el motor

//Pistones
const int pistonLataPin = 5;
const int pistonBotPin = 6;

//Encoder
const int encoderAPin = 7; // Pin para la lectura del encoder
const int encoderBPin = 8;

//Constantes
bool hayLata = false;
bool hayBotella = false;
int conteoLatas = 0;
int conteoBotellas = 0;

time_base_t t_latas;
time_base_t t_botellas;
time_base_t t_reset;
time_base_t t_send;

time_base_t t_low_trigger;
time_base_t t_high_trigger;

int tInicioEcho;
bool inicioEcho;
int distancia;

//Para el encoder
int Encoder_C1Last;
int paso = 0;
bool direccion = true;
float rpm = 0;
int pulsesperturn = 11;
unsigned long timeold = 0;

void setup() {
  // Configuración de los pines de entrada
  pinMode(sensorLataPin, INPUT);
  pinMode(ultrasonidoObjetoEchoPin, INPUT);
  pinMode(encoderAPin, INPUT);
  pinMode(encoderBPin, INPUT);
  // Configuración de los pines de salida
  pinMode(ultrasonidoObjetoTrigPin, OUTPUT);
  pinMode(motorPWM, OUTPUT);
  
  // Configuración de la comunicación serial
  Serial.begin(9600);

  // Configuración de los timers
  tb_init(&t_latas, 200000, false);
  tb_init(&t_reset, 750000, false);
  tb_init(&t_botellas, 200000, false);
  tb_init(&t_send, 200000, true);

  tb_init(&t_low_trigger, 2, true);
  tb_init(&t_high_trigger, 10, true);

  //Encoder
  attachInterrupt(digitalPinToInterrupt(encoderAPin), calculapulso, CHANGE);
}

void loop() {


  //Se envían los datos al serial
  sendDataToSerial();
  
  //Acondicionamiento de las distancias
  generateTrigger(ultrasonidoObjetoTrigPin);
  tInicioEcho = micros();
  float t = pulseIn(ultrasonidoObjetoEchoPin, HIGH); //obtenemos el ancho del pulso
  distancia = t/59;             //escalamos el tiempo a una distancia en cm
  //Serial.println(distancia);
  

  // Si la distancia es menor a 20 cm
  if(distancia <= 2){
    tb_enable(&t_botellas);
    tb_update(&t_botellas);
  }
  // Lectura de latas
  if(sensorLataPin==LOW){
    tb_enable(&t_latas);
    tb_update(&t_latas);
  }
  
  if(tb_check(&t_latas)){
    digitalWrite(pistonLataPin, HIGH);
    conteoLatas++;
    tb_disable(&t_latas);
    tb_enable(&t_reset);
    tb_update(&t_reset);
  }

  if(tb_check(&t_botellas)){
    digitalWrite(pistonBotPin, HIGH);
    conteoBotellas++;
    tb_disable(&t_botellas);
    tb_enable(&t_reset);
    tb_update(&t_reset);
  }

  if (tb_check(&t_reset)){
    tb_disable(&t_reset);
    digitalWrite(pistonLataPin, LOW);
    digitalWrite(pistonBotPin, LOW);
  }

  //Encoder
  if (millis() - timeold >= 200){

  rpm = -(60 * 1000 / pulsesperturn ) / (millis() - timeold) * paso/4;
  //Serial.print("RPM: ");
  // Serial.println(rpm);
  timeold = millis();
  paso = 0;

  }

}

//Funcion para enviar los datos en el formato que recibe LabVIEW
void sendDataToSerial(){
  if(tb_check(&t_send)){
    Serial.print("P");
    Serial.print(conteoBotellas);
    Serial.print("L");
    Serial.print(conteoLatas);
    Serial.print("R");
    Serial.print(rpm);
    Serial.println("X");
    tb_next(&t_send);
  }
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