/*
 * Zigbee.h
 *
 *  Created on: Feb 27, 2022
 *      Author: paul
 */

#ifndef SRC_INTEGRATION_CONTROLLERS_ZIGBEE_ZIGBEE_H_
#define SRC_INTEGRATION_CONTROLLERS_ZIGBEE_ZIGBEE_H_
#include "..\Utils\Utils.h"
namespace Integration {

class Zigbee : public UARTBoardDevice {
public:
	virtual ~Zigbee();
	Zigbee(Logger * log,  UART_HandleTypeDef * dev) :UARTBoardDevice(log, dev, "Zigbee")
	{
		log->WriteLog(logName, "Starting Up");
		HAL_GPIO_WritePin(ZIGBEE_WAKE_GPIO_Port, ZIGBEE_WAKE_Pin, GPIO_PIN_RESET);
		osDelay(100);
		HAL_GPIO_WritePin(ZIGBEE_RESET_GPIO_Port, ZIGBEE_RESET_Pin, GPIO_PIN_SET);
		osDelay(10);
		HAL_GPIO_WritePin(ZIGBEE_BAUD_RESET_GPIO_Port, ZIGBEE_BAUD_RESET_Pin, GPIO_PIN_RESET);
		osDelay(1200);
		HAL_GPIO_WritePin(ZIGBEE_BAUD_RESET_GPIO_Port, ZIGBEE_BAUD_RESET_Pin, GPIO_PIN_SET);

		log->WriteLog(logName, "Started");

	}

private:
 char *logName = "Zigbee";
 void WriteLog(char*s)
 {
	 logger->WriteLog(logName,s);
 }


};
} /* namespace Integration */

#endif /* SRC_INTEGRATION_CONTROLLERS_ZIGBEE_ZIGBEE_H_ */
