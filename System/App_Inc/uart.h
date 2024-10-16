#ifndef __UART_H
#define __UART_H

#include "bsp.h"
#include "stm32f1xx_ll_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define UART1_RX_PIN				        BSP_BT_RX_PIN
#define UART1_RX_PORT				        BSP_BT_RX_PORT
#define UART1_TX_PIN 				        BSP_BT_TX_PIN
#define UART1_TX_PORT				        BSP_BT_TX_PORT
#define UART1_INTERUPT_PRIORITY		        (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1U)

#define UART3_RX_PIN				        BSP_UART_RX_PIN
#define UART3_RX_PORT				        BSP_UART_RX_PORT
#define UART3_TX_PIN 				        BSP_UART_TX_PIN
#define UART3_TX_PORT 				        BSP_UART_TX_PORT
#define UART3_INTERUPT_PRIORITY		        (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+2U)
#define UART_BUFFER_SIZE 			        256U

#define UART_QUEUE_LENGTH 					UART_BUFFER_SIZE
#define UART_QUEUE_ITEM_SIZE      			sizeof(uint8_t)

typedef struct{
	UART_HandleTypeDef hal;
	QueueHandle_t txQueue;
	StaticQueue_t txQueueBuffer;
	uint8_t txQueueStack[UART_QUEUE_LENGTH*UART_QUEUE_ITEM_SIZE];
	QueueHandle_t rxQueue;
	StaticQueue_t rxQueueBuffer;
	uint8_t rxQueueStack[UART_QUEUE_LENGTH*UART_QUEUE_ITEM_SIZE];
	xSemaphoreHandle lock;
	StaticSemaphore_t lockBuffer;
	uint8_t initFlag;
}uart_ts;

/* Global functions declarations */
int32_t UART_Init(uart_ts *_huart, USART_TypeDef *_instance, uint32_t _baudrate);
void UART_DeInit(uart_ts *_huart);
uint8_t UART_Get_HEX_Digit(uint8_t _n);
void UART_Char_To_Hex(uint8_t _c, uint8_t _hex[3]);
void UART_Send_HEX(uart_ts *_huart, uint8_t _ch);
void UART_Send_String(uart_ts *_huart, const char *_string);
void UART_Send_Buffer(uart_ts *_huart, const uint8_t *_data, uint16_t _len);
int32_t UART_Receive_Byte(uart_ts *_huart, uint8_t *_byte, TickType_t _ticksToWait);
void UART_IRQ_Handler(uart_ts *_huart);

#endif
