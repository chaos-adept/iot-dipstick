#include <Arduino.h>

int measurePin = A0;
int ledPower = D5;

unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float voRawVoltage = 0;
float dustDensity = 0;

float afterDividedMaxVoltage = 1;
float realOriginalSensorVoltage = 5;

#ifdef ESP8266
float analogPinVoltMaxRange = 3.3; //tested on wemos d1 r2
#else
float analogPinVoltMaxRange = 5;
#endif

void setup(){
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
}

void loop(){
  
  
  digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin);

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH);
  delayMicroseconds(sleepTime);

  voRawVoltage = voMeasured * (analogPinVoltMaxRange/1024);
  calcVoltage = (voRawVoltage * realOriginalSensorVoltage/afterDividedMaxVoltage);
  dustDensity = 0.17*calcVoltage-0.1;

  if ( dustDensity < 0)
  {
    dustDensity = 0.00;
  }

  Serial.print("Raw Signal Value (0-1023): ");
  Serial.print(voMeasured);

  Serial.print(" Raw Voltage Devided: ");
  Serial.print(voRawVoltage);

  Serial.print(" Real Voltage: ");
  Serial.print(calcVoltage);

  Serial.print(" Dust Density: ");
  Serial.println(dustDensity);

  delay(1000);
}
