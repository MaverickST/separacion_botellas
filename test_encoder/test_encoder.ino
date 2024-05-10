//Motor de CD con motorreductor, caja de engranes metálicos y encoder cuadratura. Torque de 5.2 kg.cm a 110 rpm.

//Autor: Ing. Josep Ramon Vidal. Joober Tecnologies. https://joober.page.link/joober

const char Encoder_C1 = 3; // Cable amarillo pin 3 digital

const char Encoder_C2 = 4; // Cable verde al pin 4 digital

int Encoder_C1Last;

int paso = 0;

bool direccion = true;

float rpm = 0;
int pulsesperturn = 11;

unsigned long timeold = 0;

void setup()
{

  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(3), calculapulso, CHANGE);

}



void loop()
{
  pinMode(Encoder_C2, INPUT);

  /*Serial.print("Paso: ");

    Serial.print(paso);

    Serial.print(", ");

    Serial.print("Ángulo: ");

    Serial.println(paso * 0.481283422459893);*/

  /*Para por cada vuelta del rotor antes de la caja de engranes, se producen 22 pulsos (o pasos) del encoder.

    El motor tiene una relación de reducción en su caja de engranes de 1:34. Por tanto, se tienen 748 ticks o pulsos

    del encoder por cada revolución del rotor después de la caja de engranes. Por lo que 748/360 = 0.48128...*/


  if (millis() - timeold >= 200){

    rpm = (60 * 1000 / pulsesperturn ) / (millis() - timeold) * paso;
    //Serial.print("RPM: ");
    Serial.println(rpm);
    timeold = millis();
    paso = 0;

  }

}



void calculapulso()
{

  if ((Encoder_C1Last == LOW) && digitalRead(Encoder_C1) == HIGH){

    if (digitalRead(Encoder_C2) == LOW && direccion){
      direccion = false; //Reverse
    }
    else if (digitalRead(Encoder_C2) == HIGH && !direccion){
      direccion = true;  //Forward
    }
  }

  Encoder_C1Last = digitalRead(Encoder_C1);

  if (!direccion)  paso++;
  else  paso--;

}