#ifndef __BUZZER_H
#define __BUZZER_H

#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"

/* BUZZER peripherals configuration */
#define BUZZER_PWM_PIN				        BSP_BUZZER_PIN
#define BUZZER_PWM_PORT				        BSP_BUZZER_PORT
#define BUZZER_PWM_TIM_N			        4
#define BUZZER_PWM_CHANNEL_N		        4
#define BUZZER_PWM_DEFAULT_PERIOD	        1000U /* 1kHz */
#define BUZZER_PWM_DEFAULT_DUTY		        50U /* 50% */
#define BUZZER_PWM_TIM				        BSP_DEF_CONCAT_2(TIM,BUZZER_PWM_TIM_N)
#define BUZZER_PWM_CHANNEL			        BSP_DEF_CONCAT_2(TIM_CHANNEL_,BUZZER_PWM_CHANNEL_N)
#define BUZZER_PWM_CLOCK_ENABLE() 	        BSP_DEF_CONCAT_3(__HAL_RCC_TIM,BUZZER_PWM_TIM_N,_CLK_ENABLE)()
#define BUZZER_PWM_CLOCK_DISABLE() 	        BSP_DEF_CONCAT_3(__HAL_RCC_TIM,BUZZER_PWM_TIM_N,_CLK_DISABLE)()
#define BUZZER_MELODY_SYSTEM_USE	        2U
#define BUZZER_TASK_STACK_SIZE 				configMINIMAL_STACK_SIZE

