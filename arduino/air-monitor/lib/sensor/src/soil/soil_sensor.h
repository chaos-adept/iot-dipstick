#include <Arduino.h>
#include "../abstract_sensor.h"

class SoilSensor : public AbstractSensor {
   public:
    SoilSensor(int sdaPin, int sclPin);
    ~SoilSensor();

    void begin();
    MetricResult* getMetrics();
    int getMetricsCount();
    void onLoopCycle();
    void onDataClean();
    bool isAlive();

   private:
    int sclPin;
    int sdaPin;
    MetricResult soilHumidity[4];
};