#pragma once

#include <Arduino.h>
#include <GP2Y1010AU0F.h>

#include "../../abstract_sensor.h"

struct DustSensorMeausure {
    double ugm3;
    double dustOutvoltageRaw;
    double dustOutvoltagePercent;
    String grade;
};

class GP2Y1010AU0FSensor : public AbstractSensor {
   public:

    GP2Y1010AU0FSensor(int ledPower, int measurePin);
    ~GP2Y1010AU0FSensor();

    void begin();
    void onLoopCycle();
    void onDataClean();
    bool isAlive();

    MetricResult getMetrics();
    DustSensorMeausure getDustMeasure();

   private:

    GP2Y1010AU0F dustSensor;
    MetricResult metricResult;
    double getAverageValue();

    int ledPower;

    double sumOfOutputV = 0;
    unsigned int processedSamplesCount = 0;

    void processDustMeasureCycle(int defaultNumberOfSamples,
                                 long delayBetweenSamples);
    
};
