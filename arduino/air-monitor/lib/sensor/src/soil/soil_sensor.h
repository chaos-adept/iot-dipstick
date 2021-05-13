#include <Arduino.h>
#include "../abstract_sensor.h"

#define DEFAULT_MAX_SOIL_SENSOR_COUNT 4
#define DEFAULT_POWER_CTRL_PIN D5

class SoilSensor : public AbstractSensor {
   public:
    SoilSensor(int sdaPin, int sclPin, int sensorCount = DEFAULT_MAX_SOIL_SENSOR_COUNT, int powerControlPin = DEFAULT_POWER_CTRL_PIN);
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