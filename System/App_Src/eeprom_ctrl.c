/**
  ***************************************************************************************************************************************
  * @file     eeprom_ctrl.c
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

#include "eeprom_ctrl.h"

static ee_ctrl_ts EE_HCTRL;

/* Local functions declarations */
static void EE_CTRL_Write_Handler(void *_params);

/**
  ***************************************************************************************************************************************
  * @brief  EE control task initialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void EE_CTRL_Init(void)
{
	EE_HCTRL.hWriteCmd=xQueueCreateStatic(EE_CTRL_QUEUE_LENGTH,EE_CTRL_QUEUE_ITEM_SIZE, \
										  EE_HCTRL.hWriteCmdStack,&EE_HCTRL.hWriteCmdBuffer);
	EE_HCTRL.hTask=xTaskCreateStatic(EE_CTRL_Write_Handler,"EE_Write_Handler",EE_CTRL_TASK_STACK_SIZE, \
	    							 NULL,(tskIDLE_PRIORITY+1U),EE_HCTRL.hTaskStack,&EE_HCTRL.hTaskBuffer);

	if((NULL!=EE_HCTRL.hTask)&&(NULL!=EE_HCTRL.hWriteCmd)){EE_HCTRL.initFlag=1U;}
	else{BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  EE write handler task
  * @param  Parameters (void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void EE_CTRL_Write_Handler(void *_params)
{
	ee_w_cmd_ts _writeCmd;

	while(1U)
	{
	    if(pdPASS==xQueueReceive(EE_HCTRL.hWriteCmd,&_writeCmd,pdMS_TO_TICKS(500U)))
	    {
		    EE_HCTRL.busyFlag=1U;
			HAL_FLASH_Unlock();
            EE_Write_Variable(_writeCmd.virtualAddr,_writeCmd.value);
			HAL_FLASH_Lock();
		}else{EE_HCTRL.busyFlag=0U;}
	}

	/* Task Error */
	vTaskDelete(NULL);
}

/**
  ***************************************************************************************************************************************
  * @brief  EE write profile settings request
  * @param  Register address (uint16_t), registers (uint16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void EE_CTRL_Settings_Write(uint16_t _virtualAddr, uint16_t _value)
{
	if(!EE_HCTRL.initFlag){return;}

	ee_w_cmd_ts _writeCmd={_virtualAddr,_value};
	xQueueSend(EE_HCTRL.hWriteCmd,&_writeCmd,portMAX_DELAY);
}

/**
  ***************************************************************************************************************************************
  * @brief  Get busy flag
  * @param  None
  * @retval Flag (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t EE_CTRL_Get_Busy_Flag(void)
{
	return EE_HCTRL.busyFlag;
}

/**
  ***************************************************************************************************************************************
  * @brief  Suspend flash write handler
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void EE_CTRL_Handler_Suspend(void)
{
	vTaskSuspend(EE_HCTRL.hTask);
}
