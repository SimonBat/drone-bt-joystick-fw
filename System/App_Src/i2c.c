/**
  ***************************************************************************************************************************************
  * @file     i2c.c
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

#include "i2c.h"
#include "system_utick.h"

static i2c_ts H_I2C;

/* Local functions declarations */
static void I2C_Driver_Unlock_Bus(GPIO_TypeDef* _portSCL,GPIO_TypeDef* _portSDA,uint32_t _pinSCL,uint32_t _pinSDA);
static void I2C_Driver_GPIO_Wait_For_High(GPIO_TypeDef *_gpio,uint32_t _pin,uint16_t _timeoutUs);
static void I2C_Driver_Sleep_Us(uint32_t _us);

/**
  ***************************************************************************************************************************************
  * @brief  I2C1 Initialization Function
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void I2C_Driver_Init(void)
{
    GPIO_InitTypeDef _gpioInitStruct={0};

    if(H_I2C.initFlag){return;}
  
    SYSTEM_UTick_Init();
    /* Configure I2C sensors pins to unlock bus */
    _gpioInitStruct.Mode=GPIO_MODE_OUTPUT_OD;
    _gpioInitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    _gpioInitStruct.Pin=BSP_I2C_SCL_PIN; /* SCL */
    HAL_GPIO_Init(BSP_I2C_PORT, &_gpioInitStruct);
    _gpioInitStruct.Pin=BSP_I2C_SDA_PIN; /* SDA */
    HAL_GPIO_Init(BSP_I2C_PORT,&_gpioInitStruct);

    I2C_Driver_Unlock_Bus(BSP_I2C_PORT,BSP_I2C_PORT,BSP_I2C_SCL_PIN,BSP_I2C_SDA_PIN);
    SYSTEM_UTick_Deinit();

    H_I2C.hal.Instance=I2C1;
    H_I2C.hal.Init.ClockSpeed=400000U;
    H_I2C.hal.Init.DutyCycle=I2C_DUTYCYCLE_2;
	H_I2C.hal.Init.OwnAddress1=0U;
	H_I2C.hal.Init.AddressingMode=I2C_ADDRESSINGMODE_7BIT;
	H_I2C.hal.Init.DualAddressMode=I2C_DUALADDRESS_DISABLE;
	H_I2C.hal.Init.OwnAddress2=0U;
	H_I2C.hal.Init.GeneralCallMode=I2C_GENERALCALL_DISABLE;
	H_I2C.hal.Init.NoStretchMode=I2C_NOSTRETCH_DISABLE;
	if(HAL_OK!=HAL_I2C_Init(&H_I2C.hal)){BSP_Error_Handler();}

    H_I2C.lock=xSemaphoreCreateMutexStatic(&H_I2C.lockBuffer);

	if(NULL!=H_I2C.lock)
	{
		xSemaphoreGive(H_I2C.lock);
		H_I2C.initFlag=1U;
	}
    else{BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  I2C driver unlock bus
  * @param  SCL port (GPIO_TypeDef*), SDA port (GPIO_TypeDef*), SCL pin (uint32_t), SDA pin (uint32_t)
  * @retval None
  ***************************************************************************************************************************************
  */
static void I2C_Driver_Unlock_Bus(GPIO_TypeDef* _portSCL,GPIO_TypeDef* _portSDA,uint32_t _pinSCL,uint32_t _pinSDA)
{
    HAL_GPIO_WritePin(_portSDA,_pinSDA,GPIO_PIN_SET);
    /* Check SDA line to determine if slave is asserting bus and clock out if so */
    while(!HAL_GPIO_ReadPin(_portSDA,_pinSDA))
    {
        /* Set clock high */
        HAL_GPIO_WritePin(_portSCL,_pinSCL,GPIO_PIN_SET);
        /* Wait for any clock stretching to finish. */
        I2C_Driver_GPIO_Wait_For_High(_portSCL,_pinSCL,(10U*1000U));
        I2C_Driver_Sleep_Us(I2C_DRV_CLK_TS);
        /* Generate a clock cycle */
        HAL_GPIO_WritePin(_portSCL,_pinSCL,GPIO_PIN_RESET);
        I2C_Driver_Sleep_Us(I2C_DRV_CLK_TS);
        HAL_GPIO_WritePin(_portSCL,_pinSCL,GPIO_PIN_SET);
        I2C_Driver_Sleep_Us(I2C_DRV_CLK_TS);
    }

    /* Generate a start then stop condition */
    HAL_GPIO_WritePin(_portSCL,_pinSCL,GPIO_PIN_SET);
    I2C_Driver_Sleep_Us(I2C_DRV_CLK_TS);
    HAL_GPIO_WritePin(_portSDA,_pinSDA,GPIO_PIN_RESET);
    I2C_Driver_Sleep_Us(I2C_DRV_CLK_TS);
    HAL_GPIO_WritePin(_portSCL,_pinSCL,GPIO_PIN_RESET);
    I2C_Driver_Sleep_Us(I2C_DRV_CLK_TS);
    /* Set data and clock high and wait for any clock stretching to finish. */
    HAL_GPIO_WritePin(_portSDA,_pinSDA,GPIO_PIN_SET);
    HAL_GPIO_WritePin(_portSCL,_pinSCL,GPIO_PIN_SET);
    I2C_Driver_GPIO_Wait_For_High(_portSCL,_pinSCL,10U*1000U);
    /* Wait for data to be high */
    I2C_Driver_GPIO_Wait_For_High(_portSDA,_pinSDA,10U*1000U);
}

/**
  ***************************************************************************************************************************************
  * @brief  I2C wait for gpio pin high level
  * @param  Port (GPIO_TypeDef*), pin (uint32_t), timeout (uint16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
static void I2C_Driver_GPIO_Wait_For_High(GPIO_TypeDef *_gpio,uint32_t _pin,uint16_t _timeoutUs)
{
    __IO uint64_t _start=SYSTEM_UTick_Get_Timestamp();
    while((!HAL_GPIO_ReadPin(_gpio,_pin))&&((SYSTEM_UTick_Get_Timestamp()-_start)<=_timeoutUs)){__NOP();};
}

/**
  ***************************************************************************************************************************************
  * @brief  I2C driver sleep in microseconds
  * @param  Time (uint32_t)
  * @retval None
  ***************************************************************************************************************************************
  */
static void I2C_Driver_Sleep_Us(uint32_t _us)
{
    __IO uint64_t _start=SYSTEM_UTick_Get_Timestamp();
    while((_start+_us)>SYSTEM_UTick_Get_Timestamp()){__NOP();};
}

/**
  ***************************************************************************************************************************************
  * @brief I2C MSP Initialization
  * @param I2C handle (I2C_HandleTypeDef*)
  * @retval None
  ***************************************************************************************************************************************
  */
// cppcheck-suppress constParameterPointer
void HAL_I2C_MspInit(I2C_HandleTypeDef* _hi2c)
{
    GPIO_InitTypeDef _gpioInitStruct={0};

	if(I2C1==_hi2c->Instance)
	{
		/* Peripheral clock enable */
		__HAL_RCC_I2C1_CLK_ENABLE();

        _gpioInitStruct.Pin=BSP_I2C_SCL_PIN|BSP_I2C_SDA_PIN;
        _gpioInitStruct.Mode=GPIO_MODE_AF_OD;
        _gpioInitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(BSP_I2C_PORT,&_gpioInitStruct);
	}
}

/**
  ***************************************************************************************************************************************
  * @brief I2C MSP De-Initialization
  * @param I2C handle (I2C_HandleTypeDef*)
  * @retval None
  ***************************************************************************************************************************************
  */
// cppcheck-suppress constParameterPointer
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* _hi2c)
{
    if(I2C1==_hi2c->Instance)
    {
        /* Peripheral clock disable */
        __HAL_RCC_I2C1_CLK_DISABLE();

        HAL_GPIO_DeInit(BSP_I2C_PORT,BSP_I2C_SCL_PIN);
        HAL_GPIO_DeInit(BSP_I2C_PORT,BSP_I2C_SDA_PIN);
    }
}

