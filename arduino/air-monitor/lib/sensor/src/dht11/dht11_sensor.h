#include <Arduino.h>

#include "../abstract_sensor.h"
#include <DHT.h>


class DHT11Sensor : public AbstractSensor {
   public:
    DHT11Sensor(int pin, int type);
    ~DHT11Sensor();

    void begin();
    MetricResult* getMetrics();
    int getMetricsCount();
    void onLoopCycle();
    void onDataClean();
    bool isAlive();
   private:
    bool dht11Read;
    bool alive;
    float temperature;
    float humidity;
    
    MetricResult termperatureMetric;
    MetricResult humidityMetric;
    MetricResult metrics[2];

    DHT dht;
};