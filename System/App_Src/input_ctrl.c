/**
  ***************************************************************************************************************************************
  * @file     input_ctrl.c
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

#include "input_ctrl.h"
#include "eeprom_ctrl.h"
#include "profile.h"
#include "led.h"

static input_ctrl_ts H_ICTRL;

static uint32_t INPUT_CTRL_ADC_CH_ORDER[INPUT_CTRL_ADC_CH]={
    INPUT_CTRL_V1_ADC_CH, \
    INPUT_CTRL_H1_ADC_CH, \
    INPUT_CTRL_V2_ADC_CH, \
    INPUT_CTRL_H2_ADC_CH
};

typedef void (*const f_ictrl_set_profile_status)(uint8_t);
f_ictrl_set_profile_status INPUT_CTRL_SET_PROFILE_STATUS[INPUT_CTRL_ADC_CH]={
	PROFILE_Set_Joystick_V1_Status, \
	PROFILE_Set_Joystick_H1_Status, \
    PROFILE_Set_Joystick_V2_Status, \
    PROFILE_Set_Joystick_H2_Status
};

/* Local functions declarations */
static void INPUT_CTRL_ADC_Task_Handler(void *_params);
static void INPUT_CTRL_GPI_Task_Handler(void *_params);
static uint16_t INPUT_CTRL_Read_ADC_Channel_Value(ADC_HandleTypeDef* _hadc,uint32_t _channel);
static void INPUT_CTRL_ADC_FILTER_Init(input_ctrl_filter_ts *_filter,float _threshold,float _snap,uint16_t _max, \
                                       input_ctrl_filter_sleep_te _sleep,input_ctrl_filter_edge_te _edge);
static float INPUT_CTRL_ADC_FILTER_Process(input_ctrl_filter_ts *_filter, float _newValue);
static uint16_t INPUT_CTRL_ADC_FILTER_Update(input_ctrl_filter_ts *_filter, float _newValue);
static float INPUT_CTRL_ADC_FILTER_Snap_Curve(float _x);

