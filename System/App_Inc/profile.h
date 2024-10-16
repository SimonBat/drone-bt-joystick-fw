#ifndef __PROFILE_H
#define __PROFILE_H

#include "bsp.h"

typedef enum{
    PROFILE_BUTTON_1=0,
    PROFILE_BUTTON_2=1,
    PROFILE_BUTTON_3=2,
    PROFILE_BUTTON_4=3,
}profile_button_te;

typedef struct{
    uint8_t joystickBattSoc;
    int8_t joystickBattTemperature;
    uint16_t joystickVoltage;
    uint8_t buttonStatus[4];
    uint8_t v1Status;
    uint8_t v2Status;
    uint8_t h1Status;
    uint8_t h2Status;
    uint8_t sel1Status;
    uint8_t sel2Status;
    int8_t joystickRssiValue;
    uint8_t videoEnStatus;

    uint8_t droneBattSoc;
    int8_t droneTxPower;

    struct{
        uint16_t joystickBattCapacity;
        uint16_t joystickAdcAvgLevel;
    }settings;
}profile_ts;

/* Global functions declartions */
void PROFILE_Set_Joystick_Battery_SOC(uint8_t _soc);
uint8_t PROFILE_Get_Joystick_Battery_SOC(void);
void PROFILE_Set_Joystick_Battery_Temperature(int8_t _temp);
int8_t PROFILE_Get_Joystick_Battery_Temperature(void);
void PROFILE_Set_Joystick_Battery_Voltage(uint16_t _voltage);
uint16_t PROFILE_Get_Joystick_Battery_Voltage(void);
void PROFILE_Set_Joystick_Button_Status(profile_button_te _idx, uint8_t _status);
uint8_t PROFILE_Get_Joystick_Button_Status(profile_button_te _idx);
void PROFILE_Set_Joystick_V1_Status(uint8_t _status);
uint8_t PROFILE_Get_Joystick_V1_Status(void);
void PROFILE_Set_Joystick_V2_Status(uint8_t _status);
uint8_t PROFILE_Get_Joystick_V2_Status(void);
void PROFILE_Set_Joystick_H1_Status(uint8_t _status);
uint8_t PROFILE_Get_Joystick_H1_Status(void);
void PROFILE_Set_Joystick_H2_Status(uint8_t _status);
uint8_t PROFILE_Get_Joystick_H2_Status(void);
void PROFILE_Set_Joystick_SEL1_Status(uint8_t _status);
uint8_t PROFILE_Get_Joystick_SEL1_Status(void);
void PROFILE_Set_Joystick_SEL2_Status(uint8_t _status);
uint8_t PROFILE_Get_Joystick_SEL2_Status(void);
void PROFILE_Settings_Set_Joystick_Battery_Capacity(uint16_t _value);
uint16_t PROFILE_Settings_Get_Joystick_Battery_Capacity(void);
void PROFILE_Settings_Set_Joystick_ADC_AVG_Level(uint16_t _value);
uint16_t PROFILE_Settings_Get_Joystick_ADC_AVG_Level(void);
void PROFILE_Set_Drone_Battery_SOC(uint8_t _soc);
uint8_t PROFILE_Get_Drone_Battery_SOC(void);
void PROFILE_Set_Joystick_RSSI_Value(int8_t _rssi);
int8_t PROFILE_Get_Joystick_RSSI_Value(void);
void PROFILE_Set_Drone_TX_Power_Value(int8_t _txPower);
int8_t PROFILE_Get_Drone_TX_Power_Value(void);
void PROFILE_Set_Joystick_Video_EN_Status(uint8_t _status);
uint8_t PROFILE_Get_Joystick_Video_EN_Status(void);

#endif
