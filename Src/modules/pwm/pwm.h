#ifndef PWM_H
#define PWM_H

#include <cstdint>
#include <string>

#include "modules/module.h"
#include "drivers/HardwarePwm/HardwarePwm.h"

#include "extern.h"

void createPWM(void);

class PWM : public Module
{

	private:

		volatile float* ptrSP; 			// pointer to the data source
		int 			SP;             // speed %
		std::string 	portAndPin;
		int 			pwmMax;

		HardwarePWM* 		pwm;			// pointer to PWM object - output


	public:

		PWM(volatile float&, std::string);
		virtual void update(void);
		virtual void slowUpdate(void);
};

#endif