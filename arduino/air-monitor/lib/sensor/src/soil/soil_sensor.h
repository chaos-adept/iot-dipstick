#include <Arduino.h>
#include "../abstract_sensor.h"

#define DEFAULT_MAX_SOIL_SENSOR_COUNT 4

class SoilSensor : public AbstractSensor {
   public:
    SoilSensor(int sdaPin, int sclPin, int sensorCount = DEFAULT_MAX_SOIL_SENSOR_COUNT);
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
    int sensorCount;
    MetricResult soilHumidity[4];
};