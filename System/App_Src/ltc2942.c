/**
  ***************************************************************************************************************************************
  * @file     ltc2942.c
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

#include "ltc2942.h"
#include "i2c.h"
#include "profile.h"
#include "eeprom_ctrl.h"

/* Local functions declarations */
static uint16_t LTC2942_Read_Value(uint8_t _reg);

/**
  ***************************************************************************************************************************************
  * @brief  LTC2942 initialization
  * @param  Handle (ltc2942_ts*)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Init(ltc2942_ts* _hLtc)
{
    float _tempValue;
    GPIO_InitTypeDef _gpioInitStruct={0};

    _gpioInitStruct.Pin=BSP_AL_CC_PIN;
    _gpioInitStruct.Mode=GPIO_MODE_INPUT;
    _gpioInitStruct.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(BSP_AL_CC_PORT,&_gpioInitStruct);

    /* Disable AL/CC pin */
    LTC2942_Set_ALCC_Mode(LTC2942_ALCC_DISABLED);
    /* Enable auto measurement of battery voltage and temperature */
    LTC2942_Set_ADC_Mode(LTC2942_ADC_AUTO);
    /* Enable analog section of the chip (in case if it disabled) */
    LTC2942_Set_Analog(LTC2942_AN_ENABLED);
    /* Set prescaler M value */
    /* M=32 for 1350mAh battery, the 1LSB of AC value is 0,085*32/128 mAh */
    LTC2942_Set_Prescaler(LTC2942_PSCM_32);

    _hLtc->batteryCapacity=PROFILE_Settings_Get_Joystick_Battery_Capacity();
    _tempValue=(float)_hLtc->batteryCapacity/(0.085f*32.0f/128.0f);
    _hLtc->acThresholdHigh=(uint16_t)(_tempValue+0.5f);
    LTC2942_Set_Charge_Threshold_H(_hLtc->acThresholdHigh); /* 1350mAH */
    LTC2942_Set_Charge_Threshold_L(0x0000); /* 0mAH */
    LTC2942_Set_Voltage_Threshold_H(0xB3); /* 4.2V MSB -> 4200/6000*65535/256 */
    LTC2942_Set_Voltage_Threshold_L(0x8D); /* 3.3V MSB -> 3300/6000*65535/256 */
    _hLtc->initFlag=1U;
}

/**
  ***************************************************************************************************************************************
  * @brief  Update battery capacity
  * @param  Handle (ltc2942_ts*)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Update_Battery_Capacity(ltc2942_ts* _hLtc)
{
     uint16_t _regValue=LTC2942_Read_Value(LTC2942_REG_AC_H);
	_regValue=_regValue*0.085*32/128;

	if((_hLtc->batteryCapacity>_regValue)&&(0U!=_regValue))
    {
        _hLtc->batteryCapacity-=_regValue;
        float _tempValue=(float)_hLtc->batteryCapacity/(0.085f*32.0f/128.0f);
        _hLtc->acThresholdHigh=(uint16_t)(_tempValue+0.5f);
        LTC2942_Set_Charge_Threshold_H(_hLtc->acThresholdHigh);
        LTC2942_Set_AC_Value(0U);
        PROFILE_Settings_Set_Joystick_Battery_Capacity(_hLtc->batteryCapacity);
        EE_CTRL_Settings_Write(EE_SETTINGS_BATTERY_CAPACITY,_hLtc->batteryCapacity);
    }
}

/**
  ***************************************************************************************************************************************
  * @brief  Reset battery capacity
  * @param  Handle (ltc2942_ts*)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Reset_Battery_Capacity(ltc2942_ts* _hLtc)
{
    float _tempValue;

    _hLtc->batteryCapacity=LTC2942_BATTERY_CAPACITY;
    _tempValue=(float)_hLtc->batteryCapacity/(0.085f*32.0f/128.0f);
    _hLtc->acThresholdHigh=(uint16_t)(_tempValue+0.5f);
    LTC2942_Set_Charge_Threshold_H(_hLtc->acThresholdHigh); /* 1350mAH */
    LTC2942_Set_AC_Value(_hLtc->acThresholdHigh);
    PROFILE_Settings_Set_Joystick_Battery_Capacity(_hLtc->batteryCapacity);
    EE_CTRL_Settings_Write(EE_SETTINGS_BATTERY_CAPACITY,_hLtc->batteryCapacity);
}

