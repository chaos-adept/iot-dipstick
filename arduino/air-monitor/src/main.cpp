#define DEBUG true
#define LOGGING_ENABLED true

#include <debug/common.h>

extern "C" {
    #include "user_interface.h"
}
//uint32_t free = system_get_free_heap_size();

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <PubSubClient.h>  // MQTT Client
#include <WiFiUDP.h>

// yandex cloud registry settings
#include "yandex_cloud_mqtt_settings.h"

// device specific config should be created by
// './local_specific_variables.h.sample'
#include "format/json_formatter.h"
#include "local_specific_variables.h"
#include <co2/z19b/z19b_sensor.h>
#include <dht11/dht11_sensor.h>
#include <soil/soil_sensor.h>
// #include "sensor/dust/GP2Y1010AU0F/dust_sensor.h"
#include <dust/zh03b/zh03b_sensor.h>
#include "time_utils.h"

#define PUBLISH_INTERVAL (1000 * 60 * 1)  // once in the 10 minutes
#define CYCLE_DELAY 5000                   // 6s
#define SESNOR_MIN_CLYCLE_DELAY_TO_SLEEP 3000     // sensors will go sleep only if cycle delay more than this value
#define NTP_OFFSET 0                       // In seconds
#define NTP_INTERVAL 60 * 1000             // In miliseconds
#define NTP_ADDRESS "ru.pool.ntp.org"

// PINS
#define CO2_RX_PIN D6
#define CO2_TX_PIN D7
#define DHT_PIN D1
#define DUST_ZH038_RX D2
#define DUST_ZH038_TX D3
#define DUST_GP2Y_ANALOG_IN_PIN D5
#define DUST_GP2Y_LED_CONTROL_PIN A0

// FIXME overlap with the rest sensor pins, it is a reason of SOIL_MODE
#define SOIL_MODE true
#define SOIL_POD_COUNT 2
#define I2C_PIN_SDA D1
#define I2C_pin_SCL D2

#define DHTTYPE DHT11  // DHT 11


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

// GP2Y1010AU0FSensor sharpSensor(DUST_GP2Y_LED_CONTROL_PIN, DUST_GP2Y_ANALOG_IN_PIN);

#ifdef SOIL_MODE
SoilSensor soilSensor(I2C_PIN_SDA, I2C_pin_SCL, SOIL_POD_COUNT);
AbstractSensor* sensors[] = {&soilSensor};
#else
Z19BSensor co2sensor(CO2_RX_PIN, CO2_TX_PIN);
DHT11Sensor dth11Sensor(DHT_PIN, DHTTYPE);
ZH03BSensor zh03Sensor(DUST_ZH038_RX, DUST_ZH038_TX);
AbstractSensor* sensors[] = {&co2sensor, &dth11Sensor, &zh03Sensor};
#endif

const int sensorCount = sizeof(sensors) / sizeof(sensors[0]);

String topicRegistryCommands = String("$registries/") +
                               String(yandexIoTCoreRegistryId) +
                               String("/commands/#");
String topicRegistryCommandLight = String("$registries/") +
                                   String(yandexIoTCoreRegistryId) +
                                   String("/commands/light");
String topicEvents =
    String("$devices/") + String(yandexIoTCoreDeviceId) + String("/events");

boolean needPublish = true;
boolean isSensorsSleeping = true;
bool lightIsOn = true;

unsigned long lastLoopStartTime = 0L;
unsigned long timeFromTheLastPublish = PUBLISH_INTERVAL;

WiFiClientSecure net;
PubSubClient client(net);
BearSSL::X509List x509(registry_root_ca);


void dumpMemory() {
    TRACE("Free memory: ");
    TRACELN(system_get_free_heap_size());
}

bool connect() {
    delay(5000);
    TRACE("Conecting to wifi ...");
    int MAX_WIFI_CONNECT_ATTEMPTS = 3;
    int wifiConnectAttempts = 0;
    while (WiFi.status() != WL_CONNECTED) {
        TRACE(".");
        delay(1000);
        wifiConnectAttempts++;
        if (wifiConnectAttempts > MAX_WIFI_CONNECT_ATTEMPTS) {
            TRACELN("Max Wi-Fi connection attempts were reached.");
            return false;
        }
    }
    TRACELN(" Connected");
    net.setInsecure();
    TRACE("Connecting to Yandex IoT Core as");
    TRACE(yandexIoTCoreDeviceId);
    TRACE(" ...");
    while (!client.connect("Esp8266Client", yandexIoTCoreDeviceId,
                           yandexIoTCoreDevicePassword)) {
        TRACE(".");
        delay(1000);
    }
    TRACELN(" Connected");

    TRACELN("registry command topic:");
    TRACELN(topicRegistryCommands);

    boolean subcribed = client.subscribe(topicRegistryCommands.c_str());
    TRACELN(subcribed);

    return true;
}

