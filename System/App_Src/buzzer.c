/**
  ***************************************************************************************************************************************
  * @file     buzzer.c
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

#include "buzzer.h"

/* Startup melody */
uint16_t BUZZER_MELODY_STARTUP[]={
    BUZZER_NOTE_B4,16U, \
	BUZZER_NOTE_A4,16U, \
	BUZZER_NOTE_CS4,8U, \
	BUZZER_NOTE_E4,8U, \
	BUZZER_NOTE_A4,4U
};

/* Warning/Fault melody */
uint16_t BUZZER_MELODY_ERROR[]={
	BUZZER_NOTE_A4,16U, \
	BUZZER_NOTE_RST,32U, \
  	BUZZER_NOTE_A4,16U, \
	BUZZER_NOTE_RST,32U, \
    BUZZER_NOTE_A4,16U, \
	BUZZER_NOTE_RST,32U, \
  	BUZZER_NOTE_A4,16U, \
	BUZZER_NOTE_RST,32U, \
	BUZZER_NOTE_A4,16U, \
	BUZZER_NOTE_RST,32U
};

/* Global buzzer object */
static buzzer_drv_ts BUZZER_DRV;
/* Buzzer melody list */
uint16_t *BUZZER_MELODY_LIST[BUZZER_MELODY_SYSTEM_USE]={&BUZZER_MELODY_STARTUP[0],&BUZZER_MELODY_ERROR[0]};
uint16_t BUZZER_MELODY_SIZE[BUZZER_MELODY_SYSTEM_USE]={sizeof(BUZZER_MELODY_STARTUP),sizeof(BUZZER_MELODY_ERROR)};

/* Local functions declarations */
static void BUZZER_Peripheral_Init(void);
static void BUZZER_Note_Play_Start(uint16_t _noteFrequency, uint8_t _duty);
static void BUZZER_Note_Play_Stop(void);
static void BUZZER_Driver_Handler_Task(void *_params);

/**
  ***************************************************************************************************************************************
  * @brief  Buzzer peripheral initialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
static void BUZZER_Peripheral_Init(void)
{
	/** TIM input clock (TIMCLK) is set to APB1 clock (PCLK1), since APB1 prescaler is equal to 1.
	  *	TIMCLK=PCLK1
	  *	PCLK1=HCLK
	  *	=>TIMCLK=HCLK=SystemCoreClock
	  *	To get TIM counter clock at 1 MHz, the prescaler is computed as follows:
	  *	Prescaler=(TIMCLK/TIM counter clock)-1
	  *	Prescaler=((SystemCoreClock)/1MHz)-1
	  *	To get TIM output clock at 1KHz, the period (ARR)) is computed as follows:
	  *	ARR=(TIM counter clock/TIM output clock)-1=1000-1
	  *	TIM Channel duty cycle = (TIM_CCR/ TIM_ARR + 1)* 100 = 50%
	  */
	BUZZER_DRV.timPWM.Instance=BUZZER_PWM_TIM;
	HAL_TIM_PWM_DeInit(&BUZZER_DRV.timPWM);
	BUZZER_DRV.timPWM.Init.Prescaler=(SystemCoreClock/1000000U)-1U;
	BUZZER_DRV.timPWM.Init.Period=BUZZER_PWM_DEFAULT_PERIOD-1U;
	BUZZER_DRV.timPWM.Init.CounterMode=TIM_COUNTERMODE_UP;
	BUZZER_DRV.timPWM.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	BUZZER_DRV.timPWM.Init.AutoReloadPreload=TIM_AUTORELOAD_PRELOAD_DISABLE;
	if(HAL_OK!=HAL_TIM_PWM_Init(&BUZZER_DRV.timPWM)){BSP_Error_Handler();}
	BUZZER_DRV.timPWMCfg.OCMode=TIM_OCMODE_PWM1;
	BUZZER_DRV.timPWMCfg.OCPolarity=TIM_OCPOLARITY_HIGH;
	BUZZER_DRV.timPWMCfg.OCFastMode=TIM_OCFAST_DISABLE;
	BUZZER_DRV.timPWMCfg.OCNPolarity=TIM_OCNPOLARITY_HIGH;
	BUZZER_DRV.timPWMCfg.OCNIdleState=TIM_OCNIDLESTATE_RESET;
	BUZZER_DRV.timPWMCfg.OCIdleState=TIM_OCIDLESTATE_RESET;
	BUZZER_DRV.timPWMCfg.Pulse=BUZZER_PWM_DEFAULT_PERIOD*BUZZER_PWM_DEFAULT_DUTY/100U-1U;
	if(HAL_OK!=HAL_TIM_PWM_ConfigChannel(&BUZZER_DRV.timPWM,&BUZZER_DRV.timPWMCfg,BUZZER_PWM_CHANNEL)){BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief HAL timer PWM initialization
  * @param TIM handle pointer (TIM_HandleTypeDef*)
  * @retval None
  ***************************************************************************************************************************************
  */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *_htim)
{
	GPIO_InitTypeDef _gpioInitStruct={0};

	/* TIM Peripheral clock enable */
	BUZZER_PWM_CLOCK_ENABLE();

	_gpioInitStruct.Mode=GPIO_MODE_AF_PP;
	_gpioInitStruct.Pull=GPIO_NOPULL;
	_gpioInitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
	_gpioInitStruct.Pin=BUZZER_PWM_PIN;
	HAL_GPIO_Init(BUZZER_PWM_PORT,&_gpioInitStruct);
}

