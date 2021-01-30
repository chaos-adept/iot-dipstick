#include <DHT.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <PubSubClient.h> // MQTT Client

// yandex cloud registry settings
#include "yandex_cloud_mqtt_settings.h"

// device specific config should be created by './local_specific_variables.h.sample'
#include "local_specific_variables.h"
#include "time_utils.h"

// it will be replaced with a third-party lib later
#include "dust_sensor.h"

#define PUBLISH_TIMEOUT 1000/2 * mqttKeepAlive
#define NTP_OFFSET   0      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "ru.pool.ntp.org"
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

#define DHTPIN D2 
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

char msg[400];
const char* msgTemplateDefault = "{\"TimeStamp\":\"%s\",\n \"Values\":\n[{\"Type\":\"Float\",\"Name\":\"Dust ug/m3\",\"Value\":\"%d.%02d\"},\n {\"Type\":\"Float\",\"Name\":\"Temperature\",\"Value\":\"%d.%02d\"},\n {\"Type\":\"Float\",\"Name\":\"Humidity\",\"Value\":\"%d.%02d\"}\n]}";
const char* msgTemplateWithoutTemp = "{\"TimeStamp\":\"%s\",\n \"Values\":\n[\n {\"Type\":\"Float\",\"Name\":\"Dust ug/m3\",\"Value\":\"%d.%02d\"}\n]}";

String topicRegistryCommands = String("$registries/")+String(yandexIoTCoreRegistryId)+String("/commands/#");
String topicRegistryCommandLight = String("$registries/")+String(yandexIoTCoreRegistryId)+String("/commands/light");
String topicEvents = String("$devices/")+String(yandexIoTCoreDeviceId)+String("/events");

boolean needPublish = true;
bool lightIsOn = false;

 

WiFiClientSecure  net;
PubSubClient client(net);
BearSSL::X509List x509(registry_root_ca);

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
  while (!client.connect("Esp8266Client", yandexIoTCoreDeviceId, yandexIoTCoreDevicePassword)) {
    DEBUG_SERIAL.print(".");
    delay(1000);
  }
  DEBUG_SERIAL.println(" Connected");
  
  DEBUG_SERIAL.println("registry command topic:");
  DEBUG_SERIAL.println(topicRegistryCommands);

  boolean subcribed = client.subscribe(topicRegistryCommands.c_str());
  DEBUG_SERIAL.println(subcribed);
}

void setup() {

  setupDustSensor();

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

  timeClient.begin();
  dht.begin();

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

  if (topicRegistryCommandLight == topicString) {
    DEBUG_SERIAL.print("Handling light cmd");
    lightIsOn = payloadStr.equals("True");
    DEBUG_SERIAL.println(lightIsOn);
  
    digitalWrite(LED_BUILTIN, lightIsOn ? LOW : HIGH);    
  } else {
    DEBUG_SERIAL.println("Unknown command");
  }

}

void loop() {

  DEBUG_SERIAL.println("next cycle");
  
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // Turn the LED on by making the voltage LOW
  if (lightIsOn) {
    digitalWrite(LED_BUILTIN, LOW); 
  } else {
    digitalWrite(LED_BUILTIN, HIGH); 
  }

  if(needPublish) {


    
    if (lightIsOn) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(10);
      digitalWrite(LED_BUILTIN, LOW);
    }
    

    bool dht11Read = true;
    float t11;
    float h11;
       
    // Read temperature as Celsius (the default)
    t11 = dht.readTemperature();
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h11 = dht.readHumidity();

    
    // Check if any reads failed and exit early (to try again).

    if (isnan(h11) || isnan(t11)) {
      Serial.println("Failed to read from DHT12 sensor!");
      dht11Read = false;
    }


    timeClient.update();

    String formattedTime = getFormattedDate(timeClient.getEpochTime());
    
    float dustValue = measureDustSensorValue();
    
    if (!dht11Read) {
      snprintf(msg, sizeof(msg), msgTemplateWithoutTemp,
        formattedTime.c_str(),
        (int)dustValue, (int)(dustValue*100)%100
      );
    } else {
      snprintf(msg, sizeof(msg), msgTemplateDefault,
        formattedTime.c_str(),
        (int)dustValue, (int)(dustValue*100)%100,
        (int)t11, (int)(t11*100)%100,
        (int)h11, (int)(h11*100)%100
      );      
    }
    
    if (client.publish(topicEvents.c_str(), msg)) {        
        DEBUG_SERIAL.println(topicEvents);
        DEBUG_SERIAL.println("Publish ok");
    } else {
      DEBUG_SERIAL.println("Publish failed");
    }
  }



  DEBUG_SERIAL.println(msg);
      


  DEBUG_SERIAL.print("sleep for ");
  DEBUG_SERIAL.println(PUBLISH_TIMEOUT);
  delay(PUBLISH_TIMEOUT);
}
