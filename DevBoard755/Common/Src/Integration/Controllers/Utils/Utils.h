/*
 * Utils.h
 *
 *  Created on: Feb 24, 2022
 *      Author: paul
 */

#ifndef SRC_INTEGRATION_CONTROLLERS_UTILS_UTILS_HPP_
#define SRC_INTEGRATION_CONTROLLERS_UTILS_UTILS_HPP_

#ifdef __cplusplus
/* It's unfortunate but this is the balance between clean and HUGE, skip the redirections and make
 * utils aware of the hardware directly
 */


/* real shame the generator cannot be configured to put defs somewhere seperate from main. */
#define DebugUART_TX_Pin GPIO_PIN_8
#define DebugUART_TX_GPIO_Port GPIOD
#define DebugUART_RX_Pin GPIO_PIN_9
#define DebugUART_RX_GPIO_Port GPIOD

#define Si4463_SCK_Pin GPIO_PIN_2
#define Si4463_SCK_GPIO_Port GPIOE
#define Si4463_MISO_Pin GPIO_PIN_5
#define Si4463_MISO_GPIO_Port GPIOE
#define Si4463_MOSI_Pin GPIO_PIN_6
#define Si4463_MOSI_GPIO_Port GPIOE
#define Si4463_Int_Pin GPIO_PIN_1
#define Si4463_Int_GPIO_Port GPIOB
#define Si4463_CS_Pin GPIO_PIN_14
#define Si4463_CS_GPIO_Port GPIOG
#define Si4463_Shutdown_Pin GPIO_PIN_5
#define Si4463_Shutdown_GPIO_Port GPIOB

#define ZIGBEE_BAUD_RESET_Pin GPIO_PIN_9
#define ZIGBEE_BAUD_RESET_GPIO_Port GPIOF
#define ZIGBEE_RESET_Pin GPIO_PIN_8
#define ZIGBEE_RESET_GPIO_Port GPIOE
#define ZIGBEE_MODE_SWITCH_Pin GPIO_PIN_11
#define ZIGBEE_MODE_SWITCH_GPIO_Port GPIOB
#define ZIGBEE_WAKE_Pin GPIO_PIN_4
#define ZIGBEE_WAKE_GPIO_Port GPIOD


#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_uart.h"
#include "stm32h7xx_hal_spi.h"
#include "stm32h7xx_hal_spi_ex.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "freertos.h"

#include "circbuff.h"

namespace Integration
{

class Logger
{
public:
	bool includeTimeStamp;
	void WriteLog(char * logname,char *logstring)
	{
		if(includeTimeStamp)
		{
			char buff[100];
			sprintf(buff, "%lu --", HAL_GetTick());
			WriteLine(buff);
		}
		if(logname) {
			WriteLine(logname);
			WriteLine(" -- ");
		}
		WriteLine(logstring);
		WriteLine("\r\n");
	}
	virtual void WriteLine(char * s) = 0;
};



class SerialLogger : public Logger
{
public:

	void WriteLog(char*logName, char *logstring)
	{

		if(includeTimeStamp)
		{
			char buff[100];
			sprintf(buff, "%lu --", HAL_GetTick());
			WriteLine(buff);
		}
		if(logName) WriteLine(logName);
		WriteLine(logstring);
		WriteLine("\r\n");
	}
	SerialLogger(UART_HandleTypeDef * serialPort): _serialPort(serialPort) {};
private:

	bool includeTimeStamp = true;
	UART_HandleTypeDef * _serialPort;
	void Send(uint8_t c)
	{
		HAL_UART_Transmit(_serialPort, &c, sizeof(c), 10);
	}
	void WriteLine(const char *pcF, ...)
	{

	}

	void WriteLine(char * s)
	{
		while (*s)
		{
			Send(*(s++));
		}
	}
};


class SWVLogger : public Logger
{
public:

	SWVLogger(){};
private:
	void WriteLine(char *name, char * s)
	{

		printf("%s - %s",name,s);
	}
	void WriteLine(char *s)
	{
		printf("%s");
	}
};


class UARTBoardDevice
{
public:
	Logger*  logger;
	UART_HandleTypeDef * device;
	circular_buf_t*  circular_buffer;

	UARTBoardDevice(Logger * log, UART_HandleTypeDef * dev, char* name)
	{
		logger =log;
		device = dev;
		buffer  = (uint8_t*) malloc(64 * sizeof(uint8_t));
		circular_buffer = circular_buf_init(buffer, 64);
		logger->WriteLog(name, "Initialised UART Circular Buffer");
	}

private:
	uint8_t *buffer;

public:
    //virtual void irqhandler(int) = 0;
};


class SPIBoardDevice
{
public:
	Logger*  logger;
	SPI_HandleTypeDef * device;
	circular_buf_t*  circular_buffer;

	SPIBoardDevice(Logger * log, SPI_HandleTypeDef * dev)
	{
		logger =log;
		device = dev;
//		buffer  = (uint8_t*) malloc(64 * sizeof(uint8_t));
//		circular_buffer = circular_buf_init(buffer, 64);
//		logger->WriteLog("SPIBOARD", "Initialised UART Circular Buffer");
	}

private:
	uint8_t *buffer;

public:
    //virtual void irqhandler(int) = 0;
};


};


#endif

#endif /* SRC_INTEGRATION_CONTROLLERS_UTILS_UTILS_HPP_ */