/**
  ***************************************************************************************************************************************
  * @brief HAL timer PWM deinitialization
  * @param TIM handle pointer (TIM_HandleTypeDef*)
  * @retval None
  ***************************************************************************************************************************************
  */
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *_htim)
{
	/* TIM Peripheral clock disable */
	BUZZER_PWM_CLOCK_DISABLE();
	HAL_GPIO_DeInit(BUZZER_PWM_PORT,BUZZER_PWM_PIN);
}

/**
  ***************************************************************************************************************************************
  * @brief  Buzzer play note start function
  * @param  Note frequency (uint16_t), duty cycle (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
static void BUZZER_Note_Play_Start(uint16_t _noteFrequency, uint8_t _duty)
{
	uint32_t _period=1000000UL/_noteFrequency-1U;
	BUZZER_DRV.timPWM.Init.Period=_period;
	BUZZER_DRV.timPWMCfg.Pulse=_period*_duty/100U;
	HAL_TIM_PWM_Init(&BUZZER_DRV.timPWM);
	HAL_TIM_PWM_ConfigChannel(&BUZZER_DRV.timPWM,&BUZZER_DRV.timPWMCfg,BUZZER_PWM_CHANNEL);
	HAL_TIM_PWM_Start(&BUZZER_DRV.timPWM,BUZZER_PWM_CHANNEL);
}

/**
  ***************************************************************************************************************************************
  * @brief  Buzzer play note stop function
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
static void BUZZER_Note_Play_Stop(void)
{
	HAL_TIM_PWM_Stop(&BUZZER_DRV.timPWM,BUZZER_PWM_CHANNEL);
}

/**
  ***************************************************************************************************************************************
  * @brief  Buzzer driver initialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void BUZZER_Driver_Init(void)
{
	if(BUZZER_DRV.initFlg){return;}
	/* Init buzzer peripherals */
	BUZZER_Peripheral_Init();
	/* Create buzzer driver handler task */
	BUZZER_DRV.hTask=xTaskCreateStatic(BUZZER_Driver_Handler_Task,"BUZZER_Driver_Handler_Task", \
									   BUZZER_TASK_STACK_SIZE,NULL,(tskIDLE_PRIORITY+1U),BUZZER_DRV.hTaskStack,&BUZZER_DRV.hTaskBuffer);

    if(NULL!=BUZZER_DRV.hTask){BUZZER_DRV.initFlg=1U;}
    else{BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  Buzzer driver peripheral update
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void BUZZER_Driver_Update(void)
{
	if(BUZZER_DRV.initFlg){BUZZER_Peripheral_Init();}
}

/**
  ***************************************************************************************************************************************
  * @brief  Buzzer driver start
  * @param  Melody (buzzer_melody_te), mode (buzzer_mode_te)
  * @retval Buzzer state (buzzer_state_te)
  ***************************************************************************************************************************************
  */
buzzer_state_te BUZZER_Driver_Start(buzzer_melody_te _melody, buzzer_mode_te _mode)
{
    if(!BUZZER_DRV.initFlg){return BUZZER_STATE_OFF;}

	if(BUZZER_STATE_ON==BUZZER_DRV.state){return BUZZER_STATE_BUSY;}
	else
	{
		BUZZER_DRV.state=BUZZER_STATE_ON;
		BUZZER_DRV.mode=_mode;
		BUZZER_DRV.melody=_melody;
		/* Notify buzzer driver task to unblock */
		xTaskNotify(BUZZER_DRV.hTask,0x00,eNoAction);

		return BUZZER_DRV.state;
	}
}

/**
  ***************************************************************************************************************************************
  * @brief  Buzzer driver stop
  * @param  None
  * @retval Buzzer state (buzzer_state_te)
  ***************************************************************************************************************************************
  */
buzzer_state_te BUZZER_Driver_Stop(void)
{
	BUZZER_DRV.state=BUZZER_STATE_OFF;

	return BUZZER_DRV.state;
}

/**
  ***************************************************************************************************************************************
  * @brief  Buzzer driver get state
  * @param  None
  * @retval Buzzer state (buzzer_state_te)
  ***************************************************************************************************************************************
  */
buzzer_state_te BUZZER_Driver_Get_State(void)
{
	return BUZZER_DRV.state;
}

/**
  ***************************************************************************************************************************************
  * @brief  Buzzer driver handler task
  * @param  Parameters (void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void BUZZER_Driver_Handler_Task(void *_params)
{
	TickType_t _xLastWakeTime;
	uint16_t _thisNote;
	uint16_t _notes;
	uint32_t _noteDuration;

	while(1U)
	{
		/* Wait until we receive notification from BUZZER_Driver_Start() */
		if(pdTRUE==xTaskNotifyWait(0U,0U,NULL,portMAX_DELAY))
		{
			_xLastWakeTime=xTaskGetTickCount();

			while(BUZZER_STATE_OFF!=BUZZER_DRV.state)
			{
				_notes=BUZZER_MELODY_SIZE[BUZZER_DRV.melody]/sizeof(BUZZER_MELODY_LIST[BUZZER_DRV.melody][0])/2U;

				for(_thisNote=0U;_thisNote<_notes*2U;_thisNote+=2U)
				{
					_noteDuration=1000U/BUZZER_MELODY_LIST[BUZZER_DRV.melody][_thisNote+1U];
					if(BUZZER_NOTE_RST!=BUZZER_MELODY_LIST[BUZZER_DRV.melody][_thisNote])
					{BUZZER_Note_Play_Start(BUZZER_MELODY_LIST[BUZZER_DRV.melody][_thisNote],BUZZER_PWM_DEFAULT_DUTY);}
					else{BUZZER_Note_Play_Stop();}
					vTaskDelayUntil(&_xLastWakeTime,pdMS_TO_TICKS(_noteDuration));
					BUZZER_Note_Play_Stop();
					vTaskDelayUntil(&_xLastWakeTime,pdMS_TO_TICKS(_noteDuration*0.8f));
				}
				if(BUZZER_MODE_AUTOSTOP==BUZZER_DRV.mode){BUZZER_DRV.state=BUZZER_STATE_OFF;}
			}
		}
	}

	/* Task Error */
	vTaskDelete(NULL);
}
