/**
  ***************************************************************************************************************************************
  * @file     system.c
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

#include "system.h"
#include "buzzer.h"
#include "i2c.h"
#include "display.h"
#include "ltc2942_ctrl.h"
#include "eeprom_ctrl.h"
#include "input_ctrl.h"
#include "profile.h"
#include "led.h"
#include "debug.h"
#include "rn4020.h"

/* Global system handle */
static system_ts H_SYSTEM;

/* Local functions declarations */
static void SYSTEM_Task_Handler(void *_params);
static void SYSTEM_EE_Init(void);

/**
  ***************************************************************************************************************************************
  * @brief  The application entry point
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick */
    HAL_Init();
    /* Configure the system clock */
    BSP_System_Clock_Config();
    SYSTEM_EE_Init();

    /* Initialize all configured peripherals */
    BSP_System_GPIO_Init();
    I2C_Driver_Init();
    HAL_Delay(20U);
    BUZZER_Driver_Init();
    EE_CTRL_Init();
    LTC2942_CTRL_Init();
    DISPLAY_Init();
    INPUT_CTRL_Init();
    LED_Init();
    DEBUG_Init();

    H_SYSTEM.hTask=xTaskCreateStatic(SYSTEM_Task_Handler,"SYSTEM_Task_Handler", \
    								 SYSTEM_TASK_STACK_SIZE,NULL,(tskIDLE_PRIORITY+1U),H_SYSTEM.hTaskStack,&H_SYSTEM.hTaskBuffer);

    if(NULL!=H_SYSTEM.hTask){vTaskStartScheduler();}

    while(1U)
    {
        /* System Erro Indication */
        LED_On();
        HAL_Delay(100U);
        LED_Off();
        HAL_Delay(100U);
    };

    return 0;
}

/**
  ***************************************************************************************************************************************
  * @brief  Main system Task handler
  * @param  Parameters (void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void SYSTEM_Task_Handler(void *_params)
{
    TickType_t _xLastWakeTime=xTaskGetTickCount();

    BUZZER_Driver_Start(BUZZER_MELODY_ID_STARTUP, BUZZER_MODE_AUTOSTOP);
    DEBUG_Send_Message("DRAGONFLY Drone Joystick v1.0\r\n\r\n");
    RN4020_Init();
    
    H_SYSTEM.changeBTIdTmo=SYSTEM_CHANGE_BT_ID_TMO;

	while(1U)
	{
        /* Video enable status control */
        if(RN4020_Get_Connection_Status())
        {
            if(PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_1)&&(!H_SYSTEM.videoEnStatusState))
            {
                H_SYSTEM.videoEnStatusState=1U;
                if(!H_SYSTEM.videoEnStatus){H_SYSTEM.videoEnStatus=1U; LED_Blink();}
                else{H_SYSTEM.videoEnStatus=0U; LED_Blink();}
                PROFILE_Set_Joystick_Video_EN_Status(H_SYSTEM.videoEnStatus);
            }
            else if(!PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_1)){H_SYSTEM.videoEnStatusState=0U;}

            if(H_SYSTEM.droneBatteryMonitorTmo>=(SYSTEM_DRONE_BATTERY_MONITOR_TMO/SYSTEM_TASK_TMO))
            {
                if((PROFILE_Get_Drone_Battery_SOC()<=SYSTEM_DRONE_BATTERY_SOC_ERROR)&&(!H_SYSTEM.droneBatteryErrorFlag))
                {
                    BUZZER_Driver_Start(BUZZER_MELODY_ID_ERROR, BUZZER_MODE_CONTINUOUS);
                    H_SYSTEM.droneBatteryErrorFlag=1U;
                }
            }
            else{H_SYSTEM.droneBatteryMonitorTmo++;}

            if(H_SYSTEM.displayContextTmo>=(SYSTEM_DISPLAY_CONTEXT_TMO/SYSTEM_TASK_TMO))
            {
                if(H_SYSTEM.droneBatteryErrorFlag){DISPLAY_Set_Data_Context(DISPLAY_DATA_3_CONTEXT);}
                else{DISPLAY_Set_Data_Context(DISPLAY_DATA_2_CONTEXT);}
            }
            else{H_SYSTEM.displayContextTmo++;}
        }
        else
        {
            if(H_SYSTEM.droneBatteryErrorFlag)
            {
                if(!LTC2942_CTRL_Get_Buzzer_Active_Flag()){BUZZER_Driver_Stop();}
                H_SYSTEM.droneBatteryErrorFlag=0U;
            }

            if((PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_2))&&(!H_SYSTEM.changeBTIdTmo))
            {
                H_SYSTEM.changeBTIdTmo=SYSTEM_CHANGE_BT_ID_TMO;
                RN4020_Change_Target_Address();
            }
            else if(H_SYSTEM.changeBTIdTmo){H_SYSTEM.changeBTIdTmo--;}

            H_SYSTEM.videoEnStatus=0U;
            H_SYSTEM.videoEnStatusState=0U;
            H_SYSTEM.droneBatteryMonitorTmo=0U;
            H_SYSTEM.displayContextTmo=0U;
            PROFILE_Set_Joystick_Video_EN_Status(H_SYSTEM.videoEnStatus);
            DISPLAY_Set_Data_Context(DISPLAY_DATA_1_CONTEXT);
        }

		vTaskDelayUntil(&_xLastWakeTime,pdMS_TO_TICKS(SYSTEM_TASK_TMO));
	}

	/* Task Error */
	vTaskDelete(NULL);
}

/**
  ***************************************************************************************************************************************
  * @brief System eeprom initialization
  * @param None
  * @retval None
  ***************************************************************************************************************************************
  */
static void SYSTEM_EE_Init(void)
{
    uint16_t _status;
    uint16_t _eeValue;

    /* Unlock the Flash Program Erase controller */
	HAL_FLASH_Unlock();
    /* EEPROM Init */
    EE_Init();

    _status=EE_Read_Variable(EE_SETTINGS_BATTERY_CAPACITY,&_eeValue);
    if((!_status)&&(0x0000!=_eeValue)&&(0xFFFF!=_eeValue)){PROFILE_Settings_Set_Joystick_Battery_Capacity(_eeValue);}
    else
    {
        PROFILE_Settings_Set_Joystick_Battery_Capacity(LTC2942_BATTERY_CAPACITY);
        EE_Write_Variable(EE_SETTINGS_BATTERY_CAPACITY,LTC2942_BATTERY_CAPACITY);
    }

    _status=EE_Read_Variable(EE_SETTINGS_ADC_AVG_LEVEL,&_eeValue);
    if((!_status)&&(_eeValue<=INPUT_CTRL_ADC_AVG_LEVEL_MAX)){PROFILE_Settings_Set_Joystick_ADC_AVG_Level(_eeValue);}
    else
    {
        PROFILE_Settings_Set_Joystick_ADC_AVG_Level(INPUT_CTRL_ADC_AVG_LEVEL_DEFAULT);
        EE_Write_Variable(EE_SETTINGS_ADC_AVG_LEVEL,INPUT_CTRL_ADC_AVG_LEVEL_DEFAULT);
    }

    /* Lock the Flash Program Erase controller */
	HAL_FLASH_Lock();
}
