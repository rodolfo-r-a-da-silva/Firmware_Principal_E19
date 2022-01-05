/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOC
#define OUT0_Pin GPIO_PIN_12
#define OUT0_GPIO_Port GPIOB
#define OUT1_Pin GPIO_PIN_13
#define OUT1_GPIO_Port GPIOB
#define EXTI0_Pin GPIO_PIN_14
#define EXTI0_GPIO_Port GPIOB
#define EXTI0_EXTI_IRQn EXTI15_10_IRQn
#define EXTI1_Pin GPIO_PIN_15
#define EXTI1_GPIO_Port GPIOB
#define EXTI1_EXTI_IRQn EXTI15_10_IRQn
#define SDIO_CD_Pin GPIO_PIN_7
#define SDIO_CD_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
