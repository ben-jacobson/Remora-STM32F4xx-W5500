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
};

#ifdef NUCLEO_F446
const PWM_Enabled_Pin pwm_enabled_pins[] = {
    { "PA_8",  GPIOA, GPIO_PIN_8,  TIM1 },  // CH1
    { "PA_9",  GPIOA, GPIO_PIN_9,  TIM1 },  // CH2
    { "PA_10", GPIOA, GPIO_PIN_10, TIM1 },  // CH3
    { "PA_11", GPIOA, GPIO_PIN_11, TIM1 },  // CH4
    { "PA_0",  GPIOA, GPIO_PIN_0,  TIM2 },  // CH1
    { "PA_1",  GPIOA, GPIO_PIN_1,  TIM2 },  // CH2
    { "PB_10", GPIOB, GPIO_PIN_10, TIM2 },  // CH3
    { "PB_2",  GPIOB, GPIO_PIN_2,  TIM2 },  // CH4
    { "PC_6",  GPIOC, GPIO_PIN_6,  TIM3 },  // CH1
    { "PC_7",  GPIOC, GPIO_PIN_7,  TIM3 },  // CH2
    { "PB_0",  GPIOB, GPIO_PIN_0,  TIM3 },  // CH3
    { "PB_1",  GPIOB, GPIO_PIN_1,  TIM3 }   // CH4
};
#else
const PWM_Enabled_Pin pwm_enabled_pins[] = {};  // blank catchall.
#endif

// Todo - complete other pin mappings based on available Pins.

const PWM_Enabled_Pin* find_compatible_pwm_pin(std::string); 

class HardwarePWM
{
	private:

		std::string pin;			        // PWM output pin
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