/**
  ***************************************************************************************************************************************
  * @file     uart.c
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

#include <stdio.h>
#include <string.h>
#include "uart.h"

/* Local functions declarations */
static void UART_Send_Byte(uart_ts *_huart, uint8_t _byte);
static void UART_IRQ_RXNE_Callback(uart_ts *_huart);
static void UART_IRQ_TXE_Callback(uart_ts *_huart);

/**
  ***************************************************************************************************************************************
  * @brief  UART Initialization
  * @param  UART handle (uart_ts*), port (USART_TypeDef*), baudrate (uint32_t)
  * @retval Status (int32_t)
  ***************************************************************************************************************************************
  */
int32_t UART_Init(uart_ts *_huart, USART_TypeDef *_instance, uint32_t _baudrate)
{
	int32_t _status=0;

	if(_huart->initFlag){return 1;}

	_huart->hal.Instance=_instance;
	_huart->hal.Init.BaudRate=_baudrate;
	_huart->hal.Init.WordLength=UART_WORDLENGTH_8B;
	_huart->hal.Init.StopBits=UART_STOPBITS_1;
	_huart->hal.Init.Parity=UART_PARITY_NONE;
	_huart->hal.Init.Mode=UART_MODE_TX_RX;
	_huart->hal.Init.HwFlowCtl=UART_HWCONTROL_NONE;
	_huart->hal.Init.OverSampling=UART_OVERSAMPLING_16;

	if(HAL_OK==HAL_UART_Init(&_huart->hal))
	{
		/* Create tx and rx data queue */
		_huart->txQueue=xQueueCreateStatic(UART_QUEUE_LENGTH,UART_QUEUE_ITEM_SIZE,_huart->txQueueStack,&_huart->txQueueBuffer);
		_huart->rxQueue=xQueueCreateStatic(UART_QUEUE_LENGTH,UART_QUEUE_ITEM_SIZE,_huart->rxQueueStack,&_huart->rxQueueBuffer);
		_huart->lock=xSemaphoreCreateMutexStatic(&_huart->lockBuffer);

		if((NULL!=_huart->txQueue)&&(NULL!=_huart->rxQueue)&&(NULL!=_huart->lock))
		{
			xSemaphoreGive(_huart->lock);
			_huart->initFlag=1U;
			_status=_huart->initFlag;
		}

		/* Turn on UART rx interupt */
		LL_USART_EnableIT_RXNE(_huart->hal.Instance);
	}

	return _status;
}

/**
  ***************************************************************************************************************************************
  * @brief  UART deinitialization
  * @param  UART handle (uart_ts*)
  * @retval None
  ***************************************************************************************************************************************
  */
void UART_DeInit(uart_ts *_huart)
{
	HAL_UART_DeInit(&_huart->hal);
	_huart->initFlag=0U;
}

/**
  ***************************************************************************************************************************************
  * @brief  UART MSP initialization
  * @param  UART handle pointer (UART_HandleTYpeDef*)
  * @retval None
  ***************************************************************************************************************************************
  */
// cppcheck-suppress constParameterPointer
void HAL_UART_MspInit(UART_HandleTypeDef *_huart)
{
	GPIO_InitTypeDef _gpioInitStruct={0};

    if(USART1==_huart->Instance)
	{
		/* Peripheral clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();

        _gpioInitStruct.Pin=UART1_RX_PIN;
        _gpioInitStruct.Mode=GPIO_MODE_AF_PP;
        _gpioInitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(UART1_RX_PORT, &_gpioInitStruct);

        _gpioInitStruct.Pin=UART1_TX_PIN;
        _gpioInitStruct.Mode=GPIO_MODE_INPUT;
        _gpioInitStruct.Pull=GPIO_PULLUP;
        _gpioInitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(UART1_RX_PORT, &_gpioInitStruct);

		/* UART interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn,UART1_INTERUPT_PRIORITY,0U);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
	else if(USART3==_huart->Instance)
	{
		/* Peripheral clock enable */
		__HAL_RCC_USART3_CLK_ENABLE();

        _gpioInitStruct.Pin=UART3_TX_PIN;
        _gpioInitStruct.Mode=GPIO_MODE_AF_PP;
        _gpioInitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(UART3_TX_PORT, &_gpioInitStruct);

        _gpioInitStruct.Pin=UART3_RX_PIN;
        _gpioInitStruct.Mode=GPIO_MODE_INPUT;
        _gpioInitStruct.Pull=GPIO_PULLUP;
        _gpioInitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(UART3_RX_PORT, &_gpioInitStruct);

		/* UART interrupt Init */
		HAL_NVIC_SetPriority(USART3_IRQn,UART3_INTERUPT_PRIORITY,0U);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
	}
}

/**
  ***************************************************************************************************************************************
  * @brief  UART MSP de-initialization
  * @param  UART handle pointer (UART_HandleTYpeDef*)
  * @retval None
  ***************************************************************************************************************************************
  */
// cppcheck-suppress constParameterPointer
void HAL_UART_MspDeInit(UART_HandleTypeDef *_huart)
{
    if(USART1==_huart->Instance)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();

		HAL_GPIO_DeInit(UART1_RX_PORT, UART1_RX_PIN);
        HAL_GPIO_DeInit(UART1_TX_PORT, UART1_TX_PIN);
	}
	else if(USART3==_huart->Instance)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USART3_CLK_DISABLE();

		HAL_GPIO_DeInit(UART3_RX_PORT, UART3_RX_PIN);
        HAL_GPIO_DeInit(UART3_TX_PORT, UART3_TX_PIN);
	}
}

