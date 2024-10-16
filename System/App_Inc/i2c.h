#ifndef __I2C_H
#define __I2C_H

#include "bsp.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define I2C_TIMEOUT		        	0xFFFF
#define I2C_DRV_CLK_TS          	10U

typedef struct{
	I2C_HandleTypeDef hal;
    xSemaphoreHandle lock;
    StaticSemaphore_t lockBuffer;
	uint8_t initFlag;
}i2c_ts;

/* Global functions declarations */
void I2C_Driver_Init(void);
HAL_StatusTypeDef I2C_Driver_Write(uint16_t _devAddress, uint16_t _memAddress, uint16_t _memAddrSize, uint8_t *_data, uint16_t _dataSize);
HAL_StatusTypeDef I2C_Driver_Read(uint16_t _devAddress, uint16_t _memAddress, uint16_t _memAddrSize, uint8_t *_data, uint16_t _dataSize);

#endif
