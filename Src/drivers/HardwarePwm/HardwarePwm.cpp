#include "HardwarePwm.h"

TIM_ClockConfigTypeDef sClockSourceConfig = {0};
TIM_MasterConfigTypeDef sMasterConfig = {0};
TIM_OC_InitTypeDef sConfigOC = {0};
TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

const PWM_Enabled_Pin* find_compatible_pwm_pin(std::string pin)
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
    const PWM_Enabled_Pin *configured_pin = find_compatible_pwm_pin(pin); 

    if (configured_pin != nullptr) {
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
    this->pwm_tim_handler.Instance = TIM1;
    this->pwm_tim_handler.Init.Prescaler = 0;
    this->pwm_tim_handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    this->pwm_tim_handler.Init.Period = 65535;  // sets up with a default value for time being, will manaually set later
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
    sConfigOC.Pulse = 0;
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
    if(this->pwm_tim_handler.Instance==TIM1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**TIM1 GPIO Configuration
        PA8     ------> TIM1_CH1
        PA9     ------> TIM1_CH2
        PA10     ------> TIM1_CH3
        PA11     ------> TIM1_CH4
        */
        GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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