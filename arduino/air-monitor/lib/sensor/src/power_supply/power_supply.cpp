#define DEBUG true
#define LOGGING_ENABLED true

#include <Arduino.h>
#include "power_supply.h"
#include <debug/common.h>

PowerVoltageSensor::PowerVoltageSensor(int analogPin): analogPin(analogPin) {
    lastVoltageMeasure = { String("PowerVoltage"), String("Float"), String("0") };

}


PowerVoltageSensor::~PowerVoltageSensor() {

}


void PowerVoltageSensor::begin() {
    pinMode(analogPin, INPUT);
}

void PowerVoltageSensor::onDataClean() {
}


void PowerVoltageSensor::onLoopCycle() {
    int voltage = analogRead(analogPin);
    String propVal = String(map(voltage, 0, 4095, 0, 100));
    TRACEVALN("voltage", voltage);
    TRACEVALN("prop val", propVal);
    lastVoltageMeasure.valueAsJsonPropVal = propVal;
}


bool PowerVoltageSensor::isAlive() {
    return true;
}

int PowerVoltageSensor::getMetricsCount() {
    return 1;
}

MetricResult* PowerVoltageSensor::getMetrics() {
    return &lastVoltageMeasure;
}