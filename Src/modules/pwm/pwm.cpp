#include "pwm.h"

#define PID_PWM_MAX 256		// 8 bit resolution

/***********************************************************************
                MODULE CONFIGURATION AND CREATION FROM JSON     
************************************************************************/

void createPWM(void)
{
    PWM* new_pwm;
    volatile float*     variable_pointers[VARIABLES];

    int sp = module["SP[i]"];  // when reading this value off the rx buffer, it will return the duty cycle.  
    int pwmMax = module["PWM Max"];
    const char* pin = module["PWM Pin"];
    const char* hardware = module["Hardware PWM"];
    //const char* variable = module["Variable Freq"]; // by default all PWMs are variable.
    int period_sp = module["Period SP[i]"];
    int period_us = module["Period us"];
    //const char* comment = module["Comment"];

    //printf("\n%s\n",comment);
    printf("Creating PWM at pin %s\n", pin);
   
    // Create pointers for set point variables
    variable_pointers[sp] = &rxPingPongBuffer.rxBuffers->setPoint[sp];  // sp value will store the duty cycle.  
    variable_pointers[period_sp] = &rxPingPongBuffer.rxBuffers->setPoint[period_sp]; // todo - if this isn't enabled what does it do, see if we can check for errors. 
    
    if (!strcmp(hardware, "False")) // Software PWM
    {
        printf("Software PWM not yet supported\n");
    }

    new_pwm = new PWM(*variable_pointers[period_sp], *variable_pointers[sp], period_us, pin);   
    new_pwm->setPwmMax(pwmMax);
    servoThread->registerModule(new_pwm);
}

/***********************************************************************
                METHOD DEFINITIONS
************************************************************************/

PWM::PWM(volatile float &ptrPwmPeriod, volatile float &ptrPwmPulseWidth, int pwmPeriod, std::string pin) :
    ptrPwmPeriod(&ptrPwmPeriod),
    ptrPwmPulseWidth(&ptrPwmPulseWidth),
    pin(pin)
{
    printf("Creating variable frequency Hardware PWM at pin %s\n", this->pin.c_str());

    // set initial period and pulse width
    this->pwmPeriod_us = *(this->ptrPwmPeriod);
    //this->pwmPeriod_us = pwmPeriod;

    if (this->pwmPeriod_us == 0)
    {
        this->pwmPeriod_us = DEFAULT_PWM_PERIOD;
    }

    this->pwmPulseWidth = *(this->ptrPwmPulseWidth);
    this->pwmPulseWidth_us = (this->pwmPeriod_us * this->pwmPulseWidth) / 100.0;

    //this->pwmPulseWidth_us = (this->pwmPeriod_us * this->pwmPulseWidth) / 100.0;
    hardware_PWM = new HardwarePWM(this->pwmPeriod_us, this->pwmPulseWidth_us, this->pin); 
}


float PWM::getPwmPeriod(void) { return pwmPeriod_us; }
float PWM::getPwmPulseWidth(void) { return pwmPulseWidth; }
int PWM::getPwmPulseWidth_us(void) { return pwmPulseWidth_us; }
void PWM::setPwmMax(int pwmMax) { this->pwmMax = pwmMax; }

void PWM::update()
{
    if (*(this->ptrPwmPeriod) != 0 && (*(this->ptrPwmPeriod) != this->pwmPeriod_us))
    {
        //if (*(this->ptrPwmPeriod) < this->pwmMax)  // todo - calculate this from 0-256
        //{
            // PWM period has changed
            this->pwmPeriod_us = *(this->ptrPwmPeriod);
            this->hardware_PWM->change_period(this->pwmPeriod_us);

            // force pulse width update by triggering the next if block.
            this->pwmPulseWidth = 0;
        //}
    }

    if (*(this->ptrPwmPulseWidth) != this->pwmPulseWidth)
    {
        // PWM duty has changed

        this->pwmPulseWidth = *(this->ptrPwmPulseWidth);

        // clamp the percentage
        if (this->pwmPulseWidth < 0) 
        {
            this->pwmPulseWidth = 0;
        }
        if (this->pwmPulseWidth > 100) 
        {
            this->pwmPulseWidth = 100;
        }

        this->pwmPulseWidth_us = (this->pwmPeriod_us * this->pwmPulseWidth) / 100.0;
        this->hardware_PWM->change_pulsewidth(this->pwmPulseWidth_us);
    } 
}

void PWM::slowUpdate()
{
	return;
}