/**
  ***************************************************************************************************************************************
  * @brief  I2C data write
  * @param  Device address (uint16_t), memory address (uint16_t), address size (uint16_t), data (uint8_t*), data size (uint16_t)
  * @retval Status (HAL_StatusTypeDef)
  ***************************************************************************************************************************************
  */
HAL_StatusTypeDef I2C_Driver_Write(uint16_t _devAddress, uint16_t _memAddress, uint16_t _memAddrSize, uint8_t *_data, uint16_t _dataSize)
{
    HAL_StatusTypeDef _halStatus=HAL_ERROR;

    if(!H_I2C.initFlag){return _halStatus;}

    xSemaphoreTake(H_I2C.lock,portMAX_DELAY);
	_halStatus=HAL_I2C_Mem_Write(&H_I2C.hal,_devAddress,_memAddress,_memAddrSize,_data,_dataSize,I2C_TIMEOUT);
    xSemaphoreGive(H_I2C.lock);

    return _halStatus;
}

/**
  ***************************************************************************************************************************************
  * @brief  I2C data read
  * @param  Device address (uint16_t), memory address (uint16_t), address size (uint16_t), data (uint8_t*), data size (uint16_t)
  * @retval Status (HAL_StatusTypeDef)
  ***************************************************************************************************************************************
  */
HAL_StatusTypeDef I2C_Driver_Read(uint16_t _devAddress, uint16_t _memAddress, uint16_t _memAddrSize, uint8_t *_data, uint16_t _dataSize)
{
    HAL_StatusTypeDef _halStatus=HAL_ERROR;

    if(!H_I2C.initFlag){return _halStatus;}

    xSemaphoreTake(H_I2C.lock,portMAX_DELAY);
	_halStatus=HAL_I2C_Mem_Read(&H_I2C.hal,_devAddress,_memAddress,_memAddrSize,_data,_dataSize,I2C_TIMEOUT);
    xSemaphoreGive(H_I2C.lock);

    return _halStatus;
}
