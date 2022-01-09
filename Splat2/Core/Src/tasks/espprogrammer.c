
#include "../../Inc/main.h"
extern UART_HandleTypeDef ESP_UART(void);

void BootToProgram()
{
	HAL_GPIO_TogglePin(PG15_GPI09_ON_ESP_GPIO_Port, PG15_GPI09_ON_ESP_Pin);
	osDelay(50);
	HAL_GPIO_TogglePin(PE15_EN_ON_ESP_GPIO_Port, PE15_EN_ON_ESP_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(PE15_EN_ON_ESP_GPIO_Port, PE15_EN_ON_ESP_Pin);
	osDelay(50);
	HAL_GPIO_TogglePin(PG15_GPI09_ON_ESP_GPIO_Port, PG15_GPI09_ON_ESP_Pin);

}

void Reset()
{
	HAL_GPIO_TogglePin(PE15_EN_ON_ESP_GPIO_Port, PE15_EN_ON_ESP_Pin);
	osDelay(50);
	HAL_GPIO_TogglePin(PE15_EN_ON_ESP_GPIO_Port, PE15_EN_ON_ESP_Pin);
}


void StartProgrammer(void *argument)
{

	// Setup
    HAL_UART_Receive_IT(&huart6, incomingfromESP, 1);
    HAL_UART_Receive_IT(&huart3, incomingfromSTM, 1);

    // Let the user know its going ok so far
    uint8_t Test[] = "Entering boot mode\r\n";
	HAL_UART_Transmit(&huart3,Test,sizeof(Test),10);



 	BootToProgram();
 	osDelay(100);
 	uint8_t ready[] = "Ready to receive\r\n";
 	HAL_UART_Transmit(&huart3,ready,sizeof(ready),10);
	for(;;)
	{
		uint8_t dataFromESP;
		uint8_t dataFromSTM;
		if(circular_buf_get(cbufESPToSTM, &dataFromESP)==0)
		{
			HAL_UART_Transmit(&huart3,&dataFromESP,sizeof(dataFromESP),300);
		}

		if(circular_buf_get(cbufSTMToESP, &dataFromSTM)==0)
		{
			HAL_UART_Transmit(&huart6,&dataFromSTM,sizeof(dataFromSTM),300);
// and echo back for testing
			HAL_UART_Transmit(&huart3,&dataFromSTM,sizeof(dataFromSTM),300);
		}

		//TODO: How do we know we're done?
		bool readyToReset = false;
		if( readyToReset )
		{
			Reset();
			break;
		}
	}
}

