/*
 * ThreadTest.cpp
 *
 *  Created on: 14 Feb. 2022
 *      Author: paul
 */

#include "ThreadTest.h"

ThreadTest::ThreadTest() {
	// TODO Auto-generated constructor stub

}

ThreadTest::~ThreadTest() {
	// TODO Auto-generated destructor stub
}


void ThreadTest::Start()
{
//	xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask)
//	osThreadNew((os_pthread)&ThreadTest::Thread1, NULL, attr)
//	osThreadDef(Thread1def, (os_pthread)&ThreadTest::Thread1, osPriorityAboveNormal, 0, 128);
//	Thread1TaskHandle = osThreadCreate(osThread(Thread1def), NULL);
	tskTaskControlBlock * Thread1TaskHandle;
	xTaskCreate((TaskFunction_t) &ThreadTest::Thread1, "Thread1", 128, this, osPriorityAboveNormal, &Thread1TaskHandle);

}
void ThreadTest::Thread1(void const * argument)
{
	while (1)
		{
		HAL_Delay(250);
		}
}
