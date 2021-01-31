#include <Arduino.h>

float afterDividedMaxVoltage = 1;
float realOriginalSensorVoltage = 5;

int measurePin = A0;
int ledPower = D5;

unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 10;

float voMeasured = 0;
float calcVoltage = 0;
float voRawVoltage = 0;
float dustDensity = 0;

#ifdef ESP8266
float analogPinVoltMaxRange = 3.3; //tested on wemos d1 r2, nodemcu v3
#else
float analogPinVoltMaxRange = 5;
#endif  

void setupDustSensor() {
  pinMode(ledPower,OUTPUT);
}

float measureDustSensorValue() {
  digitalWrite(ledPower, LOW);
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin);

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH);
  delayMicroseconds(sleepTime);

  voRawVoltage = voMeasured * (analogPinVoltMaxRange/1024);
  calcVoltage = (voRawVoltage * realOriginalSensorVoltage/afterDividedMaxVoltage);
  dustDensity = 0.17*calcVoltage-0.1;

  if ( dustDensity < 0)
  {
    dustDensity = 0.00;
  }

  return dustDensity;
}
