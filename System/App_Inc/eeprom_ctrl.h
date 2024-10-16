#ifndef __EEPROM_CTRL_H
#define __EEPROM_CTRL_H

#include "bsp.h"
#include "eeprom.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define EE_CTRL_TASK_STACK_SIZE 			(configMINIMAL_STACK_SIZE*2U)
#define EE_CTRL_QUEUE_LENGTH 				20U
#define EE_CTRL_QUEUE_ITEM_SIZE      		sizeof(ee_w_cmd_ts)

typedef struct{
	uint16_t virtualAddr;
	uint16_t value;
}ee_w_cmd_ts;

typedef struct{
	TaskHandle_t hTask;
    StaticTask_t hTaskBuffer;
    StackType_t hTaskStack[EE_CTRL_TASK_STACK_SIZE];
	QueueHandle_t hWriteCmd;
	StaticQueue_t hWriteCmdBuffer;
	uint8_t hWriteCmdStack[EE_CTRL_QUEUE_LENGTH*EE_CTRL_QUEUE_ITEM_SIZE];
	uint8_t busyFlag;
	uint8_t initFlag;
}ee_ctrl_ts;

/* Global functions declarations */
void EE_CTRL_Init(void);
void EE_CTRL_Settings_Write(uint16_t _virtualAddr, uint16_t _value);
uint8_t EE_CTRL_Get_Busy_Flag(void);
void EE_CTRL_Handler_Suspend(void);

#endif
