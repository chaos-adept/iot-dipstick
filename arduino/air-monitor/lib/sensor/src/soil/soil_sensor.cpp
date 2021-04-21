#include <Arduino.h>
#include "soil_sensor.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#define PCF8591 0x48 


SoilSensor::SoilSensor(int sclPin, int scaPin): sclPin(sclPin), scaPin(scaPin) {
    this->onDataClean();
}

SoilSensor::~SoilSensor() {

}

void SoilSensor::begin() {
    #if defined (ESP8266) || defined(ESP32)
    Wire.begin(scaPin, sclPin);
    #else
    Wire.begin();
    #endif

    onDataClean();
}

void SoilSensor::onLoopCycle() {
     Wire.beginTransmission(PCF8591); // wake up PCF8591
    Wire.write(0x04); // control byte - read ADC0 then auto-increment
    Wire.endTransmission(); // end tranmission
    Wire.requestFrom(PCF8591, 5);

    byte value0, value1, value2, value3;

    value0=Wire.read();
    value0=Wire.read();
    value1=Wire.read();
    value2=Wire.read();
    value3=Wire.read();


    Serial.print(value0); Serial.print(" ");
    Serial.print(value1); Serial.print(" ");
    Serial.print(value2); Serial.print(" ");
    Serial.print(value3); Serial.print(" ");
    Serial.println();
 
}


bool SoilSensor::isAlive() {
    return true;
}

void SoilSensor::onDataClean() {
    MetricResult r1 = soilHumidity[0];    
}

int SoilSensor::getMetricsCount() {
    return 4;
}

MetricResult* SoilSensor::getMetrics() {
    return soilHumidity;
}