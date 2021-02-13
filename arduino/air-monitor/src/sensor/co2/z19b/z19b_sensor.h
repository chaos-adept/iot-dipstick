#include <Arduino.h>
#include <SoftwareSerial.h>  //  Remove if using HardwareSerial or non-uno compatabile device

#include "../../abstract_sensor.h"
#include "MHZ19.h"

class Z19BSensor : public AbstractSensor {
   public:
    Z19BSensor(int rxPin, int txPin);
    ~Z19BSensor();

    void begin();
    MetricResult getMetrics();
    void onLoopCycle();
    void onDataClean();
    bool isAlive();
    
   private:
    SoftwareSerial mySerial;
    MHZ19 myMHZ19;
    int CO2;
};