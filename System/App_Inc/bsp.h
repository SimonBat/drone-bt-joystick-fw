#ifndef __BSP_H
#define __BSP_H

#include "stm32f1xx_hal.h"

/* Standard C style concatenation macros */
#define BSP_DEF_CONCAT_2(_x_, _y_)          	__BSP_DEF_CONCAT_2__(_x_, _y_)
#define __BSP_DEF_CONCAT_2__(_x_, _y_)      	_x_ ## _y_
#define BSP_DEF_CONCAT_3(_x_, _y_, _z_)     	__BSP_DEF_CONCAT_3__(_x_, _y_, _z_)
#define __BSP_DEF_CONCAT_3__(_x_, _y_, _z_)		_x_ ## _y_ ## _z_

/* Private defines */
#define BSP_V1_ADC_PIN            	GPIO_PIN_1
#define BSP_V1_ADC_PORT           	GPIOA
#define BSP_H1_ADC_PIN            	GPIO_PIN_2
#define BSP_H1_ADC_PORT           	GPIOA
#define BSP_V2_ADC_PIN            	GPIO_PIN_3
#define BSP_V2_ADC_PORT           	GPIOA
#define BSP_H2_ADC_PIN            	GPIO_PIN_4
#define BSP_H2_ADC_PORT           	GPIOA
#define BSP_SEL1_PIN              	GPIO_PIN_5
#define BSP_SEL1_PORT             	GPIOA
#define BSP_SEL2_PIN              	GPIO_PIN_6
#define BSP_SEL2_PORT             	GPIOA
#define BSP_UART_TX_PIN           	GPIO_PIN_10
#define BSP_UART_TX_PORT          	GPIOB
#define BSP_UART_RX_PIN           	GPIO_PIN_11
#define BSP_UART_RX_PORT          	GPIOB
#define BSP_BUTTON1_PIN           	GPIO_PIN_12
#define BSP_BUTTON1_PORT          	GPIOB
#define BSP_BUTTON2_PIN           	GPIO_PIN_13
#define BSP_BUTTON2_PORT          	GPIOB
#define BSP_BUTTON3_PIN           	GPIO_PIN_14
#define BSP_BUTTON3_PORT          	GPIOB
#define BSP_BUTTON4_PIN           	GPIO_PIN_15
#define BSP_BUTTON4_PORT          	GPIOB
#define BSP_BT_WAKE_HW_PIN        	GPIO_PIN_8
#define BSP_BT_WAKE_HW_PORT       	GPIOA
#define BSP_BT_RX_PIN             	GPIO_PIN_9
#define BSP_BT_RX_PORT            	GPIOA
#define BSP_BT_TX_PIN             	GPIO_PIN_10
#define BSP_BT_TX_PORT            	GPIOA
#define BSP_BT_WAKE_SW_PIN        	GPIO_PIN_11
#define BSP_BT_WAKE_SW_PORT       	GPIOA
#define BSP_BT_CMD_MLDP_PIN       	GPIO_PIN_12
#define BSP_BT_CMD_MLDP_PORT      	GPIOA
#define BSP_BT_PORT               	GPIOA
#define BSP_SWIO_PIN              	GPIO_PIN_13
#define BSP_SWIO_PORT             	GPIOA
#define BSP_SWCLK_PIN             	GPIO_PIN_14
#define BSP_SWCLK_PORT            	GPIOA
#define BSP_LED4_PIN              	GPIO_PIN_15
#define BSP_LED4_PORT             	GPIOA
#define BSP_SWO_PIN               	GPIO_PIN_3
#define BSP_SWO_PORT              	GPIOB
#define BSP_LTC_KILL_PIN          	GPIO_PIN_4
#define BSP_LTC_KILL_PORT         	GPIOB
#define BSP_LTC_INT_PIN           	GPIO_PIN_5
#define BSP_LTC_INT_PORT          	GPIOB
#define BSP_I2C_SCL_PIN           	GPIO_PIN_6
#define BSP_I2C_SDA_PIN           	GPIO_PIN_7
#define BSP_I2C_PORT              	GPIOB
#define BSP_AL_CC_PIN             	GPIO_PIN_8
#define BSP_AL_CC_PORT            	GPIOB
#define BSP_BUZZER_PIN            	GPIO_PIN_9
#define BSP_BUZZER_PORT           	GPIOB

/* Global functions declarations */
void BSP_System_Clock_Config(void);
void BSP_System_GPIO_Init(void);
void BSP_Error_Handler(void);

#endif
