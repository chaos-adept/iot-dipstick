#define DEBUG true
#define LOGGING_ENABLED true

#include <Arduino.h>
#include "water_controller.h"
#include <debug/common.h>

// 0 himiduty, 100 in water
// min souil humidiuty should be determined
#define MIN_SOIL_HUMIDITY 10

// #define OPEN_WATERING_GATETIME 1000 * 10
#define WATER_CYCLE_DELAY 3000
#define WATER_CYCLE_COUNT 6

// 10s = 600 ml
#define WATER_SPEED_MILLS_PER_SEC 60
#define WATER_SPEED_MILLS_PER_MILL_SEC WATER_SPEED_MILLS_PER_SEC * 1000

WaterController::WaterController(int potsCount, int* gatePins, int* gateWaterAmount) : 
    potsCount(potsCount), gatePins(gatePins), gateWaterAmount(gateWaterAmount) {

}

void WaterController::begin () {
    for (int i = 0; i < getPotsCount(); i++) {
        pinMode(gatePins[i], OUTPUT);  
        digitalWrite(gatePins[i], LOW);  

        metrics[i].kind = "Pod-" + String(i) + "-Watering";
        metrics[i].valueTypeName = "Float"; 
        metrics[i].valueAsJsonPropVal = "0"; 
    }
}

void WaterController::performWateringIfNeeded (MetricResult* soilHumidityMetrics) {
    TRACELN("performWateringIfNeeded");
    // MetricResult* metrics = soilHumidity;
    //MetricResult* gateMetrics = metrics;
    
    for (int i = 0; i < getPotsCount(); i++) {
        TRACE("Processing Pot #"); TRACELN(i);
        float value = soilHumidityMetrics[i].valueAsJsonPropVal.toFloat();

        if (value < MIN_SOIL_HUMIDITY) {
            // start watering cycles
            int amountOfWater = gateWaterAmount[i];
            TRACEVALN("Gate amount of water ", amountOfWater);
            int amountOfWaterPerCycle = gateWaterAmount[i] / WATER_CYCLE_COUNT;
            int gatePerCycleDelay = amountOfWaterPerCycle / WATER_SPEED_MILLS_PER_MILL_SEC;
            TRACEVALN("Gate Cycle Delay ", gatePerCycleDelay);
            TRACELN("Start Watering Cycles");
            TRACEVALN("Total Cycle Iterations", WATER_CYCLE_COUNT);
            

            for (int wi = 0; wi < WATER_CYCLE_COUNT; wi++) {
                TRACEVALN("cycle", wi);
                TRACEVALN("open pod gate for ms", gatePerCycleDelay);
                digitalWrite(gatePins[i], HIGH);
                delay(gatePerCycleDelay);
                digitalWrite(gatePins[i], LOW); 
                TRACELN("... closed gate");
                TRACEVALN("sleep in cycle ms", WATER_CYCLE_DELAY);
                delay(WATER_CYCLE_DELAY);
            }

            TRACELN("Complete Watering Cycles");
            metrics[i].valueAsJsonPropVal = "1";
        } else {
            TRACE("watering don't need for pod #"); TRACELN(i+1);
            metrics[i].valueAsJsonPropVal = "0";
        }
        
    }
}

int WaterController::getPotsCount() {
    return potsCount;
}

MetricResult* WaterController::getMetrics() {
    return metrics;
}