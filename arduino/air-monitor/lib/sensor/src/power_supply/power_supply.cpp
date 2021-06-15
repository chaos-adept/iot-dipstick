#define DEBUG true
#define LOGGING_ENABLED true

#include <Arduino.h>
#include "power_supply.h"
#include <debug/common.h>
#include <driver/adc.h>

PowerVoltageSensor::PowerVoltageSensor(String name, adc1_channel_t analogPin): analogPin(analogPin) {
    lastVoltageMeasure = { name, String("Float"), String("0") };

}


PowerVoltageSensor::~PowerVoltageSensor() {

}


void PowerVoltageSensor::begin() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(analogPin, ADC_ATTEN_DB_11);
}

void PowerVoltageSensor::onDataClean() {
}


void PowerVoltageSensor::onLoopCycle() {
    int voltage  = adc1_get_raw(analogPin);
    String propVal = String(map(voltage, 0, 4095, 0, 3300));

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