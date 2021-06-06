#include <Arduino.h>
#include "../abstract_sensor.h"

class PowerVoltageSensor : public AbstractSensor
{
    public:
        PowerVoltageSensor(int analogPin);
        ~PowerVoltageSensor();

        void begin();
        MetricResult *getMetrics();
        int getMetricsCount();
        void onLoopCycle();
        void onDataClean();
        bool isAlive();

    private:
        int analogPin;
        MetricResult lastVoltageMeasure;
};