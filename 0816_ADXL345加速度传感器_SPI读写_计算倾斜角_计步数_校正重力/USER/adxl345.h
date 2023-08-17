#ifndef __ADXL345_H__
#define __ADXL345_H__

#include "stm32f10x.h"

void adxl345_init(void);  //时序方案按照时钟 极性(CPOL)= 1、 时钟相位(CPHA)= 1执行。
u8 adxl345_getRespont(void); //id:0XE5

void adxl345_WriteReg(u8 reg_addr ,u8 WriteByte) ; //    
u8 adxl345_ReadReg(u8 reg_addr ) ; //  
u8 adxl345_Buf_Read(u8 reg_addr ,u8* buf_byte_read,u8 count);  //  

	 
	s32 adxl345_axis_dataTransform(u8 data_low,u8 data_high) ; //d15-d12 高四位符号位，d11-d0 数据位   ，二进制补码，对负数处理：负号（取反加一）
	void stepping_occur_Z_axis(s32 Z_axis_gForce_mg,u32* step_num); //Z轴mg 发生上升 并下降 表示完成一步

 

void adxl345_3axis_Offset_check( void) ;//字符测量xyz表示三轴, 计算并写入 偏差寄存器的值	
#endif





