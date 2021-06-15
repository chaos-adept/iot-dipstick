#include <Arduino.h>
#include "../abstract_sensor.h"


class SoilSensor : public AbstractSensor {
   public:
    SoilSensor(int sdaPin, int sclPin, int sensorCount, int powerControlPin);
    ~SoilSensor();

    void begin();
    MetricResult* getMetrics();
    int getMetricsCount();
    void onLoopCycle();
    void onDataClean();
    bool isAlive();
    bool sleep();
    bool wakeup();

   private:
    int sclPin;
    int sdaPin;
    int sensorCount;
    int powerControlPin;
    MetricResult soilHumidity[4];
};