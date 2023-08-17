#include "adxl345.h"
#include "spi_hard.h"
#include "stm32f10x.h"
#include "systick.h"
#include "adxl345_instruction.h"
#include "stdio.h"
#include "global.h"


void adxl345_init(void) //ʱ�򷽰�����ʱ�� ����(CPOL)= 1�� ʱ����λ(CPHA)= 1ִ�С�
{
	spi1_hard_init(3);
	
	//�ϵ���ʱ
	sysTickDelay_ms(5);
	
	if( 0 == adxl345_getRespont() )
	{
		ErrorInfo( "adxl345_init erro,no respond");
		return ;
	}
	//��ʼ������
	adxl345_WriteReg(  ADXL345_DATA_FORMAT , DATA_FORMAT__FULL_RES|DATA_FORMAT__Rang_16g  ) ;  //16g,ȫ�ֱ���ģʽ
	
			adxl345_WriteReg(  ADXL345_OFSX , 0X00 ) ;  
	adxl345_WriteReg(  ADXL345_OFSY , 0X00 ) ;   
	adxl345_WriteReg(  ADXL345_OFSZ ,  0X00   ) ; 
	
	adxl345_WriteReg(ADXL345_FIFO_CTL, ADXL345_FIFO_CTL__passby_mode     );//	//	��·ģʽ
	
	adxl345_WriteReg(  ADXL345_POWER_CTL  , ADXL345_POWER_CTL__measurement   ) ;//��ʼ����
	
	adxl345_WriteReg(  ADXL345_INT_ENABLE  , ADXL345_INT_ENABLE__data_read   ) ;// ʹ������׼�����ж�
	
	 
 
	
	
	
	
	
	
	
	
	
	
	
	
}
void adxl345_WriteReg(u8 reg_addr ,u8 WriteByte)  //  
{ 
	 
	 reg_addr &= 0x3f;//d7d6 ����
     spi_hard_CS(0);   //�����ź�
	 //sysTickDelay_us_Block(5);//��ʼ����ʱ��
	
	 spi_hard_swapByte( reg_addr );//D7:����д��
	  spi_hard_swapByte(WriteByte);
		
	// sysTickDelay_us_Block(5);//��������ʱ��								
     spi_hard_CS(1) ;   //ֹͣ�ź�

	return  ;
}

//SCLK�½���ʱ���ݸ��£� SCLK������ʱ���в�����ģʽ3

u8 adxl345_ReadReg(u8 reg_addr )  //  
{ 
	 u8 temp_read_byte;
	 reg_addr &= 0x3f;//d7d6 ����
     spi_hard_CS(0);   //�����ź�
	// sysTickDelay_us_Block(5);//��ʼ����ʱ��
	
	 spi_hard_swapByte( reg_addr | 0x80);//D7:����д�� 0X80 reg_addr
	 temp_read_byte = spi_hard_swapByte(0XFF);
		
	// sysTickDelay_us_Block(5);//��������ʱ��								
     spi_hard_CS(1) ;   //ֹͣ�ź�

	return temp_read_byte;
}


u8 adxl345_Buf_Read(u8 reg_addr ,u8* buf_byte_read,u8 count)  //  
{ 
	 
	 reg_addr &= 0x3f;//d7d6 ����
	
	 if (0 == adxl345_getRespont()) {  return 1; }//id:0XE5
	
     spi_hard_CS(0);   //�����ź�
	 sysTickDelay_us_Block(5);//��ʼ����ʱ��
	
	spi_hard_swapByte( reg_addr| 0X80|0X40);//D7:����д�� MB:0X40 ������ݲ���
	 
	for(u8 i=0;i<count ;i++)
	{
		buf_byte_read[i] = spi_hard_swapByte(0XFF);
	
	}
	
	 sysTickDelay_us_Block(5);//��������ʱ��									
     spi_hard_CS(1) ;   //ֹͣ�ź�

	return 0;
}

u8 adxl345_getRespont(void) //id:0XE5
{
 if( adxl345_ReadReg(ADXL345_DEVID) == 0XE5)
 {	
	 
	return 1;
 }else{
	   
	return 0;
 }


}

 s32 adxl345_axis_dataTransform(u8 data_low,u8 data_high) //d15-d12 ����λ����λ��d11-d0 ����λ   �������Ʋ��룬�Ը����������ţ�ȡ����һ��
{
	u8 signed_data;                      //data[0] ���ֽ� data[1] ���ֽ�          
	s32 axis_dataTransform_scale;
	u16 composite_data;
	if(data_high & 0x80)
	{
		signed_data = 1;
	}else{
		signed_data = 0;
	}
	
	composite_data = (((u16)data_high)<<8 ) | data_low;
	
	if(signed_data) //����
	{
		//axis_dataTransform_scale = -( (~composite_data)+1  ); //���ַ���,������
		axis_dataTransform_scale =   ADXL345_Axis_scale * (composite_data - (1<<16));	//	���ַ���
	
	}else{
	
		axis_dataTransform_scale = ADXL345_Axis_scale * composite_data;
	
	}
	
	return axis_dataTransform_scale;

}

