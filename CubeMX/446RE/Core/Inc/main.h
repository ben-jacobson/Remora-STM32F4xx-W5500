/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Y_STEP_Pin GPIO_PIN_13
#define Y_STEP_GPIO_Port GPIOC
#define Y_DIR_Pin GPIO_PIN_14
#define Y_DIR_GPIO_Port GPIOC
#define Input_12_Pin GPIO_PIN_15
#define Input_12_GPIO_Port GPIOC
#define B_STEP_Pin GPIO_PIN_2
#define B_STEP_GPIO_Port GPIOC
#define B_DIR_Pin GPIO_PIN_3
#define B_DIR_GPIO_Port GPIOC
#define Output_0_Pin GPIO_PIN_0
#define Output_0_GPIO_Port GPIOA
#define Z_STEP_Pin GPIO_PIN_1
#define Z_STEP_GPIO_Port GPIOA
#define Z_DIR_Pin GPIO_PIN_4
#define Z_DIR_GPIO_Port GPIOA
#define Output_7_Pin GPIO_PIN_4
#define Output_7_GPIO_Port GPIOC
#define Output_5_Pin GPIO_PIN_0
#define Output_5_GPIO_Port GPIOB
#define Input_10_Pin GPIO_PIN_1
#define Input_10_GPIO_Port GPIOB
#define Input_8_Pin GPIO_PIN_2
#define Input_8_GPIO_Port GPIOB
#define Input_11_Pin GPIO_PIN_10
#define Input_11_GPIO_Port GPIOB
#define Input_5_Pin GPIO_PIN_12
#define Input_5_GPIO_Port GPIOB
#define A_STEP_Pin GPIO_PIN_13
#define A_STEP_GPIO_Port GPIOB
#define A_DIR_Pin GPIO_PIN_14
#define A_DIR_GPIO_Port GPIOB
#define Output_6_Pin GPIO_PIN_15
#define Output_6_GPIO_Port GPIOB
#define Input_2_Pin GPIO_PIN_6
#define Input_2_GPIO_Port GPIOC
#define Input_6_Pin GPIO_PIN_7
#define Input_6_GPIO_Port GPIOC
#define Input_1_Pin GPIO_PIN_8
#define Input_1_GPIO_Port GPIOC
#define Input_0_Pin GPIO_PIN_9
#define Input_0_GPIO_Port GPIOC
#define Input_9_Pin GPIO_PIN_8
#define Input_9_GPIO_Port GPIOA
#define Input_7_Pin GPIO_PIN_9
#define Input_7_GPIO_Port GPIOA
#define Output_3_Pin GPIO_PIN_10
#define Output_3_GPIO_Port GPIOA
#define Input_4_Pin GPIO_PIN_11
#define Input_4_GPIO_Port GPIOA
#define Input_3_Pin GPIO_PIN_12
#define Input_3_GPIO_Port GPIOA
#define Output_4_Pin GPIO_PIN_15
#define Output_4_GPIO_Port GPIOA
#define X_STEP_Pin GPIO_PIN_11
#define X_STEP_GPIO_Port GPIOC
#define X_DIR_Pin GPIO_PIN_12
#define X_DIR_GPIO_Port GPIOC
#define Input_13_Pin GPIO_PIN_2
#define Input_13_GPIO_Port GPIOD
#define C_STEP_Pin GPIO_PIN_3
#define C_STEP_GPIO_Port GPIOB
#define C_DIR_Pin GPIO_PIN_4
#define C_DIR_GPIO_Port GPIOB
#define WIZ_RST_Pin GPIO_PIN_5
#define WIZ_RST_GPIO_Port GPIOB
#define WIZ_CS_Pin GPIO_PIN_6
#define WIZ_CS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
