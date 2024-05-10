#include "time_base.h"

// Definición de pines
const int sensorLataPin = 17; // Pin para el sensor inductivo
const int ultrasonidoObjetoEchoPin = 4;
const int ultrasonidoObjetoTrigPin = 5;

// Sentido de giro del motor no se implementa, solo un sentido
// const int motorSent1Pin = 19; // Pin para el sentido del motor
// const int motorSent2Pin = 18; // Pin para el sentido del motor
const int motorPWM = 16; //Pin PWM para controlar el motor

// Dos ultasonido extras
// const int ultrasonido1EchoPin = 32;
// const int ultrasonido1TrigPin = 36;
// const int ultrasonido2EchoPin = 33;
// const int ultrasonido2TrigPin = 39;

const int pistonLataPin = 25;
const int pistonBotPin = 26;

const int encoderPin = 4; // Pin para la lectura del encoder

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

  // Lectura de latas
  if(sensorLataPin==HIGH && sensorBotPin==HIGH){
    tb_enable(&t_latas);
    tb_update(&t_latas);
  }
  // Lectura de botellas
  else if(sensorBotPin==HIGH){
    tb_enable(&t_botellas);
    tb_update(&t_botellas);
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

  sendDataToSerial(&t_send);

  generateTrigger(ultrasonidoObjetoTrigPin);
  if(digitalRead(ultrasonidoObjetoEchoPin)==HIGH){
    tInicioEcho = micros();
    inicioEcho = true;
  }
  if(inicioEcho && digitalRead(ultrasonidoObjetoEchoPin)==LOW){
    // Distancia del objeto en cm
    distancia = ((micros() - tInicioEcho)*0.0343)/2;
  }
  // Si la distancia es menor a 20 cm
  if(distancia <= 20){

  }

}

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