/**
  ***************************************************************************************************************************************
  * @brief  Input control initialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void INPUT_CTRL_Init(void)
{
    GPIO_InitTypeDef _gpioInitStruct={0};

    _gpioInitStruct.Mode=GPIO_MODE_INPUT;
    _gpioInitStruct.Pull=GPIO_PULLUP;
    _gpioInitStruct.Pin=BSP_SEL1_PIN;
    HAL_GPIO_Init(BSP_SEL1_PORT,&_gpioInitStruct);
    _gpioInitStruct.Pin=BSP_SEL2_PIN;
    HAL_GPIO_Init(BSP_SEL2_PORT,&_gpioInitStruct);
    _gpioInitStruct.Pin=BSP_BUTTON1_PIN;
    HAL_GPIO_Init(BSP_BUTTON1_PORT,&_gpioInitStruct);
    _gpioInitStruct.Pin=BSP_BUTTON2_PIN;
    HAL_GPIO_Init(BSP_BUTTON2_PORT,&_gpioInitStruct);
    _gpioInitStruct.Pin=BSP_BUTTON3_PIN;
    HAL_GPIO_Init(BSP_BUTTON3_PORT,&_gpioInitStruct);
    _gpioInitStruct.Pin=BSP_BUTTON4_PIN;
    HAL_GPIO_Init(BSP_BUTTON4_PORT,&_gpioInitStruct);

	H_ICTRL.hTaskAdc=xTaskCreateStatic(INPUT_CTRL_ADC_Task_Handler,"INPUT_CTRL_ADC_Handler",INPUT_CTRL_TASK_STACK_SIZE, \
	    							   NULL,(tskIDLE_PRIORITY+1U),H_ICTRL.hTaskAdcStack,&H_ICTRL.hTaskAdcBuffer);
    H_ICTRL.hTaskGpi=xTaskCreateStatic(INPUT_CTRL_GPI_Task_Handler,"INPUT_CTRL_GPI_Handler",INPUT_CTRL_TASK_STACK_SIZE, \
        									 NULL,(tskIDLE_PRIORITY+1U),H_ICTRL.hTaskGpiStack,&H_ICTRL.hTaskGpiBuffer);

	if((NULL!=H_ICTRL.hTaskAdc)&&(NULL!=H_ICTRL.hTaskGpi))
    {
        /* Channels filter Init */
	    for(uint8_t _idx=0U;_idx<INPUT_CTRL_ADC_CH;_idx++)
	    {
		    INPUT_CTRL_ADC_FILTER_Init(&H_ICTRL.adcFilter[_idx],INPUT_CTRL_FT_TH, \
                                       INPUT_CTRL_FT_SNAP,4095U,INPUT_CTRL_FILTER_SLEEP_ENABLE,INPUT_CTRL_FILTER_EDGE_ENABLE);
	    }

        H_ICTRL.hadc.Instance=ADC1;
	    H_ICTRL.hadc.Init.ScanConvMode=ADC_SCAN_DISABLE;
	    H_ICTRL.hadc.Init.ContinuousConvMode=DISABLE;
	    H_ICTRL.hadc.Init.DiscontinuousConvMode=DISABLE;
	    H_ICTRL.hadc.Init.ExternalTrigConv=ADC_SOFTWARE_START;
	    H_ICTRL.hadc.Init.DataAlign=ADC_DATAALIGN_RIGHT;
	    H_ICTRL.hadc.Init.NbrOfConversion=1U;
	    HAL_ADC_Init(&H_ICTRL.hadc);
	    HAL_ADCEx_Calibration_Start(&H_ICTRL.hadc);
        H_ICTRL.initFlag=1U;
    }
	else{BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  ADC MSP Initialization
  * @param  ADC handle (ADC_HandleTypeDef*)
  * @retval None
  ***************************************************************************************************************************************
  */
// cppcheck-suppress constParameterPointer
void HAL_ADC_MspInit(ADC_HandleTypeDef* _hadc)
{
    GPIO_InitTypeDef _gpioInitStruct={0};

    if(ADC1==_hadc->Instance)
    {
        /* Peripheral clock enable */
        __HAL_RCC_ADC1_CLK_ENABLE();

        _gpioInitStruct.Mode=GPIO_MODE_ANALOG;
        _gpioInitStruct.Pin=BSP_V1_ADC_PIN;
        HAL_GPIO_Init(BSP_V1_ADC_PORT, &_gpioInitStruct);
        _gpioInitStruct.Pin=BSP_H1_ADC_PIN;
        HAL_GPIO_Init(BSP_H1_ADC_PORT, &_gpioInitStruct);
        _gpioInitStruct.Pin=BSP_V2_ADC_PIN;
        HAL_GPIO_Init(BSP_V2_ADC_PORT, &_gpioInitStruct);
        _gpioInitStruct.Pin=BSP_H2_ADC_PIN;
        HAL_GPIO_Init(BSP_H2_ADC_PORT, &_gpioInitStruct);
    }
}

/**
  ***************************************************************************************************************************************
  * @brief  ADC MSP De-Initialization
  * @param  ADC handle (ADC_HandleTypeDef*)
  * @retval None
  ***************************************************************************************************************************************
  */
// cppcheck-suppress constParameterPointer
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* _hadc)
{
    if(ADC1==_hadc->Instance)
    {
        /* Peripheral clock disable */
        __HAL_RCC_ADC1_CLK_DISABLE();

        HAL_GPIO_DeInit(BSP_V1_ADC_PORT,BSP_V1_ADC_PIN);
        HAL_GPIO_DeInit(BSP_H1_ADC_PORT,BSP_H1_ADC_PIN);
        HAL_GPIO_DeInit(BSP_V2_ADC_PORT,BSP_V2_ADC_PIN);
        HAL_GPIO_DeInit(BSP_H2_ADC_PORT,BSP_H2_ADC_PIN);
    }
}

