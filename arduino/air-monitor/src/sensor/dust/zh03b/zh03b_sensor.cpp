#include "zh03b_sensor.h"

ZH03BSensor::ZH03BSensor(int rxPin, int txPin)
    : zhSerial(rxPin, txPin), zh03B(zhSerial) {
    pm1_0_metricResult = {String("PM 1.0"), String("Float")};
    pm2_5_metricResult = {String("PM 2.5"), String("Float")};
    pm10_metricResult = {String("PM 10"), String("Float")};
    onDataClean();
}

ZH03BSensor::~ZH03BSensor() {}

void ZH03BSensor::begin() {
    Serial.println("-- ZH03B initialization started --");
    zhSerial.begin(9600);
    delay(100);
    zh03B.setMode(SD_ZH03B::QA_MODE);
    // zh03B.setQandAmode();

    Serial.println("-- ZH03B initialized --");
    delay(200);
    onDataClean();
}
void ZH03BSensor::onLoopCycle() {
    processDustMeasureCycle(defaultNumberOfSamples, defaultDelayBetweenSamples);
}

void ZH03BSensor::onDataClean() {
    pm1_0_sumOfOutput = 0;
    pm2_5_sumOfOutput = 0;
    pm10_sumOfOutput = 0;
    processedSamplesCount = 0;
    alive = true;
}

int ZH03BSensor::getMetricsCount() { return 3; }

MetricResult* ZH03BSensor::getMetrics() {
    // processDustMeasureCycle(1, 1000);

    pm1_0_metricResult.valueAsJsonPropVal =
        pm1_0_sumOfOutput / processedSamplesCount;
    pm2_5_metricResult.valueAsJsonPropVal =
        pm2_5_sumOfOutput / processedSamplesCount;
    pm10_metricResult.valueAsJsonPropVal =
        pm10_sumOfOutput / processedSamplesCount;

    metricResults[0] = pm1_0_metricResult;
    metricResults[1] = pm2_5_metricResult;
    metricResults[2] = pm10_metricResult;

    return metricResults;
}

void ZH03BSensor::processDustMeasureCycle(int defaultNumberOfSamples,
                                          long delayBetweenSamples) {
    char printbuf1[80];
    
    Serial.print("zh03B mode: ");
    Serial.println( zh03B.getMode() == SD_ZH03B::IU_MODE ? "IU:" : "Q&A:" ); 

    for (int indx = 0; indx < defaultNumberOfSamples; indx++) {
        alive = zh03B.readData();
        if (!alive) {
            Serial.println("ZH03B cant read data.");
            break;
        }

        sprintf(printbuf1, "PM1.0, PM2.5, PM10=[%d %d %d]", 
            zh03B.getPM1_0(), zh03B.getPM2_5(), zh03B.getPM10_0());
                
        Serial.println(printbuf1);

        pm1_0_sumOfOutput += zh03B.getPM1_0();
        pm2_5_sumOfOutput += zh03B.getPM2_5();
        pm10_sumOfOutput += zh03B.getPM10_0();

        processedSamplesCount++;

        delay(delayBetweenSamples);
    }
}

bool ZH03BSensor::isAlive() { return this->alive; }

bool ZH03BSensor::sleep() {
     Serial.print("zh03b sleep: ");
     bool confirmed = zh03B.sleep(); 
     Serial.println(confirmed);
     return confirmed;
}

bool ZH03BSensor::wakeup() {
    Serial.print("zh03b wakeup: ");
    bool confirmed = zh03B.wakeup(); 
    delay(1000);
    Serial.println(confirmed);
    return confirmed;
}
