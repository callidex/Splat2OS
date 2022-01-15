#ifndef ESPPROG_CONFIG_H
#define ESPPROG_CONFIG_H

#include "../../Inc/main.h"
#include "circbuff.h"
void StartProgrammer(void *argument);
uint8_t * ESPToSTMbuffer;
cbuf_handle_t cbufESPToSTM;

uint8_t * STMToESPbuffer;
cbuf_handle_t cbufSTMToESP;

#endif
