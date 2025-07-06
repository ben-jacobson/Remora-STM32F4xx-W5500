#ifndef TEST_PWM_H
#define TEST_PWM_H

#include "configuration.h"
#include "modules/module.h"
#include "modules/pwm/pwm.h"

std::string test_pwm_pin = "PA_1";

volatile float*     ptrSetPoint[VARIABLES];
float testPwmPeriod;                      // Period (us)   
float testPwmPulseWidth;                // Pulse width (%)
int testPwmPulseWidth_us;               // Pulse width (us)

PWM* abstract_pwm;
HardwarePWM* hardware_pwm = nullptr;

#endif