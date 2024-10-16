#ifndef __SYSTEM_UTICK_H
#define __SYSTEM_UTICK_H

#include "bsp.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_bus.h"
#include "FreeRTOS.h"

#define SYSTEM_UTICK_INTERUPT_PRIORITY         (3-1U)

typedef struct{
    __IO uint32_t usecTimerHighCount;
	uint8_t initFlag;
}system_utick_ts;

/* Global functions declarations */
void SYSTEM_UTick_Init(void);
void SYSTEM_UTick_Deinit(void);
uint64_t SYSTEM_UTick_Get_Timestamp(void);
void SYSTEM_UTick_IRQ_Callback(void);

#endif
