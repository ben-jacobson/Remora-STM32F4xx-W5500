#include "pwm.h"

#define PID_PWM_MAX 256		// 8 bit resolution

/***********************************************************************
                MODULE CONFIGURATION AND CREATION FROM JSON     
************************************************************************/

#ifndef NATIVE_UNITTEST
void createPWM(void)
{
    const char* comment = module["Comment"];
    printf("\n%s\n",comment);

    int sp = module["SP[i]"];
    //int pwmMax = module["PWM Max"];
    const char* pin = module["PWM Pin"];
    const char* hardware = module["Hardware PWM"];
    //const char* variable = module["Variable Freq"]; // by default all PWMs are variable.
    int period_sp = module["Period SP[i]"];
    //int period = module["Period us"];

    printf("Creating PWM at pin %s\n", pin);
    
    // Create a pointer for set point
    //rxData_t* currentRxPacket = getCurrentRxBuffer(&rxPingPongBuffer);
    volatile float*     ptrSetPoint[VARIABLES];
    //ptrSetPoint[sp] = &currentRxPacket->setPoint[sp];
    ptrSetPoint[sp] = &rxPingPongBuffer.rxBuffers->setPoint[sp];

    if (!strcmp(hardware,"True")) // Hardware PWM
    {
        // We don't implement fixed duty PWM, simpler to initialise a variable one and don't change it.  
        //ptrSetPoint[period_sp] = &currentRxPacket->setPoint[period_sp]; //doesn't seem to be allocated correctly, the index of ptrSetPoint should only go to 4? This might go to 20?!
        ptrSetPoint[period_sp] = &rxPingPongBuffer.rxBuffers->setPoint[period_sp];   
        Module* new_pwm = new PWM(*ptrSetPoint[period_sp], *ptrSetPoint[sp], pin);
        servoThread->registerModule(new_pwm);
    }
    else
    {
        printf("Software PWM not yet supported\n");
    }
}
#endif


/***********************************************************************
                METHOD DEFINITIONS
************************************************************************/

PWM::PWM(volatile float &ptrPwmPeriod, volatile float &ptrPwmPulseWidth, std::string pin) :
    ptrPwmPeriod(&ptrPwmPeriod),
    ptrPwmPulseWidth(&ptrPwmPulseWidth),
    pin(pin)
{
    printf("Creating variable frequency Hardware PWM at pin %s\n", this->pin.c_str());

    if (pwmPeriod_us == 0)
    {
        this->pwmPeriod_us = DEFAULT_PWM_PERIOD;
    }

    // set initial period and pulse width
    this->pwmPeriod_us = *(this->ptrPwmPeriod);
    this->pwmPulseWidth = *(this->ptrPwmPulseWidth);
    this->pwmPulseWidth_us = (this->pwmPeriod_us * this->pwmPulseWidth) / 100.0;
    hardware_PWM = new HardwarePWM(this->pwmPeriod_us, this->pwmPulseWidth_us, this->pin); 
}


void PWM::update()
{
    if (*(this->ptrPwmPeriod) != 0 && (*(this->ptrPwmPeriod) != this->pwmPeriod_us))
    {
        // PWM period has changed
        this->pwmPeriod_us = *(this->ptrPwmPeriod);
        //this->pwmPulseWidth_us = (this->pwmPeriod * this->pwmPulseWidth) / 100.0; // safer to force an update below.
        this->hardware_PWM->change_period(this->pwmPeriod_us);

        // force pulse width update by triggering the next if block.
        this->pwmPulseWidth = 0;
    }

    if (*(this->ptrPwmPulseWidth) != this->pwmPulseWidth)
    {
        // PWM duty has changed
        this->pwmPulseWidth = *(this->ptrPwmPulseWidth);
        this->pwmPulseWidth_us = (this->pwmPeriod_us * this->pwmPulseWidth) / 100.0;
        this->hardware_PWM->change_pulsewidth(this->pwmPulseWidth_us);
    } 
}

float PWM::getPwmPeriod(void) { return pwmPeriod_us; }
float PWM::getPwmPulseWidth(void) { return pwmPulseWidth; }
int PWM::getPwmPulseWidth_us(void) { return pwmPulseWidth_us; }

void PWM::slowUpdate()
{
	return;
}