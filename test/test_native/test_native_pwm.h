#ifndef TEST_PWM_H
#define TEST_PWM_H

#include <string>

// Stub class for HardwarePWM
class HardwarePWM
{
	public:
        HardwarePWM(int, int, std::string) {}
		~HardwarePWM(void) {}			        
        void change_period(int) {}
        void change_pulsewidth(int) {}
};

/* 
This is a nasty consequence of having native tests run along side embedded tests, PIO claims to be able to do this 
but the config is inelegant and fiddly. 

Given how extremely fiddly the dependancy tree becomes when trying to test natively, I think this might be the last native test. 
We can test everything else using the actual hardware from here on.
*/
#include "modules/module.h"
#include "modules/module.cpp"

#include "modules/pwm/pwm.h"
#include "modules/pwm/pwm.cpp"

#endif