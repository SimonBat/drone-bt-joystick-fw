/**
  ***************************************************************************************************************************************
  * @file     system_tick.c
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

#include "system_tick.h"

static TIM_HandleTypeDef H_TICK;

/**
  ***************************************************************************************************************************************
  * @brief This function configures the TIM2 as a time base source
  * @param Tick priority (uint32_t)
  * @retval Status (HAL_StatusTypeDef)
  ***************************************************************************************************************************************
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t _tickPriority)
{
    RCC_ClkInitTypeDef _clkconfig;
    uint32_t _uwTimclock=0U;
    uint32_t _uwPrescalerValue=0U;
    uint32_t _pFLatency;

    /*Configure the TIM IRQ priority */
    HAL_NVIC_SetPriority(SYSTEM_TICK_TIMER_IRQN,_tickPriority,0U);
    /* Enable the TIM global Interrupt */
    HAL_NVIC_EnableIRQ(SYSTEM_TICK_TIMER_IRQN);
    /* Enable TIM clock */
    SYSTEM_RCC_TIM_CLK_ENABLE();
    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&_clkconfig, &_pFLatency);
    /* Compute TIM clock */
    _uwTimclock=2U*HAL_RCC_GetPCLK1Freq();
    /* Compute the prescaler value to have TIM2 counter clock equal to 1MHz */
    _uwPrescalerValue=(uint32_t)((_uwTimclock/1000000U)-1U);
    /* Initialize */
    H_TICK.Instance=SYSTEM_TICK_TIMER;
    /** Initialize TIMx peripheral as follow:
      * Period = [(TIMCLK/1000) - 1]. to have a (1/1000) s time base.
      * Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
      * ClockDivision = 0
      * Counter direction = Up
      */
    H_TICK.Init.Period=(1000000U/1000U)-1U;
    H_TICK.Init.Prescaler=_uwPrescalerValue;
    H_TICK.Init.ClockDivision=0U;
    H_TICK.Init.CounterMode=TIM_COUNTERMODE_UP;

    if(HAL_OK==HAL_TIM_Base_Init(&H_TICK)){return HAL_TIM_Base_Start_IT(&H_TICK);}

    /* Return function status */
    return HAL_ERROR;
}

/**
  ***************************************************************************************************************************************
  * @brief Suspend Tick increment
  * @param None
  * @retval None
  ***************************************************************************************************************************************
  */
void SYSTEM_Tick_Suspend(void)
{
    /* Disable TIM2 update Interrupt */
    __HAL_TIM_DISABLE_IT(&H_TICK,TIM_IT_UPDATE);
}

/**
  ***************************************************************************************************************************************
  * @brief Resume Tick increment
  * @param None
  * @retval None
  ***************************************************************************************************************************************
  */
void SYSTEM_Tick_Resume(void)
{
    /* Enable TIM2 Update interrupt */
    __HAL_TIM_ENABLE_IT(&H_TICK,TIM_IT_UPDATE);
}

/**
  ***************************************************************************************************************************************
  * @brief  HAL TIM IRQ handler
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void SYSTEM_Tick_IRQ_Handler(void)
{
	/* TIM Update event */
	if((RESET!=__HAL_TIM_GET_FLAG(&H_TICK,TIM_FLAG_UPDATE))&&(RESET!=__HAL_TIM_GET_IT_SOURCE(&H_TICK,TIM_IT_UPDATE)))
	{
	    __HAL_TIM_CLEAR_IT(&H_TICK,TIM_IT_UPDATE);
		HAL_IncTick();
	}
}
