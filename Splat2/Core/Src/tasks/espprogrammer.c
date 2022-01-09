
#include "../../Inc/main.h"

void BootToProgram()
{
	osDelay(5000);
	HAL_GPIO_TogglePin(PG15_GPI09_ON_ESP_GPIO_Port, PG15_GPI09_ON_ESP_Pin);
	osDelay(50);
	HAL_GPIO_TogglePin(PE15_EN_ON_ESP_GPIO_Port, PE15_EN_ON_ESP_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(PE15_EN_ON_ESP_GPIO_Port, PE15_EN_ON_ESP_Pin);
	osDelay(50);
	HAL_GPIO_TogglePin(PG15_GPI09_ON_ESP_GPIO_Port, PG15_GPI09_ON_ESP_Pin);
    osThreadTerminate(NULL);
}

void Reset()
{
	HAL_GPIO_TogglePin(PE15_EN_ON_ESP_GPIO_Port, PE15_EN_ON_ESP_Pin);
	osDelay(50);
	HAL_GPIO_TogglePin(PE15_EN_ON_ESP_GPIO_Port, PE15_EN_ON_ESP_Pin);
}


void StartProgrammer(void *argument)
{
 	// priority task
 	
 	osDelay(1000);
 	BootToProgram();
 	osDelay(100);
 	Reset();
}

