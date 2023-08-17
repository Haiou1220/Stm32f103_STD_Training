#ifndef __ADXL345_H__
#define __ADXL345_H__

#include "stm32f10x.h"

void adxl345_init(void);  //ʱ�򷽰�����ʱ�� ����(CPOL)= 1�� ʱ����λ(CPHA)= 1ִ�С�
u8 adxl345_getRespont(void); //id:0XE5

void adxl345_WriteReg(u8 reg_addr ,u8 WriteByte) ; //    
u8 adxl345_ReadReg(u8 reg_addr ) ; //  
u8 adxl345_Buf_Read(u8 reg_addr ,u8* buf_byte_read,u8 count);  //  

	 
	s32 adxl345_axis_dataTransform(u8 data_low,u8 data_high) ; //d15-d12 ����λ����λ��d11-d0 ����λ   �������Ʋ��룬�Ը����������ţ�ȡ����һ��
	void stepping_occur_Z_axis(s32 Z_axis_gForce_mg,u32* step_num); //Z��mg �������� ���½� ��ʾ���һ��

 

void adxl345_3axis_Offset_check( void) ;//�ַ�����xyz��ʾ����, ���㲢д�� ƫ��Ĵ�����ֵ	
#endif





