#ifndef __EDITOR_H_
#define __EDITOR_H_

#include "utils.h"

extern uint8_t* prgStore;
extern uint16_t prgSize;

void resetEditor(void);
void initEditor(uint8_t* prgBody, uint16_t progSpaceSize);
uint8_t readLine();
prgline* findLine(uint16_t num);
void injectLine(uint8_t* s, uint16_t num);
uint8_t editorSave(void);
uint8_t editorLoad(void);
uint8_t editorLoadParsed(void);

#endif

