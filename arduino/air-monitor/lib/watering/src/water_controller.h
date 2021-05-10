
#ifndef WATER_CONTROLLER_H
#define WATER_CONTROLLER_H

#include <../../sensor/src/abstract_sensor.h>

#define MAX_POTS_COUNT 4

class WaterController {
    public:
        WaterController(int potsCount, int gatePins[], int gateWaterAmount[]);
        void begin();
        void performWateringIfNeeded(MetricResult* soilHumidity);
        int getPotsCount();
        MetricResult* getMetrics();
    private:
        int potsCount;
        int* gatePins;
        int* gateWaterAmount;
        MetricResult metrics[MAX_POTS_COUNT];
};

#endif