/**
  ***************************************************************************************************************************************
  * @brief  Input control ADC handler task
  * @param  Parameters (void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void INPUT_CTRL_ADC_Task_Handler(void *_params)
{
    uint8_t _idx;
    uint8_t _idj;
    uint16_t _adcValue;
    uint16_t _adcAvgLevel;
    float _adcValueF;
    uint32_t _adcCalibrationValue;
    TickType_t _xLastWakeTime=xTaskGetTickCount();

    while(1U)
	{
    	if(H_ICTRL.adcCalibrationFlag)
        {
            for(_idx=0U;_idx<INPUT_CTRL_ADC_CH;_idx++)
            {
                _adcValue=INPUT_CTRL_Read_ADC_Channel_Value(&H_ICTRL.hadc,INPUT_CTRL_ADC_CH_ORDER[_idx]);
                _adcValue=INPUT_CTRL_ADC_FILTER_Update(&H_ICTRL.adcFilter[_idx],_adcValue);

                if(_adcValue>(H_ICTRL.adcCalibrationValue[_idx]+INPUT_CTRL_ADC_THRESHOLD))
	    	    {
	    		    _adcValue=_adcValue-H_ICTRL.adcCalibrationValue[_idx];
	    		    _adcValueF=(float)_adcValue*50.0f/(4095.0f-(float)H_ICTRL.adcCalibrationValue[_idx])+50.0f;
	    	    }
	    	    else if(_adcValue<H_ICTRL.adcCalibrationValue[_idx]-INPUT_CTRL_ADC_THRESHOLD)
	    	    {
	    		    _adcValue=H_ICTRL.adcCalibrationValue[_idx]-_adcValue;
	    		    _adcValueF=50.0f-_adcValue*50.0f/((float)H_ICTRL.adcCalibrationValue[_idx]);
	    	    }
                else{_adcValueF=50.0f;}

                if(_adcValueF<0.0f){_adcValueF=0.0f;}
                _adcAvgLevel=PROFILE_Settings_Get_Joystick_ADC_AVG_Level();
                H_ICTRL.adcValueAvg[_idx]=(H_ICTRL.adcValueAvg[_idx]*(float)_adcAvgLevel+_adcValueF)/((float)_adcAvgLevel+1.0f);

                INPUT_CTRL_SET_PROFILE_STATUS[_idx]((uint8_t)(H_ICTRL.adcValueAvg[_idx]+0.5f));
            }

            /* Change ADC averaging level */
            if(PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_3))
            {
                if((++H_ICTRL.buttonTmo>=(INPUT_CTRL_BUTTON_TMO/INPUT_CTRL_ADC_TASK_TMO))&&(!H_ICTRL.buttonFlag))
                {
                    H_ICTRL.buttonTmo=INPUT_CTRL_BUTTON_TMO/INPUT_CTRL_ADC_TASK_TMO;
                    H_ICTRL.buttonFlag=1U;
                    _adcAvgLevel=PROFILE_Settings_Get_Joystick_ADC_AVG_Level();
                    if(++_adcAvgLevel>INPUT_CTRL_ADC_AVG_LEVEL_MAX){_adcAvgLevel=0U;};
                    PROFILE_Settings_Set_Joystick_ADC_AVG_Level(_adcAvgLevel);
                    EE_CTRL_Settings_Write(EE_SETTINGS_ADC_AVG_LEVEL,_adcAvgLevel);
                    LED_Blink();
                }
                else if(H_ICTRL.buttonFlag){H_ICTRL.buttonTmo=INPUT_CTRL_BUTTON_TMO/INPUT_CTRL_ADC_TASK_TMO;}
            }
            else
            {
                H_ICTRL.buttonTmo=0U;
                H_ICTRL.buttonFlag=0U;
            }

            vTaskDelayUntil(&_xLastWakeTime,pdMS_TO_TICKS(INPUT_CTRL_ADC_TASK_TMO));
        }
        else
        {
            for(_idx=0U;_idx<INPUT_CTRL_ADC_CH;_idx++)
            {
                _adcCalibrationValue=0U;
                for(_idj=0U;_idj<64U;_idj++){_adcCalibrationValue+=INPUT_CTRL_Read_ADC_Channel_Value(&H_ICTRL.hadc,INPUT_CTRL_ADC_CH_ORDER[_idx]);}
	            H_ICTRL.adcCalibrationValue[_idx]=(_adcCalibrationValue>>6)&0xFFFF;
            }

            H_ICTRL.adcCalibrationFlag=1U;
        }
	}

	/* Task Error */
	vTaskDelete(NULL);
}

