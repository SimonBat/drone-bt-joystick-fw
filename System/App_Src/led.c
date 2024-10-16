/**
  ***************************************************************************************************************************************
  * @file     led.c
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

#include "led.h"

static led_ts H_LED;

/* Local functions declarations */
static void LED_Timer_Callback(TimerHandle_t _xTimer);

/**
  ***************************************************************************************************************************************
  * @brief  LED initialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void LED_Init(void)
{
    GPIO_InitTypeDef _gpioInitStruct={0};
  
    _gpioInitStruct.Mode=GPIO_MODE_OUTPUT_PP;
    _gpioInitStruct.Pull=GPIO_NOPULL;
    _gpioInitStruct.Speed=GPIO_SPEED_FREQ_LOW;
    _gpioInitStruct.Pin=BSP_LED4_PIN;
    HAL_GPIO_WritePin(BSP_LED4_PORT,BSP_LED4_PIN,GPIO_PIN_RESET);
    HAL_GPIO_Init(BSP_LED4_PORT,&_gpioInitStruct);

    H_LED.hTimer=xTimerCreateStatic("LED_Timer",pdMS_TO_TICKS(LED_TIMER_TMO),pdTRUE,NULL,LED_Timer_Callback,&H_LED.hTimerBuffer);

    if(NULL!=H_LED.hTimer)
    {
        xTimerStart(H_LED.hTimer,0U);
        H_LED.initFlag=1U;
        LED_Blink();
    }
    else{BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  LED on
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void LED_On(void)
{
    if(!H_LED.initFlag){return;}

    H_LED.enTmoFlag=0U;
    HAL_GPIO_WritePin(BSP_LED4_PORT,BSP_LED4_PIN,GPIO_PIN_SET);
}

/**
  ***************************************************************************************************************************************
  * @brief  LED off
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void LED_Off(void)
{
    if(!H_LED.initFlag){return;}

    H_LED.enTmoFlag=0U;
    HAL_GPIO_WritePin(BSP_LED4_PORT,BSP_LED4_PIN,GPIO_PIN_RESET);
}

/**
  ***************************************************************************************************************************************
  * @brief  LED blink
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void LED_Blink(void)
{
    if(!H_LED.initFlag){return;}

    HAL_GPIO_WritePin(BSP_LED4_PORT,BSP_LED4_PIN,GPIO_PIN_SET);
    H_LED.tmo=LED_BLINK_TMO;
    H_LED.enTmoFlag=1U;
}

/**
  ***************************************************************************************************************************************
  * @brief  LED timer callback
  * @param  RTOS timer handle (TimerHandle_t)
  * @retval None
  ***************************************************************************************************************************************
  */
static void LED_Timer_Callback(TimerHandle_t _xTimer)
{
    if(H_LED.tmo>=LED_TIMER_TMO){H_LED.tmo-=LED_TIMER_TMO;}
    else{H_LED.tmo=0U;}

    if((!H_LED.tmo)&&(H_LED.enTmoFlag)){HAL_GPIO_WritePin(BSP_LED4_PORT,BSP_LED4_PIN,GPIO_PIN_RESET); H_LED.enTmoFlag=0U;}
}
