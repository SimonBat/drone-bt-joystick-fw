/**
  ***************************************************************************************************************************************
  * @file     rn4020.c
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
#include "rn4020.h"
#include "debug.h"
#include "profile.h"

static rn4020_ts H_RN4020;

static const uint8_t RN4020_DRONE_ADDR_1[]="9C956E7CF266"; /* Drone ESC+MCU */
static const uint8_t RN4020_DRONE_ADDR_2[]="9C956E7CF2A2"; /* Drone Dragonfly v2 */
static const uint8_t RN4020_DRONE_ADDR_3[]="001EC025B376"; /* Drone Dragonfly v1 */
static const uint8_t* RN4020_DRONE_ADDR[RN40020_SYSTEM_ADDR]={RN4020_DRONE_ADDR_2,RN4020_DRONE_ADDR_1,RN4020_DRONE_ADDR_3};

static const uint8_t RN4020_CMD_LED_ON[]="|O,02,02\r\n";
static const uint8_t RN4020_CMD_LED_OFF[]="|O,02,00\r\n";
static const uint8_t RN4020_CMD_SN_GET[]="GN\r\n";
static const uint8_t RN4020_CMD_SN_VALUE[]="CONSOLE";
static const uint8_t RN4020_CMD_SN_SET[]="SN,CONSOLE\r\n";
static const uint8_t RN4020_CMD_S_SET[]="S-,CONSOLE\r\n";
static const uint8_t RN4020_CMD_SR_GET[]="GR\r\n";
static const uint8_t RN4020_CMD_SR_VALUE[]="80100000";
static const uint8_t RN4020_CMD_SR_SET[]="SR,80100000\r\n";
static const uint8_t RN4020_CMD_R[]="R,1\r\n";
static const uint8_t RN4020_CMD_F[]="F\r\n";
static const uint8_t RN4020_CMD_X[]="X\r\n";
static const uint8_t RN4020_CMD_E[]="E,0,";
static const uint8_t RN4020_CMD_SS_GET[]="GS\r\n";
static const uint8_t RN4020_CMD_SS_VALUE[]="80000001";
static const uint8_t RN4020_CMD_SS_SET[]="SS,80000001\r\n";
static const uint8_t RN4020_CMD_ST_GET[]="GT\r\n";
static const uint8_t RN4020_CMD_ST_VALUE[]="0006,0000,0064";
static const uint8_t RN4020_CMD_ST_SET[]="ST,0006,0000,0064\r\n";
static const uint8_t RN4020_CMD_SP_GET[]="GP\r\n";
static const uint8_t RN4020_CMD_SP_VALUE[]="7";
static const uint8_t RN4020_CMD_SP_SET[]="SP,7\r\n";
static const uint8_t RN4020_CMD_LS[]="LS\r\n";
static const uint8_t RN4020_CMD_LC[]="LC\r\n";
static const uint8_t RN4020_CMD_K[]="K\r\n";
/* Info service - write comands */
static const uint8_t RN4020_CMD_W24[]="SUW,2A24,4A4F59535449434B\r\n"; /* JOYSTIC */
static const uint8_t RN4020_CMD_W25[]="SUW,2A25,4A3031\r\n"; /* D01 */
static const uint8_t RN4020_CMD_W26[]="SUW,2A26,56312E30\r\n"; /* V1.0 */
static const uint8_t RN4020_CMD_W27[]="SUW,2A27,56312E30\r\n"; /* V1.0 */
static const uint8_t RN4020_CMD_W28[]="SUW,2A28,56312E30\r\n"; /* V1.0 */
static const uint8_t RN4020_CMD_W29[]="SUW,2A29,53494D4F4E41535F32303135\r\n"; /* SIMONAS_2015 */
/* Private service commands */
static const uint8_t RN4020_CMD_PZ[]="PZ\r\n";
static const uint8_t RN4020_CMD_PS[]="PS,11223344556677889900AABBCCDDEEFF\r\n";
static const uint8_t RN4020_CMD_PC[]="PC,010203040506070809000A0B0C0D0E0F,02,0B\r\n";
/* Battery service */
uint8_t RN4020_CMD_RBATT[]="CURV,2A19\r\n";
/* RSSI value */
uint8_t RN4020_CMD_M[]="M\r\n";
/* Private service to send data */
uint8_t RN4020_CMD_WP[]="SUW,010203040506070809000A0B0C0D0E0F,";