/**
  ***************************************************************************************************************************************
  * @brief  Input control GPI handler task
  * @param  Parameters (void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void INPUT_CTRL_GPI_Task_Handler(void *_params)
{
    TickType_t _xLastWakeTime=xTaskGetTickCount();

	while(1U)
	{
        if(!HAL_GPIO_ReadPin(BSP_SEL1_PORT,BSP_SEL1_PIN)){PROFILE_Set_Joystick_SEL1_Status(1U);}
        else{PROFILE_Set_Joystick_SEL1_Status(0U);}

        if(!HAL_GPIO_ReadPin(BSP_SEL2_PORT,BSP_SEL2_PIN)){PROFILE_Set_Joystick_SEL2_Status(1U);}
        else{PROFILE_Set_Joystick_SEL2_Status(0U);}

        if(!HAL_GPIO_ReadPin(BSP_BUTTON1_PORT,BSP_BUTTON1_PIN)){PROFILE_Set_Joystick_Button_Status(PROFILE_BUTTON_1,1U);}
        else{PROFILE_Set_Joystick_Button_Status(PROFILE_BUTTON_1,0U);}

        if(!HAL_GPIO_ReadPin(BSP_BUTTON2_PORT,BSP_BUTTON2_PIN)){PROFILE_Set_Joystick_Button_Status(PROFILE_BUTTON_2,1U);}
        else{PROFILE_Set_Joystick_Button_Status(PROFILE_BUTTON_2,0U);}

        if(!HAL_GPIO_ReadPin(BSP_BUTTON3_PORT,BSP_BUTTON3_PIN)){PROFILE_Set_Joystick_Button_Status(PROFILE_BUTTON_3,1U);}
        else{PROFILE_Set_Joystick_Button_Status(PROFILE_BUTTON_3,0U);}

        if(!HAL_GPIO_ReadPin(BSP_BUTTON4_PORT,BSP_BUTTON4_PIN)){PROFILE_Set_Joystick_Button_Status(PROFILE_BUTTON_4,1U);}
        else{PROFILE_Set_Joystick_Button_Status(PROFILE_BUTTON_4,0U);}

        vTaskDelayUntil(&_xLastWakeTime,pdMS_TO_TICKS(INPUT_CTRL_GPI_TASK_TMO));
	}

	/* Task Error */
	vTaskDelete(NULL);
}

/**
  ***************************************************************************************************************************************
  * @brief  ADC input read channel value
  * @param  ADC handle (ADC_HandleTypeDef*), channel (uint32_t)
  * @retval Value (uint16_t)
  ***************************************************************************************************************************************
  */
