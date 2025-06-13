#include <unity.h>
#include "test_native_pwm.h"

#define VARIABLES 4

volatile float*     ptrSetPoint[VARIABLES];
float testPwmPeriod;                      // Period (us)   
float testPwmPulseWidth;                // Pulse width (%)
int testPwmPulseWidth_us;               // Pulse width (us)
PWM* pwm;

void setUp(void) {
    testPwmPeriod = 200; // uS
    testPwmPulseWidth = 50; // %

    // attempt to simulate as much as possible how this data would come in via LinuxCNC packet. Not necessarily with the same indexes though
    ptrSetPoint[0] = &testPwmPeriod;   
    ptrSetPoint[1] = &testPwmPulseWidth;

    pwm = new PWM(*ptrSetPoint[0], *ptrSetPoint[1], "PA_1");
}

void tearDown(void) {
    delete pwm;
    pwm = nullptr;
}

void test_PWM_constructor_initializes_period_and_pulsewidth_pointers(void) 
{
    // Tests that the pointers initialise the values to our test variables. Don't run the update yet, check that this works after class constructor. 
    TEST_ASSERT_EQUAL_FLOAT(testPwmPeriod, pwm->getPwmPeriod());
    TEST_ASSERT_EQUAL_FLOAT(testPwmPulseWidth, pwm->getPwmPulseWidth()); // pulsewidth % should stay the same
}

void test_PWM_update_changes_period_and_pulsewidth_us(void) {
    // Change period to new value, being that the class contains pointers to these local variables, it should update automatically. 
    testPwmPeriod = 600;
    pwm->update();

    TEST_ASSERT_EQUAL_FLOAT(testPwmPeriod, pwm->getPwmPeriod());
    TEST_ASSERT_EQUAL_FLOAT(testPwmPulseWidth, pwm->getPwmPulseWidth()); // pulse width % should be the same
    TEST_ASSERT_NOT_EQUAL(testPwmPulseWidth_us, pwm->getPwmPulseWidth_us()); // but the pulsewidth_us should have recalculated.
}

void test_PWM_update_no_change_does_nothing(void) {
    // Set to same values as initialized
    pwm->update();

    // No changes, so hardwarePWM's lastPeriod and lastPulseWidth remain initial
    TEST_ASSERT_EQUAL_FLOAT(testPwmPeriod, pwm->getPwmPeriod());
    TEST_ASSERT_EQUAL_FLOAT(testPwmPulseWidth, pwm->getPwmPulseWidth()); 
}
    
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_PWM_constructor_initializes_period_and_pulsewidth_pointers);
    RUN_TEST(test_PWM_update_changes_period_and_pulsewidth_us);
    RUN_TEST(test_PWM_update_no_change_does_nothing);
    // Note - testing of hardware PWM will need to be done on embedded target 
    return UNITY_END();
}








