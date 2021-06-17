#include <Arduino.h>
#include "../abstract_sensor.h"
#include <driver/adc.h>

class PowerVoltageSensor : public AbstractSensor
{
    public:
        PowerVoltageSensor(String name, adc1_channel_t analogPin, float ratio);
        ~PowerVoltageSensor();

        void begin();
        MetricResult *getMetrics();
        int getMetricsCount();
        void onLoopCycle();
        void onDataClean();
        bool isAlive();

    private:
        adc1_channel_t analogPin;
        float ratio;
        MetricResult lastVoltageMeasure;
};