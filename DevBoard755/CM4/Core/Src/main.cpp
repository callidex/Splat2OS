/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "realmain.h"
#include "cmsis_os.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"


#ifdef __cplusplus
extern "C" {
#endif


void real_main(void)
{

  // hand over control to the dark side
  SI446x * tx = new SI446x(&hspi4);
  /* Start scheduler */
  osKernelStart();
  while (1) {}
}


#ifdef __cplusplus
}
#endif


