void setupDustSensor();

struct DustSensorMeausure {
    double dustDensity;
    double dustOutvoltageRaw; 
    double dustOutvoltagePercent; 
};

DustSensorMeausure measureDustSensorValue();
