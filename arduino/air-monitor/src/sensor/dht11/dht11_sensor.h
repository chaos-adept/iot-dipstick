#include <Arduino.h>

#include "../abstract_sensor.h"
#include <DHT.h>


class DHT11Sensor : public AbstractSensor {
   public:
    DHT11Sensor(int pin, int type);
    ~DHT11Sensor();

    void begin();
    MetricResult getMetrics();
    void onLoopCycle();
    void onDataClean();
    bool isAlive();
   private:
    float temperature;
    float humidity;
    bool dht11Read;
    bool alive;
    DHT dht;
};