static uint16_t INPUT_CTRL_Read_ADC_Channel_Value(ADC_HandleTypeDef* _hadc,uint32_t _channel)
{
	ADC_ChannelConfTypeDef sConfig;

	/* Configure Regular Channel */
	sConfig.Channel=_channel;
	sConfig.Rank=1U;
	sConfig.SamplingTime=ADC_SAMPLETIME_1CYCLE_5;
	HAL_ADC_ConfigChannel(_hadc,&sConfig);
	HAL_ADC_Start(_hadc);
	HAL_ADC_PollForConversion(_hadc,0xFFFF);
	HAL_ADC_Stop(_hadc);

	return (uint16_t)HAL_ADC_GetValue(_hadc);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set responsive filter parameters
  * @param  Filter handle (input_ctrl_filter_ts*), threshold (float), snap (float), max (uint16_t),
  * 		   sleep (input_ctrl_filter_sleep_te), edge (input_ctrl_filter_edge_te)
  * @retval None
  ***************************************************************************************************************************************
  */
static void INPUT_CTRL_ADC_FILTER_Init(input_ctrl_filter_ts *_filter,float _threshold,float _snap,uint16_t _max, \
                                       input_ctrl_filter_sleep_te _sleep,input_ctrl_filter_edge_te _edge)
{
	_filter->activityThreshold=_threshold;
	_filter->snapMultiplier=_snap;
	_filter->maxValue=(float)_max;
	_filter->sleepEnable=_sleep;
	_filter->edgeSnapEnable=_edge;
}

/**
  ***************************************************************************************************************************************
  * @brief  Calculate new value using responsive filter
  * @param  Filter handle (input_ctrl_filter_ts*), new value (float)
  * @retval Calculated responsive value (float)
  ***************************************************************************************************************************************
  */
static float INPUT_CTRL_ADC_FILTER_Process(input_ctrl_filter_ts *_filter, float _newValue)
{
	float _diff;
	float _snap;

	/** If sleep and edge snap are enabled and the new value is very close to an edge, drag it a little closer to the edges
	  * This'll make it easier to pull the output values right to the extremes without sleeping,
	  * and it'll make movements right near the edge appear larger, making it easier to wake up
      */
	if(_filter->sleepEnable&&_filter->edgeSnapEnable)
	{
		if(_newValue<_filter->activityThreshold)
		{_newValue=_newValue*2U-_filter->activityThreshold;}
		else if(_newValue>(_filter->maxValue-_filter->activityThreshold))
		{_newValue=_newValue*2U-_filter->maxValue+_filter->activityThreshold;}
	}

	/* Get difference between new input value and current smooth value */
	if(_newValue>=_filter->smoothValue){_diff=_newValue-_filter->smoothValue;}
	else{_diff=_filter->smoothValue-_newValue;}

	/** Measure the difference between the new value and current value
	  * and use another exponential moving average to work out what
      * the current margin of error is
	  */
	_filter->errorEMA+=((float)(_newValue-_filter->smoothValue)-_filter->errorEMA)*0.4f;

	/* if sleep has been enabled, sleep when the amount of error is below the activity threshold */
	if(_filter->sleepEnable)
	{
		float _errorAbs;
		/* Recalculate sleeping status */
		if(_filter->errorEMA<0.0f){_errorAbs=_filter->errorEMA*(-1.0f);}
		else{_errorAbs=_filter->errorEMA;}
		_filter->sleeping=_errorAbs<_filter->activityThreshold;
	}

	/** If we're allowed to sleep, and we're sleeping
	  * then don't update responsiveValue this loop
	  * just output the existing responsiveValue
	  */
	if(_filter->sleepEnable&&_filter->sleeping)
	{
	    /* Ensure output is in bounds */
		if(_filter->smoothValue<0.0f){_filter->smoothValue=0.0f;}
		else if(_filter->smoothValue>_filter->maxValue){_filter->smoothValue=_filter->maxValue;}

		return (uint16_t)_filter->smoothValue;
	}

	/** Use a 'snap curve' function, where we pass in the diff (x) and get back a number from 0-1.
      * We want small values of x to result in an output close to zero, so when the smooth value is close to the input value
      * it'll smooth out noise aggressively by responding slowly to sudden changes.
      * We want a small increase in x to result in a much higher output value, so medium and large movements are snappy and responsive,
      * and aren't made sluggish by unnecessarily filtering out noise. A hyperbola (f(x) = 1/x) curve is used.
      * First x has an offset of 1 applied, so x = 0 now results in a value of 1 from the hyperbola function.
      * High values of x tend toward 0, but we want an output that begins at 0 and tends toward 1, so 1-y flips this up the right way.
	  * Finally the result is multiplied by 2 and capped at a maximum of one, which means that at a certain point all larger movements are maximally snappy
	  */

	/* Then multiply the input by SNAP_MULTIPLER so input values fit the snap curve better. */
	_snap=INPUT_CTRL_ADC_FILTER_Snap_Curve(_diff*_filter->snapMultiplier);

	/** When sleep is enabled, the emphasis is stopping on a responsiveValue quickly, and it's less about easing into position.
	  * If sleep is enabled, add a small amount to snap so it'll tend to snap into a more accurate position before sleeping starts.
	  */
	if(_filter->sleepEnable){_snap=_snap*0.5f+0.5f;}

	/* Calculate the exponential moving average based on the snap */
	_filter->smoothValue+=(_newValue-_filter->smoothValue)*_snap;

	/* Ensure output is in bounds */
	if(_filter->smoothValue<0.0f){_filter->smoothValue=0.0f;}
	else if(_filter->smoothValue>_filter->maxValue){_filter->smoothValue=_filter->maxValue;}

	return _filter->smoothValue;
}

/**
  ***************************************************************************************************************************************
  * @brief  Calculate new value using responsive filter
  * @param  Filter handle (input_ctrl_filter_ts*), new value (float)
  * @retval Calculated responsive value (uint16_t)
  ***************************************************************************************************************************************
  */
static uint16_t INPUT_CTRL_ADC_FILTER_Update(input_ctrl_filter_ts *_filter, float _newValue)
{
	/* Expected output is an integer */
	return (uint16_t)(INPUT_CTRL_ADC_FILTER_Process(_filter,_newValue)+0.5f);
}

/**
  ***************************************************************************************************************************************
  * @brief  Calculate snap curve
  * @param  x (float)
  * @retval y (float)
  ***************************************************************************************************************************************
  */
static float INPUT_CTRL_ADC_FILTER_Snap_Curve(float _x)
{
	float _y;

	_y=1.0f/(_x*0.1f+1.0f);
	_y=(1.0f-_y);
	if(_y>1.0f){return 1.0f;}
	return _y;
}
