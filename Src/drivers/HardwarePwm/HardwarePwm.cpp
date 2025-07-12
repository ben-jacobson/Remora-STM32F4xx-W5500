#include "HardwarePwm.h"

// Pin mappings for various build targets.

#ifdef NUCLEO_F446
PWM_Enabled_Pin pwm_enabled_pins[] = {
    { "PA_8",  GPIOA, GPIO_PIN_8,  TIM1, TIM_CHANNEL_1 },
    { "PA_9",  GPIOA, GPIO_PIN_9,  TIM1, TIM_CHANNEL_2 },
    { "PA_10", GPIOA, GPIO_PIN_10, TIM1, TIM_CHANNEL_3 },
    { "PA_11", GPIOA, GPIO_PIN_11, TIM1, TIM_CHANNEL_4 },
    { "PA_0",  GPIOA, GPIO_PIN_0,  TIM2, TIM_CHANNEL_1 },
    { "PA_1",  GPIOA, GPIO_PIN_1,  TIM2, TIM_CHANNEL_2 },
    { "PB_10", GPIOB, GPIO_PIN_10, TIM2, TIM_CHANNEL_3 },
    { "PB_2",  GPIOB, GPIO_PIN_2,  TIM2, TIM_CHANNEL_4 },
    { "PC_6",  GPIOC, GPIO_PIN_6,  TIM3, TIM_CHANNEL_1 },   
    { "PC_7",  GPIOC, GPIO_PIN_7,  TIM3, TIM_CHANNEL_2 },
    { "PB_0",  GPIOB, GPIO_PIN_0,  TIM3, TIM_CHANNEL_3 },
    { "PB_1",  GPIOB, GPIO_PIN_1,  TIM3, TIM_CHANNEL_4 } 
};
#else
PWM_Enabled_Pin pwm_enabled_pins[] = {};  // blank catchall.
#endif

// Todo - complete other pin mappings based on available Pins.

TIM_ClockConfigTypeDef sClockSourceConfig = {0};
TIM_MasterConfigTypeDef sMasterConfig = {0};
TIM_OC_InitTypeDef sConfigOC = {0};
TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

PWM_Enabled_Pin* find_compatible_pwm_pin(std::string pin)
{
    uint8_t pwm_enabled_pins_count = sizeof(pwm_enabled_pins) / sizeof(pwm_enabled_pins[0]); 

    if (pwm_enabled_pins_count == 0) {
        printf("pwm_enabled_pins is empty, have you set the correct build target in your env?\n");
        return nullptr;
    }

    for (uint8_t i = 0; i < pwm_enabled_pins_count; ++i) {
        if (pin == pwm_enabled_pins[i].pin_name) {
            return &pwm_enabled_pins[i];
        }
    }

    printf("Pin %s is not PWM enabled. Please set your config.txt to use the correct PWM enabled pin\n", pin.c_str());
    return nullptr; 
}

