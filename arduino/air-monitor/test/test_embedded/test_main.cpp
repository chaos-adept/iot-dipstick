#include <Arduino.h>
#include <unity.h>

#include <soil/soil_sensor.h>

SoilSensor soilSensor(D1, D2);

void setUp(void) {
// set stuff up here
    
}

void tearDown(void) {
// clean stuff up here
    soilSensor.onDataClean();
}

void test_led_builtin_pin_number(void) {
    #if defined (ESP8266) || defined(ESP32)
     TEST_ASSERT_EQUAL(2, LED_BUILTIN);
    #else
     TEST_ASSERT_EQUAL(13, LED_BUILTIN);
    #endif
    
}

void test_initialization(void) {
    
    soilSensor.begin();

    TEST_ASSERT_EQUAL_INT(true, soilSensor.isAlive());

    
}

void test_measuring(void) {
    soilSensor.onLoopCycle();
}


void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
    RUN_TEST(test_initialization);

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    RUN_TEST(test_measuring);
    UNITY_END(); // stop unit testing
}