/**
  ***************************************************************************************************************************************
  * @file     rtos_cb.c
  * @owner    SimonBat
  * @version  v0.0.1
  * @date     2021.11.11
  * @update   2021.11.11
  * @brief    dragonfly_joystick v1.0
  ***************************************************************************************************************************************
  * @attention
  *
  * (Where to use)
  *
  ***************************************************************************************************************************************
  */

#include "rtos_cb.h"

/**
  ****************************************************************************************************************************************************
  * @brief  RTOS vApplicationGetTimerTaskMemory callback
  * @param  ppxIdleTaskTCBBuffer (StaticTask_t**), ppxIdleTaskStackBuffer (StackType_t**), pulIdleTaskStackSize (uint32_t*)
  * @retval None
  ****************************************************************************************************************************************************
  */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, \
								   StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

	*ppxIdleTaskTCBBuffer=&xIdleTaskTCB;
	*ppxIdleTaskStackBuffer=uxIdleTaskStack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}

/**
  ****************************************************************************************************************************************************
  * @brief  RTOS vApplicationGetTimerTaskMemory callback
  * @param  ppxTimerTaskTCBBuffer (StaticTask_t**), ppxTimerTaskStackBuffer (StackType_t**), pulTimerTaskStackSize (uint32_t*)
  * @retval None
  ****************************************************************************************************************************************************
  */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, \
									StackType_t **ppxTimerTaskStackBuffer, configSTACK_DEPTH_TYPE *pulTimerTaskStackSize)
{
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

	*ppxTimerTaskTCBBuffer=&xTimerTaskTCB;
	*ppxTimerTaskStackBuffer=uxTimerTaskStack;
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}
