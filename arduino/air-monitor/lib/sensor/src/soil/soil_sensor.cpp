#include <Arduino.h>
#include "soil_sensor.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#define PCF8591 0x48 


void setDAC(byte val){
   Wire.beginTransmission(PCF8591);         // соединяемся с PCF8591
   Wire.write(0x40);                        // включаем DAC     
   Wire.write(val);                         // пишем в DAC значение
   Wire.endTransmission();                  // завершаем соединение
}

SoilSensor::SoilSensor(int sdaPin, int sclPin): sclPin(sclPin), sdaPin(sdaPin) {

    for (int i = 0; i < getMetricsCount(); i++) {
        this->soilHumidity[i] = { String("Soil_Humidity_") + String(i), String("Float"), String("0") };
    }

    this->onDataClean();
}

SoilSensor::~SoilSensor() {

}

void SoilSensor::begin() {
    Serial.println("Soil Sensor Begin");

    #if defined (ESP8266) || defined(ESP32)
    Serial.print("sda ");
    Serial.print(sdaPin);
    Serial.print(" scl ");
    Serial.println(sclPin);
    Wire.begin(sdaPin, sclPin);
    #else
    Wire.begin();
    #endif

    //workaround, for some reason it work correctly after several measuments
    for (byte i = 0; i < 3; i++) {
        onLoopCycle();
        delay(10);
    }

    onDataClean();
}

void SoilSensor::onLoopCycle() {
    Serial.println("Soil Sensor loop cycle");
    Wire.beginTransmission(PCF8591); // wake up PCF8591
    Wire.write(0x04); // control byte - read ADC0 then auto-increment
    Wire.endTransmission(); // end tranmission
    Wire.requestFrom(PCF8591, 5);

    byte value0, value1, value2, value3;

    value0 = Wire.read();
    value0 = Wire.read();
    value1 = Wire.read();
    value2 = Wire.read();
    value3 = Wire.read();

    this->soilHumidity[0].valueAsJsonPropVal = String(value0);
    this->soilHumidity[1].valueAsJsonPropVal = String(value1);
    this->soilHumidity[2].valueAsJsonPropVal = String(value2);
    this->soilHumidity[3].valueAsJsonPropVal = String(value3);

    Serial.println(value0);
    Serial.println(value1);
    Serial.println(value2);
    Serial.println(value3);

    setDAC(value0);
}


bool SoilSensor::isAlive() {
    return true;
}

void SoilSensor::onDataClean() {
       
}

int SoilSensor::getMetricsCount() {
    return 4;
}

MetricResult* SoilSensor::getMetrics() {
    return soilHumidity;
}