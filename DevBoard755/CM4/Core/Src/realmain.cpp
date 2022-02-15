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

void real_main(void)
{

  // hand over control to the dark side
  SI446x * tx = new SI446x(&hspi4);
  tx->nop();
  tx->apply_patch();
  tx->fifo_info_fast_read();
  tx->get_chip_status_fast_clear();


  tx->change_state(0);

  /* Start scheduler */
  osKernelStart();
  while (1) {}
}




