#include "Wire.h"
#define PCF8591 0x48 

byte value0, value1, value2, value3;

void setDAC(byte val){
   Wire.beginTransmission(PCF8591);         // соединяемся с PCF8591
   Wire.write(0x40);                        // включаем DAC     
   Wire.write(val);                         // пишем в DAC значение
   Wire.endTransmission();                  // завершаем соединение
}

byte getADC(byte ch){
  if(ch > 3) ch = 0;                        // проверка
   Wire.beginTransmission(PCF8591);         // соединяемся с PCF8591
   Wire.write(ch);                          // запрашиваем 
   Wire.endTransmission();                  // завершаем соединение
   Wire.requestFrom(PCF8591, 2);            // забераем 2 байта
   byte val0 = Wire.read();
   byte val1 = Wire.read();                 // во втором то что нам нужно
   return val1;                             // возвращаем
}

void setup()
{
 #if defined (ESP8266) || defined(ESP32)
  Wire.begin(D1, D2);
 #else
  Wire.begin();
 #endif
 Serial.begin(9600);
}
void loop()
{
 Wire.beginTransmission(PCF8591); // wake up PCF8591
 Wire.write(0x04); // control byte - read ADC0 then auto-increment
 Wire.endTransmission(); // end tranmission
 Wire.requestFrom(PCF8591, 5);
 value0=Wire.read();
 value0=Wire.read();
 value1=Wire.read();
 value2=Wire.read();
 value3=Wire.read();


 Serial.print(value0); Serial.print(" ");
 Serial.print(value1); Serial.print(" ");
 Serial.print(value2); Serial.print(" ");
 Serial.print(value3); Serial.print(" ");
 Serial.println();
 

 setDAC(value0);            // изменяем яркость светодиода в зависимости от освещенности

 #if defined (ESP8266) || defined(ESP32)

 #else
 int rawVal1 = analogRead(A0); 
 int rawVal2 = analogRead(A1); 
 Serial.print("A: "); Serial.print(" ");
 Serial.print(rawVal1); Serial.print(" ");
 Serial.print(rawVal2); Serial.print(" ");
 Serial.println();
 #endif

 delay(500);
}
