#ifndef __LED_H
#define __LED_H

#include "bsp.h"
#include "FreeRTOS.h"
#include "timers.h"

#define LED_TIMER_TMO      			25U /* ms */
#define LED_BLINK_TMO      			(LED_TIMER_TMO*3U) /* ms */

typedef struct{
    TimerHandle_t hTimer;
    StaticTimer_t hTimerBuffer;
    __IO uint32_t tmo;
    __IO uint8_t enTmoFlag;
    uint8_t initFlag;
}led_ts;

/* Global functions declarations */
void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_Blink(void);

#endif
