#ifndef __LTC2942_CTRL_H
#define __LTC2942_CTRL_H

#include "bsp.h"
#include "ltc2942.h"
#include "FreeRTOS.h"
#include "task.h"

#define LTC2942_CTRL_TASK_TMO		        200U /* ms */
#define LTC2942_CTRL_UPDATE_TMO             (LTC2942_CTRL_TASK_TMO*5U) /* ms */
#define LTC2942_CTRL_RESET_TMO              (LTC2942_CTRL_TASK_TMO*10U) /* ms */
#define LTC2942_CTRL_LOW_VOLTAGE_THS        3300U /* mV */
#define LTC2942_CTRL_ERROR_SOC              5U /* % */
#define LTC2942_CTRL_TASK_STACK_SIZE 		(configMINIMAL_STACK_SIZE*2U)

typedef struct{
    TaskHandle_t hTask;
    StaticTask_t hTaskBuffer;
    StackType_t hTaskStack[LTC2942_CTRL_TASK_STACK_SIZE];
    ltc2942_ts ltcHandle;
    uint8_t updateTmo;
    uint8_t errorFlag;
    uint8_t initFlag;
    uint8_t buttonTmo;
    uint8_t resetFlag;
    uint8_t buzzerActiveFlag;
}ltc2942_ctrl_ts;

/* Global functions declarations */
void LTC2942_CTRL_Init(void);
uint8_t LTC2942_CTRL_Get_Buzzer_Active_Flag(void);

#endif
