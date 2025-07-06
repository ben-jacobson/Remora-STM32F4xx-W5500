#include "HardwarePwm.h"

// Pin mappings for various build targets.

#ifdef NUCLEO_F446
PWM_Enabled_Pin pwm_enabled_pins[] = {
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

    // Initialise the timers, channels and pin.
    this->initialise_timers();
    this->initialise_pwm_channels();    
    this->initialise_pwm_pins();

    // set the initial period and pulsewidth
    this->change_period(initial_period_us);
    this->change_pulsewidth(initial_pulsewidth_us);
}

void HardwarePWM::initialise_timers(void) 
{
    if (this->configured_pin->timer != NULL)
    {
        this->pwm_tim_handler.Instance = this->configured_pin->timer;
    }
    else 
    {
        printf("Error referencing PWM pin timer channel");
    }

    if (this->pwm_tim_handler.Instance == TIM1 || this->pwm_tim_handler.Instance == TIM8) {
        this->pwm_tim_handler.Init.Prescaler = (HAL_RCC_GetPCLK2Freq() / 1000000) * 2;    // todo - go back and check how the actual clock was set up and create new values based on that.
    }
    else
    {
        this->pwm_tim_handler.Init.Prescaler = (HAL_RCC_GetPCLK1Freq() / 1000000) * 2;    
    }
    
    this->pwm_tim_handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    this->pwm_tim_handler.Init.Period = 1000 - 1;   // test for 1000hz for now
    this->pwm_tim_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    this->pwm_tim_handler.Init.RepetitionCounter = 0;
    this->pwm_tim_handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_Base_Init(&this->pwm_tim_handler) != HAL_OK)
    {
        Error_Handler();
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
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;  // test 50% duty cycle for now.
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    
    if (HAL_TIM_PWM_ConfigChannel(&this->pwm_tim_handler, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&this->pwm_tim_handler, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&this->pwm_tim_handler, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&this->pwm_tim_handler, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
        Error_Handler();
    }
    
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
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
    if(this->pwm_tim_handler.Instance == TIM1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
//        GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11; // will this still work if we initialise individual pins on the same channel? Will this break 9,10 and 11 if configd as digital pins? 
        GPIO_InitStruct.Pin = GPIO_PIN_8; 
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    __HAL_RCC_TIM1_CLK_ENABLE();    

    if (HAL_TIM_PWM_Start(&this->pwm_tim_handler, TIM_CHANNEL_1) != HAL_OK) // start the PWM
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
}

void HardwarePWM::change_period(int new_period_us)
{

}

void HardwarePWM::change_pulsewidth(int new_pulsewidth_us)
{

}