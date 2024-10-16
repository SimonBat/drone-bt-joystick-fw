/**
  ***************************************************************************************************************************************
  * @file     bsp.c
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

#include "bsp.h"
#include "led.h"

/**
  ***************************************************************************************************************************************
  * @brief System Clock Configuration
  * @param None
  * @retval None
  ***************************************************************************************************************************************
  */
void BSP_System_Clock_Config(void)
{
    RCC_OscInitTypeDef _rccOscInitStruct={0};
    RCC_ClkInitTypeDef _rccClkInitStruct={0};
    RCC_PeriphCLKInitTypeDef _periphClkInit={0};

    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    /* NOJTAG: JTAG-DP Disabled and SW-DP Enabled */
    __HAL_AFIO_REMAP_SWJ_NOJTAG();

    /* Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure */
    _rccOscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_HSE;
    _rccOscInitStruct.HSEState=RCC_HSE_ON;
    _rccOscInitStruct.HSEPredivValue=RCC_HSE_PREDIV_DIV2;
    _rccOscInitStruct.HSIState=RCC_HSI_ON;
    _rccOscInitStruct.PLL.PLLState=RCC_PLL_ON;
    _rccOscInitStruct.PLL.PLLSource=RCC_PLLSOURCE_HSE;
    _rccOscInitStruct.PLL.PLLMUL=RCC_PLL_MUL9;
    if(HAL_OK!=HAL_RCC_OscConfig(&_rccOscInitStruct)){BSP_Error_Handler();}

    /* Initializes the CPU, AHB and APB buses clocks */
    _rccClkInitStruct.ClockType=RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    _rccClkInitStruct.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
    _rccClkInitStruct.AHBCLKDivider=RCC_SYSCLK_DIV1;
    _rccClkInitStruct.APB1CLKDivider=RCC_HCLK_DIV2;
    _rccClkInitStruct.APB2CLKDivider=RCC_HCLK_DIV1;
    if(HAL_OK!=HAL_RCC_ClockConfig(&_rccClkInitStruct,FLASH_LATENCY_2)){BSP_Error_Handler();}

    _periphClkInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;
    _periphClkInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;
    if(HAL_OK!=HAL_RCCEx_PeriphCLKConfig(&_periphClkInit)){BSP_Error_Handler();}

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
}

/**
  ***************************************************************************************************************************************
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  ***************************************************************************************************************************************
  */
void BSP_System_GPIO_Init(void)
{
    GPIO_InitTypeDef _gpioInitStruct={0};
  
    _gpioInitStruct.Mode=GPIO_MODE_OUTPUT_PP;
    _gpioInitStruct.Pull=GPIO_NOPULL;
    _gpioInitStruct.Speed=GPIO_SPEED_FREQ_LOW;
    /* Configure GPIO pin: LTC_KILL_Pin */
    _gpioInitStruct.Pin=BSP_LTC_KILL_PIN;
    HAL_GPIO_WritePin(BSP_LTC_KILL_PORT,BSP_LTC_KILL_PIN,GPIO_PIN_SET);
    HAL_GPIO_Init(BSP_LTC_KILL_PORT,&_gpioInitStruct);

    _gpioInitStruct.Mode=GPIO_MODE_ANALOG;
    /* Configure GPIO pins : PA0 PA7 */
    _gpioInitStruct.Pin=GPIO_PIN_0|GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA,&_gpioInitStruct);
    /* Configure GPIO pins : PB0 PB1 PB2 */
    _gpioInitStruct.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
    HAL_GPIO_Init(GPIOB,&_gpioInitStruct);
    /* Configure GPIO pins : PC13 PC14 PC15 */
    _gpioInitStruct.Pin=GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOC,&_gpioInitStruct);

    _gpioInitStruct.Pin=BSP_LTC_INT_PIN;
    _gpioInitStruct.Mode=GPIO_MODE_INPUT;
    _gpioInitStruct.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(BSP_LTC_INT_PORT,&_gpioInitStruct);

    HAL_Delay(20U);
}

/**
  ***************************************************************************************************************************************
  * @brief This function is executed in case of error occurrence
  * @param None
  * @retval None
  ***************************************************************************************************************************************
  */
void BSP_Error_Handler(void)
{
    /* Only for test purposes for now */
	LED_On();
    HAL_Delay(100U);
    NVIC_SystemReset();
}

#ifdef  USE_FULL_ASSERT
/**
  ***************************************************************************************************************************************
  * @brief Reports the name of the source file and the source line number where the assert_param error has occurred
  * @param File (uint8_t*), line (uint32_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void BSP_Assert_Failed(uint8_t *_file,uint32_t _line)
{
    UNUSED(_file);
    UNUSED(_line);
}
#endif