/* Local functions declarations */
static void RN4020_Init_Handler(void *_params);
static void RN4020_Tx_Handler(void *_params);
static void RN4020_Rx_Handler(void *_params);
static uint8_t RN4020_Wait_Response(rn4020_ts* _rn4020,rn4020_rx_resp_te _respType, uint32_t _tmo);
static uint8_t RN4020_Check_Settings(rn4020_ts* _rn4020,const uint8_t* _cmd, const uint8_t* _settings, uint8_t _dataSize, uint32_t _tmo);
static uint8_t RN4020_Write_Settings(rn4020_ts* _rn4020,const uint8_t* _cmd, rn4020_rx_resp_te _respType, uint32_t _tmo);
static uint8_t RN4020_Write_Control_To_Server(rn4020_ts* _rn4020,uint8_t _v1,uint8_t _h1,uint8_t _v2,uint8_t _h2, \
                                              uint8_t _s1, uint8_t _s2,uint8_t _b1,uint8_t _b2, uint8_t _b3, uint8_t _b4, uint8_t _c);
static uint8_t RN4020_String_HEX_To_Number(uint8_t _nb1, uint8_t _nb2);
static char RN4020_Nible_To_Char(uint8_t _nible);

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 module initialization
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void RN4020_Init(void)
{
    GPIO_InitTypeDef _gpioInitStruct={0};

    _gpioInitStruct.Pin=RN4020_WAKE_HW_PIN|RN4020_WAKE_SW_PIN|RN4020_CMD_MLDP_PIN;
    _gpioInitStruct.Mode=GPIO_MODE_OUTPUT_PP;
    _gpioInitStruct.Pull=GPIO_NOPULL;
    _gpioInitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_WritePin(RN4020_GPIO_PORT,RN4020_WAKE_HW_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RN4020_GPIO_PORT,RN4020_WAKE_SW_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RN4020_GPIO_PORT,RN4020_CMD_MLDP_PIN,GPIO_PIN_RESET);
    HAL_GPIO_Init(RN4020_GPIO_PORT,&_gpioInitStruct);

	if(UART_Init(&H_RN4020.hUart,RN4020_PORT,RN4020_BAUDRATE))
    {
        H_RN4020.cmdBusyFlag=xSemaphoreCreateBinaryStatic(&H_RN4020.cmdBusyFlagBuffer);
        H_RN4020.connectionBusyFlag=xSemaphoreCreateBinaryStatic(&H_RN4020.connectionBusyFlagBuffer);
        H_RN4020.hTaskInit.handle=xTaskCreateStatic(RN4020_Init_Handler,"RN4020_Init_Handler",RN4020_TASK_STACK_SIZE, \
                                          	  	    NULL,(tskIDLE_PRIORITY+1U),H_RN4020.hTaskInit.stack,&H_RN4020.hTaskInit.buffer);
        H_RN4020.hTaskRx.handle=xTaskCreateStatic(RN4020_Rx_Handler,"RN4020_Rx_Handler",RN4020_TASK_STACK_SIZE, \
                                                  NULL,(tskIDLE_PRIORITY+1U),H_RN4020.hTaskRx.stack,&H_RN4020.hTaskRx.buffer);
        H_RN4020.hTaskTx.handle=xTaskCreateStatic(RN4020_Tx_Handler,"RN4020_Tx_Handler",RN4020_TASK_STACK_SIZE, \
                                                  NULL,(tskIDLE_PRIORITY+1U),H_RN4020.hTaskTx.stack,&H_RN4020.hTaskTx.buffer);
        
        if((NULL!=H_RN4020.hTaskInit.handle)&&(NULL!=H_RN4020.hTaskRx.handle)&&(NULL!=H_RN4020.hTaskTx.handle)&& \
           (NULL!=H_RN4020.cmdBusyFlag)&&(NULL!=H_RN4020.connectionBusyFlag))
        {
            H_RN4020.addrSetId=0U;
            H_RN4020.addrSet=RN4020_DRONE_ADDR[0];
            H_RN4020.initFlag=1U;
            vTaskSuspend(H_RN4020.hTaskTx.handle);
            xSemaphoreGive(H_RN4020.cmdBusyFlag);
            xSemaphoreTake(H_RN4020.connectionBusyFlag,0U);
        }else{BSP_Error_Handler();}
    }else{BSP_Error_Handler();}
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 initialization handler
  * @param  Parameters(void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void RN4020_Init_Handler(void *_params)
{
    __IO uint8_t _scanStatus;

    vTaskDelay(pdMS_TO_TICKS(10U));
    HAL_GPIO_WritePin(RN4020_GPIO_PORT,RN4020_WAKE_HW_PIN,GPIO_PIN_SET);
    HAL_GPIO_WritePin(RN4020_GPIO_PORT,RN4020_WAKE_SW_PIN,GPIO_PIN_SET);
    HAL_GPIO_WritePin(RN4020_GPIO_PORT,RN4020_CMD_MLDP_PIN,GPIO_PIN_SET);
    if(!RN4020_Wait_Response(&H_RN4020,RN4020_RX_RESP_CMD,5000U)){BSP_Error_Handler();}

	while(1U)
	{
        if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_LED_ON,RN4020_RX_RESP_AOK,100U)){BSP_Error_Handler();}
        if(!RN4020_Check_Settings(&H_RN4020,RN4020_CMD_ST_GET,RN4020_CMD_ST_VALUE,14U,100U))
        {if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_ST_SET,RN4020_RX_RESP_AOK,500U)){BSP_Error_Handler();}}

        if(!RN4020_Check_Settings(&H_RN4020,RN4020_CMD_SN_GET,RN4020_CMD_SN_VALUE,7U,100U))
        {
            if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_SN_SET,RN4020_RX_RESP_AOK,500U)){BSP_Error_Handler();}
            if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_S_SET,RN4020_RX_RESP_AOK,500U)){BSP_Error_Handler();}
        }

        if(!RN4020_Check_Settings(&H_RN4020,RN4020_CMD_SS_GET,RN4020_CMD_SS_VALUE,8U,100U))
        {if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_SS_SET,RN4020_RX_RESP_AOK,500U)){BSP_Error_Handler();}}

        if(!RN4020_Check_Settings(&H_RN4020,RN4020_CMD_SR_GET,RN4020_CMD_SR_VALUE,8U,100U))
        {if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_SR_SET,RN4020_RX_RESP_AOK,500U)){BSP_Error_Handler();}}

        if(!RN4020_Check_Settings(&H_RN4020,RN4020_CMD_SP_GET,RN4020_CMD_SP_VALUE,1U,100U))
        {if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_SP_SET,RN4020_RX_RESP_AOK,500U)){BSP_Error_Handler();}}

        if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_PZ,RN4020_RX_RESP_AOK,100U)){BSP_Error_Handler();}
        if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_PS,RN4020_RX_RESP_AOK,100U)){BSP_Error_Handler();}
        if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_PC,RN4020_RX_RESP_AOK,100U)){BSP_Error_Handler();}
        if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_R,RN4020_RX_RESP_CMD,3000U)){BSP_Error_Handler();}
        if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_LED_ON,RN4020_RX_RESP_AOK,100U)){BSP_Error_Handler();}
        vTaskDelay(pdMS_TO_TICKS(100U));

        /* Start scanning */
        while(1U)
        {
            _scanStatus=0U;
            if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_LED_OFF,RN4020_RX_RESP_AOK,100U)){BSP_Error_Handler();}
            if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_F,RN4020_RX_RESP_AOK,100U)){BSP_Error_Handler();}
            if(RN4020_Wait_Response(&H_RN4020,RN4020_RX_RESP_ADDR,5000U)){_scanStatus=1U;}
            if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_X,RN4020_RX_RESP_AOK,100U)){BSP_Error_Handler();}
            if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_LED_ON,RN4020_RX_RESP_AOK,100U)){BSP_Error_Handler();}
            vTaskDelay(pdMS_TO_TICKS(100U));

            if(_scanStatus)
            {       
                H_RN4020.txDataBuffer[0]='\0';
	            strcat((char*)H_RN4020.txDataBuffer,(char*)RN4020_CMD_E);
                strcat((char*)H_RN4020.txDataBuffer,(char*)H_RN4020.addrSet);
                strcat((char*)H_RN4020.txDataBuffer,"\r\n");
                if(!RN4020_Write_Settings(&H_RN4020,H_RN4020.txDataBuffer,RN4020_RX_RESP_AOK,100U)){BSP_Error_Handler();}

                if(RN4020_Wait_Response(&H_RN4020,RN4020_RX_RESP_CONNECTED,3000U))
                { 
                    H_RN4020.displayValueUpdateTmo=RN4020_DISPLAY_VALUE_UPDATE_TMO;            
                    if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_LS,RN4020_RX_RESP_END,20U)){BSP_Error_Handler();}
                    if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_LC,RN4020_RX_RESP_END,20U)){BSP_Error_Handler();}
                    if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_W24,RN4020_RX_RESP_AOK,20U)){BSP_Error_Handler();}
                    if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_W25,RN4020_RX_RESP_AOK,20U)){BSP_Error_Handler();}
                    if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_W26,RN4020_RX_RESP_AOK,20U)){BSP_Error_Handler();}
                    if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_W27,RN4020_RX_RESP_AOK,20U)){BSP_Error_Handler();}
                    if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_W28,RN4020_RX_RESP_AOK,20U)){BSP_Error_Handler();}
                    if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_W29,RN4020_RX_RESP_AOK,20U)){BSP_Error_Handler();}
                    if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_LED_OFF,RN4020_RX_RESP_AOK,20U)){BSP_Error_Handler();}
         
                    vTaskDelay(pdMS_TO_TICKS(100U));
                    H_RN4020.connectionEndState=0x00;
                    vTaskResume(H_RN4020.hTaskTx.handle);
                    vTaskSuspend(NULL);
                    vTaskDelay(pdMS_TO_TICKS(100U));
                }
            }
        }
	}

	/* Task Error */
	vTaskDelete(NULL);
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 transmit handler
  * @param  Parameters(void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void RN4020_Tx_Handler(void *_params)
{
    int8_t _joystickRssi;
    uint8_t _droneBattery;

    vTaskDelay(pdMS_TO_TICKS(10U));

	while(1U)
	{
        if(pdTRUE==xSemaphoreTake(H_RN4020.connectionBusyFlag,pdMS_TO_TICKS(10U)))
        {
            PROFILE_Set_Drone_Battery_SOC(0U);
            PROFILE_Set_Drone_TX_Power_Value(0);
            PROFILE_Set_Joystick_RSSI_Value(0);
            H_RN4020.rssiAvgSetFlag=0U;
            H_RN4020.connectionErrorIdx=0U;
            vTaskResume(H_RN4020.hTaskInit.handle);
            vTaskSuspend(NULL);
        }
        else if(H_RN4020.connectionFlag)
        {
            if(++H_RN4020.displayValueUpdateTmo>RN4020_DISPLAY_VALUE_UPDATE_TMO)
            {
                H_RN4020.displayValueUpdateTmo=0U;
                /* Read drone battery level */
                if(RN4020_Write_Settings(&H_RN4020,RN4020_CMD_RBATT,RN4020_RX_RESP_VALUE,20U))
                {
                    _droneBattery=RN4020_String_HEX_To_Number((H_RN4020.rxDataBuffer[0]-48U),(H_RN4020.rxDataBuffer[1]-48U));
                    PROFILE_Set_Drone_Battery_SOC(_droneBattery);
                    if(H_RN4020.connectionErrorIdx){H_RN4020.connectionErrorIdx--;}
                }
                else{H_RN4020.connectionErrorIdx++;}

                /* Read joystick RSSI level */
                if(RN4020_Write_Settings(&H_RN4020,RN4020_CMD_M,RN4020_RX_RESP_RSSI,20U))
                {
                    _joystickRssi=0-RN4020_String_HEX_To_Number((H_RN4020.rxDataBuffer[0]-48U),(H_RN4020.rxDataBuffer[1]-48U));
            
                    if(!H_RN4020.rssiAvgSetFlag)
                    {
                        H_RN4020.rssiAvgValue=_joystickRssi;
                        H_RN4020.rssiAvgValueF=(float)_joystickRssi;
                        H_RN4020.rssiAvgSetFlag=1U;
                    }
                    else
                    {
                        H_RN4020.rssiAvgValueF=(H_RN4020.rssiAvgValueF*4.0f+(float)_joystickRssi)/5.0f;
                        H_RN4020.rssiAvgValue=(int8_t)(H_RN4020.rssiAvgValueF+0.5f);
                    }

                    PROFILE_Set_Joystick_RSSI_Value(H_RN4020.rssiAvgValue);                
                }

                PROFILE_Set_Drone_TX_Power_Value(7);
            }

            /* Write control to the drone BT server */
            if(RN4020_Write_Control_To_Server(&H_RN4020, \
                                              PROFILE_Get_Joystick_V1_Status(), \
                                              PROFILE_Get_Joystick_H1_Status(), \
                                              PROFILE_Get_Joystick_V2_Status(), \
                                              PROFILE_Get_Joystick_H2_Status(), \
                                              PROFILE_Get_Joystick_SEL1_Status(), \
                                              PROFILE_Get_Joystick_SEL2_Status(), \
                                              PROFILE_Get_Joystick_Video_EN_Status(), \
                                              PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_2), \
                                              PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_3), \
                                              PROFILE_Get_Joystick_Button_Status(PROFILE_BUTTON_4), \
                                              0U))
            {
                if(H_RN4020.connectionErrorIdx){H_RN4020.connectionErrorIdx--;}
            }
            else{H_RN4020.connectionErrorIdx++;}

            if(++H_RN4020.txBlinkTmo>20U)
            {
                H_RN4020.txBlinkTmo=0U;
                if(!H_RN4020.txBlinkState)
                {
                    H_RN4020.txBlinkState=1U;
                    RN4020_Write_Settings(&H_RN4020,RN4020_CMD_LED_ON,RN4020_RX_RESP_AOK,20U);
                }      
            }
            else if(H_RN4020.txBlinkState)
            {
                H_RN4020.txBlinkTmo=0U;
                H_RN4020.txBlinkState=0U;
                RN4020_Write_Settings(&H_RN4020,RN4020_CMD_LED_OFF,RN4020_RX_RESP_AOK,20U);
            }
        }
   
        if(H_RN4020.connectionErrorIdx>RN4020_CONNECTION_ERROR_IDX)
        {
            H_RN4020.connectionEndState=0x00;
            if(!RN4020_Write_Settings(&H_RN4020,RN4020_CMD_K,RN4020_RX_RESP_CONNECTION_END,20U))
            {               
                H_RN4020.rxState=0x00;
                H_RN4020.connectionFlag=0U;
                H_RN4020.rxResp=RN4020_RX_RESP_CONNECTION_END;
                xSemaphoreGive(H_RN4020.connectionBusyFlag);
                xSemaphoreGive(H_RN4020.cmdBusyFlag);
            }
        }
	}

	/* Task Error */
	vTaskDelete(NULL);
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 receive handler
  * @param  Parameters(void*)
  * @retval None
  ***************************************************************************************************************************************
  */
