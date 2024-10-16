/**
  ***************************************************************************************************************************************
  * @file     system_utick.c
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

#include "system_utick.h"

static system_utick_ts H_SYS_UTICK;

/**
  ***************************************************************************************************************************************
  * @brief  Micro seconds timer initialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void SYSTEM_UTick_Init(void)
{
    if(H_SYS_UTICK.initFlag){return;}

    H_SYS_UTICK.usecTimerHighCount=0U;
    /* Enable the timer peripheral clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3); 
    /* Set counter mode */
    LL_TIM_SetCounterMode(TIM3,LL_TIM_COUNTERMODE_UP);
    /* Set the pre-scaler value to have TIM2 counter clock equal to 1 MHz */
    LL_TIM_SetPrescaler(TIM3,__LL_TIM_CALC_PSC(SystemCoreClock/2U,1000000U));
    LL_TIM_SetAutoReload(TIM3,0xFFFF);
    /* Enable the update interrupt */
    LL_TIM_EnableIT_UPDATE(TIM3);
    /* Configure the NVIC to handle TIM2 update interrupt */
    NVIC_SetPriority(TIM3_IRQn,SYSTEM_UTICK_INTERUPT_PRIORITY);
    NVIC_EnableIRQ(TIM3_IRQn);
    /* Enable counter */
    LL_TIM_EnableCounter(TIM3);
    /* Force update generation */
    LL_TIM_GenerateEvent_UPDATE(TIM3);
    H_SYS_UTICK.initFlag=1U;
}

/**
  ***************************************************************************************************************************************
  * @brief  Micro seconds timer deinitialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void SYSTEM_UTick_Deinit(void)
{
    LL_TIM_DisableIT_UPDATE(TIM3);
    NVIC_EnableIRQ(TIM3_IRQn);
    LL_TIM_DisableCounter(TIM3);
    LL_TIM_ClearFlag_UPDATE(TIM3);
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM3); 
    H_SYS_UTICK.initFlag=1U;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get micro seconds timestamp
  * @param  None
  * @retval Timestamp (uint64_t)
  ***************************************************************************************************************************************
  */
uint64_t SYSTEM_UTick_Get_Timestamp(void)
{
    uint32_t _high;
    uint32_t _high0;

    if(!H_SYS_UTICK.initFlag){return 0U;}
  
    __atomic_load(&H_SYS_UTICK.usecTimerHighCount,&_high0,__ATOMIC_SEQ_CST);
    uint32_t _low=TIM3->CNT;
    __atomic_load(&H_SYS_UTICK.usecTimerHighCount,&_high,__ATOMIC_SEQ_CST);
    /* There was no increment in between */
    if(_high==_high0){return (((uint64_t)_high)<<16)+_low;}
    /* There was an increment, but we don't expect another one soon */
    return (((uint64_t)_high)<<16)+TIM3->CNT;
}

/**
  ***************************************************************************************************************************************
  * @brief  Micro seconds timer interupt callback
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void SYSTEM_UTick_IRQ_Callback(void)
{
    LL_TIM_ClearFlag_UPDATE(TIM3);
    __sync_fetch_and_add(&H_SYS_UTICK.usecTimerHighCount,1U);
}
