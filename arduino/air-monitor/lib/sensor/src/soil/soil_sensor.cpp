#include <Arduino.h>
#include "soil_sensor.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <debug/common.h>
#define PCF8591 0x48 


void setDAC(byte val){
   Wire.beginTransmission(PCF8591);         // соединяемся с PCF8591
   Wire.write(0x40);                        // включаем DAC     
   Wire.write(val);                         // пишем в DAC значение
   Wire.endTransmission();                  // завершаем соединение
}

SoilSensor::SoilSensor(int sdaPin, int sclPin, int sensorCount): sclPin(sclPin), sdaPin(sdaPin), sensorCount(sensorCount) {

    for (int i = 0; i < getMetricsCount(); i++) {
        this->soilHumidity[i] = { String("Soil_Humidity_") + String(i), String("Float"), String("0") };
    }

    this->onDataClean();
}

SoilSensor::~SoilSensor() {

}

void SoilSensor::begin() {
    TRACELN("Soil Sensor Begin");

    #if defined (ESP8266) || defined(ESP32)
    TRACE("sda ");
    TRACE(sdaPin);
    TRACE(" scl ");
    TRACELN(sclPin);
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
    TRACELN("Soil Sensor loop cycle");
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

    TRACELN(value0);
    TRACELN(value1);
    TRACELN(value2);
    TRACELN(value3);

    setDAC(value0);
}


bool SoilSensor::isAlive() {
    return true;
}

void SoilSensor::onDataClean() {
       
}

int SoilSensor::getMetricsCount() {
    return sensorCount;
}

MetricResult* SoilSensor::getMetrics() {
    return soilHumidity;
}