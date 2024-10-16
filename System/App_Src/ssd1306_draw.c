/**
  ***************************************************************************************************************************************
  * @file     ssd1306_draw.c
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
#include "ssd1306_draw.h"
#include "ssd1306_fonts.h"
#include "ssd1306.h"

/**
  ***************************************************************************************************************************************
  * @brief  Draw arrow UP
  * @param  X(int16_t), y(int16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void SSD1306_Draw_Arrow_UP(int16_t _x, int16_t _y)
{
    SSD1306_Draw_Filled_Rectangle(_x,_y,10U,5U,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-5,_y-1,_x+15,_y-1,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-4,_y-2,_x+14,_y-2,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-3,_y-3,_x+13,_y-3,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-2,_y-4,_x+12,_y-4,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-1,_y-5,_x+11,_y-5,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x,_y-6,_x+10,_y-6,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x+1,_y-7,_x+9,_y-7,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x+2,_y-8,_x+8,_y-8,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x+3,_y-9,_x+7,_y-9,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x+4,_y-10,_x+6,_y-10,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x+5,_y-11,_x+5,_y-11,OLED_COLOR_WHITE);
}

/**
  ***************************************************************************************************************************************
  * @brief  Draw arrow DOWN
  * @param  X(int16_t), y(int16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void SSD1306_Draw_Arrow_DOWN(int16_t _x, int16_t _y)
{
	SSD1306_Draw_Filled_Rectangle(_x,_y-11,10U,5U,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-5,_y-5,_x+15,_y-5,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-4,_y-4,_x+14,_y-4,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-3,_y-3,_x+13,_y-3,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-2,_y-2,_x+12,_y-2,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-1,_y-1,_x+11,_y-1,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x,_y,_x+10,_y,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x+1,_y+1,_x+9,_y+1,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x+2,_y+2,_x+8,_y+2,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x+3,_y+3,_x+7,_y+3,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x+4,_y+4,_x+6,_y+4,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x+5,_y+5,_x+5,_y+5,OLED_COLOR_WHITE);
}

/**
  ***************************************************************************************************************************************
  * @brief  Draw arrow UP/DOWN
  * @param  X(int16_t), y(int16_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void SSD1306_Draw_Arrow_UP_DOWN(int16_t _x, int16_t _y)
{
	SSD1306_Draw_Line(_x,_y,_x,_y,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-2,_y-1,_x+2,_y-1,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-4,_y-2,_x+4,_y-2,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-6,_y-3,_x+6,_y-3,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-8,_y-4,_x+8,_y-4,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-10,_y-5,_x+10,_y-5,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-5,_y-6,_x+5,_y-6,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-5,_y-7,_x+5,_y-7,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-5,_y-8,_x+5,_y-8,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-5,_y-9,_x+5,_y-9,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-10,_y-10,_x+10,_y-10,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-8,_y-11,_x+8,_y-11,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-6,_y-12,_x+6,_y-12,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-4,_y-13,_x+4,_y-13,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x-2,_y-14,_x+2,_y-14,OLED_COLOR_WHITE);
	SSD1306_Draw_Line(_x,_y-15,_x,_y-15,OLED_COLOR_WHITE);
}

/**
  ***************************************************************************************************************************************
  * @brief  Draw button state
  * @param  X(int16_t), y(int16_t), state(uint8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void SSD1306_Draw_Button_State(int16_t _x, int16_t _y, uint8_t _state)
{
	if(!_state){SSD1306_Draw_Rectangle(_x,_y,8U,8U,OLED_COLOR_WHITE);}
	else{SSD1306_Draw_Filled_Rectangle(_x,_y,8U,8U,OLED_COLOR_WHITE);}
}

/**
  ***************************************************************************************************************************************
  * @brief  Draw RSSI
  * @param  RSSI(int8_t)
  * @retval None
  ***************************************************************************************************************************************
  */
void SSD1306_Draw_RSSI(int8_t _rssi)
{
	uint8_t _recHeight=0U;

	if(_rssi<-100){_rssi=-100;}
	if(_rssi>0){_rssi=0;}
	_rssi=_rssi*(-1);
	_recHeight=_rssi/5;
	SSD1306_Draw_Filled_Rectangle(119,23,8U,20U,OLED_COLOR_BLACK);
	SSD1306_Draw_Filled_Rectangle(119,23+(20-_recHeight),8U,_recHeight,OLED_COLOR_WHITE);
}
