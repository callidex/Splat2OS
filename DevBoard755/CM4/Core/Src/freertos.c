/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "Interpreter/utils.h"
#include "Interpreter/textual.h"
#include "Interpreter/tokens.h"
#include "Interpreter/exectoks.h"
#include "Integration/Controllers/Radio/SI446x.h"
#include "realmain.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint8_t * cbuffer;
cbuf_handle_t cbufInterpreter;
uint8_t  uartRxBuf[1];
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for interpreter */
osThreadId_t interpreterHandle;
const osThreadAttr_t interpreter_attributes = {
  .name = "interpreter",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for queueBASIC */
osMessageQueueId_t queueBASICHandle;
const osMessageQueueAttr_t queueBASIC_attributes = {
  .name = "queueBASIC"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartInterpreter(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of queueBASIC */
  queueBASICHandle = osMessageQueueNew (16, sizeof(uint16_t), &queueBASIC_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of interpreter */
  interpreterHandle = osThreadNew(StartInterpreter, NULL, &interpreter_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartInterpreter */
/**
* @brief Function implementing the interpreter thread.
* @param argument: Not used
* @retval None
*/



//TODO: move to container class
token* toksBody;
char lineSpace[40 * 3];
char lastInput;
short prgSize;
static short listLine, listPage;
char mainState;
char dataSpace[150 + 850];
void preload(char* line, token* t) {
    if (editorLoadParsed()) {
        outputConstStr(ID_COMMON_STRINGS, 10, NULL); // code found, autorun message
        outputCr();
        setDelay(1000);
        mainState = STATE_PRELOAD;
    } else {
        prgReset();
    }
}
void init(short dataSize, short lineSize, short progSize) {
    outputCr();
    outputConstStr(ID_COMMON_STRINGS, 0, NULL); // Miskatino vX.X
    outputCr();
    initEditor(dataSpace + dataSize, progSize);
    initTokenExecutor(dataSpace, dataSize);
    listLine = 1;
    listPage = 10;
    mainState = STATE_INTERACTIVE;
    toksBody = (token*)(void*) (lineSpace + lineSize);
    preload(lineSpace, toksBody);
}
void showInfo(void) {
    outputConstStr(ID_COMMON_STRINGS, 1, NULL); // code:
    outputInt(prgSize);
    outputCr();
    outputConstStr(ID_COMMON_STRINGS, 2, NULL); // vars:
    outputInt(varSize());
    outputCr();
    outputConstStr(ID_COMMON_STRINGS, 3, NULL); // next:
    outputInt(nextLineNum);
    outputCr();
}
void prgReset(void) {
    resetEditor();
    resetTokenExecutor();
}
void executeRun() {
    if (editorSave()) {
        editorLoadParsed();
        initParsedRun();
    } else {
        executeNonParsed(-1);
    }
}


void printProgram(void) {
    prgline* p = findLine(listLine);
    if (p->num == 0 && listLine > 1) {
        p = findLine(1);
    }
    short lineCount = 0;
    while (p->num != 0 && lineCount < listPage) {
        listLine = p->num + 1;
        outputInt(p->num);
        outputChar(' ');
        outputNStr(&(p->str));
        outputCr();
        p = findLine(p->num + 1);
        lineCount += 1;
    }
}

void listProgram(token* t) {
    t = nextToken(nextToken(t));
    if (t->type == TT_NUMBER) {
        listLine = t->body.integer;
        t = nextToken(t);
        if (t->type == TT_NUMBER) {
            listPage = t->body.integer;
        }
    }
    printProgram();
}

void executeSteps() {
    token* t = nextToken(nextToken(toksBody));
    mainState |= STATE_STEPS;
    executeNonParsed(t->type == TT_NUMBER ? t->body.integer : 1);
}
void metaOrError() {
    numeric h = tokenHash(toksBody);
    if (h == 0x3B6) { // LIST
        listProgram(toksBody);
    } else
    	if (h == 0x312) { // STEP
        executeSteps();
    } else
  	if (h == 0x1AC) { // RUN
        executeRun();
//    } else if (h == 0x375) { // SAVE
//        manualSave();
//    } else if (h == 0x39A) { // LOAD
//        manualLoad();
//
        } else if (h == 0x69A) { // RESET
        prgReset();
    } else if (h == 0x3B3) { // INFO
        showInfo();
    } else {
        getParseErrorMsg(lineSpace);
        outputStr(lineSpace);
        outputChar(' ');
        outputInt((long)(getParseErrorPos() - lineSpace) + 1);
        outputCr();
    }
}

int uartRead(void)
{
	unsigned char c;
	// just pop off the buffer
	if(circular_buf_get(cbufInterpreter, &c)==0) return c;
	return -1;
}
char sysGetc(void)
{
	short c = uartRead();
	return c>=0? (char) c:0;
}
void uartSend(uint8_t c) {
	 HAL_UART_Transmit(&huart3, &c, sizeof(c), 10);
}
char mainState;
void sysPutc(uint8_t c) {
    if (c == '\n') {
        uartSend('\r');
    } else if (c == '\b') {
        uartSend('\b');
        uartSend(' ');
    }
    uartSend(c);
}

void sysEcho(uint8_t c) {
    sysPutc(c);
}
void uartSends(uint8_t* s) {
	while (*s) {
        uartSend(*(s++));
    }
}
void debugPrint(uint8_t * buff){
	uartSends(buff);
}


char hex[] = "0123456789ABCDEF";
void uartSendHex(int x, int d) {
    while (d-- > 0) {
        uartSend(hex[(x >> (d * 4)) & 0xF]);
    }
}

void uartSendDec(int x) {
    static char s[10];
    int i;
    int x1;
    i = 0;
    while (x > 0) {
        x1 = x / 10;
        s[i++] = x - x1 * 10;
        x = x1;
    }
    if (i == 0) {
        s[i++] = 0;
    }
    while (i > 0) {
        uartSend('0' + s[--i]);
    }
}
char extraCmdArgCnt[] = {2, 2, 2, 2};
char extraFuncArgCnt[] = {1, 1, 1, 1, 1};

void printTokens(token* t) {
}
void processLine() {
    if (lineSpace[0] == 0) {
        return;
    }
    parseLine(lineSpace, toksBody);
    printTokens(toksBody);
    if (getParseErrorPos() != NULL) {
        metaOrError();
        return;
    }
    if (toksBody->type != TT_NUMBER) {
        executeTokens(toksBody);
    } else {
        injectLine(skipSpaces(skipDigits(lineSpace)), toksBody->body.integer);
    }
}
short pinRead(char pin) {
    return 0;
	//return pinInput(GPIOA_BASE, pin);
}

void pinOut(char pin, schar state) {
//    if (state >= 0) {
//        pinMode(GPIOA_BASE, pin, PIN_MODE_OUT, PIN_CNF_O_PP);
//        pinOutput(GPIOA_BASE, pin, state);
//    } else {
//        pinMode(GPIOA_BASE, pin, PIN_MODE_IN, state == -1 ? PIN_CNF_I_FLT : PIN_CNF_I_PULL);
//        if (state < -1) {
//            pinOutput(GPIOA_BASE, pin, state == -2 ? 1 : 0);
//        }
//    }
}
void waitPreloadRunDelay() {
    if (lastInput > 0) {
        mainState &= ~STATE_PRELOAD;
        outputConstStr(ID_COMMON_STRINGS, 11, NULL); // canceled
        outputCr();
        editorLoad();
    } else if (checkDelay()) {
        mainState &= ~STATE_PRELOAD;
        initParsedRun();
    }
}
void dispatch() {
    if (lastInput == 3) {
        mainState |= STATE_BREAK;
    }
    if ((mainState & (STATE_RUN | STATE_SLOWED)) == STATE_RUN) {
        executeParsedRun();
        return;
    }
    switch (mainState & STATE_SLOWED) {
        case STATE_DELAY:
            dispatchDelay();
            return;
        case STATE_INPUT:
            dispatchInput();
            lastInput = 0;
            return;
        case STATE_BREAK:
            dispatchBreak();
            lastInput = 0;
            return;
    }
    if ((mainState & STATE_STEPS) != 0) {
        executeNonParsed(0);

    } else if ((mainState & STATE_PRELOAD) != 0) {
        waitPreloadRunDelay();

    } else {
        if (lastInput > 0) {
            if (readLine()) {
                processLine();
            }
            lastInput = 0;
        }
    }
    return;
}
//TODO: Where are these?
#define RTC_CRL 0x04
#define RTC_PRLH 0x08
#define RTC_PRLL 0x0C
#define RTC_CNTH 0x18
#define RTC_CNTL 0x1C
/*
numeric sysMillis(numeric div) {
    unsigned short h1 = REG_L(RTC_BASE, RTC_CNTH);
    unsigned short lo = REG_L(RTC_BASE, RTC_CNTL);
    unsigned short h2 = REG_L(RTC_BASE, RTC_CNTH);
    long v;
    if (h1 == h2) {
        v = (((long) h1) << 16) | lo;
    } else {
        v = ((long) h2) << 16;
    }
    return (div <= 1) ? v : v / div;
}*/

void outputConstStr(char strId, char index, char* w) {
    char* s;
    switch (strId) {
        case ID_COMMON_STRINGS:
            s = "BDARS BASIC 0.1\ncode: \nvars: \nnext: \nBREAK\nEnd of code\nSaved\nLoaded\nbytes\nLoad failed\nAutorun in 1 sec\nCanceled!\nLow VARS mem\nLow PROG mem\n";
            break;
        case ID_PARSING_ERRORS:
        	s= "\nCmd or Var exp-d\nSymbol '=' exp-d\nName expected\nSymbol ';' exp-d\nExtra chars at end\nUnexpected error\nNumber out of range\nNumber expected\nUnexpected symbol\nUnexp-d line end\n";
            break;
        default:
            return;
    }
    while (index > 0) {
        while (*s++ != '\n') {
        }
        index -= 1;
    }
    while (*s != '\n') {
        if (w) {
            *(w++) = (*s++);
        } else {
            sysPutc(*s++);
        }
    }
    if (w) {
        *w = 0;
    }
}

short extraCommandByHash(numeric h) {
    switch (h) {
        case 0x036F: // POKE
            return CMD_EXTRA + 0;
        case 0x06EC: // POKE2
            return CMD_EXTRA + 1;
        case 0x06EA: // POKE4
            return CMD_EXTRA + 2;
        case 0x019C: // PIN
            return CMD_EXTRA + 3;
        default:
            return -1;
    }
}

short extraFunctionByHash(numeric h) {
    switch (h) {
        case 0x019C: // PIN
            return 3;
        case 0x01CF: // ADC
            return 4;
        case 0x0355: // PEEK
            return 0;
        case 0x0698: // PEEK2
            return 1;
        case 0x069E: // PEEK4
            return 2;
        default:
            return -1;
    }
}

void extraCommand(char cmd, numeric args[]) {
    switch (cmd) {
        case 0:
            *((unsigned char*)(args[0])) = (unsigned char) args[1];
            break;
        case 1:
            *((unsigned short*)(args[0])) = (unsigned short) args[1];
            break;
        case 2:
            *((unsigned long*)(args[0])) = (unsigned long) args[1];
            break;
        case 3:
            pinOut(args[0], args[1]);
            break;
    }
}

numeric extraFunction(char cmd, numeric args[]) {
    switch (cmd) {
        case 0:
            return *((unsigned char*)(args[0]));
        case 1:
            return *((unsigned short*)(args[0]));
        case 2:
            return *((unsigned long*)(args[0]));
        case 3:
            return pinRead(args[0]);
//        case 4:
//            return adcRead(args[0]);
    }
    return 0;
}

static void storageSend(uchar c) {
}

unsigned char storageChecksum(short size) {
	return 0;
}

char storageOperation(void* data, short size) {
	return 0;
}

/* USER CODE END Header_StartInterpreter */
__weak void StartInterpreter(void *argument)
{
  /* USER CODE BEGIN StartInterpreter */

//	Integration::Si446x handle = new Integration::Si446x();


	cbuffer  = malloc(100 * sizeof(uint8_t));
	cbufInterpreter = circular_buf_init(cbuffer, 100);
	HAL_UART_Receive_IT(&huart3, &uartRxBuf,1);
	/* Infinite loop */
	init(150, 40,850);
	for(;;)
  {
     lastInput = sysGetc();
     if(lastInput>0)
   	 	 dispatch();
  }
  /* USER CODE END StartInterpreter */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

