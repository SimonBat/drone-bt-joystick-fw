/**
  ***************************************************************************************************************************************
  * @file     ltc2942_ctrl.c
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

#include "ltc2942_ctrl.h"
#include "eeprom_ctrl.h"
#include "profile.h"
#include "buzzer.h"
#include "led.h"

static ltc2942_ctrl_ts H_LTC_CTRL;

/* Local functions declarations */
static void LTC2942_CTRL_Handler(void *_params);

/**
  ***************************************************************************************************************************************
  * @brief  LTC2942 initialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_CTRL_Init(void)
{
    H_LTC_CTRL.hTask=xTaskCreateStatic(LTC2942_CTRL_Handler,"LTC2942_CTRL_Handler",LTC2942_CTRL_TASK_STACK_SIZE, \
        							   NULL,(tskIDLE_PRIORITY+1U),H_LTC_CTRL.hTaskStack,&H_LTC_CTRL.hTaskBuffer);

    if(NULL!=H_LTC_CTRL.hTask){H_LTC_CTRL.initFlag=1U;}
	else{BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  LTC2942 control handle
  * @param  Parameters (void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void LTC2942_CTRL_Handler(void *_params)
{
    uint32_t _tick;
	uint32_t _tickstart;
    TickType_t _xLastWakeTime=xTaskGetTickCount();

	while(1U)
	{
        if(!H_LTC_CTRL.ltcHandle.initFlag){LTC2942_Init(&H_LTC_CTRL.ltcHandle);}

        uint16_t _voltage=LTC2942_Get_Voltage();
        int32_t _temperature=LTC2942_Get_Temperature();
        uint8_t _soc=LTC2942_Get_SOC(&H_LTC_CTRL.ltcHandle);
      
        if((_voltage<LTC2942_CTRL_LOW_VOLTAGE_THS)&&(_voltage!=0U))
        {
            BUZZER_Driver_Start(BUZZER_MELODY_ID_ERROR, BUZZER_MODE_CONTINUOUS);
            H_LTC_CTRL.buzzerActiveFlag=1U;
            LTC2942_Update_Battery_Capacity(&H_LTC_CTRL.ltcHandle);
            PROFILE_Set_Joystick_Battery_SOC(_soc);
            PROFILE_Set_Joystick_Battery_Voltage(_voltage);
            PROFILE_Set_Joystick_Battery_Temperature(((_temperature+50U)/100U));
            vTaskDelayUntil(&_xLastWakeTime, pdMS_TO_TICKS(1250U));
            /* Wait EE flash operations to complete */
            _tickstart=xTaskGetTickCount();
            while(EE_CTRL_Get_Busy_Flag())
            {
                _tick=xTaskGetTickCount();
			    if((_tick-_tickstart)>1000U){break;}
            };
            /* System power off! */
            HAL_GPIO_WritePin(BSP_LTC_KILL_PORT,BSP_LTC_KILL_PIN,GPIO_PIN_RESET);
        }
        else if((_soc<=LTC2942_CTRL_ERROR_SOC)&&(!H_LTC_CTRL.errorFlag)&&(BUZZER_STATE_OFF==BUZZER_Driver_Get_State()))
        {
            BUZZER_Driver_Start(BUZZER_MELODY_ID_ERROR, BUZZER_MODE_CONTINUOUS);
            H_LTC_CTRL.buzzerActiveFlag=1U;
            PROFILE_Set_Joystick_Battery_SOC(_soc);
            PROFILE_Set_Joystick_Battery_Voltage(_voltage);
            PROFILE_Set_Joystick_Battery_Temperature(((_temperature+50U)/100U));
            H_LTC_CTRL.errorFlag=1U;
            H_LTC_CTRL.updateTmo=0U;
        }
        else if(++H_LTC_CTRL.updateTmo>=(LTC2942_CTRL_UPDATE_TMO/LTC2942_CTRL_TASK_TMO))
        {
            PROFILE_Set_Joystick_Battery_SOC(_soc);
            PROFILE_Set_Joystick_Battery_Voltage(_voltage);
            PROFILE_Set_Joystick_Battery_Temperature(((_temperature+50U)/100U));
            H_LTC_CTRL.updateTmo=0U;
        }
        else if(((0U==_soc)||(_temperature>6000)||(_temperature<0))&&(0U!=_voltage))
        {
            BUZZER_Driver_Start(BUZZER_MELODY_ID_ERROR, BUZZER_MODE_CONTINUOUS);
            H_LTC_CTRL.buzzerActiveFlag=1U;
            vTaskDelayUntil(&_xLastWakeTime, pdMS_TO_TICKS(1250U));
            /* Wait EE flash operations to complete */
            _tickstart=xTaskGetTickCount();
            while(EE_CTRL_Get_Busy_Flag())
            {
                _tick=xTaskGetTickCount();
			    if((_tick-_tickstart)>1000U){break;}
            };
            /* System power off! */
            HAL_GPIO_WritePin(BSP_LTC_KILL_PORT,BSP_LTC_KILL_PIN,GPIO_PIN_RESET);
        }

        if((_soc>LTC2942_CTRL_ERROR_SOC)&&(H_LTC_CTRL.errorFlag))
        {
            BUZZER_Driver_Stop();
            H_LTC_CTRL.buzzerActiveFlag=0U;
            H_LTC_CTRL.errorFlag=0U;
        }

        /* Reset battery capacity */
        if(PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_4))
        {
            if((++H_LTC_CTRL.buttonTmo>=(LTC2942_CTRL_RESET_TMO/LTC2942_CTRL_TASK_TMO))&&(!H_LTC_CTRL.resetFlag))
            {
                H_LTC_CTRL.buttonTmo=LTC2942_CTRL_RESET_TMO/LTC2942_CTRL_TASK_TMO;
                H_LTC_CTRL.resetFlag=1U;
                LTC2942_Reset_Battery_Capacity(&H_LTC_CTRL.ltcHandle);
                LED_Blink();
                H_LTC_CTRL.updateTmo=4U;
            }
            else if(H_LTC_CTRL.resetFlag){H_LTC_CTRL.buttonTmo=LTC2942_CTRL_RESET_TMO/LTC2942_CTRL_TASK_TMO;}
        }
        else
        {
            H_LTC_CTRL.buttonTmo=0U;
            H_LTC_CTRL.resetFlag=0U;
        }

        vTaskDelayUntil(&_xLastWakeTime,pdMS_TO_TICKS(LTC2942_CTRL_TASK_TMO));
	}

	/* Task Error */
	vTaskDelete(NULL);
}

/**
  ***************************************************************************************************************************************
  * @brief  LTC2942 get buzzer active flag
  * @param  None
  * @retval Flag (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t LTC2942_CTRL_Get_Buzzer_Active_Flag(void)
{
    return H_LTC_CTRL.buzzerActiveFlag;
}
