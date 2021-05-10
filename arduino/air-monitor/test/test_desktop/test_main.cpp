#include <Arduino.h>
#include <unity.h>
#include <ArduinoFake.h>

using namespace fakeit;

#include <water_controller.h>

int pins[] = {1, 2};
int waterAmount[] = {300, 200};

WaterController wc(2, pins, waterAmount);

void test_initialization_controller(void) {
    wc.begin();
    TEST_ASSERT_EQUAL_INT(2, wc.getPotsCount());
    Verify(Method(ArduinoFake(), digitalWrite).Using(1, LOW)).Once();
    Verify(Method(ArduinoFake(), digitalWrite).Using(2, LOW)).Once();
}

void test_gettingMetrics(void) {
    TEST_ASSERT_NOT_NULL(wc.getMetrics())
}

void test_watering(void) {
    When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
    When(Method(ArduinoFake(), delay)).AlwaysReturn();
    MetricResult humidity[] = { ("10", "10", "10"), ("100", "100", "100") };
    wc.performWateringIfNeeded(humidity);

    Verify(Method(ArduinoFake(), digitalWrite).Using(1, HIGH)).Once();
    Verify(Method(ArduinoFake(), digitalWrite).Using(1, LOW)).Once();

    Verify(Method(ArduinoFake(), digitalWrite).Using(2, HIGH)).Never();
    Verify(Method(ArduinoFake(), digitalWrite).Using(2, LOW)).Never();

    // Verify(Method(ArduinoFake(), delay).Using(100)).Exactly(2_Times);
}


void setUp(void) {
// set stuff up here
    ArduinoFakeReset();
    // When(Method(ArduinoFake(), String)).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
    When(Method(ArduinoFake(), delay)).AlwaysReturn();
    When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
}

void tearDown(void) {
// clean stuff up here
}

void test_initialization(void) {
    TEST_ASSERT_EQUAL_INT(true, true);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialization_controller);
    RUN_TEST(test_gettingMetrics);
    RUN_TEST(test_watering);
    UNITY_END();

    return 0;
}