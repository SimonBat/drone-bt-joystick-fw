#ifndef __SYSTEM_TICK_H
#define __SYSTEM_TICK_H

#include "bsp.h"

#define SYSTEM_TICK_TIMER					TIM2
#define SYSTEM_TICK_TIMER_IRQN				TIM2_IRQn
#define SYSTEM_RCC_TIM_CLK_ENABLE()			__HAL_RCC_TIM2_CLK_ENABLE()

/* Global functions declarations */
void SYSTEM_Tick_Suspend(void);
void SYSTEM_Tick_Resume(void);
void SYSTEM_Tick_IRQ_Handler(void);

#endif