void messageReceived(char* topic, byte* payload, unsigned int length) {
    String topicString = String(topic);
    TRACE("Message received. Topic: ");
    TRACELN(topicString.c_str());
    String payloadStr = "";
    for (unsigned int i = 0; i < length; i++) {
        payloadStr += (char)payload[i];
    }
    TRACE("Payload: ");
    TRACELN(payloadStr);

    if (topicRegistryCommandLight == topicString) {
        TRACE("Handling light cmd");
        lightIsOn = payloadStr.equals("True");
        TRACELN(lightIsOn);

        digitalWrite(LED_BUILTIN, lightIsOn ? LOW : HIGH);
    } else {
        TRACELN("Unknown command");
    }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    delay(1000);

    DEBUG_SERIAL.setTimeout(2000);
    DEBUG_SERIAL.begin(DEBUG_SERIAL_BAUDRATE);
    
    dumpMemory();

    delay(10);
    TRACELN("Device started");
    TRACE("Sensor count: ");
    TRACELN(sensorCount);
    
    WiFi.begin(ssid, password);
    client.setServer(mqttserver, mqttport);
    client.setCallback(messageReceived);
    client.setBufferSize(mqttBufferSize);
    client.setKeepAlive(mqttKeepAlive / 1000);

    timeClient.begin();

    digitalWrite(LED_BUILTIN, HIGH);

    for (int indx = 0; indx < sensorCount; indx++) {
        sensors[indx]->begin();
    }
    
    delay(3000);
}

void publishActualMetrics() {
    if (lightIsOn) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(10);
        digitalWrite(LED_BUILTIN, LOW);
    }

    timeClient.update();

    // get alive sensors
    int aliveSensorCount = 0;
    int totalExpectedMetricCount = 0;
    AbstractSensor* aliveSensors[sensorCount];
    for (int indx = 0; indx < sensorCount; indx++) {
        AbstractSensor* sensor = sensors[indx];
        if (sensor->isAlive()) {
            aliveSensors[aliveSensorCount] = sensor;
            aliveSensorCount++;
            totalExpectedMetricCount += sensor->getMetricsCount();
        }
    }

    if (aliveSensorCount == 0) {
        TRACELN("no alive sensors to send data");
    } else {
        
        // results might be more than sensors
        
        MetricResult results[totalExpectedMetricCount];  
        int resultMetricsCount = 0;
        for (int sensorIndx = 0; sensorIndx < aliveSensorCount; sensorIndx++) {
            AbstractSensor* sensor = aliveSensors[sensorIndx];
            MetricResult* metrics = sensor->getMetrics();
            for (int metricIndx = 0; metricIndx < sensor->getMetricsCount(); metricIndx++) {
                results[resultMetricsCount] = metrics[metricIndx];
                resultMetricsCount++;
            }
        }

        char* msg = threadUnsafeFormatMetricsAsJson(timeClient.getEpochTime(),
                                                    results, resultMetricsCount);
        TRACELN(msg);

        if (client.publish(topicEvents.c_str(), msg)) {
            TRACELN(topicEvents);
            TRACELN("Publish ok");
        } else {
            TRACELN("Publish failed");
        }
    }

    timeFromTheLastPublish = 0L;
}

void updateLedStatus() {
    // Turn the LED on by making the voltage LOW
    if (lightIsOn) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
}

void loop() {
    dumpMemory();

    unsigned long loopStartTime = millis();
    timeFromTheLastPublish += loopStartTime - lastLoopStartTime;
    lastLoopStartTime = loopStartTime;

    needPublish = PUBLISH_INTERVAL <= timeFromTheLastPublish;

    if (isSensorsSleeping) {
        for (int indx = 0; indx < sensorCount; indx++) {
            sensors[indx]->wakeup();
        }      
        isSensorsSleeping = false;  
    }

    dumpMemory();

    TRACELN("next cycle, time from last publish: " +
                         String(timeFromTheLastPublish));

    client.loop();

    bool hasConnected = client.connected() || connect();;
    if (!hasConnected) {
        TRACELN("Device can't connect to the network.");
    }

    dumpMemory();

    

    for (int indx = 0; indx < sensorCount; indx++) {
        sensors[indx]->onLoopCycle(); 
    }

    dumpMemory();

    updateLedStatus(); //fixme it needs to be moved to the update state/render state section
    if (needPublish && hasConnected) { //fixme it needs to be moved to the update state/render state section
        publishActualMetrics();
    } else {
        TRACELN("Metric publishing is skipped needPublish:" + String(needPublish) + " hasConnected:" + String(hasConnected));
    }

    dumpMemory();
    
    for (int indx = 0; indx < sensorCount; indx++) {
        sensors[indx]->onDataClean();
    }

    unsigned long loopEndTime = millis();

    TRACE("sleep for ");
    TRACE(CYCLE_DELAY);
    TRACE(" cycle actual time ");
    unsigned long delta = (loopEndTime - loopStartTime);
    TRACE(delta);
    if (delta > CYCLE_DELAY) {
        TRACELN(", Cycle time more than expected - skip cycle delay.");
        return;
    }

    TRACE(" actualDelay ");
    unsigned long actualDelay = CYCLE_DELAY - delta;
    TRACELN(actualDelay);

    dumpMemory();

    //turn sensors in the sleep mode
    if (actualDelay > SESNOR_MIN_CLYCLE_DELAY_TO_SLEEP) {
        for (int indx = 0; indx < sensorCount; indx++) {
            sensors[indx]->sleep();
        }
        isSensorsSleeping = true;
    }

    dumpMemory();
    delay(actualDelay);
}
