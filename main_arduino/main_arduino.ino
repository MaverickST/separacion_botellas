// Definición de pines
const int sensorCapPin = 5; // Pin para el primer sensor digital
const int sensorInPin = 17; // Pin para el segundo sensor digital
const int motorSent1Pin = 19; // Pin para el sentido del motor
const int motorSent2Pin = 18; // Pin para el sentido del motor
const int motorSent2Pin = 16; //Pin PWM para controlar el motor

const int ultrasonido1EchoPin = 32;
const int ultrasonido1TrigPin = 36;
const int ultrasonido2EchoPin = 33;
const int ultrasonido2TrigPin = 39;

const int rele1Pin = 25;
const int rele2Pin = 26;


const int encoderPin = 4; // Pin para la lectura del encoder

// Variables globales
int sensor1State = 0; // Variable para almacenar el estado del sensor 1
int sensor2State = 0; // Variable para almacenar el estado del sensor 2
int motorSpeed = 0; // Velocidad del motor

void setup() {
  // Configuración de los pines
  pinMode(sensor1Pin, INPUT);
  pinMode(sensor2Pin, INPUT);

  pinMode(motorPin, OUTPUT);

  pinMode(encoderPin, INPUT);

  pinMode(ultrasonido1EchoPin, INPUT);
  pinMode(ultrasonido1TrigPin, OUTPUT);
  pinMode(ultrasonido2EchoPin, INPUT);
  pinMode(ultrasonido2TrigPin, OUTPUT);

  // Configuración de la comunicación serial
  Serial.begin(9600);
}

void loop() {
  // Lectura de los sensores
  sensor1State = digitalRead(sensor1Pin);
  sensor2State = digitalRead(sensor2Pin);

  // Si se activa el sensor 1, mover el motor en una dirección
  if (sensor1State == HIGH) {
    digitalWrite(motorPin, HIGH); // Encender el motor
    motorSpeed = 255; // Velocidad máxima
    analogWrite(motorPin, motorSpeed); // Control del motor mediante PWM
  } else if (sensor2State == HIGH) {
    // Si se activa el sensor 2, mover el motor en la dirección opuesta
    digitalWrite(motorPin, HIGH); // Encender el motor
    motorSpeed = 255; // Velocidad máxima
    analogWrite(motorPin, motorSpeed); // Control del motor mediante PWM
  } else {
    // Si no se activa ninguno de los sensores, detener el motor
    digitalWrite(motorPin, LOW);
  }

  // Lectura del encoder
  int encoderValue = digitalRead(encoderPin);
  Serial.println(encoderValue); // Mostrar el valor del encoder en el monitor serial

  // Pequeña pausa para evitar la saturación del puerto serial y para dar tiempo a otros procesos
  delay(100);
}