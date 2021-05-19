#define DEBUG true
#define LOGGING_ENABLED true


#include "dht11_sensor.h"
#include <debug/common.h>

#define BAUDRATE 9600                                      // Native to the sensor (do not change)
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200

DHT11Sensor::DHT11Sensor(int pin, int type) : dht(pin, type) {
    alive = true;
    this->termperatureMetric = { String("Temperature"), String("Float") };
    this->humidityMetric = { String("Humidity"), String("Float") };
    onDataClean();
}

DHT11Sensor::~DHT11Sensor() {

}

void DHT11Sensor::onLoopCycle() {
    // Read temperature as Celsius (the default)
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    
    TRACEVALN("DTH11 termperature", temperature);
    TRACEVALN("DTH11 humidity", humidity);


    // Sensor readings may also be up to 2 seconds 'old' (its a very slow
    // sensor)

    
    // Check if any reads failed and exit early (to try again).
    if (isnan(temperature) || isnan(humidity)) {
        TRACE("Failed to read from DHT sensor!");
        dht11Read = false;
        alive = false;
    } else {
        dht11Read = true;
        alive = true;
    }
}

void DHT11Sensor::onDataClean() {
    dht11Read = false;
    alive = false;
}

void DHT11Sensor::begin() {
    dht.begin();
}

MetricResult* DHT11Sensor::getMetrics() {
    termperatureMetric.valueAsJsonPropVal = temperature;
    humidityMetric.valueAsJsonPropVal = humidity;

    metrics[0] = termperatureMetric;
    metrics[1] = humidityMetric;
    return metrics;
}

int DHT11Sensor::getMetricsCount() {
    return 2;
}

bool DHT11Sensor::isAlive() {
    return alive;
}