#include <unity.h>
#include "test_pwm.h"

void setUp(void) {
    testPwmPeriod = 200; // uS
    testPwmPulseWidth = 50; // %

    // attempt to simulate as much as possible how this data would come in via LinuxCNC packet. Not necessarily with the same indexes though
    ptrSetPoint[0] = &testPwmPeriod;   
    ptrSetPoint[1] = &testPwmPulseWidth;

    abstract_pwm = new PWM(*ptrSetPoint[0], *ptrSetPoint[1], test_pwm_pin);
    hardware_pwm = new HardwarePWM(testPwmPeriod, testPwmPulseWidth, test_pwm_pin);
}

void tearDown(void) {
    delete abstract_pwm;
    abstract_pwm = nullptr;

    delete hardware_pwm;
    hardware_pwm = nullptr;    
}

void test_PWM_constructor_initializes_period_and_pulsewidth_pointers(void) 
{
    // Tests that the pointers initialise the values to our test variables. Don't run the update yet, check that this works after class constructor. 
    TEST_ASSERT_EQUAL_FLOAT(testPwmPeriod, abstract_pwm->getPwmPeriod());
    TEST_ASSERT_EQUAL_FLOAT(testPwmPulseWidth, abstract_pwm->getPwmPulseWidth()); // pulsewidth % should stay the same
}

void test_PWM_update_changes_period_and_pulsewidth_us(void) {
    // Change period to new value, being that the class contains pointers to these local variables, it should update automatically. 
    testPwmPeriod = 600;
    abstract_pwm->update();

    TEST_ASSERT_EQUAL_FLOAT(testPwmPeriod, abstract_pwm->getPwmPeriod());
    TEST_ASSERT_EQUAL_FLOAT(testPwmPulseWidth, abstract_pwm->getPwmPulseWidth()); // pulse width % should be the same
    TEST_ASSERT_NOT_EQUAL(testPwmPulseWidth_us, abstract_pwm->getPwmPulseWidth_us()); // but the pulsewidth_us should have recalculated.
}

void test_PWM_update_no_change_does_nothing(void) 
{
    // Set to same values as initialized
    abstract_pwm->update();

    // No changes, so hardwarePWM's lastPeriod and lastPulseWidth remain initial
    TEST_ASSERT_EQUAL_FLOAT(testPwmPeriod, abstract_pwm->getPwmPeriod());
    TEST_ASSERT_EQUAL_FLOAT(testPwmPulseWidth, abstract_pwm->getPwmPulseWidth()); 
}

void test_PWM_constructor_sets_DEFAULT_PWM_PERIOD_when_zero(void) 
{
    volatile float zeroPeriod = 0.0f;
    volatile float pulseWidth = 30.0f;

    PWM* pwmZero = new PWM(zeroPeriod, pulseWidth, test_pwm_pin);
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_PWM_PERIOD, pwmZero->getPwmPeriod());

    delete pwmZero;    
}

void test_find_compatible_pwm_pin_returns_nullptr_for_unknown_pin(void) {
    const PWM_Enabled_Pin* result = find_compatible_pwm_pin("INVALID_PIN");
    TEST_ASSERT_NULL(result);
}

void test_find_compatible_pwm_pin_returns_valid_for_known_pin(void) {
    if (sizeof(pwm_enabled_pins) / sizeof(pwm_enabled_pins[0]) > 0) {
        const char* valid_pin = pwm_enabled_pins[0].pin_name.c_str();
        const PWM_Enabled_Pin* result = find_compatible_pwm_pin(std::string(valid_pin));
        TEST_ASSERT_NOT_NULL(result);
        TEST_ASSERT_EQUAL_STRING(valid_pin, result->pin_name.c_str());
    } else {
        TEST_IGNORE_MESSAGE("No pwm_enabled_pins defined for testing");
    }
}

void test_find_compatible_pwm_pin_returns_error_for_unknown_pin(void) {
    const char* invalidPin = "GARBAGE_PIN_123";

    const PWM_Enabled_Pin* result = find_compatible_pwm_pin(invalidPin);
    TEST_ASSERT_NULL(result);  // Should return nullptr for invalid pin
}

void test_change_period_and_pulsewidth_methods(void) {
    TEST_ASSERT_NOT_NULL(hardware_pwm);
    
    // These should not crash or trigger Error_Handler
    hardware_pwm->change_period(1500);
    hardware_pwm->change_pulsewidth(750);

    // Optionally add state assertions if you expose internal values later
    // e.g., TEST_ASSERT_EQUAL(expected_value, hardware_pwm->getPeriod());
}

void test_HardwarePWM_destructor_does_not_crash(void) {
    // Allocate and delete within the test to isolate destructor call
    HardwarePWM* temp_pwm = new HardwarePWM(1000, 500, test_pwm_pin);
    TEST_ASSERT_NOT_NULL(temp_pwm);
    delete temp_pwm;

    // If Error_Handler or other faults trigger, this will fail or crash test
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_PWM_constructor_initializes_period_and_pulsewidth_pointers);
    RUN_TEST(test_PWM_update_changes_period_and_pulsewidth_us);
    RUN_TEST(test_PWM_update_no_change_does_nothing);
    RUN_TEST(test_PWM_constructor_sets_DEFAULT_PWM_PERIOD_when_zero);
    RUN_TEST(test_find_compatible_pwm_pin_returns_nullptr_for_unknown_pin);
    RUN_TEST(test_find_compatible_pwm_pin_returns_valid_for_known_pin);
    RUN_TEST(test_change_period_and_pulsewidth_methods);
    RUN_TEST(test_HardwarePWM_destructor_does_not_crash);    
    return UNITY_END();
}