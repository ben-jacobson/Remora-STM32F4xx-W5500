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

#endif