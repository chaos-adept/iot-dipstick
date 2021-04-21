#include <Arduino.h>
#include <SoftwareSerial.h>  //  Remove if using HardwareSerial or non-uno compatabile device

#include "../../abstract_sensor.h"
#include "MHZ19.h"

class Z19BSensor : public AbstractSensor {
   public:
    Z19BSensor(int rxPin, int txPin);
    ~Z19BSensor();

    void begin();
    MetricResult* getMetrics();
    int getMetricsCount();
    void onLoopCycle();
    void onDataClean();
    bool isAlive();

   private:
    SoftwareSerial mySerial;
    MHZ19 myMHZ19;
    MetricResult coMetricResult;
    MetricResult temperatureMetricResult;
    MetricResult metricResults[2];
    int CO2;
    float temperature;
};