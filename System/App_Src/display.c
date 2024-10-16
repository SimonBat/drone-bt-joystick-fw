/**
  ***************************************************************************************************************************************
  * @file     display.c
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

#include "display.h"
#include "ssd1306.h"
#include "ssd1306_logo.h"
#include "ssd1306_draw.h"
#include "profile.h"
#include "rn4020.h"

static display_ts H_DISPLAY;

/* Local functions declarations */
static void DISPLAY_Init_Handler(void *_params);
static void DISPLAY_Update_Handler(void *_params);
static void DISPLAY_Data_1_Context(void);
static void DISPLAY_Data_2_Context(void);
static void DISPLAY_Data_3_Context(void);

typedef void (*const f_display_context)(void);
f_display_context DISPLAY_DATA_CONTEXT[DISPLAY_CONTEXTS]={
    DISPLAY_Data_1_Context, \
	DISPLAY_Data_2_Context, \
    DISPLAY_Data_3_Context
};

/**
  ***************************************************************************************************************************************
  * @brief  Display module initalization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void DISPLAY_Init(void)
{
    H_DISPLAY.hTaskInit=xTaskCreateStatic(DISPLAY_Init_Handler,"DISPLAY_Init_Handler",DISPLAY_TASK_STACK_SIZE, \
    									  NULL,(tskIDLE_PRIORITY+1U),H_DISPLAY.hTaskInitStack,&H_DISPLAY.hTaskInitBuffer);
    H_DISPLAY.hTaskUpdate=xTaskCreateStatic(DISPLAY_Update_Handler,"DISPLAY_Update_Handler",DISPLAY_TASK_STACK_SIZE, \
    										NULL,(tskIDLE_PRIORITY+1U),H_DISPLAY.hTaskUpdateStack,&H_DISPLAY.hTaskUpdateBuffer);

    if((NULL!=H_DISPLAY.hTaskInit)&&(NULL!=H_DISPLAY.hTaskUpdate)){vTaskSuspend(H_DISPLAY.hTaskUpdate);}
	else{BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  Display initialization handler
  * @param  Parameters(void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void DISPLAY_Init_Handler(void *_params)
{
    while(1U)
	{
        SSD1306_Driver_Init();
        SSD1306_Draw_Logo();
        H_DISPLAY.initFlag=1U;
        DISPLAY_Set_Data_Context(DISPLAY_DATA_1_CONTEXT);
        vTaskDelay(pdMS_TO_TICKS(DISPLAY_LOGO_TMO));
		vTaskResume(H_DISPLAY.hTaskUpdate);
		vTaskSuspend(NULL);
	}

	/* Task Error */
	vTaskDelete(NULL);
}

/**
  ***************************************************************************************************************************************
  * @brief  Display update handler
  * @param  Parameters(void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void DISPLAY_Update_Handler(void *_params)
{
    TickType_t _xLastWakeTime=xTaskGetTickCount();

    while(1U)
	{
        DISPLAY_DATA_CONTEXT[H_DISPLAY.dataContext]();
        SSD1306_Driver_Update();
        vTaskDelayUntil(&_xLastWakeTime, pdMS_TO_TICKS(DISPLAY_UPDATE_TMO));
	}

	/* Task Error */
	vTaskDelete(NULL);
}