uint32_t get_timer_clk_freq(TIM_TypeDef* TIMx)
{
    uint32_t pclk, multiplier;

    if (TIMx == TIM1 || TIMx == TIM8 || TIMx == TIM9 || TIMx == TIM10 || TIMx == TIM11) 
    {       
        // On APB2
        pclk = HAL_RCC_GetPCLK2Freq();
        uint32_t ppre2 = ((RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos);
        multiplier = (ppre2 < 4) ? 1 : 2;
    }
    else    
    {
        // On APB1
        pclk = HAL_RCC_GetPCLK1Freq();
        uint32_t ppre1 = ((RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos);
        multiplier = (ppre1 < 4) ? 1 : 2;       
    }
    return pclk * multiplier;
}

HardwarePWM::HardwarePWM(int initial_period_us, int initial_pulsewidth_us, std::string pin) :
	pin(pin)
{
    this->configured_pin = find_compatible_pwm_pin(pin); 
    
    if (this->configured_pin != nullptr) {
        printf("Creating Hardware PWM at pin %s\n", this->pin.c_str());
    }

    // Initialise the timer, channel and pin.
    this->initialise_timers();
    this->initialise_pwm_channels();    
    this->initialise_pwm_pins();

    // set the initial period and pulsewidth
    //this->change_period(initial_period_us);
    //this->change_pulsewidth(initial_pulsewidth_us);

    // for testing
    this->change_period(100);
    this->change_pulsewidth(50);    

    printf("Timer clk frequency: %lu Hz\n", this->timer_clk_hz);
    printf("Prescaler: %lu\n", this->pwm_tim_handler.Init.Prescaler);

}

void HardwarePWM::initialise_timers(void) 
{
    if (this->configured_pin->timer != NULL)
    {
        this->pwm_tim_handler.Instance = this->configured_pin->timer; // this variable doesn't get used again, but at least good for error checking, and if we ever rely on it later. 
    }
    else 
    {
        printf("Error in PWM Timer channel selection. Please refer to documentation.\n");
    }

    this->timer_clk_hz = get_timer_clk_freq(this->configured_pin->timer); 
    this->pwm_tim_handler.Init.Prescaler = (this->timer_clk_hz / 1000000) - 1;
    this->pwm_tim_handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    this->pwm_tim_handler.Init.Period = 0; // start with 0us period, we'll initialise right after 
    this->pwm_tim_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    this->pwm_tim_handler.Init.RepetitionCounter = 0;
    this->pwm_tim_handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_Base_Init(&this->pwm_tim_handler) != HAL_OK)
    {
        Error_Handler(); // noted that Error_Handler does nothing for the time being, would be good if it could provide some useful error messaging.
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    
    if (HAL_TIM_ConfigClockSource(&this->pwm_tim_handler, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    
    if (HAL_TIM_PWM_Init(&this->pwm_tim_handler) != HAL_OK)
    {
        Error_Handler();
    }
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    
    if (HAL_TIMEx_MasterConfigSynchronization(&this->pwm_tim_handler, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void HardwarePWM::initialise_pwm_channels(void) 
{
    sConfigOC.OCMode = TIM_OCMODE_PWM1; // compare mode, output is active high. Probably doesn't make much difference. 
    sConfigOC.Pulse = this->pwm_tim_handler.Init.Period * 0.5;  // defaults to zero on init. TODO CHANGE BACK TO ZERO 
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    
    if (HAL_TIM_PWM_ConfigChannel(&this->pwm_tim_handler, &sConfigOC, this->configured_pin->channel) != HAL_OK)
    {
        Error_Handler();
    }

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;  // We may need to adjust this for safety when using PWM to drive higher current loads. Obvious use case is spindle PWM, but if ever used for anything else may need to be revisited
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    
    if (HAL_TIMEx_ConfigBreakDeadTime(&this->pwm_tim_handler, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void HardwarePWM::initialise_pwm_pins(void) 
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(this->configured_pin->timer == TIM1 ||
       this->configured_pin->timer == TIM2 || 
       this->configured_pin->timer == TIM3 || 
       this->configured_pin->timer == TIM4)
    {
        if (this->configured_pin->gpio_port == GPIOA)  // aint no DRY way to do this I'm afraid.. Hope that STM32 doesn't go beyond PortG!
        {
            __HAL_RCC_GPIOA_CLK_ENABLE();
        }
        else if (this->configured_pin->gpio_port == GPIOB) 
        {
            __HAL_RCC_GPIOB_CLK_ENABLE();
        }
        else if (this->configured_pin->gpio_port == GPIOC) 
        {
            __HAL_RCC_GPIOC_CLK_ENABLE();
        }
        else if (this->configured_pin->gpio_port == GPIOD) 
        {
            __HAL_RCC_GPIOD_CLK_ENABLE();
        }
        else if (this->configured_pin->gpio_port == GPIOE) 
        {
            __HAL_RCC_GPIOE_CLK_ENABLE();
        }
        else if (this->configured_pin->gpio_port == GPIOF) 
        {
            __HAL_RCC_GPIOF_CLK_ENABLE();
        }
        else if (this->configured_pin->gpio_port == GPIOG) 
        {
            __HAL_RCC_GPIOG_CLK_ENABLE();
        }
        else 
        {
            printf("Invalid GPIO port selection, please refer to documentation\n");
        }        

        GPIO_InitStruct.Pin = this->configured_pin->gpio_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    // may indicate a max of 2Mhz, to investigate. May not be a problem since our PWM probably won't need to go that high. 

        // there probably is a DRY way to do this, it is what it is... 
        if (this->configured_pin->timer == TIM1) 
        {
            GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        }
        else if (this->configured_pin->timer == TIM2) 
        {
            GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
        }
        else if (this->configured_pin->timer == TIM3) 
        {
            GPIO_InitStruct.Alternate = GPIO_AF2_TIM3; 
        }
        else if (this->configured_pin->timer == TIM4) 
        {
            GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
        }
        else 
        {
            printf("Invalid Timer selected for GPIO alternate function, selecting a default but it may not work as expected\n");
            GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        }

        HAL_GPIO_Init(this->configured_pin->gpio_port, &GPIO_InitStruct); 
    }

    if (this->configured_pin->timer == TIM1) 
    {
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
    else if (this->configured_pin->timer == TIM2) 
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
    else if (this->configured_pin->timer == TIM3) 
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
    else if (this->configured_pin->timer == TIM4) 
    {
        __HAL_RCC_TIM4_CLK_ENABLE();
    }
    else {
        printf("incorrect timer selected, please refer to documentation and select TIM1-TIM4.\n");
    }

    if (HAL_TIM_PWM_Start(&this->pwm_tim_handler, this->configured_pin->channel) != HAL_OK) 
    {
        Error_Handler();
    }       
}

HardwarePWM::~HardwarePWM(void) 
{
    if(this->configured_pin->timer == TIM1)
    {
        __HAL_RCC_TIM1_CLK_DISABLE();
    }
    if(this->configured_pin->timer == TIM2)
    {
        __HAL_RCC_TIM2_CLK_DISABLE();
    }    
    if(this->configured_pin->timer == TIM3)
    {
        __HAL_RCC_TIM3_CLK_DISABLE();
    }        
    if(this->configured_pin->timer == TIM4)
    {
        __HAL_RCC_TIM4_CLK_DISABLE();
    }           
}

void HardwarePWM::change_period(int new_period_us)
{
    uint32_t timer_freq_after_prescaler = this->timer_clk_hz / (this->pwm_tim_handler.Init.Prescaler + 1);
    uint32_t period_ticks = (timer_freq_after_prescaler * new_period_us) / 1000000;

    if (period_ticks < 1) 
    {
        period_ticks = 1;
    }

    this->pwm_tim_handler.Init.Period = period_ticks - 1;

    // attempt to restart PWM with minimal interruption, inclduing recalculation of pulse width
    __HAL_TIM_DISABLE(&this->pwm_tim_handler);
    __HAL_TIM_SET_AUTORELOAD(&this->pwm_tim_handler, period_ticks - 1);

    // Ensure pulse width is not out of bounds of new period value
    if (__HAL_TIM_GET_COMPARE(&this->pwm_tim_handler, this->configured_pin->channel) > (period_ticks - 1)) {
        __HAL_TIM_SET_COMPARE(&this->pwm_tim_handler, this->configured_pin->channel, period_ticks - 1);
    }

    // re-enable
    __HAL_TIM_ENABLE(&this->pwm_tim_handler);   
}

void HardwarePWM::change_pulsewidth(int new_pulsewidth_us)
{
    uint32_t timer_freq_after_prescaler = this->timer_clk_hz / (this->pwm_tim_handler.Init.Prescaler + 1);
    uint32_t pulse_ticks = (timer_freq_after_prescaler * new_pulsewidth_us) / 1000000;    

    // clamp it
    if (pulse_ticks < 1) 
    {
        pulse_ticks = 0;
    }
    if (pulse_ticks > this->pwm_tim_handler.Init.Period)    
    {
        pulse_ticks = this->pwm_tim_handler.Init.Period;
    }

    __HAL_TIM_SET_COMPARE(&this->pwm_tim_handler, this->configured_pin->channel, pulse_ticks);
}