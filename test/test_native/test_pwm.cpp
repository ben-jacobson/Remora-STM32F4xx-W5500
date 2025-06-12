#include <unity.h>
#include "modules/pwm/pwm.h"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_find_compatible_pwm_pin(void) {
  //find_compatible_pwm_pin()
  TEST_ASSERT_EQUAL(4, 2 + 2);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_find_compatible_pwm_pin);
    return UNITY_END();
}