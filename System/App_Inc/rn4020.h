#ifndef __RN4020_H
#define __RN4020_H

#include "bsp.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define RN4020_PORT		                        USART1
#define RN4020_BAUDRATE	                        115200U
#define RN4020_WAKE_HW_PIN                      BSP_BT_WAKE_HW_PIN
#define RN4020_WAKE_SW_PIN                      BSP_BT_WAKE_SW_PIN
#define RN4020_CMD_MLDP_PIN                     BSP_BT_CMD_MLDP_PIN
#define RN4020_GPIO_PORT                        BSP_BT_PORT
#define RN4020_CONNECTION_ERROR_IDX             3U
#define RN4020_DISPLAY_VALUE_UPDATE_TMO         10U
#define RN40020_SYSTEM_ADDR                     3U
#define RN4020_TASK_STACK_SIZE 					(configMINIMAL_STACK_SIZE*4U)

typedef enum{
    RN4020_RX_RESP_NONE,
    RN4020_RX_RESP_CMD,
    RN4020_RX_RESP_AOK,
    RN4020_RX_RESP_DATA,
    RN4020_RX_RESP_ADDR,
    RN4020_RX_RESP_CONNECTED,
    RN4020_RX_RESP_END,
    RN4020_RX_RESP_RSSI,
    RN4020_RX_RESP_CONNECTION_END,
    RN4020_RX_RESP_VALUE
}rn4020_rx_resp_te;

typedef struct{
	TaskHandle_t handle;
	StaticTask_t buffer;
	StackType_t stack[RN4020_TASK_STACK_SIZE];
}rn4020_task_ts;

typedef struct{
	rn4020_task_ts hTaskInit;
	rn4020_task_ts hTaskRx;
	rn4020_task_ts hTaskTx;
	xSemaphoreHandle cmdBusyFlag;
	StaticSemaphore_t cmdBusyFlagBuffer;
	xSemaphoreHandle connectionBusyFlag;
	StaticSemaphore_t connectionBusyFlagBuffer;
    uart_ts hUart;
    uint8_t initFlag;
    __IO rn4020_rx_resp_te rxResp;
    __IO uint32_t rxTmo;
    __IO uint8_t rxState;
    uint8_t txDataBuffer[128];
    uint8_t rxDataBuffer[128];
    uint8_t rxDataIdx;
    __IO uint8_t connectionEndState;
    __IO uint8_t connectionFlag;
    uint8_t txBlinkState;
    uint8_t txBlinkTmo;
    uint8_t connectionErrorIdx;
    float rssiAvgValueF;
    int8_t rssiAvgValue;
    uint8_t rssiAvgSetFlag;
    uint8_t displayValueUpdateTmo;
    const uint8_t *addrSet;
    uint8_t addrSetId;
}rn4020_ts;

/* Global functions declarations */
void RN4020_Init(void);
uint8_t RN4020_Get_Connection_Status(void);
void RN4020_Change_Target_Address(void);
uint8_t RN4020_Get_Target_Address_ID(void);
void RN4020_PORT_IRQ_Handler(void);

#endif
