#include <Arduino.h>
#include <unity.h>

#include <soil/soil_sensor.h>


#define EXPECTED_METRIC_COUNT 4

SoilSensor soilSensor(D1, D2, EXPECTED_METRIC_COUNT, D8);

void setUp(void) {
// set stuff up here
    
}

void tearDown(void) {
// clean stuff up here
    //soilSensor.onDataClean();
}


void test_led_builtin_pin_number(void) {
    #if defined (ESP8266) || defined(ESP32)
     TEST_ASSERT_EQUAL(2, LED_BUILTIN);
    #else
     TEST_ASSERT_EQUAL(13, LED_BUILTIN);
    #endif
    
}

void test_initialization(void) {
    TEST_ASSERT_EQUAL_INT(true, soilSensor.isAlive());
}

void test_metric_count(void) {
    TEST_ASSERT_EQUAL_INT(EXPECTED_METRIC_COUNT, soilSensor.getMetricsCount());
}

void test_metric_values(void) {
    MetricResult* metricResults = soilSensor.getMetrics();

    // note, it is unordered because for some reasons indexes are wrong under non-test env
    TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(5, metricResults[0].valueAsJsonPropVal.toFloat(), (String("val 1: ") + metricResults[0].valueAsJsonPropVal).c_str());
    TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(5, metricResults[1].valueAsJsonPropVal.toFloat(), (String("val 2: ")  + metricResults[1].valueAsJsonPropVal).c_str());
    TEST_ASSERT_EQUAL_MESSAGE(100, metricResults[2].valueAsJsonPropVal.toInt(), "val 3");
    TEST_ASSERT_EQUAL_MESSAGE(100, metricResults[3].valueAsJsonPropVal.toInt(), "val 4");
}

void test_metric_typeNames(void) {
    MetricResult* metricResults = soilSensor.getMetrics();

    for (int i = 0; i < EXPECTED_METRIC_COUNT; i++) {
        String expected = String("Soil_Humidity_") + String(i);
        TEST_ASSERT_EQUAL_STRING(expected.c_str(), metricResults[i].kind.c_str());
    }
}

void test_metric_typeKind(void) {
    MetricResult* metricResults = soilSensor.getMetrics();

    for (int i = 0; i < EXPECTED_METRIC_COUNT; i++) {
        TEST_ASSERT_EQUAL_STRING("Float", metricResults[i].valueTypeName.c_str());
    }
}


void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    soilSensor.begin();
    soilSensor.wakeup();
    
    pinMode(LED_BUILTIN, OUTPUT);

    RUN_TEST(test_led_builtin_pin_number);
    RUN_TEST(test_initialization);

}

void loop() {
    delay(500);

    
    soilSensor.onLoopCycle();

    RUN_TEST(test_metric_count);
    RUN_TEST(test_metric_values);
    RUN_TEST(test_metric_typeKind);
    RUN_TEST(test_metric_typeNames);

    //soilSensor.sleep();

    UNITY_END(); // stop unit testing
}