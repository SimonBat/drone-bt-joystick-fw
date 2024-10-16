#ifndef __SSD1306_DRAW_H
#define __SSD1306_DRAW_H

#include "bsp.h"

typedef enum{
    SSD1306_FORMAT_BAT=1,
    SSD1306_FORMAT_DBM=2
}ssd1306_value_te;

/* Global functions declarations */
void SSD1306_Draw_Arrow_UP(int16_t _x, int16_t _y);
void SSD1306_Draw_Arrow_DOWN(int16_t _x, int16_t _y);
void SSD1306_Draw_Arrow_UP_DOWN(int16_t _x, int16_t _y);
void SSD1306_Draw_Button_State(int16_t _x, int16_t _y, uint8_t _state);
void SSD1306_Draw_RSSI(int8_t _rssi);

#endif
