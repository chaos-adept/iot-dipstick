#include <Arduino.h>
#include <dust_sensor.h>

float afterDividedMaxVoltage = 1;
float realOriginalSensorVoltage = 5;

int measurePin = A0;
int ledPower = D5;

unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 10;

float voMeasured = 0;
float calcVoltage = 0;
float voRawVoltage = 0;
float dustDensity = 0;

#include <GP2Y1010AU0F.h>
#define PIN_LED D5
#define PIN_OUTPUT A0

GP2Y1010AU0F dustSensor(PIN_LED, PIN_OUTPUT);

void setupDustSensor()
{
  pinMode(ledPower, OUTPUT);
}

DustSensorMeausure measureDustSensorValue()
{

  double outputV = dustSensor.getOutputV();         // выборка получает выходное напряжение
  double ugm3 = dustSensor.getDustDensity(outputV); // расчет концентрации пыли
  double aqi = dustSensor.getAQI(ugm3);             // вычисление aqi
  int gradeInfo = dustSensor.getGradeInfo(aqi);     // вычисление уровня
  String grade;
  switch (gradeInfo)
  {
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
  Serial.println(String("outputV=") + outputV + "\tug/m3= " + ugm3 + "\tAQI=" + aqi + "\tgrade=" + grade);
  //Serial.println(outputV + String(" ") + ugm3 + String(" ") + aqi);

  if (dustDensity < 0)
  {
    dustDensity = 0.00;
  }

  
  DustSensorMeausure result;
  result.dustDensity = dustDensity;
  result.dustOutvoltageRaw = dustDensity;
  result.dustOutvoltagePercent = outputV;
  
  return result;
}
