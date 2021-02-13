#define DEBUG true
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
#include "sensor/co2/z19b/z19b_sensor.h"
#include "sensor/dht11/dht11_sensor.h"
#include "sensor/dust/GP2Y1010AU0F/dust_sensor.h"
#include "sensor/dust/zh03b/zh03b_sensor.h"
#include "time_utils.h"

#define PUBLISH_INTERVAL (1000 * 60 * 5)  // once in the 5 minutes
#define CYCLE_DELAY 9000                   // 6s
#define SESNOR_MIN_CLYCLE_DELAY_TO_SLEEP 3000     // sensors will go sleep only if cycle delay more than this value
#define NTP_OFFSET 0                       // In seconds
#define NTP_INTERVAL 60 * 1000             // In miliseconds
#define NTP_ADDRESS "ru.pool.ntp.org"
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200

// PINS
#define CO2_RX_PIN D6
#define CO2_TX_PIN D7
#define DHT_PIN D1
#define DUST_ZH038_RX D2
#define DUST_ZH038_TX D3
#define DUST_GP2Y_ANALOG_IN_PIN D5
#define DUST_GP2Y_LED_CONTROL_PIN A0


#define DHTTYPE DHT11  // DHT 11


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

GP2Y1010AU0FSensor sharpSensor(DUST_GP2Y_LED_CONTROL_PIN, DUST_GP2Y_ANALOG_IN_PIN);
Z19BSensor co2sensor(CO2_RX_PIN, CO2_TX_PIN);
DHT11Sensor dth11Sensor(DHT_PIN, DHTTYPE);
ZH03BSensor zh03Sensor(DUST_ZH038_RX, DUST_ZH038_TX);

AbstractSensor* sensors[] = {&co2sensor, &dth11Sensor, &zh03Sensor, &sharpSensor};
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
    DEBUG_SERIAL.print("Free memory: ");
    DEBUG_SERIAL.println(system_get_free_heap_size());
}

void connect() {
    delay(5000);
    DEBUG_SERIAL.print("Conecting to wifi ...");
    while (WiFi.status() != WL_CONNECTED) {
        DEBUG_SERIAL.print(".");
        delay(1000);
    }
    DEBUG_SERIAL.println(" Connected");
    net.setInsecure();
    DEBUG_SERIAL.print("Connecting to Yandex IoT Core as");
    DEBUG_SERIAL.print(yandexIoTCoreDeviceId);
    DEBUG_SERIAL.print(" ...");
    while (!client.connect("Esp8266Client", yandexIoTCoreDeviceId,
                           yandexIoTCoreDevicePassword)) {
        DEBUG_SERIAL.print(".");
        delay(1000);
    }
    DEBUG_SERIAL.println(" Connected");

    DEBUG_SERIAL.println("registry command topic:");
    DEBUG_SERIAL.println(topicRegistryCommands);

    boolean subcribed = client.subscribe(topicRegistryCommands.c_str());
    DEBUG_SERIAL.println(subcribed);
}

void messageReceived(char* topic, byte* payload, unsigned int length) {
    String topicString = String(topic);
    DEBUG_SERIAL.print("Message received. Topic: ");
    DEBUG_SERIAL.println(topicString.c_str());
    String payloadStr = "";
    for (unsigned int i = 0; i < length; i++) {
        payloadStr += (char)payload[i];
    }
    DEBUG_SERIAL.print("Payload: ");
    DEBUG_SERIAL.println(payloadStr);

    if (topicRegistryCommandLight == topicString) {
        DEBUG_SERIAL.print("Handling light cmd");
        lightIsOn = payloadStr.equals("True");
        DEBUG_SERIAL.println(lightIsOn);

        digitalWrite(LED_BUILTIN, lightIsOn ? LOW : HIGH);
    } else {
        DEBUG_SERIAL.println("Unknown command");
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
    DEBUG_SERIAL.println("Device started");
    DEBUG_SERIAL.print("Sensor count: ");
    DEBUG_SERIAL.println(sensorCount);
    
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
        DEBUG_SERIAL.println("no alive sensors to send data");
    } else {
        
        // results might be more than sensors
        
        MetricResult results[totalExpectedMetricCount];  
        int resultMetricsCount = 0;
        for (int sensorIndx = 0; sensorIndx < aliveSensorCount; sensorIndx++) {
            AbstractSensor* sensor = aliveSensors[sensorIndx];
            for (int metricIndx = 0; metricIndx < sensor->getMetricsCount(); metricIndx++) {
                results[resultMetricsCount] = sensor->getMetrics()[metricIndx];
                resultMetricsCount++;
            }
        }

        char* msg = threadUnsafeFormatMetricsAsJson(timeClient.getEpochTime(),
                                                    results, resultMetricsCount);
        DEBUG_SERIAL.println(msg);

        if (client.publish(topicEvents.c_str(), msg)) {
            DEBUG_SERIAL.println(topicEvents);
            DEBUG_SERIAL.println("Publish ok");
        } else {
            DEBUG_SERIAL.println("Publish failed");
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

    DEBUG_SERIAL.println("next cycle, time from last publish: " +
                         String(timeFromTheLastPublish));

    client.loop();

    if (!client.connected()) {
        connect();
    }

    dumpMemory();

    updateLedStatus();

    for (int indx = 0; indx < sensorCount; indx++) {
        sensors[indx]->onLoopCycle(); 
    }

    dumpMemory();

    if (needPublish) {
        publishActualMetrics();
    }

    dumpMemory();
    

    for (int indx = 0; indx < sensorCount; indx++) {
        sensors[indx]->onDataClean();
    }

    unsigned long loopEndTime = millis();

    DEBUG_SERIAL.print("sleep for ");
    DEBUG_SERIAL.print(CYCLE_DELAY);
    DEBUG_SERIAL.print(" cycle actual time ");
    unsigned long delta = (loopEndTime - loopStartTime);
    DEBUG_SERIAL.print(delta);
    if (delta > CYCLE_DELAY) {
        DEBUG_SERIAL.println(", Cycle time more than expected - skip cycle delay.");
        return;
    }

    DEBUG_SERIAL.println(" actualDelay ");
    unsigned long actualDelay = CYCLE_DELAY - delta;
    DEBUG_SERIAL.println(actualDelay);

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
