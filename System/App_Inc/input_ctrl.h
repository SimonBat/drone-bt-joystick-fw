#ifndef __INPUT_CTRL_H
#define __INPUT_CTRL_H

#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"

#define INPUT_CTRL_ADC_CH                       	4U
#define INPUT_CTRL_V1_ADC_CH	                	ADC_CHANNEL_1
#define INPUT_CTRL_H1_ADC_CH	                	ADC_CHANNEL_2
#define INPUT_CTRL_V2_ADC_CH	                	ADC_CHANNEL_3
#define INPUT_CTRL_H2_ADC_CH	                	ADC_CHANNEL_4
#define INPUT_CTRL_ADC_TASK_TMO                 	100U /* ms */
#define INPUT_CTRL_GPI_TASK_TMO                 	200U /* ms */
#define INPUT_CTRL_BUTTON_TMO                   	(INPUT_CTRL_ADC_TASK_TMO*5U) /* ms */
#define INPUT_CTRL_FT_TH			            	2.0f
#define INPUT_CTRL_FT_SNAP			            	0.05f
#define INPUT_CTRL_ADC_THRESHOLD                	300U
#define INPUT_CTRL_ADC_AVG_LEVEL_DEFAULT        	0x0001
#define INPUT_CTRL_ADC_AVG_LEVEL_MAX            	0x0003
#define INPUT_CTRL_TASK_STACK_SIZE 					(configMINIMAL_STACK_SIZE*2U)

typedef enum{
	INPUT_CTRL_FILTER_SLEEP_DISABLE,
	INPUT_CTRL_FILTER_SLEEP_ENABLE
}input_ctrl_filter_sleep_te;

typedef enum{
	INPUT_CTRL_FILTER_EDGE_DISABLE,
	INPUT_CTRL_FILTER_EDGE_ENABLE
}input_ctrl_filter_edge_te;

typedef struct{
	float snapMultiplier;
	float activityThreshold;
	float smoothValue;
	float errorEMA;
	float maxValue;
	input_ctrl_filter_sleep_te sleepEnable;
	input_ctrl_filter_edge_te edgeSnapEnable;
	uint8_t sleeping;
}input_ctrl_filter_ts;

typedef struct{
	TaskHandle_t hTaskAdc;
	StaticTask_t hTaskAdcBuffer;
	StackType_t hTaskAdcStack[INPUT_CTRL_TASK_STACK_SIZE];
    TaskHandle_t hTaskGpi;
    StaticTask_t hTaskGpiBuffer;
    StackType_t hTaskGpiStack[INPUT_CTRL_TASK_STACK_SIZE];
    ADC_HandleTypeDef hadc;
    input_ctrl_filter_ts adcFilter[INPUT_CTRL_ADC_CH];
    uint16_t adcCalibrationValue[INPUT_CTRL_ADC_CH];
    float adcValueAvg[INPUT_CTRL_ADC_CH];
    uint8_t adcCalibrationFlag;
	uint8_t initFlag;
    uint8_t buttonTmo;
    uint8_t buttonFlag;
}input_ctrl_ts;

/* Global functions declarations */
void INPUT_CTRL_Init(void);

#endif
