#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdio.h>
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"

#define DISPLAY_LOGO_TMO        		1200U /* ms */
#define DISPLAY_UPDATE_TMO				50U /* ms */
#define DISPLAY_CONTEXTS				3U
#define DISPLAY_TASK_STACK_SIZE 		(configMINIMAL_STACK_SIZE*2U)

typedef enum{
    DISPLAY_DATA_1_CONTEXT,
	DISPLAY_DATA_2_CONTEXT,
	DISPLAY_DATA_3_CONTEXT
}display_data_te;

typedef struct{
    TaskHandle_t hTaskInit;
    StaticTask_t hTaskInitBuffer;
    StackType_t hTaskInitStack[DISPLAY_TASK_STACK_SIZE];
    TaskHandle_t hTaskUpdate;
    StaticTask_t hTaskUpdateBuffer;
    StackType_t hTaskUpdateStack[DISPLAY_TASK_STACK_SIZE];
    display_data_te dataContext;
    uint8_t initFlag;
}display_ts;

/* Global functions declarations */
void DISPLAY_Init(void);
void DISPLAY_Set_Data_Context(display_data_te _context);

#endif
