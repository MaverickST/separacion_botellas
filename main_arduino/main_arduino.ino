#include "time_base.h"

// DEFINICIONES DE PINES

//Sensor inductivo
const int sensorLataPin = 17; // Pin para el sensor inductivo

//ultrasonidos
const int ultrasonidoObjetoEchoPin = 4;
const int ultrasonidoObjetoTrigPin = 5;
//const int ultrasonidoLataEchoPin = 32;
//const int ultrasonidoLataTrigPin = 36;
//const int ultrasonidoBotEchoPin = 33;
//const int ultrasonidoBotTrigPin = 39;

//Motor
const int motorPWM = 16; //Pin PWM para controlar el motor

//Pistones
const int pistonLataPin = 25;
const int pistonBotPin = 26;

//Encoder
const int encoderAPin = 4; // Pin para la lectura del encoder
const int encoderBPin = 

//Constantes
bool hayLata = false;
bool hayBotella = false;
int conteoLatas = 0;
int conteoBotellas = 0;

time_base_t t_latas;
time_base_t t_botellas;
time_base_t t_send;

time_base_t t_low_trigger;
time_base_t t_high_trigger;

int tInicioEcho;
bool inicioEcho;
int distancia;

void setup() {
  // Configuración de los pines de entrada
  pinMode(sensorLataPin, INPUT);
  pinMode(ultrasonidoObjetoEchoPin, INPUT);
  pinMode(encoderPin, INPUT);
  // Configuración de los pines de salida
  pinMode(ultrasonidoObjetoTrigPin, OUTPUT);
  pinMode(motorPWM, OUTPUT);
  
  // Configuración de la comunicación serial
  Serial.begin(9600);

  // Configuración de los timers
  tb_init(&t_latas, 200, false);
  tb_init(&t_botellas, 200, false);
  tb_init(&t_send, 250000, true);

  tb_init(&t_low_trigger, 2, true);
  tb_init(&t_high_trigger, 10, true);
}

void loop() {

  //Se envían los datos al serial
  sendDataToSerial(&t_send);

  //Acondicionamiento de las distancias
  generateTrigger(ultrasonidoObjetoTrigPin);
  tInicioEcho = micros();
  if(digitalRead(ultrasonidoObjetoEchoPin)==HIGH){
    distancia = ((micros() - tInicioEcho)*0.0343)/2;
  }

  // Si la distancia es menor a 20 cm
  if(distancia <= 20){
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
  }

  if(tb_check(&t_botellas)){
    digitalWrite(pistonBotPin, HIGH);
    conteoBotellas++;
    tb_disable(&t_botellas);
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
