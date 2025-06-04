#include "hardwarePwm.h"

HardwarePWM::HardwarePWM(volatile float &ptrPwmPulseWidth, int pwmPeriod, std::string pin)
    : HardwarePWM(*(volatile float *)nullptr, ptrPwmPulseWidth, pwmPeriod, pin) // delegate to the main constructor
{
    variablePeriod = false;
    ptrPwmPeriod = nullptr; 
}

HardwarePWM::HardwarePWM(volatile float &ptrPwmPeriod, volatile float &ptrPwmPulseWidth, int pwmPeriod, std::string pin) :
    ptrPwmPeriod(&ptrPwmPeriod),
    ptrPwmPulseWidth(&ptrPwmPulseWidth),
    pwmPeriod(pwmPeriod),
	pin(pin)
{
    printf("Creating Hardware PWM at pin %s\n", this->pin);

    variablePeriod = true;

    if (pwmPeriod == 0)
    {
        this->pwmPeriod = DEFAULT_PWMPERIOD;
    }

    // Note that the original code base took advantage of an MBed Quirk that allowed for quick and easy checking to see if the pin
    // had PWM capabilities, ours doesn't but might be worth creating some sort of hardware specific lookup table for each supported board 

    this->set_period_us(this->pwmPeriod);
}

void HardwarePWM::update()
{
    if (variablePeriod)
    {
        if (*(this->ptrPwmPeriod) != 0 && (*(this->ptrPwmPeriod) != this->pwmPeriod))
        {
            // PWM period has changed
            this->pwmPeriod = *(this->ptrPwmPeriod);
            this->set_period_us(this->pwmPeriod);
            this->pwmPulseWidth_us = (this->pwmPeriod * this->pwmPulseWidth) / 100.0;
            this->set_pulsewidth_us(this->pwmPulseWidth_us);
        }
    }

    if (*(this->ptrPwmPulseWidth) != this->pwmPulseWidth)
    {
        // PWM duty has changed
        this->pwmPulseWidth = *(this->ptrPwmPulseWidth);
        this->pwmPulseWidth_us = (this->pwmPeriod * this->pwmPulseWidth) / 100.0;
        this->set_pulsewidth_us(this->pwmPulseWidth_us);
    } 
}

void HardwarePWM::setPwmSP(int newPwmSP)
{
	this->pwmSP = newPwmSP; 

    // todo - change the pulse width % and recalculate the pulse width in us if needed. currently this does nothing. 
}

void HardwarePWM::set_period_us(int pwmPeriod) 
{

}

void HardwarePWM::set_pulsewidth_us(int pwmPulseWidth_us) 
{

}
