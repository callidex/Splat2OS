
#include "tasks/espprogrammer.h"
#include "usart.h"
#include "circbuff.h"
#include "../../Inc/slip.h"


//uint8_t sync[] = {0x00,0x07,0x07,0x12, 0x20,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
uint8_t sync[] = {0x00,0x08,0x24,0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x12, 0x20,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
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

void SendSync()
{
	SLIP_send(sync, sizeof(sync));

}

void StartProgrammer(void *argument)
{
	ESPToSTMbuffer = malloc(3500 * sizeof(uint8_t));
	cbufESPToSTM = circular_buf_init(ESPToSTMbuffer,3500);

	STMToESPbuffer = malloc(3500 * sizeof(uint8_t));
	cbufSTMToESP = circular_buf_init(STMToESPbuffer,3500);

	uint8_t Test[] = "Spinning up uart relay\r\n";
	HAL_UART_Transmit(&huart3,Test,sizeof(Test),10);
	for(;;)
	{
		uint8_t dataFromESP;
		if(circular_buf_get(cbufESPToSTM, &dataFromESP)==0)
		{
			HAL_UART_Transmit(&huart3,&dataFromESP,sizeof(dataFromESP),300);
		}
		uint8_t dataFromSTM;
		if(circular_buf_get(cbufSTMToESP, &dataFromSTM)==0)
		{
			HAL_UART_Transmit(&huart6,&dataFromSTM,sizeof(dataFromSTM),300);
		}
	}
}
/*
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
	circular_buf_empty(cbufESPToSTM);
	SendSync();


	HAL_UART_Transmit(&huart3,ready,sizeof(ready),10);
}
*/
