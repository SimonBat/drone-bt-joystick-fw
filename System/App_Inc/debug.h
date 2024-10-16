#ifndef __DEBUG_H
#define __DEBUG_H

#include "stm32f1xx_hal.h"
#include "uart.h"

#define DEBUG_PORT		   		USART3
#define DEBUG_BAUDRATE	   		115200U

typedef struct{
	uart_ts hUart;
    uint8_t initFlag;
}debug_ts;

/* Global functions declarations */
void DEBUG_Init(void);
void DEBUG_Send_Char(uint8_t _char);
void DEBUG_Send_Message(const char *_string);
void DEBUG_PORT_IRQ_Handler(void);

#endif
