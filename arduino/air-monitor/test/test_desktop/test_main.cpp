#include <unity.h>


void setUp(void) {
// set stuff up here
    
}

void tearDown(void) {
// clean stuff up here
}

void test_initialization(void) {
    TEST_ASSERT_EQUAL_INT(true, true);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialization);
    UNITY_END();

    return 0;
}