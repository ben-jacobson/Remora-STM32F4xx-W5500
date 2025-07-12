#ifndef PWM_H
#define PWM_H

#include <string>
#include <iostream>

#include "extern.h"
#include "modules/module.h"
#include "drivers/HardwarePwm/HardwarePwm.h"

#define DEFAULT_PWM_PERIOD 100 // 100us

void createPWM(void);

class PWM : public Module
{
	private:
		std::string pin;			        // PWM output pin
		int pwmMax;					        // maximum PWM output
		//int pwmSP;					        // PWM setpoint as a percentage of maxPwm

		HardwarePWM *hardware_PWM;

        volatile float *ptrPwmPeriod; 	    // pointer to the data source
		volatile float *ptrPwmPulseWidth; 	// pointer to the data source

        float pwmPeriod_us;                      // Period (us)
        float pwmPulseWidth;                // Pulse width (%)
        int pwmPulseWidth_us;               // Pulse width (us)

		bool variable_freq;

	public:
		PWM(volatile float&, volatile float&, bool, int, std::string);

		virtual void update(void);          // Module default interface
		virtual void slowUpdate(void);      // Module default interface

		void setPwmMax(int);
		float getPwmPeriod(void);			// getters, primarily for testing
		float getPwmPulseWidth(void);
		int getPwmPulseWidth_us(void);
};

#endif