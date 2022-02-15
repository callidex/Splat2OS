
#ifndef __REALMAIN_H
#define __REALMAIN_H


#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "cmsis_os.h"
#include "circbuff.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "Integration/Controllers/Radio/SI446x.h"



void real_main();

#ifdef __cplusplus
}
#endif

#endif
