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
        printf("pwm_enabled_pins is empty, did you set the correct build target in your env?\n");
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
    this->change_period(initial_period_us);
    this->change_pulsewidth(initial_pulsewidth_us);
}

void HardwarePWM::initialise_timers(void) 
{
    uint32_t timer_clock;
    uint32_t base_freq = 1000000;       // according  to the code examples referenced this was for best resolution, but admittedly I'm struggling to get context on this. 
    uint32_t target_freq_hz = 1000;     // 1000hz for testing right now. to be piped in

    if (this->configured_pin->timer != NULL)
    {
        this->pwm_tim_handler.Instance = this->configured_pin->timer;
    }
    else 
    {
        printf("Error in PWM Timer channel selection. Please refer to documentation.\n");
    }

    if (this->pwm_tim_handler.Instance == TIM1 || 
        this->pwm_tim_handler.Instance == TIM8 || 
        this->pwm_tim_handler.Instance == TIM9 || 
        this->pwm_tim_handler.Instance == TIM10 || 
        this->pwm_tim_handler.Instance == TIM11)  // PWM won't work on anything above TIM4, for sake of completion
    {
        if ((RCC->CFGR & RCC_CFGR_PPRE2) != RCC_CFGR_PPRE2_DIV1)    // I will not remember how any of this timer code works after I move onto the next thing....
        {
            timer_clock = HAL_RCC_GetPCLK2Freq() * 2;
        } 
        else 
        {
            timer_clock = HAL_RCC_GetPCLK2Freq();
        }        
    }
    else    
    {
        if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1) 
        {
            timer_clock = HAL_RCC_GetPCLK1Freq() * 2;
        } 
        else 
        {
            timer_clock = HAL_RCC_GetPCLK1Freq();
        }  
    }

    this->pwm_tim_handler.Init.Prescaler = (timer_clock / base_freq) - 1;
    this->pwm_tim_handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    this->pwm_tim_handler.Init.Period = (base_freq / target_freq_hz) - 1; 
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
    sConfigOC.Pulse = this->pwm_tim_handler.Init.Period / 2;  // Defaults to 50% duty cycle as it's starting value.
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

    if(this->pwm_tim_handler.Instance == TIM1 ||
       this->pwm_tim_handler.Instance == TIM2 || 
       this->pwm_tim_handler.Instance == TIM3 || 
       this->pwm_tim_handler.Instance == TIM4)
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
    if(this->pwm_tim_handler.Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_DISABLE();
    }
    if(this->pwm_tim_handler.Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_DISABLE();
    }    
    if(this->pwm_tim_handler.Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_DISABLE();
    }        
    if(this->pwm_tim_handler.Instance == TIM4)
    {
        __HAL_RCC_TIM4_CLK_DISABLE();
    }           
}

void HardwarePWM::change_period(int new_period_us)
{

}

void HardwarePWM::change_pulsewidth(int new_pulsewidth_us)
{

}