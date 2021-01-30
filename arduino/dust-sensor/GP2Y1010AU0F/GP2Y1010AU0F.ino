// source from https://atceiling.blogspot.com/2019/10/arduino73-gp2y1010au0f-pm25.html

#include <GP2Y1010AU0F.h>
#define PIN_LED 5
#define PIN_OUTPUT A0

GP2Y1010AU0F GP2Y1010AU0F(PIN_LED, PIN_OUTPUT);

void setup() {
  Serial.begin(115200);
}

void loop() {
  double outputV = GP2Y1010AU0F.getOutputV(); // выборка получает выходное напряжение
  double ugm3 = GP2Y1010AU0F.getDustDensity(outputV); // расчет концентрации пыли
  double aqi = GP2Y1010AU0F.getAQI(ugm3); // вычисление aqi
  int gradeInfo = GP2Y1010AU0F.getGradeInfo(aqi); // вычисление уровня
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
  Serial.println(String("outputV=") + outputV + "\tug/m3= " + ugm3 + "\tAQI= " + aqi + "\tgrade= " + grade);
  //Serial.println(outputV + String(" ") + ugm3 + String(" ") + aqi);

 // интервал
 delay(500);

}