/* Notes for buzzer melody */
#define BUZZER_NOTE_RST 			        0U
#define BUZZER_NOTE_B0  			        31U
#define BUZZER_NOTE_C1  			        33U
#define BUZZER_NOTE_CS1 			        35U
#define BUZZER_NOTE_D1  			        37U
#define BUZZER_NOTE_DS1 			        39U
#define BUZZER_NOTE_E1  			        41U
#define BUZZER_NOTE_F1  			        44U
#define BUZZER_NOTE_FS1 			        46U
#define BUZZER_NOTE_G1  			        49U
#define BUZZER_NOTE_GS1 			        52U
#define BUZZER_NOTE_A1  			        55U
#define BUZZER_NOTE_AS1 			        58U
#define BUZZER_NOTE_B1  			        62U
#define BUZZER_NOTE_C2  			        65U
#define BUZZER_NOTE_CS2 			        69U
#define BUZZER_NOTE_D2  			        73U
#define BUZZER_NOTE_DS2 			        78U
#define BUZZER_NOTE_E2  			        82U
#define BUZZER_NOTE_F2  			        87U
#define BUZZER_NOTE_FS2 			        93U
#define BUZZER_NOTE_G2  			        98U
#define BUZZER_NOTE_GS2 			        104U
#define BUZZER_NOTE_A2  			        110U
#define BUZZER_NOTE_AS2 			        117U
#define BUZZER_NOTE_B2  			        123U
#define BUZZER_NOTE_C3  			        131U
#define BUZZER_NOTE_CS3 			        139U
#define BUZZER_NOTE_D3  			        147U
#define BUZZER_NOTE_DS3 			        156U
#define BUZZER_NOTE_E3  			        165U
#define BUZZER_NOTE_F3  			        175U
#define BUZZER_NOTE_FS3 			        185U
#define BUZZER_NOTE_G3  			        196U
#define BUZZER_NOTE_GS3 			        208U
#define BUZZER_NOTE_A3  			        220U
#define BUZZER_NOTE_AS3 			        233U
#define BUZZER_NOTE_B3  			        247U
#define BUZZER_NOTE_C4  			        262U
#define BUZZER_NOTE_CS4 			        277U
#define BUZZER_NOTE_D4  			        294U
#define BUZZER_NOTE_DS4 			        311U
#define BUZZER_NOTE_E4  			        330U
#define BUZZER_NOTE_F4  			        349U
#define BUZZER_NOTE_FS4 			        370U
#define BUZZER_NOTE_G4  			        392U
#define BUZZER_NOTE_GS4 			        415U
#define BUZZER_NOTE_A4  			        440U
#define BUZZER_NOTE_AS4 			        466U
#define BUZZER_NOTE_B4  			        494U
#define BUZZER_NOTE_C5  			        523U
#define BUZZER_NOTE_CS5 			        554U
#define BUZZER_NOTE_D5  			        587U
#define BUZZER_NOTE_DS5 			        622U
#define BUZZER_NOTE_E5  			        659U
#define BUZZER_NOTE_F5  			        698U
#define BUZZER_NOTE_FS5 			        740U
#define BUZZER_NOTE_G5  			        784U
#define BUZZER_NOTE_GS5 			        831U
#define BUZZER_NOTE_A5  			        880U
#define BUZZER_NOTE_AS5 			        932U
#define BUZZER_NOTE_B5  			        988U
#define BUZZER_NOTE_C6  			        1047U
#define BUZZER_NOTE_CS6 			        1109U
#define BUZZER_NOTE_D6  			        1175U
#define BUZZER_NOTE_DS6 			        1245U
#define BUZZER_NOTE_E6  			        1319U
#define BUZZER_NOTE_F6  			        1397U
#define BUZZER_NOTE_FS6 			        1480U
#define BUZZER_NOTE_G6  			        1568U
#define BUZZER_NOTE_GS6 			        1661U
#define BUZZER_NOTE_A6  			        1760U
#define BUZZER_NOTE_AS6 			        1865U
#define BUZZER_NOTE_B6  			        1976U
#define BUZZER_NOTE_C7  			        2093U
#define BUZZER_NOTE_CS7 			        2217U
#define BUZZER_NOTE_D7  			        2349U
#define BUZZER_NOTE_DS7 			        2489U
#define BUZZER_NOTE_E7  			        2637U
#define BUZZER_NOTE_F7  			        2794U
#define BUZZER_NOTE_FS7 			        2960U
#define BUZZER_NOTE_G7  			        3136U
#define BUZZER_NOTE_GS7 			        3322U
#define BUZZER_NOTE_A7  			        3520U
#define BUZZER_NOTE_AS7 			        3729U
#define BUZZER_NOTE_B7  			        3951U
#define BUZZER_NOTE_C8  			        4186U
#define BUZZER_NOTE_CS8 			        4435U
#define BUZZER_NOTE_D8  			        4699U
#define BUZZER_NOTE_DS8 			        4978U

typedef enum{
    BUZZER_MELODY_ID_STARTUP,
	BUZZER_MELODY_ID_ERROR
}buzzer_melody_te;

typedef enum{
	BUZZER_MODE_AUTOSTOP,
	BUZZER_MODE_CONTINUOUS
}buzzer_mode_te;

typedef enum{
	BUZZER_STATE_OFF,
	BUZZER_STATE_ON,
	BUZZER_STATE_BUSY
}buzzer_state_te;

typedef struct{
	uint8_t initFlg;
	buzzer_melody_te melody;
	buzzer_mode_te mode;
	__IO buzzer_state_te state;
	TIM_HandleTypeDef timPWM;
	TIM_OC_InitTypeDef timPWMCfg;
	TaskHandle_t hTask;
	StaticTask_t hTaskBuffer;
	StackType_t hTaskStack[BUZZER_TASK_STACK_SIZE];
}buzzer_drv_ts;

/* Global buzzer driver functions declarations */
void BUZZER_Driver_Init(void);
void BUZZER_Driver_Update(void);
buzzer_state_te BUZZER_Driver_Start(buzzer_melody_te _melody, buzzer_mode_te _mode);
buzzer_state_te BUZZER_Driver_Stop(void);
buzzer_state_te BUZZER_Driver_Get_State(void);

#endif
