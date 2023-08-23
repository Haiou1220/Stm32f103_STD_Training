#ifndef __WS2812_H__
#define __WS2812_H__

#include "stm32f10x.h"
#include "stm32f10x.h"

#define DUMMY_COLOR 0X000000  //占位一个led，用于转码最后一个为resst code
typedef struct
{
	u8 R;
	u8 G;
	u8 B;
}RGB_t;

typedef u32 RGB24BIT_t ;

 

typedef struct{  	//CCRx是16bit ，配置dma 数据宽度是 源8bit目的16bit		 
u8 G[8];//MSB 	
u8 R[8]; 
u8 B[8]	;//LSB     
}GRB_CCRx_t;// 



void print_CCRxBuf_column24(GRB_CCRx_t* GRB_CCRx_buf ,u32 led_count); //打印  转码值
	
void sprint_CCRxBuf_column24( char* sprintf_str_buf,u32 sprintf_str_buf_len , GRB_CCRx_t* GRB_CCRx_buf ,u32 GRB_CCRx_buf_len,u32* sprintf_output_len) ;//打印  转码值 到字符串




void RGB24BIT_To_TIMxCCRx_Codebuf_AddResCode( RGB24BIT_t* rgb24bit  ,GRB_CCRx_t* GRB_CCRx_buf ,u16 LED_count, u8 light_percent );




void RGB24bitColorBuf_effect_move(RGB24BIT_t* RGB24bitcolor, u16 led_count,u8 flag_dir_ForHead);

void RGB24bitColorBuf_modifier_location_breathe(RGB24BIT_t* RGB24bitcolor,RGB24BIT_t* Return_RGB24bitcolor, u16 led_count  ,u32  location_1_0To299,float  loc1_breathe_gradually_0To1,u32  loc1_breathe_range);//固定或者动态位置，呼吸效果
	void RGB24bitColorBuf_effect__sequence_add_del_color(RGB24BIT_t* RGB24bitcolor, u16 led_count);// 添加 顺序颜色 颜色
void RGB24bitColorBuf_effect__sequence_add_del_color1(RGB24BIT_t* RGB24bitcolor, u16 led_count)  ;  //  顺序添加颜色  顺序删除颜色
#endif