static void RN4020_Rx_Handler(void *_params)
{
    uint8_t _byte;

	while(1U)
	{
        if(UART_Receive_Byte(&H_RN4020.hUart,&_byte,H_RN4020.rxTmo))
		{
            switch(H_RN4020.rxState)
            {
                case(0x00):
                	if('A'==_byte){H_RN4020.rxState=0x01;}
                	else if('C'==_byte){H_RN4020.rxState=0x03;}
                	else if(H_RN4020.addrSet[0]==_byte){H_RN4020.rxState=0x05;} /* Address */
                	else if('E'==_byte){H_RN4020.rxState=0x18;}
                	else if('R'==_byte){H_RN4020.rxState=0x1A;}
                	else if('-'==_byte){H_RN4020.rxState=0x1C; H_RN4020.rxDataIdx=0U;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x01):
                	if('O'==_byte){H_RN4020.rxState=0x02;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x02):
                	if('K'==_byte)
                	{
                		H_RN4020.rxState=0xFE;
                		H_RN4020.rxResp=RN4020_RX_RESP_AOK;
                	}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x03):
                	if('M'==_byte){H_RN4020.rxState=0x04;}
                	else if('o'==_byte){H_RN4020.rxState=0x11;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x04):
                	if('D'==_byte)
                	{
                		H_RN4020.rxState=0xFE;
                		H_RN4020.rxResp=RN4020_RX_RESP_CMD;
                	}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x05):
                	if(H_RN4020.addrSet[1]==_byte){H_RN4020.rxState=0x06;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x06):
                	if(H_RN4020.addrSet[2]==_byte){H_RN4020.rxState=0x07;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x07):
                	if(H_RN4020.addrSet[3]==_byte){H_RN4020.rxState=0x08;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x08):
                	if(H_RN4020.addrSet[4]==_byte){H_RN4020.rxState=0x09;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x09):
                	if(H_RN4020.addrSet[5]==_byte){H_RN4020.rxState=0x0A;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x0A):
                	if(H_RN4020.addrSet[6]==_byte){H_RN4020.rxState=0x0B;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x0B):
                	if(_byte==H_RN4020.addrSet[7]){H_RN4020.rxState=0x0C;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x0C):
                	if(H_RN4020.addrSet[8]==_byte){H_RN4020.rxState=0x0D;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x0D):
                	if(H_RN4020.addrSet[9]==_byte){H_RN4020.rxState=0x0E;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x0E):
                	if(H_RN4020.addrSet[10]==_byte){H_RN4020.rxState=0x0F;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x0F):
                	if(H_RN4020.addrSet[11]==_byte){H_RN4020.rxState=0x10;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x10):
                	if('\r'==_byte)
                	{
                		H_RN4020.rxState=0xFF;
                		H_RN4020.rxResp=RN4020_RX_RESP_ADDR;
                	}
                break;

                case(0x11):
                	if('n'==_byte){H_RN4020.rxState=0x12;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x12):
                	if('n'==_byte){H_RN4020.rxState=0x13;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x13):
                	if('e'==_byte){H_RN4020.rxState=0x14;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x14):
                	if('c'==_byte){H_RN4020.rxState=0x15;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x15):
                	if('t'==_byte){H_RN4020.rxState=0x16;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x16):
                	if('e'==_byte){H_RN4020.rxState=0x17;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x17):
                	if('d'==_byte)
                	{
                		H_RN4020.rxState=0xFE;
                		H_RN4020.connectionFlag=1U;
                		H_RN4020.rxResp=RN4020_RX_RESP_CONNECTED;
                	}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x18):
                	if('N'==_byte){H_RN4020.rxState=0x19;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x19):
                	if('D'==_byte)
                	{
                		H_RN4020.rxState=0xFE;
                		H_RN4020.rxResp=RN4020_RX_RESP_END;
                	}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x1A):
                	if(','==_byte){H_RN4020.rxState=0x1B; H_RN4020.rxDataIdx=0U;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0x1B):
                	if('.'!=_byte){H_RN4020.rxDataBuffer[H_RN4020.rxDataIdx++]=_byte;}
                	else
                	{
                		H_RN4020.rxState=0xFE;
                		H_RN4020.rxResp=RN4020_RX_RESP_VALUE;
                	}
                break;

                case(0x1C):
                	if('\r'!=_byte){H_RN4020.rxDataBuffer[H_RN4020.rxDataIdx++]=_byte;}
                	else
                	{
                		H_RN4020.rxState=0xFF;
                		H_RN4020.rxResp=RN4020_RX_RESP_RSSI;
                	}
                break;

                case(0xFD):
                	if('\r'!=_byte){H_RN4020.rxDataBuffer[H_RN4020.rxDataIdx++]=_byte;}
                	else
                	{
                		H_RN4020.rxState=0xFF;
                		H_RN4020.rxResp=RN4020_RX_RESP_DATA;
                	}
                break;

                case(0xFE):
                	if('\r'==_byte){H_RN4020.rxState=0xFF;}
                	else{H_RN4020.rxState=0x00;}
                break;

                case(0xFF):
                	if('\n'==_byte){xSemaphoreGive(H_RN4020.cmdBusyFlag);}
                	H_RN4020.rxState=0x00;
                break;

                default:
                break;
            }

            switch(H_RN4020.connectionEndState)
            {
                case(0x00):
                	if('C'==_byte){H_RN4020.connectionEndState=0x01;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x01):
                	if('o'==_byte){H_RN4020.connectionEndState=0x02;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x02):
                	if('n'==_byte){H_RN4020.connectionEndState=0x03;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x03):
                	if('n'==_byte){H_RN4020.connectionEndState=0x04;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x04):
                	if('e'==_byte){H_RN4020.connectionEndState=0x05;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x05):
                	if('c'==_byte){H_RN4020.connectionEndState=0x06;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x06):
                	if('t'==_byte){H_RN4020.connectionEndState=0x07;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x07):
                	if('i'==_byte){H_RN4020.connectionEndState=0x08;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x08):
                	if('o'==_byte){H_RN4020.connectionEndState=0x09;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x09):
                	if('n'==_byte){H_RN4020.connectionEndState=0x0A;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x0A):
                	if(' '==_byte){H_RN4020.connectionEndState=0x0B;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x0B):
                	if('E'==_byte){H_RN4020.connectionEndState=0x0C;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x0C):
                	if('n'==_byte){H_RN4020.connectionEndState=0x0D;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x0D):
                	if('d'==_byte){H_RN4020.connectionEndState=0x0E;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x0E):
                	if('\r'==_byte){H_RN4020.connectionEndState=0x0F;}
                	else{H_RN4020.connectionEndState=0x00;}
                break;

                case(0x0F):
                	if('\n'==_byte)
                	{
                		H_RN4020.connectionFlag=0U;
                		H_RN4020.rxResp=RN4020_RX_RESP_CONNECTION_END;
                		xSemaphoreGive(H_RN4020.connectionBusyFlag);
                		xSemaphoreGive(H_RN4020.cmdBusyFlag);
                	}
                	H_RN4020.rxState=0x00;
                	H_RN4020.connectionEndState=0x00;
                break;

                default:
                break;
            }

            DEBUG_Send_Char(_byte);
		}
		else{xSemaphoreGive(H_RN4020.cmdBusyFlag);}
	}

	/* Task Error */
	vTaskDelete(NULL);
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 wait response
  * @param  RN4020 handle (rn4020_ts*), response type (rn4020_rx_resp_te), tmo (uint32_t)
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
static uint8_t RN4020_Wait_Response(rn4020_ts* _rn4020,rn4020_rx_resp_te _respType, uint32_t _tmo)
{
    uint8_t _status=1U;

    xSemaphoreTake(_rn4020->cmdBusyFlag,portMAX_DELAY);

    if(RN4020_RX_RESP_DATA==_respType){_rn4020->rxState=0xFD;}
    else {_rn4020->rxState=0x00;}

    _rn4020->rxTmo=portMAX_DELAY;
    _rn4020->rxResp=RN4020_RX_RESP_NONE;

    uint32_t _tickStart=xTaskGetTickCount();

    while(_rn4020->rxResp!=_respType)
    {
    	uint32_t _tick=xTaskGetTickCount();

		if((_tick-_tickStart)>_tmo)
        {
            xSemaphoreGive(_rn4020->cmdBusyFlag);
            _status=0U;
            break;
        }
    }

    return _status;
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 check settings
  * @param  RN4020 handle (rn4020_ts*), cmd (const uint8_t*), settings (const uint8_t*), data size (uint8_t), tmo (uint32_t)
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
static uint8_t RN4020_Check_Settings(rn4020_ts* _rn4020,const uint8_t* _cmd, const uint8_t* _settings, uint8_t _dataSize, uint32_t _tmo)
{
	uint8_t _status=1U;
   
    xSemaphoreTake(_rn4020->cmdBusyFlag,portMAX_DELAY);

    _rn4020->rxDataIdx=0U;
    _rn4020->rxState=0xFD;
    _rn4020->rxTmo=portMAX_DELAY;
    _rn4020->rxResp=RN4020_RX_RESP_NONE;
    UART_Send_String(&_rn4020->hUart,(const char*)_cmd);
    DEBUG_Send_Message((const char*)_cmd);

    uint32_t _tickStart=xTaskGetTickCount();

    while(RN4020_RX_RESP_DATA!=_rn4020->rxResp)
    {
    	uint32_t _tick=xTaskGetTickCount();

		if((_tick-_tickStart)>_tmo)
        {
            xSemaphoreGive(_rn4020->cmdBusyFlag);
            _status=0U;
            break;
        }
    }

    if(_status)
    {
        for(uint8_t _idx=0U;_idx<_dataSize;_idx++)
        {
            if(_rn4020->rxDataBuffer[_idx]!=_settings[_idx])
            {
                _status=0U;
                break;
            }
        }
    }

    return _status;
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 write settings
  * @param  RN4020 handle (rn4020_ts*), cmd (const uint8_t*), response type (rn4020_rx_resp_te), tmo (uint32_t)
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
static uint8_t RN4020_Write_Settings(rn4020_ts* _rn4020,const uint8_t* _cmd, rn4020_rx_resp_te _respType, uint32_t _tmo)
{
    uint8_t _status=1U;
   
    xSemaphoreTake(_rn4020->cmdBusyFlag,portMAX_DELAY);
    _rn4020->rxDataIdx=0U;
    _rn4020->rxState=0x00;
    _rn4020->rxTmo=portMAX_DELAY;
    _rn4020->rxResp=RN4020_RX_RESP_NONE;
    UART_Send_String(&_rn4020->hUart,(const char*)_cmd);
    DEBUG_Send_Message((const char*)_cmd);

    uint32_t _tickStart=xTaskGetTickCount();

    while(_rn4020->rxResp!=_respType)
    {
    	uint32_t _tick=xTaskGetTickCount();

		if((_tick-_tickStart)>_tmo)
        {
            xSemaphoreGive(_rn4020->cmdBusyFlag);
            _status=0U;
            break;
        }
    }

    return _status;
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 write control to server
  * @param  RN4020 handle (rn4020_ts*), v1 (uint8_t), h1 (uint8_t), v2 (uint8_t), h2 (uint8_t), s1 (uint8_t), s2 (uint8_t), 
  *         b1 (uint8_t), b2 (uint8_t), b3 (uint8_t), b4 (uint8_t), c (uint8_t)
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
static uint8_t RN4020_Write_Control_To_Server(rn4020_ts* _rn4020,uint8_t _v1,uint8_t _h1,uint8_t _v2, \
                                              uint8_t _h2,uint8_t _s1, uint8_t _s2,uint8_t _b1,uint8_t _b2, uint8_t _b3, uint8_t _b4, uint8_t _c)
{
    uint8_t _status=0U;

	_rn4020->txDataBuffer[0]='\0';
	strcat((char*)_rn4020->txDataBuffer,(char*)RN4020_CMD_WP);
	_rn4020->txDataBuffer[37]=RN4020_Nible_To_Char(((_v1>>4)&0x0F));
	_rn4020->txDataBuffer[38]=RN4020_Nible_To_Char((_v1&0x0F));
	_rn4020->txDataBuffer[39]=RN4020_Nible_To_Char(((_h1>>4)&0x0F));
	_rn4020->txDataBuffer[40]=RN4020_Nible_To_Char((_h1&0x0F));
	_rn4020->txDataBuffer[41]=RN4020_Nible_To_Char(((_v2>>4)&0x0F));
	_rn4020->txDataBuffer[42]=RN4020_Nible_To_Char((_v2&0x0F));
	_rn4020->txDataBuffer[43]=RN4020_Nible_To_Char(((_h2>>4)&0x0F));
	_rn4020->txDataBuffer[44]=RN4020_Nible_To_Char((_h2&0x0F));
	_rn4020->txDataBuffer[45]=RN4020_Nible_To_Char(((_s1>>4)&0x0F));
	_rn4020->txDataBuffer[46]=RN4020_Nible_To_Char((_s1&0x0F));
	_rn4020->txDataBuffer[47]=RN4020_Nible_To_Char(((_s2>>4)&0x0F));
	_rn4020->txDataBuffer[48]=RN4020_Nible_To_Char((_s2&0x0F));
	_rn4020->txDataBuffer[49]=RN4020_Nible_To_Char(((_b1>>4)&0x0F));
	_rn4020->txDataBuffer[50]=RN4020_Nible_To_Char((_b1&0x0F));
	_rn4020->txDataBuffer[51]=RN4020_Nible_To_Char(((_b2>>4)&0x0F));
	_rn4020->txDataBuffer[52]=RN4020_Nible_To_Char((_b2&0x0F));
	_rn4020->txDataBuffer[53]=RN4020_Nible_To_Char(((_b3>>4)&0x0F));
	_rn4020->txDataBuffer[54]=RN4020_Nible_To_Char((_b3&0x0F));
	_rn4020->txDataBuffer[55]=RN4020_Nible_To_Char(((_b4>>4)&0x0F));
	_rn4020->txDataBuffer[56]=RN4020_Nible_To_Char((_b4&0x0F));
	_rn4020->txDataBuffer[57]=RN4020_Nible_To_Char(((_c>>4)&0x0F));
	_rn4020->txDataBuffer[58]=RN4020_Nible_To_Char((_c&0x0F));
	_rn4020->txDataBuffer[59]='\r';
	_rn4020->txDataBuffer[60]='\n';
	_rn4020->txDataBuffer[61]='\0';
    _status=RN4020_Write_Settings(&H_RN4020,_rn4020->txDataBuffer,RN4020_RX_RESP_AOK,20U);

    return _status;
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 convert string HEX to number
  * @param  Nb1 (uint8_t), nb2 (uint8_t)
  * @retval Number (uint8_t)
  ***************************************************************************************************************************************
  */
static uint8_t RN4020_String_HEX_To_Number(uint8_t _nb1, uint8_t _nb2)
{
    switch(_nb1)
	{
        /* 'A'-48 */
		case 17U: _nb1=10U; break;
        /* 'B'-48 */
		case 18U: _nb1=11U; break;
        /* 'C'-48 */
		case 19U: _nb1=12U; break;
        /* 'D'-48 */
		case 20U: _nb1=13U; break;
        /* 'E'-48 */
		case 21U: _nb1=14U; break;
        /* 'F'-48 */
		case 22U: _nb1=15U; break;
	}
	switch(_nb2)
	{
        /* 'A'-48 */
		case 17U: _nb2=10U; break;
        /* 'B'-48 */
		case 18U: _nb2=11U; break;
        /* 'C'-48 */
		case 19U: _nb2=12U; break;
        /* 'D'-48 */
		case 20U: _nb2=13U; break;
        /* 'E'-48 */
		case 21U: _nb2=14U; break;
        /* 'F'-48 */
		case 22U: _nb2=15U; break;
	}

	return ((_nb1<<4)|_nb2);
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 convert nible to char
  * @param  Nible (uint8_t)
  * @retval Value (char)
  ***************************************************************************************************************************************
  */
static char RN4020_Nible_To_Char(uint8_t _nible)
{
    switch(_nible)
	{
		case 10U: _nible=17U; break;
		case 11U: _nible=18U; break;
		case 12U: _nible=19U; break;
		case 13U: _nible=20U; break;
		case 14U: _nible=21U; break;
		case 15U: _nible=22U; break;
	}

	return (_nible+48U);
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 get connection status
  * @param  None
  * @retval Status (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t RN4020_Get_Connection_Status(void)
{
    return H_RN4020.connectionFlag;
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 change target address
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void RN4020_Change_Target_Address(void)
{
    H_RN4020.addrSetId++;
    if(H_RN4020.addrSetId>=RN40020_SYSTEM_ADDR){H_RN4020.addrSetId=0U;}
    H_RN4020.addrSet=RN4020_DRONE_ADDR[H_RN4020.addrSetId];
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 get target address ID
  * @param  None
  * @retval ID (uint8_t)
  ***************************************************************************************************************************************
  */
uint8_t RN4020_Get_Target_Address_ID(void)
{
    return H_RN4020.addrSetId;
}

/**
  ***************************************************************************************************************************************
  * @brief  RN4020 communication IRQ handler
  * @param  None
  * @retval None
  ***************************************************************************************************************************************
  */
void RN4020_PORT_IRQ_Handler(void)
{
    UART_IRQ_Handler(&H_RN4020.hUart);
}
