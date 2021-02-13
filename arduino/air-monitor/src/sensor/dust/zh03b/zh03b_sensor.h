#include <Arduino.h>

#include "../../abstract_sensor.h"
#include <SD_ZH03B.h>
#include <SoftwareSerial.h>


class ZH03BSensor : public AbstractSensor {
   public:
    ZH03BSensor(int rxPin, int txPin);
    ~ZH03BSensor();

    void begin();
    MetricResult* getMetrics();
    int getMetricsCount();
    void onLoopCycle();
    void onDataClean();
    bool isAlive();

    bool sleep();
    bool wakeup();

   private:
    SoftwareSerial zhSerial;
    SD_ZH03B zh03B;

    MetricResult pm1_0_metricResult;
    MetricResult pm2_5_metricResult;
    MetricResult pm10_metricResult;
    MetricResult metricResults[3];

    bool alive;
    double pm1_0_sumOfOutput = 0;
    double pm2_5_sumOfOutput = 0;
    double pm10_sumOfOutput = 0;
    unsigned int processedSamplesCount = 0;
    int defaultNumberOfSamples = 3;
    int defaultDelayBetweenSamples = 500;

    void processDustMeasureCycle(int defaultNumberOfSamples,
                                 long delayBetweenSamples);    
};