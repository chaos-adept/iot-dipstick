#include <DHT12.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUDP.h>
//#include <WiFiClientSecure.h>  //included WiFiClientSecure does not work!
//fixme find a way to use without a workaround
#include "src/dependencies/WiFiClientSecure/WiFiClientSecure.h" //using older WiFiClientSecure
#include <PubSubClient.h> // MQTT Client


// yandex cloud registry settings
#include "yandex_cloud_mqtt_settings.h"

// device specific config should be created by './local_specific_variables.h.sample'
#include "local_specific_variables.h"
#include "time_utils.h"

#define PUBLISH_TIMEOUT 2 * 1000 * mqttKeepAlive
#define NTP_OFFSET   0      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "ru.pool.ntp.org"
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

DHT12 dht12;

char msg[400];
const char* msgTemplate = "{\"TimeStamp\":\"%s\",\n \"Values\":\n[{\"Type\":\"Float\",\"Name\":\"Humidity\",\"Value\":null},\n {\"Type\":\"Float\",\"Name\":\"CarbonDioxide\",\"Value\":null},\n {\"Type\":\"Float\",\"Name\":\"Pressure\",\"Value\":null},\n {\"Type\":\"Float\",\"Name\":\"Temperature\",\"Value\":\"%d.%02d\"}]}";
String topicCommands = String("$devices/")+String(yandexIoTCoreDeviceId)+String("/commands");
String topicEvents = String("$devices/")+String(yandexIoTCoreDeviceId)+String("/events");

bool lightIsOn = false;

WiFiClientSecure  net;
PubSubClient client(net);
//BearSSL::X509List x509(registry_root_ca);

void connect() {
  delay(5000);
  DEBUG_SERIAL.print("Conecting to wifi ...");
  while (WiFi.status() != WL_CONNECTED) {
    DEBUG_SERIAL.print(".");
    delay(1000);
  }
  DEBUG_SERIAL.println(" Connected");
  //net.setInsecure();
  DEBUG_SERIAL.print("Connecting to Yandex IoT Core as");
  DEBUG_SERIAL.print(yandexIoTCoreDeviceId);
  DEBUG_SERIAL.print(" ...");
  while (!client.connect("ESP32Client", yandexIoTCoreDeviceId, yandexIoTCoreDevicePassword)) {
    DEBUG_SERIAL.print(".");
    delay(1000);
  }
  DEBUG_SERIAL.println(" Connected");
}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  DEBUG_SERIAL.setTimeout(2000);
  DEBUG_SERIAL.begin(DEBUG_SERIAL_BAUDRATE);
  delay(10);
  DEBUG_SERIAL.println("Device started");
  WiFi.begin(ssid, password);
  client.setServer(mqttserver, mqttport);
  client.setCallback(messageReceived);
  client.setBufferSize(mqttBufferSize);
  client.setKeepAlive(mqttKeepAlive);
  net.setCACert(registry_root_ca);

  timeClient.begin();
  dht12.begin();

  digitalWrite(LED_BUILTIN, HIGH);
}

void messageReceived(char* topic, byte* payload, unsigned int length) {
  String topicString = String(topic);
  DEBUG_SERIAL.print("Message received. Topic: ");
  DEBUG_SERIAL.println(topicString.c_str());
  String payloadStr = "";
  for (int i=0;i<length;i++) {
    payloadStr += (char)payload[i];
  }
  DEBUG_SERIAL.print("Payload: ");
  DEBUG_SERIAL.println(payloadStr);
  lightIsOn = payloadStr.equals("True");
  DEBUG_SERIAL.println(lightIsOn);

  digitalWrite(LED_BUILTIN, lightIsOn ? LOW : HIGH);
}

void loop() {

 DEBUG_SERIAL.println("next cycle");

  // Turn the LED on by making the voltage LOW
  if (lightIsOn) {
    digitalWrite(LED_BUILTIN, LOW); 
  }

  boolean needPublish = true;
  if(needPublish) {
    if (lightIsOn) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(10);
      digitalWrite(LED_BUILTIN, LOW);
    }
    
    
//    DHT12::ReadStatus chk = dht12.readStatus();
//    switch (chk) {
//    case DHT12::OK:
//      break;
//    case DHT12::ERROR_CHECKSUM:
//      Serial.println(F("Checksum error"));
//      break;
//    case DHT12::ERROR_TIMEOUT:
//      Serial.println(F("Timeout error"));
//      break;
//    case DHT12::ERROR_TIMEOUT_LOW:
//      Serial.println(F("Timeout error on low signal, try put high pullup resistance"));
//      break;
//    case DHT12::ERROR_TIMEOUT_HIGH:
//      Serial.println(F("Timeout error on low signal, try put low pullup resistance"));
//      break;
//    case DHT12::ERROR_CONNECT:
//      Serial.println(F("Connect error"));
//      break;
//    case DHT12::ERROR_ACK_L:
//      Serial.println(F("AckL error"));
//      break;
//    case DHT12::ERROR_ACK_H:
//      Serial.println(F("AckH error"));
//      break;
//    case DHT12::ERROR_UNKNOWN:
//      Serial.println(F("Unknown error DETECTED"));
//      break;
//    case DHT12::NONE:
//      Serial.println(F("No result"));
//      break;
//    default:
//      Serial.println(F("Unknown error"));
//      break;
//    }
    
//    // Read temperature as Celsius (the default)
//    float t12 = dht12.readTemperature();
//    // Read temperature as Fahrenheit (isFahrenheit = true)
//    float f12 = dht12.readTemperature(true);
//    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
//    float h12 = dht12.readHumidity();

    bool dht12Read = true;
    // Check if any reads failed and exit early (to try again).

//    if (isnan(h12) || isnan(t12) || isnan(f12)) {
//      Serial.println("Failed to read from DHT12 sensor!");
//
//      dht12Read = false;
//    }

    // Read temperature as Celsius (the default)
    float t12 = 24.5f;
//    // Read temperature as Fahrenheit (isFahrenheit = true)
//    float f12 = dht12.readTemperature(true);
//    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
//    float h12 = dht12.readHumidity();

    if (dht12Read){

      client.loop();

      if (!client.connected()) {
        connect();
      }

      timeClient.update();
      String formattedTime = getFormattedDate(timeClient.getEpochTime());

      snprintf(msg, sizeof(msg), msgTemplate,
        formattedTime.c_str(),
        (int)t12, (int)(t12*100)%100   
      );

      DEBUG_SERIAL.println(msg);
      
      if (client.publish(topicEvents.c_str(), msg)) {        
          DEBUG_SERIAL.println(topicEvents);
          DEBUG_SERIAL.println("Publish ok");
      } else {
        DEBUG_SERIAL.println("Publish failed");
      }

    }
  }

  DEBUG_SERIAL.println("sleep");
  DEBUG_SERIAL.println(PUBLISH_TIMEOUT / (1000 * 60));
  delay(PUBLISH_TIMEOUT);
}
