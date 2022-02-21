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

void shutdown(bool b)
{
	HAL_GPIO_WritePin(Si4463_Shutdown_GPIO_Port, Si4463_Shutdown_Pin, b?GPIO_PIN_SET:GPIO_PIN_RESET);
}
void nsel(bool b)
{
	HAL_GPIO_WritePin(Si4463_CS_GPIO_Port, Si4463_CS_Pin, b?GPIO_PIN_SET:GPIO_PIN_RESET);
}


void real_main(void)
{
	SI446x * tx = new SI446x(&hspi4, Si4463_CS_GPIO_Port, Si4463_CS_Pin , Si4463_Shutdown_GPIO_Port, Si4463_Shutdown_Pin, Si446x_CTS_GPIO_Port, Si446x_CTS_Pin);

	tx->reset();
	tx->nsel(false);
	GPIO_PinState pin  = HAL_GPIO_ReadPin(Si446x_CTS_GPIO_Port, Si446x_CTS_Pin);


	return;

  // hand over control to the dark side


//  tx->nop();
//  tx->apply_patch();
//  tx->fifo_info_fast_read();
//  tx->get_chip_status_fast_clear();
//
//
//  tx->change_state(0);

  /* Start scheduler */
  osKernelStart();
  while (1) {}
}




