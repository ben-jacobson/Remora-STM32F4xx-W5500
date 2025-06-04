#ifndef HARDWAREPWM_H
#define HARDWAREPWM_H

#include <string>
#include <iostream>

#define DEFAULT_PWMPERIOD 200

class HardwarePWM
{
	private:

		std::string pin;			        // PWM output pin
		int pwmMax;					        // maximum PWM output
		int pwmSP;					        // PWM setpoint as a percentage of maxPwm                 

        volatile float *ptrPwmPeriod; 	    // pointer to the data source
        volatile float *ptrPwmPulseWidth; 	// pointer to the data source

        int pwmPeriod;                      // Period (us)
        float pwmPulseWidth;                // Pulse width (%)
        int pwmPulseWidth_us;               // Pulse width (us)

        bool variablePeriod;

	public:

		HardwarePWM(volatile float&, int, std::string);			        
        HardwarePWM(volatile float&, volatile float&, int, std::string);	
        void setPwmSP(int);
        void set_period_us(int);
        void set_pulsewidth_us(int);
        void update(void);          
};

#endif