/**
  ***************************************************************************************************************************************
  * @brief  Read register value
  * @param  Register (uint8_t)
  * @retval Value (uint16_t)
  ***************************************************************************************************************************************
  */
static uint16_t LTC2942_Read_Value(uint8_t _reg)
{
    uint8_t _regValue[2]={0,0};

    I2C_Driver_Read(LTC2942_ADDR,_reg,I2C_MEMADD_SIZE_8BIT,_regValue,2U);

    return (_regValue[1]|(_regValue[0]<<8));
}

/**
  ***************************************************************************************************************************************
  * @brief  Get status register value
  * @param  None
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t LTC2942_Get_Status(void)
{
	uint8_t _regValue=0U;

    I2C_Driver_Read(LTC2942_ADDR,LTC2942_REG_STATUS,I2C_MEMADD_SIZE_8BIT,&_regValue,1U);
   
    return _regValue;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get control register value
  * @param  None
  * @retval Value (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t LTC2942_Get_Control(void)
{
	uint8_t _regValue=0U;

    I2C_Driver_Read(LTC2942_ADDR,LTC2942_REG_CONTROL,I2C_MEMADD_SIZE_8BIT,&_regValue,1U);

    return _regValue;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get voltage
  * @param  None
  * @retval Value (uint16_t)
  ***************************************************************************************************************************************
  */
