#ifndef __LTC2942_H
#define __LTC2942_H

#include "bsp.h"
#include "FreeRTOS.h"

#define LTC2942_BATTERY_CAPACITY                1350U /* mAh */
#define LTC2942_BATTERY_CAPACITY_REG_VALUE      0xF829 /* M prescaler equal to 32 */

/* LTC2942 address */
#define LTC2942_ADDR                            (0x64<<1)
#define LTC2942_ALERT_ADDR					    0x0C

/* LTC2942 register definitions */
#define LTC2942_REG_STATUS                      0x00 /* (A) Status */
#define LTC2942_REG_CONTROL                     0x01 /* (B) Control */
#define LTC2942_REG_AC_H                        0x02 /* (C) Accumulated charge MSB */
#define LTC2942_REG_AC_L                        0x03 /* (D) Accumulated charge LSB */
#define LTC2942_REG_CTH_H                       0x04 /* (E) Charge threshold high MSB */
#define LTC2942_REG_CTH_L                       0x05 /* (F) Charge threshold high LSB */
#define LTC2942_REG_CTL_H                       0x06 /* (G) Charge threshold low MSB */
#define LTC2942_REG_CTL_L                       0x07 /* (H) Charge threshold low LSB */
#define LTC2942_REG_VOL_H                       0x08 /* (I) Voltage MSB */
#define LTC2942_REG_VOL_L                       0x09 /* (J) Voltage LSB */
#define LTC2942_REG_VOLT_H                      0x0A /* (K) Voltage threshold high */
#define LTC2942_REG_VOLT_L                      0x0B /* (L) Voltage threshold low */
#define LTC2942_REG_TEMP_H                      0x0C /* (M) Temperature MSB */
#define LTC2942_REG_TEMP_L                      0x0D /* (N) Temperature LSB */
#define LTC2942_REG_TEMPT_H                     0x0E /* (O) Temperature threshold high */
#define LTC2942_REG_TEMPT_L                     0x0F /* (P) Temperature threshold low */

/* LTC2942 status register bit definitions */
#define LTC2942_STATUS_CHIPID                   0x80 /* A[7] Chip identification (1: LTC2942-1, 0: LTC2941-1) */
#define LTC2942_STATUS_AC_OVR                   0x20 /* A[5] Accumulated charge overflow/underflow */
#define LTC2942_STATUS_TEMP_ALRT                0x10 /* A[4] Temperature alert */
#define LTC2942_STATUS_CHG_ALRT_H               0x08 /* A[3] Charge alert high */
#define LTC2942_STATUS_CHG_ALRT_L               0x04 /* A[2] Charge alert low */
#define LTC2942_STATUS_VOL_ALRT                 0x02 /* A[1] Voltage alert */
#define LTC2942_STATUS_UVLO_ALRT                0x01 /* A[0] Undervoltage lockout alert */

/* LTC2942 control register bit definitions */
#define LTC2942_CTL_ADC_MSK                     0x3F /* ADC mode bits [7:6] */
#define LTC2942_CTL_PSCM_MSK                    0xC7 /* Prescaler M bits [5:3] */
#define LTC2942_CTL_ALCC_MSK                    0xF9 /* AL/CC pin control [2:1] */
#define LTC2942_CTL_SHUTDOWN                    0x01 /* B[0] Shutdown */

/* LTC2942 ADC mode enumeration */
typedef enum{
    LTC2942_ADC_AUTO=0xC0, /* Automatic mode */
	LTC2942_ADC_M_VOL=0x80, /* Manual voltage mode */
	LTC2942_ADC_M_TEMP=0x40, /* Manual temperature mode */
	LTC2942_ADC_SLEEP=0x00 /* Sleep */
}ltc2942_adc_mode_te;

/* LTC2942 prescaler M enumeration */
typedef enum{
	LTC2942_PSCM_1=0x00,
	LTC2942_PSCM_2=0x08,
	LTC2942_PSCM_4=0x10,
	LTC2942_PSCM_8=0x18,
	LTC2942_PSCM_16=0x20,
	LTC2942_PSCM_32=0x28,
	LTC2942_PSCM_64=0x30,
	LTC2942_PSCM_128=0x38
}ltc2942_prescaler_te;

/* LTC2942 AL/CC pin mode enumeration */
typedef enum{
	LTC2942_ALCC_DISABLED=0x00, /* AL/CC pin disabled */
	LTC2942_ALCC_CHG=0x02, /* Charge complete mode */
	LTC2942_ALCC_ALERT=0x04 /* Alert mode */
}ltc2942_alcc_te;

/* State of analog section enumeration */
typedef enum{
	LTC2942_AN_DISABLED=0x00,
	LTC2942_AN_ENABLED=!LTC2942_AN_DISABLED
}ltc2942_an_state_te;

typedef struct{
    uint16_t batteryCapacity;
    uint16_t acThresholdHigh;
    uint8_t initFlag;
}ltc2942_ts;

/* Global functions declarations */
void LTC2942_Init(ltc2942_ts* _hLtc);
void LTC2942_Update_Battery_Capacity(ltc2942_ts* _hLtc);
void LTC2942_Reset_Battery_Capacity(ltc2942_ts* _hLtc);
uint8_t LTC2942_Get_Status(void);
uint8_t LTC2942_Get_Control(void);
uint16_t LTC2942_Get_Voltage(void);
int32_t LTC2942_Get_Temperature(void);
uint8_t LTC2942_Get_SOC(const ltc2942_ts* _hLtc);
void LTC2942_Set_AC_Value(uint16_t _ac);
void LTC2942_Set_ADC_Mode(ltc2942_adc_mode_te _mode);
void LTC2942_Set_Prescaler(ltc2942_prescaler_te _psc);
void LTC2942_Set_ALCC_Mode(ltc2942_alcc_te _mode);
void LTC2942_Set_Analog(ltc2942_an_state_te _state);
void LTC2942_Set_Charge_Threshold_H(uint16_t _level);
void LTC2942_Set_Charge_Threshold_L(uint16_t _level);
void LTC2942_Set_Voltage_Threshold_H(uint8_t _level);
void LTC2942_Set_Voltage_Threshold_L(uint8_t _level);
void LTC2942_Set_Temperature_Threshold_H(uint8_t _level);
void LTC2942_Set_Temperature_Threshold_L(uint8_t _level);

#endif
