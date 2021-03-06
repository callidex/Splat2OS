/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define Si4463_SCK_Pin GPIO_PIN_2
#define Si4463_SCK_GPIO_Port GPIOE
#define Si4463_MISO_Pin GPIO_PIN_5
#define Si4463_MISO_GPIO_Port GPIOE
#define Si4463_MOSI_Pin GPIO_PIN_6
#define Si4463_MOSI_GPIO_Port GPIOE
#define ZIGBEE_BAUD_RESET_Pin GPIO_PIN_9
#define ZIGBEE_BAUD_RESET_GPIO_Port GPIOF
#define Si4463_Int_Pin GPIO_PIN_1
#define Si4463_Int_GPIO_Port GPIOB
#define ZIGBEE_RESET_Pin GPIO_PIN_8
#define ZIGBEE_RESET_GPIO_Port GPIOE
#define ZIGBEE_MODE_SWITCH_Pin GPIO_PIN_11
#define ZIGBEE_MODE_SWITCH_GPIO_Port GPIOB
#define DebugUART_TX_Pin GPIO_PIN_8
#define DebugUART_TX_GPIO_Port GPIOD
#define DebugUART_RX_Pin GPIO_PIN_9
#define DebugUART_RX_GPIO_Port GPIOD
#define ZIGBEE_WAKE_Pin GPIO_PIN_4
#define ZIGBEE_WAKE_GPIO_Port GPIOD
#define Si4463_CS_Pin GPIO_PIN_14
#define Si4463_CS_GPIO_Port GPIOG
#define Si4463_Shutdown_Pin GPIO_PIN_5
#define Si4463_Shutdown_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
