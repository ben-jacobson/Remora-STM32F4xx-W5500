#include <unity.h>
#include "test_native_pwm.h"
#include "modules/pwm/pwm.h"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_equality(void) {
  TEST_ASSERT_EQUAL(4, 2 + 2);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_equality);
    return UNITY_END();
}