/**
  ***************************************************************************************************************************************
  * @brief  Display data 1 context
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
static void DISPLAY_Data_1_Context(void)
{
    char _strBuffer[16];

    SSD1306_Fill(OLED_COLOR_BLACK);
    SSD1306_Draw_String(25U,22U,0U,"CONNECTING",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(25U,34U,0U,"TO DRONE...",&TM_Font_7x10,OLED_COLOR_WHITE);
    sprintf(_strBuffer,"#%1d%%",(RN4020_Get_Target_Address_ID()+1));
	SSD1306_Draw_String(114U,50U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);
}

/**
  ***************************************************************************************************************************************
  * @brief  Display data 2 context
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
static void DISPLAY_Data_2_Context(void)
{
    char _strBuffer[16];

    SSD1306_Fill(OLED_COLOR_BLACK);
    SSD1306_Draw_String(0U,0U,0U,"BJ:",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(56U,0U,0U,"RS:",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(56U,10U,0U,"TX:",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(0U,10U,0U,"BD:",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(0U,20U,0U,"THROTTLE:",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(0U,30U,0U,"ROLL:",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(0U,40U,0U,"PITCH:",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(0U,50U,0U,"YAW:",&TM_Font_7x10,OLED_COLOR_WHITE);

    /* Joystick battery value */
	sprintf(_strBuffer,"%3d%%",PROFILE_Get_Joystick_Battery_SOC());
	SSD1306_Draw_String(21U,0U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);

    /* Drone battery value */
	sprintf(_strBuffer,"%3d%%",PROFILE_Get_Drone_Battery_SOC());
	SSD1306_Draw_String(21U,10U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);

    /* Joystick RSSI value */
	sprintf(_strBuffer,"%4ddBm",PROFILE_Get_Joystick_RSSI_Value());
	SSD1306_Draw_String(77U,0U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_RSSI(PROFILE_Get_Joystick_RSSI_Value());

    /* Drone TX Power value */
	sprintf(_strBuffer,"%4ddBm",PROFILE_Get_Drone_TX_Power_Value());
	SSD1306_Draw_String(77U,10U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);

    /* Throttle */
    sprintf(_strBuffer,"%3d",PROFILE_Get_Joystick_V1_Status());
    SSD1306_Draw_String(63U,20U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);
    /* Roll */
    sprintf(_strBuffer,"%3d",PROFILE_Get_Joystick_H2_Status());
    SSD1306_Draw_String(63U,30U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);
    /* Pitch */
    sprintf(_strBuffer,"%3d",PROFILE_Get_Joystick_V2_Status());
    SSD1306_Draw_String(49U,40U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);
    /* YAW */
    sprintf(_strBuffer,"%3d",PROFILE_Get_Joystick_H1_Status());
    SSD1306_Draw_String(49U,50U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);

    if(PROFILE_Get_Joystick_SEL1_Status()&&(!PROFILE_Get_Joystick_SEL2_Status())){SSD1306_Draw_Arrow_UP(98U,35U);}
    else if(PROFILE_Get_Joystick_SEL2_Status()&&(!PROFILE_Get_Joystick_SEL1_Status())){SSD1306_Draw_Arrow_DOWN(98U,35U);}
    else{SSD1306_Draw_Arrow_UP_DOWN(103U,40U);}

    if(RN4020_Get_Connection_Status()){SSD1306_Draw_Button_State(77U,49U,PROFILE_Get_Joystick_Video_EN_Status());}
    else{SSD1306_Draw_Button_State(77U,49U,PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_1));}
    SSD1306_Draw_Button_State(91U,49U,PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_2));
    SSD1306_Draw_Button_State(105U,49U,PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_3));
    SSD1306_Draw_Button_State(119U,49U,PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_4));
}

/**
  ***************************************************************************************************************************************
  * @brief  Display data 3 context
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
static void DISPLAY_Data_3_Context(void)
{
    char _strBuffer[16];

    SSD1306_Fill(OLED_COLOR_BLACK);
    SSD1306_Draw_String(0U,0U,0U,"BJ:",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(56U,0U,0U,"RS:",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(56U,10U,0U,"TX:",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(0U,10U,0U,"BD:",&TM_Font_7x10,OLED_COLOR_WHITE);

    /* Joystick battery value */
	sprintf(_strBuffer,"%3d%%",PROFILE_Get_Joystick_Battery_SOC());
	SSD1306_Draw_String(21U,0U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);

    /* Drone battery value */
	sprintf(_strBuffer,"%3d%%",PROFILE_Get_Drone_Battery_SOC());
	SSD1306_Draw_String(21U,10U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);

    /* Joystick RSSI value */
	sprintf(_strBuffer,"%4ddBm",PROFILE_Get_Joystick_RSSI_Value());
	SSD1306_Draw_String(77U,0U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);

    /* Drone TX Power value */
	sprintf(_strBuffer,"%4ddBm",PROFILE_Get_Drone_TX_Power_Value());
	SSD1306_Draw_String(77U,10U,0U,_strBuffer,&TM_Font_7x10,OLED_COLOR_WHITE);

    SSD1306_Draw_String(25U,30U,0U,"Empty drone",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(36U,40U,0U,"battery!",&TM_Font_7x10,OLED_COLOR_WHITE);
    SSD1306_Draw_String(1U,50U,0U,"Automatic landing!",&TM_Font_7x10,OLED_COLOR_WHITE);
}

/**
  ***************************************************************************************************************************************
  * @brief  Display set data context
  * @param  Data context (display_data_te)
  * @retval None
  ***************************************************************************************************************************************
  */
void DISPLAY_Set_Data_Context(display_data_te _context)
{
    H_DISPLAY.dataContext=_context;
}
