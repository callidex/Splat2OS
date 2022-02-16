#ifndef __EXTERN_H_
#define __EXTERN_H_

extern uint8_t extraCmdArgCnt[];
extern uint8_t extraFuncArgCnt[];

short extraCommandByHash(numeric h);
short extraFunctionByHash(numeric h);
extern uint8_t dataSpace[];
extern uint8_t lineSpace[];
extern uint8_t lastInput;

void sysPutc(uint8_t c);
void sysEcho(uint8_t c);
numeric sysMillis(numeric div);
void extraCommand(uint8_t cmd, numeric args[]);
numeric extraFunction(uint8_t cmd, numeric args[]);
void outputConstStr(uint8_t strId, uint8_t index, uint8_t* s);

/* data=NULL for open/close, size > 0 for write,
size < 0 for read, size=0 for close,
abs(size) can be fileId */
char storageOperation(void* data, uint16_t size);

#endif

