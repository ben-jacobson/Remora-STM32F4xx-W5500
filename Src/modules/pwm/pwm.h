#ifndef PWM_H
#define PWM_H

#include <string>
#include <iostream>
#include "modules/module.h"

#ifndef NATIVE_UNITTEST
	#include "extern.h"
	#include "drivers/HardwarePwm/HardwarePwm.h"
#endif

#define DEFAULT_PWM_PERIOD 200

void createPWM(void);

class PWM : public Module
{
	private:

		std::string pin;			        // PWM output pin
		int pwmMax;					        // maximum PWM output
		int pwmSP;					        // PWM setpoint as a percentage of maxPwm

		HardwarePWM *hardware_PWM;

        volatile float *ptrPwmPeriod; 	    // pointer to the data source
        volatile float *ptrPwmPulseWidth; 	// pointer to the data source

        int pwmPeriod;                      // Period (us)
        float pwmPulseWidth;                // Pulse width (%)
        int pwmPulseWidth_us;               // Pulse width (us)

	public:
		PWM(volatile float&, volatile float&, std::string);
		virtual void update(void);          // Module default interface
		virtual void slowUpdate(void);      // Module default interface
};

#endif