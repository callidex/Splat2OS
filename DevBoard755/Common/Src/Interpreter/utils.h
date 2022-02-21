#ifndef __UTILS_H_
#define __UTILS_H_

#include "mytypes.h"

void trim(uint8_t * s);
uint8_t * skipSpaces(uint8_t * s);
uint8_t * skipDigits(uint8_t * s);
uint8_t InStr(uint8_t  c, uint8_t * s);
uint8_t  cmpNStrToStr(nstring* ns, uint8_t * s);
int decFromStr(uint8_t * s);
uint8_t  isDigit(uint8_t  c);
uint8_t  isDigitBased(uint8_t  c, uint8_t  base);
uint8_t  isAlpha(uint8_t  c);
uint8_t  isAlNum(uint8_t  c);
uint8_t  isSpace(uint8_t  c);
uint8_t toUpper(uint8_t  c);
uint8_t  makeDigit(uint8_t  c, uint8_t  base);
numeric hashOfNStr(nstring* t);
void pause(numeric millis);

void outputuint8_t (uint8_t  c);
void outputStr(uint8_t * s);
void outputNStr(nstring* s);
void outputInt(long n);
void outputCr();

#endif

