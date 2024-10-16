/**
  ***************************************************************************************************************************************
  * @file     profile.c
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

#include "profile.h" 

static profile_ts PROFILE;

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick battery SOC
  * @param  SOC (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_Battery_SOC(uint8_t _soc)
{
    if(_soc>100U){_soc=100U;}
    PROFILE.joystickBattSoc=_soc;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick battery SOC
  * @param  None
  * @retval SOC (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t PROFILE_Get_Joystick_Battery_SOC(void)
{
    return PROFILE.joystickBattSoc;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick battery temperature
  * @param  Temperature (int8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_Battery_Temperature(int8_t _temp)
{
    if(_temp>100){_temp=100;}
    else if(_temp<-20){_temp=-20;}

    PROFILE.joystickBattTemperature=_temp;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick battery temperature
  * @param  None
  * @retval Temperature (int8_t)
  ***************************************************************************************************************************************
  */
int8_t PROFILE_Get_Joystick_Battery_Temperature(void)
{
    return PROFILE.joystickBattTemperature;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick battery voltage
  * @param  Voltage (uint16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_Battery_Voltage(uint16_t _voltage)
{
    PROFILE.joystickVoltage=_voltage;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick battery temperature
  * @param  None
  * @retval Voltage (uint16_t)
  ***************************************************************************************************************************************
  */
uint16_t PROFILE_Get_Joystick_Battery_Voltage(void)
{
    return PROFILE.joystickVoltage;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick button status
  * @param  Index (profile_button_te), status (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_Button_Status(profile_button_te _idx, uint8_t _status)
{
    if(_status){PROFILE.buttonStatus[_idx]=1U;}
    else{PROFILE.buttonStatus[_idx]=0U;}
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick button status
  * @param  Index (profile_button_te)
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t PROFILE_Get_Joystick_Button_Status(profile_button_te _idx)
{  
    return PROFILE.buttonStatus[_idx];
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick V1 status
  * @param  Status (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_V1_Status(uint8_t _status)
{
    if(_status>100U){PROFILE.v1Status=100U;}
    else{PROFILE.v1Status=_status;}
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick V1 status
  * @param  None
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t PROFILE_Get_Joystick_V1_Status(void)
{  
    return PROFILE.v1Status;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick V2 status
  * @param  Status (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_V2_Status(uint8_t _status)
{
    if(_status>100U){PROFILE.v2Status=100U;}
    else{PROFILE.v2Status=_status;}
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick V2 status
  * @param  None
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t PROFILE_Get_Joystick_V2_Status(void)
{  
    return PROFILE.v2Status;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick H1 status
  * @param  Status (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_H1_Status(uint8_t _status)
{
    if(_status>100U){PROFILE.h1Status=100U;}
    else{PROFILE.h1Status=_status;}
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick H1 status
  * @param  None
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t PROFILE_Get_Joystick_H1_Status(void)
{  
    return PROFILE.h1Status;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick H2 status
  * @param  Status (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_H2_Status(uint8_t _status)
{
    if(_status>100U){PROFILE.h2Status=100U;}
    else{PROFILE.h2Status=_status;}
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick H2 status
  * @param  None
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t PROFILE_Get_Joystick_H2_Status(void)
{  
    return PROFILE.h2Status;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick SEL1 status
  * @param  Status (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_SEL1_Status(uint8_t _status)
{
    if(_status){PROFILE.sel1Status=1U;}
    else{PROFILE.sel1Status=0U;}
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick SEL1 status
  * @param  None
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t PROFILE_Get_Joystick_SEL1_Status(void)
{  
    return PROFILE.sel1Status;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick SEL2 status
  * @param  Status (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_SEL2_Status(uint8_t _status)
{
    if(_status){PROFILE.sel2Status=1U;}
    else{PROFILE.sel2Status=0U;}
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick SEL2 status
  * @param  None
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t PROFILE_Get_Joystick_SEL2_Status(void)
{  
    return PROFILE.sel2Status;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick battery capacity settings
  * @param  Value (uint16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Settings_Set_Joystick_Battery_Capacity(uint16_t _value)
{
    PROFILE.settings.joystickBattCapacity=_value;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick battery capacity settings
  * @param  None
  * @retval Value (uint16_t)
  ***************************************************************************************************************************************
  */
uint16_t PROFILE_Settings_Get_Joystick_Battery_Capacity(void)
{  
    return PROFILE.settings.joystickBattCapacity;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick ADC averaging level settings
  * @param  Value (uint16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Settings_Set_Joystick_ADC_AVG_Level(uint16_t _value)
{
    PROFILE.settings.joystickAdcAvgLevel=_value;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick ADC averaging level settings
  * @param  None
  * @retval Value (uint16_t)
  ***************************************************************************************************************************************
  */
uint16_t PROFILE_Settings_Get_Joystick_ADC_AVG_Level(void)
{  
    return PROFILE.settings.joystickAdcAvgLevel;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set drone battery SOC
  * @param  SOC (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Drone_Battery_SOC(uint8_t _soc)
{
    if(_soc>100U){_soc=100U;}
    PROFILE.droneBattSoc=_soc;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get drone battery SOC
  * @param  None
  * @retval SOC (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t PROFILE_Get_Drone_Battery_SOC(void)
{
    return PROFILE.droneBattSoc;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set joystick RSSI value
  * @param  Value (int8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_RSSI_Value(int8_t _rssi)
{
    if(_rssi>0){_rssi=0;}
    PROFILE.joystickRssiValue=_rssi;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick RSSI value
  * @param  None
  * @retval Value (int8_t)
  ***************************************************************************************************************************************
  */
int8_t PROFILE_Get_Joystick_RSSI_Value(void)
{
    return PROFILE.joystickRssiValue;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set drone TX power value
  * @param  Value (int8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Drone_TX_Power_Value(int8_t _txPower)
{
    PROFILE.droneTxPower=_txPower;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get joystick RSSI value
  * @param  None
  * @retval Value (int8_t)
  ***************************************************************************************************************************************
  */
int8_t PROFILE_Get_Drone_TX_Power_Value(void)
{
    return PROFILE.droneTxPower;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set video enable status
  * @param  Value (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void PROFILE_Set_Joystick_Video_EN_Status(uint8_t _status)
{
    PROFILE.videoEnStatus=_status;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get video enable status
  * @param  None
  * @retval Value (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t PROFILE_Get_Joystick_Video_EN_Status(void)
{
    return PROFILE.videoEnStatus;
}