uint16_t LTC2942_Get_Voltage(void)
{
	uint32_t _regValue;

    _regValue=LTC2942_Read_Value(LTC2942_REG_VOL_H);
    _regValue*=6000U;
    _regValue/=65535U;

    return (uint16_t)_regValue;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get temperature
  * @param  None
  * @retval Value (int32_t)
  ***************************************************************************************************************************************
  */
int32_t LTC2942_Get_Temperature(void)
{
    int32_t _regValue;

    _regValue=LTC2942_Read_Value(LTC2942_REG_TEMP_H);
    _regValue>>=4U;
    _regValue*=60000;
    _regValue/=4092;
    /* By now the temperature value in Kelvins, convert it to Celsius degrees */
    _regValue-=27315;

    /* Return temperature in Celsius (value of '2538' represents 25.38C) */
    return _regValue;
}

/**
  ***************************************************************************************************************************************
  * @brief  Get SOC
  * @param  Handle (const ltc2942_ts*)
  * @retval Value (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t LTC2942_Get_SOC(const ltc2942_ts* _hLtc)
{
    uint16_t _soc;
    float _socTemp;
    uint16_t _regValue;

    _regValue=LTC2942_Read_Value(LTC2942_REG_AC_H);
	_regValue=_regValue*0.085*32/128;

	if(_regValue>_hLtc->batteryCapacity)
    {
        _regValue=_hLtc->batteryCapacity;
        LTC2942_Set_AC_Value(_hLtc->acThresholdHigh);
    }

    _socTemp=(float)_regValue*100.0f/(float)_hLtc->batteryCapacity+0.5f;
    _soc=(uint8_t)_socTemp;
    if(_soc>100U){_soc=100U;}

    return _soc;
}

/**
  ***************************************************************************************************************************************
  * @brief  Set AC value
  * @param  Value (uint16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_AC_Value(uint16_t _ac)
{
    uint8_t _regAc;
	uint8_t _regValue=0U;
    uint8_t _regValueMod=0U;

    /* Before programming new AC value the analog section must be shut down */
    I2C_Driver_Read(LTC2942_ADDR,LTC2942_REG_CONTROL,I2C_MEMADD_SIZE_8BIT,&_regValue,1U);
    _regValueMod=_regValue|LTC2942_CTL_SHUTDOWN;
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_CONTROL, I2C_MEMADD_SIZE_8BIT,&_regValueMod,1U);
    /* Program new AC value */
    _regAc=(uint8_t)(_ac>>8);
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_AC_H, I2C_MEMADD_SIZE_8BIT,&_regAc,1U);
    _regAc=(uint8_t)(_ac&0x00FF);
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_AC_L, I2C_MEMADD_SIZE_8BIT,&_regAc,1U);
    /* Restore old CONTROL register value */
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_CONTROL, I2C_MEMADD_SIZE_8BIT,&_regValue,1U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set ADC mode
  * @param  Mode (ltc2942_adc_mode_te)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_ADC_Mode(ltc2942_adc_mode_te _mode)
{
	uint8_t  _regValue=0U;

    /* Read CONTROL register, clear ADC mode bits and configure new value */
    I2C_Driver_Read(LTC2942_ADDR,LTC2942_REG_CONTROL,I2C_MEMADD_SIZE_8BIT,&_regValue,1U);
	_regValue=_regValue&LTC2942_CTL_ADC_MSK;
    _regValue=_regValue|_mode;
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_CONTROL, I2C_MEMADD_SIZE_8BIT,&_regValue,1U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set prescaler
  * @param  Value (ltc2942_prescaler_te)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_Prescaler(ltc2942_prescaler_te _psc)
{
    uint8_t _regValue=0U;

    /* Read CONTROL register, clear prescaler M bits and configure new value */
    I2C_Driver_Read(LTC2942_ADDR,LTC2942_REG_CONTROL,I2C_MEMADD_SIZE_8BIT,&_regValue,1U);
	_regValue=_regValue&LTC2942_CTL_PSCM_MSK;
    _regValue=_regValue|_psc;
    I2C_Driver_Write(LTC2942_ADDR,LTC2942_REG_CONTROL,I2C_MEMADD_SIZE_8BIT,&_regValue,1U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set ALCC mode
  * @param  Value (ltc2942_alcc_te)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_ALCC_Mode(ltc2942_alcc_te _mode)
{
    uint8_t _regValue=0U;

    /* Read CONTROL register, clear AL/CC bits and configure new value */
    I2C_Driver_Read(LTC2942_ADDR,LTC2942_REG_CONTROL,I2C_MEMADD_SIZE_8BIT,&_regValue,1U);
	_regValue=_regValue&LTC2942_CTL_ALCC_MSK;
    _regValue=_regValue|_mode;
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_CONTROL, I2C_MEMADD_SIZE_8BIT,&_regValue,1U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set analog state
  * @param  Value (ltc2942_an_state_te)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_Analog(ltc2942_an_state_te _state)
{
    uint8_t _regValue=0U;

    /* Read CONTROL register value */
    I2C_Driver_Read(LTC2942_ADDR,LTC2942_REG_CONTROL,I2C_MEMADD_SIZE_8BIT,&_regValue,1U);

    /* Set new state of SHUTDOWN bit in CONTROL register B[0] */
    if(LTC2942_AN_DISABLED==_state){_regValue|=LTC2942_CTL_SHUTDOWN;}
    else{_regValue&=~LTC2942_CTL_SHUTDOWN;}

    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_CONTROL, I2C_MEMADD_SIZE_8BIT,&_regValue,1U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set charge high threshold
  * @param  Value (uint16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_Charge_Threshold_H(uint16_t _level)
{
    uint8_t _regLevel=0U;

    _regLevel=(uint8_t)(_level>>8);
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_CTH_H, I2C_MEMADD_SIZE_8BIT,&_regLevel,1U);
    _regLevel=(uint8_t)(_level&0x00FF);
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_CTH_L, I2C_MEMADD_SIZE_8BIT,&_regLevel,1U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set charge low threshold
  * @param  Value (uint16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_Charge_Threshold_L(uint16_t _level)
{
    uint8_t _regLevel=0U;

    _regLevel=(uint8_t)(_level>>8);
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_CTL_H, I2C_MEMADD_SIZE_8BIT,&_regLevel,1U);
    _regLevel=(uint8_t)(_level&0x00FF);
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_CTL_L, I2C_MEMADD_SIZE_8BIT,&_regLevel,1U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set voltage high threshold
  * @param  Value (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_Voltage_Threshold_H(uint8_t _level)
{
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_VOLT_H, I2C_MEMADD_SIZE_8BIT,&_level,1U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set voltage low threshold
  * @param  Value (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_Voltage_Threshold_L(uint8_t _level)
{
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_VOLT_L, I2C_MEMADD_SIZE_8BIT,&_level,1U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set temperature high threshold
  * @param  Value (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_Temperature_Threshold_H(uint8_t _level)
{
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_TEMPT_H, I2C_MEMADD_SIZE_8BIT,&_level,1U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Set temperature low threshold
  * @param  Value (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void LTC2942_Set_Temperature_Threshold_L(uint8_t _level)
{
    I2C_Driver_Write(LTC2942_ADDR, LTC2942_REG_TEMPT_L, I2C_MEMADD_SIZE_8BIT,&_level,1U);
}
