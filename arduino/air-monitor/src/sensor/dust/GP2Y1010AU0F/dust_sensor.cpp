#include "dust_sensor.h"

#include <Arduino.h>
#include <GP2Y1010AU0F.h>

#include "../../abstract_sensor.h"

// const char* msgTemplateWithoutTemp =
//     "{\"Type\":\"Float\",\"Name\":\"Dust ug/m3\",\"Value\":\"%d.%02d\"}";

int defaultNumberOfSamples = 10;
int defaultDelayBetweenSamples = 200;

GP2Y1010AU0FSensor::GP2Y1010AU0FSensor(int ledPower, int measurePin)
    : dustSensor(ledPower, measurePin) {
    this->ledPower = ledPower;
    metricResult.kind = "Dust ug/m3";
    metricResult.valueTypeName = "Float";
}

GP2Y1010AU0FSensor::~GP2Y1010AU0FSensor() {}

void GP2Y1010AU0FSensor::begin() {
    pinMode(ledPower, OUTPUT);
    onDataClean();
}
void GP2Y1010AU0FSensor::onLoopCycle() {
    processDustMeasureCycle(defaultNumberOfSamples, defaultDelayBetweenSamples);
}
void GP2Y1010AU0FSensor::onDataClean() {
    sumOfOutputV = 0;
    processedSamplesCount = 0;
}

MetricResult GP2Y1010AU0FSensor::getMetrics() {
    processDustMeasureCycle(1, 10);
    DustSensorMeausure measure = getDustMeasure();

    metricResult.valueAsJsonPropVal = String(measure.ugm3);
    return metricResult;
}

DustSensorMeausure GP2Y1010AU0FSensor::getDustMeasure() {
    double outputV = getAverageValue();  // average outputV
    double ugm3 =
        dustSensor.getDustDensity(outputV);  // расчет концентрации пыли
    double aqi = dustSensor.getAQI(ugm3);          // вычисление aqi
    int gradeInfo = dustSensor.getGradeInfo(aqi);  // вычисление уровня
    String grade;
    switch (gradeInfo) {
        case GRADE_PERFECT:
            grade = String("GRADE_PERFECT");
            break;
        case GRADE_GOOD:
            grade = String("GRADE_GOOD");
            break;
        case GRADE_POLLUTED_MILD:
            grade = String("GRADE_POLLUTED_MILD");
            break;
        case GRADE_POLLUTED_MEDIUM:
            grade = String("GRADE_POLLUTED_MEDIUM");
            break;
        case GRADE_POLLUTED_HEAVY:
            grade = String("GRADE_POLLUTED_HEAVY");
            break;
        case GRADE_POLLUTED_SEVERE:
            grade = String("GRADE_POLLUTED_SEVERE");
            break;
    }
    Serial.println(String("outputV=") + outputV + "\tug/m3= " + ugm3 +
                   "\tAQI=" + aqi + "\tgrade=" + grade);

    if (ugm3 < 0) {
        ugm3 = 0.00;
    }

    DustSensorMeausure result;
    result.ugm3 = ugm3;
    result.dustOutvoltageRaw = outputV;
    result.dustOutvoltagePercent = outputV / 5.0 * 100;
    result.grade = grade;

    return result;
}

void GP2Y1010AU0FSensor::processDustMeasureCycle(int defaultNumberOfSamples,
                                                 long delayBetweenSamples) {
    for (int indx = 0; indx < defaultNumberOfSamples; indx++) {
        double outVoltage = dustSensor.getOutputV();
        sumOfOutputV += outVoltage;
        processedSamplesCount++;
        delay(delayBetweenSamples);
    }
}

double GP2Y1010AU0FSensor::getAverageValue() {
    return sumOfOutputV / processedSamplesCount;
}

/**
 * it is actually a workaround because, 
 * it reads analog input, so we dont know for sure then it is really alive
*/
bool GP2Y1010AU0FSensor::isAlive() {
    return this->getAverageValue() > 0.04;
}
