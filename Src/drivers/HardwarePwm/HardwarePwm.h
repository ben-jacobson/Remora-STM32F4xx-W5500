#ifndef HARDWAREPWM_H
#define HARDWAREPWM_H

#include <string>
#include <iostream>
#include "main_init.h"

#define DEFAULT_PWMPERIOD 200

/* Pin, timer and channel definitions - see documentation for tables*/

struct PWM_Enabled_Pin {
    std::string pin_name;         
    GPIO_TypeDef* gpio_port;
    uint16_t gpio_pin;
    TIM_TypeDef* timer;
    uint32_t channel; 
};

extern PWM_Enabled_Pin pwm_enabled_pins[];

PWM_Enabled_Pin* find_compatible_pwm_pin(std::string); 

class HardwarePWM
{
	private:

		std::string pin;			        // PWM output pin
        PWM_Enabled_Pin *configured_pin;
        TIM_HandleTypeDef pwm_tim_handler;

        void initialise_timers(void);
        void initialise_pwm_channels(void);
        void initialise_pwm_pins(void);

	public:
        HardwarePWM(int, int, std::string);
		~HardwarePWM(void);			        
        void change_period(int);
        void change_pulsewidth(int);
};

#endif