/**
  ***************************************************************************************************************************************
  * @brief  Return hex digit of the byte
  * @param  byte (uint8_t)
  * @retval hex digit (uint32_t)
  ***************************************************************************************************************************************
  */
uint8_t UART_Get_HEX_Digit(uint8_t _n)
{
	if(_n<10U){return _n+'0';}
	else{return (_n-10U)+'A';}
}

/**
  ***************************************************************************************************************************************
  * @brief  Char to HEX symbols conversion
  * @param  Char value (uint8-t), buffer (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void UART_Char_To_Hex(uint8_t _c, uint8_t _hex[3])
{
	_hex[0]=UART_Get_HEX_Digit(_c/0x10);
	_hex[1]=UART_Get_HEX_Digit(_c%0x10);
	_hex[2]='\0';
}

/**
  ***************************************************************************************************************************************
  * @brief  Print HEX number
  * @param  UART handle (uart_ts*), number (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void UART_Send_HEX(uart_ts *_huart, uint8_t _ch)
{
	uint8_t _hex[2];
	_hex[0]=UART_Get_HEX_Digit(_ch/0x10);
	_hex[1]=UART_Get_HEX_Digit(_ch%0x10);
	UART_Send_Buffer(_huart,_hex,2U);
}

/**
  ***************************************************************************************************************************************
  * @brief  Send one byte to transmit buffer
  * @param  UART handle (uart_ts*), byte (uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
static void UART_Send_Byte(uart_ts *_huart, uint8_t _byte)
{
	if(!_huart->initFlag){return;}

	/* Add data to the transmit buffer */
	if(xQueueSend(_huart->txQueue,&_byte,portMAX_DELAY))
	{
		/* If transmit interrupt is disabled, enable it */
		if(!LL_USART_IsEnabledIT_TXE(_huart->hal.Instance)){LL_USART_EnableIT_TXE(_huart->hal.Instance);}
	}
}

/**
  ***************************************************************************************************************************************
  * @brief  Print text string to transmit buffer
  * @param  UART handle (uart_ts*), text string (const char*)
  * @retval None
  ***************************************************************************************************************************************
  */
void UART_Send_String(uart_ts *_huart, const char *_string)
{
	if(!_huart->initFlag){return;}

	xSemaphoreTake(_huart->lock,portMAX_DELAY);
	while(*_string){UART_Send_Byte(_huart,*_string++);}
	xSemaphoreGive(_huart->lock);
}

/**
  ***************************************************************************************************************************************
  * @brief  Print data to transmit buffer
  * @param  UART handle (uart_ts*), data buffer (uint8_t*), data length (unt16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void UART_Send_Buffer(uart_ts *_huart, const uint8_t *_data, uint16_t _len)
{
	if(!_huart->initFlag){return;}

	xSemaphoreTake(_huart->lock,portMAX_DELAY);
	while(_len--){UART_Send_Byte(_huart,*_data++);}
	xSemaphoreGive(_huart->lock);
}

/**
  ***************************************************************************************************************************************
  * @brief  Receive one byte from receive buffer
  * @param  UART handle (uart_ts*), byte buffer (uint8_t*), delay (TickType_t)
  * @retval Status (int32_t)
  ***************************************************************************************************************************************
  */
int32_t UART_Receive_Byte(uart_ts *_huart, uint8_t *_byte, TickType_t _ticksToWait)
{
	int32_t _status=0;

	if(!_huart->initFlag){return _status;}
	if(xQueueReceive(_huart->rxQueue,_byte,_ticksToWait)==pdPASS){_status=1;}

	return _status;
}

/**
  ***************************************************************************************************************************************
  * @brief  UART receive byte interupt callback
  * @param  UART handle (uart_ts*)
  * @retval None
  ***************************************************************************************************************************************
  */
void UART_IRQ_Handler(uart_ts *_huart)
{
	if(LL_USART_IsActiveFlag_RXNE(_huart->hal.Instance)&&LL_USART_IsEnabledIT_RXNE(_huart->hal.Instance))
	{UART_IRQ_RXNE_Callback(_huart);}

	if(LL_USART_IsEnabledIT_TXE(_huart->hal.Instance)&&LL_USART_IsActiveFlag_TXE(_huart->hal.Instance))
	{UART_IRQ_TXE_Callback(_huart);}
}

/**
  ***************************************************************************************************************************************
  * @brief  UART receive byte interupt callback
  * @param  UART handle (uart_ts*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void UART_IRQ_RXNE_Callback(uart_ts *_huart)
{
    uint8_t _byte;
	BaseType_t _xHigherPriorityTaskWoken=pdFALSE;

	/* RXNE flag will be cleared by reading of RDR register */
	_byte=LL_USART_ReceiveData8(_huart->hal.Instance);
	xQueueSendFromISR(_huart->rxQueue,&_byte,&_xHigherPriorityTaskWoken);

	/* Now the buffer is empty we can switch context if necessary. */
	if(_xHigherPriorityTaskWoken){taskYIELD();}
}

/**
  ***************************************************************************************************************************************
  * @brief  UART transmit byte interupt callback
  * @param  UART handle (uart_ts*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void UART_IRQ_TXE_Callback(uart_ts* _huart)
{
	uint8_t _byte;
	BaseType_t _xTaskWokenByReceive=pdFALSE;

	if(xQueueReceiveFromISR(_huart->txQueue,(void*)&_byte,&_xTaskWokenByReceive))
	{
		/* Send one byte */
		LL_USART_TransmitData8(_huart->hal.Instance,_byte);
	}
	else{LL_USART_DisableIT_TXE(_huart->hal.Instance);}

	if(_xTaskWokenByReceive){taskYIELD();}
}
