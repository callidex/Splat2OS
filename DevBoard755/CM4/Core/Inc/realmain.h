
#ifndef __REALMAIN_H
#define __REALMAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

#include "circbuff.h"
#include "Integration/Controllers/Radio/SI446x.h"

uint8_t * cbuffer;
cbuf_handle_t cbufInterpreter;
uint8_t  uartRxBuf[1];


#ifdef __cplusplus
}
#endif

#endif
