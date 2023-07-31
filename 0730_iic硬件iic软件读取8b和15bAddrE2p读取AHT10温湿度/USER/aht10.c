#include "aht10.h"
#include "i2c_hard.h"
#include "stm32f10x.h"
#include "global.h"
#include "stdio.h"



u8  AHT10CMD_TriggerMeasureData[] = {0xAC,0X33,0X00};   //AHT10CMD_TriggerMeasureData      //������������ �������� 0xac+0x33+0x00
u8  AHT10CMD_SoftRestet[]  = { 0XBA };   //��λ  ��������  0xba
u8  AHT10CMD_Initialize[] = {0XE1,0X08,0X00};//Ҫ����0XE1����(��ʼ��),����������������ֽ�,��һ���ֽ�Ϊ0x08,�ڶ����ֽ�Ϊ0x00��

/*
��������ȡ���̣�                1���ϵ��Ҫ�ȴ�40ms,��ȡ��ʪ��ֵ֮ǰ,����Ҫ��״̬�ֵ�У׼ʹ��λBit[3]�Ƿ�Ϊ1,
                              �����Ϊ1,Ҫ����0XE1����(��ʼ��),����������������ֽ�,��һ���ֽ�Ϊ0x08,�ڶ����ֽ�Ϊ0x00��
2,                            ֱ�ӷ���0xAC����(��������) ,����������������ֽ�,��һ���ֽ�Ϊ0x33,�ڶ����ֽ�Ϊ0x00��
3,                         �ȴ�80ms���������,æ״̬Bit[7]Ϊ0,Ȼ����Զ�ȡ�����ֽ�(����idΪ0X70����
ע:�ڵ�һ����У׼״̬����ֻ��Ҫ�ϵ�ʱ���,�������ɼ��������������
*/
u8 AHT10_ReadyMeasureData(void)      //������������ �������� 0xac+0x33+0x00
{		

	u8 num_byte_to_write = sizeof(AHT10CMD_TriggerMeasureData)/sizeof(AHT10CMD_TriggerMeasureData[0]);
	
	return   I2cHard_buf_write_NoAddr( Device_AHT10,AHT10CMD_TriggerMeasureData, num_byte_to_write);
	 	 
			
}

u8 AHT10_SoftRestet(void)   //��λ  ��������  0xba
{
	
	u8 num_byte_to_write = sizeof(AHT10CMD_SoftRestet)/sizeof(AHT10CMD_SoftRestet[0]);
	return I2cHard_buf_write_NoAddr(  Device_AHT10,AHT10CMD_SoftRestet, num_byte_to_write);
		
}

float AHT10_GetTempera(void)  //��ȡ��ʪ������: ״̬ ʪ��1 ʪ��2 ��ʪ��3����4λ���¶�1����4λ���� �¶�2 �¶�3(���)          4+8+8 = 20bit
{
//	u32 temp = 0;
	u8 AHT10ReturnData[6] = {0,0,0,0,0,0}; //[0 1 2 3 4 5]
	u8 num_byte_to_read = sizeof(AHT10ReturnData)/sizeof(AHT10ReturnData[0]);
	
   if(  I2cHard_buf_read_NoAddr( Device_AHT10,AHT10ReturnData, num_byte_to_read) ) //û��ַ����buf
   {
	   ErrorInfo("Err in:AHT10_GetTempera() ");
	   return 0;
   }
 // 00001111 00000000 00000000
  return   ( ( ( (u32)(AHT10ReturnData[3]&0X0F)<<16) + ( (u32)AHT10ReturnData[4]<<8)+  (u32)AHT10ReturnData[5] )*200.0/((u32)1<<20) )- 50;
	 
}

float ATH10_GetHumidity(void) ////��ȡ��ʪ������: ״̬ ʪ��1 ʪ��2 ��ʪ��3��4λ���¶�1��4λ���� �¶�2 �¶�3(���)          4+8+8 = 20bit
{

	u8 AHT10ReturnData[6] = {0,0,0,0,0,0}; //[0 1 2 3 4 5]
	u8 num_byte_to_read = sizeof(AHT10ReturnData)/sizeof(AHT10ReturnData[0]);
	
   if(  I2cHard_buf_read_NoAddr( Device_AHT10,AHT10ReturnData, num_byte_to_read) ) //NO��ַ����buf
   {
	   ErrorInfo(Err in:ATH10_GetHumidity() );
	   return 0;
   }
   //  1111xxxx >>4 00001111
   // 00000000 1111 1111  <<4    0000 1111 11110000
   // 00000000 1111 1111  <<4+8     X
   return   ( ( ( (u32)(AHT10ReturnData[1])<<(4+8)) + ( (u32)AHT10ReturnData[2]<<4)+  ((u32)(AHT10ReturnData[3]&0XF0)>>4)  )* 1.0/((u32)1<<20)   );

}



