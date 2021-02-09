#pragma once
#include <Arduino.h>

#ifndef ABSTRACT_SENSOR_H
#define ABSTRACT_SENSOR_H

struct MetricResult {
    String kind;
    String valueTypeName;
    String valueAsJsonPropVal;
};

class AbstractSensor {
    public:
        virtual void begin() = 0;
        virtual void onLoopCycle() = 0;
        virtual void onDataClean() = 0;
        virtual MetricResult getMetrics() = 0;
};

#endif