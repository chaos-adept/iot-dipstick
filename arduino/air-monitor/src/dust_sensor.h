void setupDustSensor();

struct DustSensorMeausure {
    double ugm3;
    double dustOutvoltageRaw; 
    double dustOutvoltagePercent;
    String grade;
};

DustSensorMeausure measureDustSensorValue();
void processDustMeasureCycle(int numberOfSamples, int delayBetweenSamples);