u8 AHT10_GetStatusWord(void) 
{
	u8 status = 0;
   if(  I2cHard_buf_read_NoAddr( Device_AHT10,&status, 1) ) //û��ַ����buf
   {
	   ErrorInfo("Err in:AHT10_GetStatusWord() ");
	   return 1;
   }
   return  status;
}
 
inline u8 AHT10_GetStatus_CALEnableFlag(void)   //У׼ʹ��λBit[3]�Ƿ�Ϊ1,
{
	return (AHT10_GetStatusWord() & 0x08 )>>3; //0000 1000    
}

inline u8 AHT10_GetStatus_BusyFlag(void)   //æ״̬Bit[7]Ϊ0,
{
	return (AHT10_GetStatusWord() & 0x80 )>>7; //1000 0000
}



u8 AHT10_Calibration(void) ////У׼ ���bit3��Ϊ1,Ҫ����0XE1����(��ʼ��),����������������ֽ�,��һ���ֽ�Ϊ0x08,�ڶ����ֽ�Ϊ0x00��
{
	u8 num_byte_to_write = sizeof(AHT10CMD_Initialize)/sizeof(AHT10CMD_Initialize[0]);
	return   I2cHard_buf_write_NoAddr( Device_AHT10,AHT10CMD_Initialize, num_byte_to_write);
}



/*
��������ȡ���̣�                1���ϵ��Ҫ�ȴ�40ms,��ȡ��ʪ��ֵ֮ǰ,����Ҫ��״̬�ֵ�У׼ʹ��λBit[3]�Ƿ�Ϊ1,
                              �����Ϊ1,Ҫ����0XE1����(��ʼ��),����������������ֽ�,��һ���ֽ�Ϊ0x08,�ڶ����ֽ�Ϊ0x00��
2,                            ֱ�ӷ���0xAC����(��������) ,����������������ֽ�,��һ���ֽ�Ϊ0x33,�ڶ����ֽ�Ϊ0x00��
3,                         �ȴ�80ms���������,æ״̬Bit[7]Ϊ0,Ȼ����Զ�ȡ�����ֽ�(��0X71�����Զ�ȡ����
ע:�ڵ�һ����У׼״̬����ֻ��Ҫ�ϵ�ʱ���,�������ɼ��������������
*/
void AHT10_init(void)   //��ʼ��
{	
	Info(AHT10 initialization );
	sysTickDelay_ms(100);//     
	if( AHT10_SoftRestet() )  //��λ  ��������  0xba
	{
		ErrorInfo(fault:AHT10_SoftRestet());
	}
	sysTickDelay_ms(100);//
	if(AHT10_GetStatus_CALEnableFlag() == 1 ) Info(AHT10_GetStatus_CALEnableFlag:1);
	else
	{
	  Info(AHT10_GetStatus_CALEnableFlag:0);
	  sysTickDelay_ms(100); 
	  Info( EXE AHT10_Calibration);
	  AHT10_Calibration( );
	}
}

float AHT10_Task_GetTemperature(void) //����-AHT10 GetTemperature
{
	AHT10_ReadyMeasureData() ; 	
	sysTickDelay_ms(75);// ������������ 75ms���� ���ж�busy��־λ
	if( AHT10_GetStatus_BusyFlag() == 1) 
	{
		ErrorInfo( AHT10_GetStatus_BusyFlag = 1 );  
		return 0 ;
	}
	else{
					sysTickDelay_ms(100);
			return  AHT10_GetTempera();
	}		     	
}

float ATH10_Task_GetHumidity(void)  //����-AHT10 GetHumidity
{	
	AHT10_ReadyMeasureData() ;
	sysTickDelay_ms(75);// ������������ 75ms���� ���ж�busy��־λ
	if( AHT10_GetStatus_BusyFlag() == 1) 
	{
		ErrorInfo( AHT10_GetStatus_BusyFlag = 1 );  
		return 0 ;
	}
	else{
					sysTickDelay_ms(100);
			return  ATH10_GetHumidity();
	}		     	
}


