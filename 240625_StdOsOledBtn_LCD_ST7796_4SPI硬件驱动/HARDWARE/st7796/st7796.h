#ifndef __ST7796_H
#define __ST7796_H
#include "stdint.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"

void st7796_bsp_init(void);
void st7796_init(void);
void st7796_write_cmd(uint8_t cmd_byte);

void st7796_write_data(uint8_t data_byte);

void st7796_DrawPoint(u16 x,u16 y,u16 color);
void st7796_ScreenClear(u16 color);
void st7796_Display_device_init(u8 dir);
void st7796_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);
void st7796_set_paint_color(u16 rgb_color);
void st7796_set_back_color(u16 rgb_color);
void st7796_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
u16 st7796_ReadPoint_color(u16 x,u16 y);
void st7796_ScreenFill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color);
u16 st7796_get_lcddev_width(void);
u16 st7796_get_lcddev_height(void);
u8 st7796_ReadIDn(u8 id_num);
void st7796_DrawPic_bmp(u16 x,u16 y,const unsigned char *p); //显示40*40图片
void st7796_DrawRectangle_color(u16 x,u16 y,u16 width,u16 height,u16 color);

typedef enum
{
	RGB_WHITE = 0xFFFF,
	RGB_BLACK = 0x0000,
	RGB_BLUE = 0x001F,
	RGB_BRED = 0XF81F,
	RGB_GRED = 0XFFE0,
	RGB_GBLUE = 0X07FF,
	RGB_RED = 0xF800,
	RGB_MAGENTA = 0xF81F,
	RGB_GREEN = 0x07E0,
	RGB_CYAN = 0x7FFF,
	RGB_YELLOW = 0xFFE0,
	RGB_BROWN = 0XBC40,
	RGB_BRRED = 0XFC07,
	RGB_GRAY = 0X8430,

	RGB_DARKBLUE = 0X01CF,
	RGB_LIGHTBLUE = 0X7D7C,
	RGB_GRAYBLUE = 0X5458,

	RGB_LIGHTGREEN = 0X841F,
	RGB_LGRAY = 0XC618,

	RGB_LGRAYBLUE = 0XA651,
	RGB_LBBLUE = 0X2B12,

} st7796_rgb_color_t;
#endif
