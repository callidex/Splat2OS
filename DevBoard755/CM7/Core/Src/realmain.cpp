
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
#ifdef __cplusplus
extern "C"
{
#endif

#include "realmain.h"
#include "../Integration/Controllers/Zigbee/Zigbee.h"
#include "../Integration/Controllers/Radio/SI446x.hpp"

/* Here is where the action happens, include the peripheral you want to play with
 * Create an instance, go nuts
 */

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern SPI_HandleTypeDef hspi4;

void shutdown(bool b)
{
	HAL_GPIO_WritePin(Si4463_Shutdown_GPIO_Port, Si4463_Shutdown_Pin, b?GPIO_PIN_SET:GPIO_PIN_RESET);
}
void nsel(bool b)
{
	HAL_GPIO_WritePin(Si4463_CS_GPIO_Port, Si4463_CS_Pin, b?GPIO_PIN_SET:GPIO_PIN_RESET);
}

#define UART_LOGGING
void real_main(void)
{

	//example : Swapping out logging methods with no change to the peripheral
#ifdef UART_LOGGING
	Integration::SerialLogger *s1 = new Integration::SerialLogger(&huart3);
#else
	Integration::SWVLogger *s1 = new Integration::SWVLogger();
#endif

	Integration::Zigbee *zig1 = new Integration::Zigbee(s1, &huart4);

//	GPIO_PinState pin  = HAL_GPIO_ReadPin(Si446x_CTS_GPIO_Port, Si446x_CTS_Pin);
//
//	PortMap * map = new PortMap();
//
//	map->Pins.push_back(new PinOut(Si4463_CS_GPIO_Port, Si4463_CS_Pin, "CS"));
//	map->Pins.push_back(new PinOut(Si446x_CTS_GPIO_Port, Si446x_CTS_Pin, "CTS"));
//	map->Pins.push_back(new PinOut(Si4463_Shutdown_GPIO_Port, Si4463_Shutdown_Pin, "Shutdown"));
////
//	SerialLogger * logger= new SerialLogger(&huart3);
	Integration::SI446x * tx = new Integration::SI446x(s1, &hspi4); // ,, nsel_port, nsel_pin, shutdown_port, shutdown_pin);

//	pin  = HAL_GPIO_ReadPin(Si446x_CTS_GPIO_Port, Si446x_CTS_Pin);
//	//tx->reset();
//	pin  = HAL_GPIO_ReadPin(Si446x_CTS_GPIO_Port, Si446x_CTS_Pin);
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



#ifdef __cplusplus
}
#endif
