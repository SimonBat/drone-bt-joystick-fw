#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"

#define SYSTEM_TASK_TMO							50U /* ms */
#define SYSTEM_DRONE_BATTERY_SOC_ERROR			5U /* % */
#define SYSTEM_DRONE_BATTERY_MONITOR_TMO		1000U /* ms */
#define SYSTEM_DISPLAY_CONTEXT_TMO				350U /* ms */
#define SYSTEM_CHANGE_BT_ID_TMO			    	(1000U/SYSTEM_TASK_TMO)
#define SYSTEM_TASK_STACK_SIZE 					(configMINIMAL_STACK_SIZE*2U)

typedef struct{
	TaskHandle_t hTask;
	StaticTask_t hTaskBuffer;
	StackType_t hTaskStack[SYSTEM_TASK_STACK_SIZE];
	uint8_t initFlag;
	uint8_t videoEnStatus;
	uint8_t videoEnStatusState;
	uint8_t droneBatteryErrorFlag;
	uint8_t droneBatteryMonitorTmo;
	uint8_t displayContextTmo;
    uint8_t changeBTIdTmo;
}system_ts;

#endif