void stepping_occur_Z_axis(s32 Z_axis_gForce_mg,u32* step_num) //Z��mg �������� ���½� ��ʾ���һ��
{
	static u8 stepping_up_falg = 0,stepping_down_falg = 0;
	
	if(Z_axis_gForce_mg >= (1030+100) )//��������
	{
		stepping_down_falg = 1;
	
	}else if(Z_axis_gForce_mg <= (1030-100) ) //���ϼ�С
	{
	
		stepping_up_falg = 1;
	}
	
	if(stepping_down_falg && stepping_up_falg)
	{
		(*step_num)++;
		stepping_down_falg = stepping_up_falg = 0;
	}
	
	
	
}

void adxl345_3axis_Offset_check( void) //�ַ�����xyz��ʾ����, ���㲢д�� ƫ��Ĵ�����ֵ
{
	u8 while_count = 5;

//���� ����� mg ����ֵ

//�ó� ƫ�� mg ����ֵ ������ д�� ƫ��Ĵ�����ֵ�����ƫ�� 100mg  Ӧ�� д��-100mg
//ƫ�ƼĴ����ķֱ���Ϊ 15.6 mg/LSB�� ��������Ϊ 15.6mg/LSB(��0x7F(���s8) = 2 g)

 
static u8 buf_byte_read_axis_xyz[6];
s8 To_write_X_axis_Offset1,To_write_X_axis_Offset2,To_write_X_axis_Offset3;
float calc1,calc2,calc3;
s32 X_axis_mg_Force,Y_axis_mg_Force,Z_axis_mg_Force;

	//printf("To_write_X_axis_Offset:%d\r\n",To_write_X_axis_Offset);
	

 if (0 == adxl345_getRespont()) { ErrorInfo("adxl345_getRespont: error ");  return ; }//id:0XE5
	
//����Offset �Ĵ���
	adxl345_WriteReg(  ADXL345_OFSX , 0X00 ) ;  
	adxl345_WriteReg(  ADXL345_OFSY , 0X00 ) ;   
	adxl345_WriteReg(  ADXL345_OFSZ ,  0X00   ) ; 
 
 adxl345_Buf_Read(  ADXL345_DATAX0 ,  buf_byte_read_axis_xyz,  6) ;// ��� DATA ready ,overrun �жϱ�־λ
 
	// �ȴ�����overrun�ж�
while( !(adxl345_ReadReg(ADXL345_INT_SOURCE) & ADXL345_INT_SOURCE__overrun) )
{ 
	 
	 sysTickDelay_ms(5);
	while_count--;
	if( !while_count    )
	{	
		ErrorInfo("adxl345_3axis_Offset_check:DATA ready error ");
		return;
	}

}
 

//����
	adxl345_Buf_Read(  ADXL345_DATAX0 ,  buf_byte_read_axis_xyz,  6) ;
		X_axis_mg_Force = adxl345_axis_dataTransform(buf_byte_read_axis_xyz[0],buf_byte_read_axis_xyz[1])  ;
		Y_axis_mg_Force = adxl345_axis_dataTransform( buf_byte_read_axis_xyz[2],buf_byte_read_axis_xyz[3])  ;
		Z_axis_mg_Force = adxl345_axis_dataTransform(buf_byte_read_axis_xyz[4],buf_byte_read_axis_xyz[5])   ;
		
		
//	//ֹͣ����
	adxl345_WriteReg(  ADXL345_POWER_CTL  , 0X00   ) ;// ֹͣ����
	adxl345_WriteReg(  ADXL345_INT_ENABLE  , 0X00  ) ;// ֹͣʹ������׼�����ж�
	
	
		
//����
    calc1 =( (0 - X_axis_mg_Force)/15.6 )-1;
	calc2 =( (0 - Y_axis_mg_Force)/15.6 )-1;
	calc3 =( (1000 - Z_axis_mg_Force)/15.6 )-1;
	
    To_write_X_axis_Offset1 = calc1;
	To_write_X_axis_Offset2 = calc2;
	To_write_X_axis_Offset3 = calc3;
	
	
	

	adxl345_WriteReg(  ADXL345_OFSX , *(u8*)(&To_write_X_axis_Offset1) ) ;  
	adxl345_WriteReg(  ADXL345_OFSY ,  *(u8*)(&To_write_X_axis_Offset2)   ) ;  
	adxl345_WriteReg(  ADXL345_OFSZ , *(u8*)(&To_write_X_axis_Offset3)    ) ;  
	

	

		adxl345_WriteReg(  ADXL345_POWER_CTL  , ADXL345_POWER_CTL__measurement   ) ;//��ʼ����
	
	adxl345_WriteReg(  ADXL345_INT_ENABLE  , ADXL345_INT_ENABLE__data_read   ) ;// ʹ������׼�����ж�




}





