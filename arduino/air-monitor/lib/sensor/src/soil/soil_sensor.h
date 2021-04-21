#include <Arduino.h>
#include "../abstract_sensor.h"

class SoilSensor : public AbstractSensor {
   public:
    SoilSensor(int sclPin, int scaPin);
    ~SoilSensor();

    void begin();
    MetricResult* getMetrics();
    int getMetricsCount();
    void onLoopCycle();
    void onDataClean();
    bool isAlive();

   private:
    int sclPin;
    int scaPin;
    MetricResult soilHumidity[4];
};