#include "pwm.h"
#include "drivers/HardwarePWM/HardwarePwm.h"

#define PID_PWM_MAX 256		// 8 bit resolution

/***********************************************************************
                MODULE CONFIGURATION AND CREATION FROM JSON     
************************************************************************/

void createPWM()
{
    const char* comment = module["Comment"];
    printf("\n%s\n",comment);

    int sp = module["SP[i]"];
    int pwmMax = module["PWM Max"];
    const char* pin = module["PWM Pin"];

    const char* hardware = module["Hardware PWM"];
    const char* variable = module["Variable Freq"];
    int period_sp = module["Period SP[i]"];
    int period = module["Period us"];

    printf("Creating PWM at pin %s\n", pin);
    
    ptrSetPoint[sp] = &rxData.setPoint[sp];

    if (!strcmp(hardware,"True"))
    {
        // Hardware PWM
        if (!strcmp(variable,"True"))
        {
            // Variable frequency hardware PWM
            ptrSetPoint[period_sp] = &rxData.setPoint[period_sp];

            HardwarePWM* pwm = new HardwarePWM(*ptrSetPoint[period_sp], *ptrSetPoint[sp], period, pin);
        }
        else
        {
            // Fixed frequency hardware PWM
            HardwarePWM* pwm = new HardwarePWM(*ptrSetPoint[sp], period, pin);
        }
    }
    else
    {
        printf("Software PWM not yet supported\n");
    }
}


/***********************************************************************
                METHOD DEFINITIONS
************************************************************************/

PWM::PWM(volatile float &ptrSP, std::string portAndPin) :
	ptrSP(&ptrSP),
	portAndPin(portAndPin)
{
    // nothing to do pointers are set, variables are handled by member initialiser
}

void PWM::update()
{
	float SP;

	// update the speed SP
	this->SP = *(this->ptrSP);

    // ensure SP is within range. LinuxCNC PID can have -ve command value
	if (this->SP > 100) this->SP = 100;
    if (this->SP < 0) this->SP = 0;

	// the SP is as a percentage (%)
	// scale the pwm output range (0 - pwmMax) = (0 - 100%)
    // and reset the set point

	SP = this->pwmMax * (this->SP / 100.0);

	this->pwm->setPwmSP(int(SP));

	this->pwm->update();
}

void PWM::slowUpdate()
{
	return;
}