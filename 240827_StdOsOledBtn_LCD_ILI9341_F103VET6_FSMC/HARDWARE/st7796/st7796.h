#ifndef __ST7796_H
#define __ST7796_H
#include "stdint.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"

//lcd 控制管脚
    //BL =>LCD_BL_PD12
    //RST =>LCD_RST_PC2
#define LCD_BL_PORT GPIOD
#define LCD_BL_PIN  GPIO_Pin_12
#define LCD_RST_PORT GPIOC
#define LCD_RST_PIN  GPIO_Pin_2

//LCD地址结构体
typedef struct
{
    vu16 LCD_REG;
    vu16 LCD_RAM;
} LCD_TypeDef;

//使用NOR/SRAM的 Bank1.sector1,地址位HADDR[27,26]=00 A16作为数据命令区分线
//不论外部接 8 位/16 位宽设备， FSMC_A[0]永远接在外部设备地址 A[0] ,由于内部硬件u16的寻址原因 ,舍弃bit0,把bit1当做bit0,相当于addr右偏移一位(既是乘以2)
//u16访问地址访问是2n,不关心bit0
//RS_BIT_A16 =0 是命令,RS_BIT_A16 =1 是数据
//本例RS_BIT = A16, RS=0的偏移量是 
#define LCD_BASE 		((u32)(0x60000000 | ((((0x01) << 16)<<1) - 2)))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
//////////////////////////////////////////////////////////////////////////////////

void st7796_bsp_init(void);
void st7796_init(void);
void st7796_write_cmd(uint16_t cmd);

void st7796_write_data(uint16_t data);

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
void st7796_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size); //?????????????????????
void st7796_DrawPic_bmp(u16 x,u16 y,const unsigned char *p); //显示40*40图片
void st7796_DrawRectangle_color(u16 x,u16 y,u16 width,u16 height,u16 color);
void st7796_backlight_duty(u8 duty);

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
