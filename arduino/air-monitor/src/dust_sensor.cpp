#include <Arduino.h>
#include <dust_sensor.h>

float afterDividedMaxVoltage = 1;
float realOriginalSensorVoltage = 5;

int measurePin = A0;
int ledPower = D5;

#include <GP2Y1010AU0F.h>
#define PIN_LED D5
#define PIN_OUTPUT A0

double sumOfOutputV = 0;
unsigned int processedSamplesCount = 0;

GP2Y1010AU0F dustSensor(PIN_LED, PIN_OUTPUT);

void resetAverageValues() {
    sumOfOutputV = 0;
    processedSamplesCount = 0;
}

void setupDustSensor() {
    pinMode(ledPower, OUTPUT);
    resetAverageValues();
}

void processDustMeasureCycle(int numberOfSamples, int delayBetweenSamples) {
    for (int indx = 0; indx < numberOfSamples; indx++) {
        double outVoltage = dustSensor.getOutputV();
        sumOfOutputV += outVoltage;
        // Serial.println(" dust out voltage " + String(outVoltage) + " sample:" +
        //                String(indx + 1) + " of " + String(numberOfSamples));
        processedSamplesCount++;
        delay(delayBetweenSamples);
    }
}

DustSensorMeausure measureDustSensorValue() {
    processDustMeasureCycle(1, 10);

    double outputV = sumOfOutputV / processedSamplesCount;  // average outputV
    double ugm3 = dustSensor.getDustDensity(outputV);  // расчет концентрации пыли
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
    // печать в окне монитора
    Serial.println(String("outputV=") + outputV + "\tug/m3= " + ugm3 +
                   "\tAQI=" + aqi + "\tgrade=" + grade);
    // Serial.println(outputV + String(" ") + ugm3 + String(" ") + aqi);

    if (ugm3 < 0) {
        ugm3 = 0.00;
    }

    DustSensorMeausure result;
    result.ugm3 = ugm3;
    result.dustOutvoltageRaw = outputV;
    result.dustOutvoltagePercent = outputV / 5.0 * 100;
    result.grade = grade;

    resetAverageValues();

    return result;
}
