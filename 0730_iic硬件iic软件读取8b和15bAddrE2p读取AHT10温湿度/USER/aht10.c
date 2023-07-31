#include "aht10.h"
#include "i2c_hard.h"
#include "stm32f10x.h"
#include "global.h"
#include "stdio.h"



u8  AHT10CMD_TriggerMeasureData[] = {0xAC,0X33,0X00};   //AHT10CMD_TriggerMeasureData      //触发测量数据 发送数据 0xac+0x33+0x00
u8  AHT10CMD_SoftRestet[]  = { 0XBA };   //软复位  发送数据  0xba
u8  AHT10CMD_Initialize[] = {0XE1,0X08,0X00};//要发送0XE1命令(初始化),此命令参数有两个字节,第一个字节为0x08,第二个字节为0x00。

/*
传感器读取流程：                1·上电后要等待40ms,读取温湿度值之前,首先要看状态字的校准使能位Bit[3]是否为1,
                              如果不为1,要发送0XE1命令(初始化),此命令参数有两个字节,第一个字节为0x08,第二个字节为0x00。
2,                            直接发送0xAC命令(触发测量) ,此命令参数有两个字节,第一个字节为0x33,第二个字节为0x00。
3,                         等待80ms待测量完成,忙状态Bit[7]为0,然后可以读取六个字节(器件id为0X70）。
注:在第一步的校准状态检验只需要上电时检查,在正常采集过程无需操作。
*/
u8 AHT10_ReadyMeasureData(void)      //触发测量数据 发送数据 0xac+0x33+0x00
{		

	u8 num_byte_to_write = sizeof(AHT10CMD_TriggerMeasureData)/sizeof(AHT10CMD_TriggerMeasureData[0]);
	
	return   I2cHard_buf_write_NoAddr( Device_AHT10,AHT10CMD_TriggerMeasureData, num_byte_to_write);
	 	 
			
}

u8 AHT10_SoftRestet(void)   //软复位  发送数据  0xba
{
	
	u8 num_byte_to_write = sizeof(AHT10CMD_SoftRestet)/sizeof(AHT10CMD_SoftRestet[0]);
	return I2cHard_buf_write_NoAddr(  Device_AHT10,AHT10CMD_SoftRestet, num_byte_to_write);
		
}

float AHT10_GetTempera(void)  //读取温湿度数据: 状态 湿度1 湿度2 【湿度3（高4位）温度1（低4位）】 温度2 温度3(最低)          4+8+8 = 20bit
{
//	u32 temp = 0;
	u8 AHT10ReturnData[6] = {0,0,0,0,0,0}; //[0 1 2 3 4 5]
	u8 num_byte_to_read = sizeof(AHT10ReturnData)/sizeof(AHT10ReturnData[0]);
	
   if(  I2cHard_buf_read_NoAddr( Device_AHT10,AHT10ReturnData, num_byte_to_read) ) //没地址读出buf
   {
	   ErrorInfo("Err in:AHT10_GetTempera() ");
	   return 0;
   }
 // 00001111 00000000 00000000
  return   ( ( ( (u32)(AHT10ReturnData[3]&0X0F)<<16) + ( (u32)AHT10ReturnData[4]<<8)+  (u32)AHT10ReturnData[5] )*200.0/((u32)1<<20) )- 50;
	 
}

float ATH10_GetHumidity(void) ////读取温湿度数据: 状态 湿度1 湿度2 【湿度3（4位）温度1（4位）】 温度2 温度3(最低)          4+8+8 = 20bit
{

	u8 AHT10ReturnData[6] = {0,0,0,0,0,0}; //[0 1 2 3 4 5]
	u8 num_byte_to_read = sizeof(AHT10ReturnData)/sizeof(AHT10ReturnData[0]);
	
   if(  I2cHard_buf_read_NoAddr( Device_AHT10,AHT10ReturnData, num_byte_to_read) ) //NO地址读出buf
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
   if(  I2cHard_buf_read_NoAddr( Device_AHT10,&status, 1) ) //没地址读出buf
   {
	   ErrorInfo("Err in:AHT10_GetStatusWord() ");
	   return 1;
   }
   return  status;
}
 
inline u8 AHT10_GetStatus_CALEnableFlag(void)   //校准使能位Bit[3]是否为1,
{
	return (AHT10_GetStatusWord() & 0x08 )>>3; //0000 1000    
}

inline u8 AHT10_GetStatus_BusyFlag(void)   //忙状态Bit[7]为0,
{
	return (AHT10_GetStatusWord() & 0x80 )>>7; //1000 0000
}



u8 AHT10_Calibration(void) ////校准 如果bit3不为1,要发送0XE1命令(初始化),此命令参数有两个字节,第一个字节为0x08,第二个字节为0x00。
{
	u8 num_byte_to_write = sizeof(AHT10CMD_Initialize)/sizeof(AHT10CMD_Initialize[0]);
	return   I2cHard_buf_write_NoAddr( Device_AHT10,AHT10CMD_Initialize, num_byte_to_write);
}



/*
传感器读取流程：                1·上电后要等待40ms,读取温湿度值之前,首先要看状态字的校准使能位Bit[3]是否为1,
                              如果不为1,要发送0XE1命令(初始化),此命令参数有两个字节,第一个字节为0x08,第二个字节为0x00。
2,                            直接发送0xAC命令(触发测量) ,此命令参数有两个字节,第一个字节为0x33,第二个字节为0x00。
3,                         等待80ms待测量完成,忙状态Bit[7]为0,然后可以读取六个字节(发0X71即可以读取）。
注:在第一步的校准状态检验只需要上电时检查,在正常采集过程无需操作。
*/
void AHT10_init(void)   //初始化
{	
	Info(AHT10 initialization );
	sysTickDelay_ms(100);//     
	if( AHT10_SoftRestet() )  //软复位  发送数据  0xba
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

float AHT10_Task_GetTemperature(void) //任务-AHT10 GetTemperature
{
	AHT10_ReadyMeasureData() ; 	
	sysTickDelay_ms(75);// 发出测量命令 75ms以上 再判断busy标志位
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

float ATH10_Task_GetHumidity(void)  //任务-AHT10 GetHumidity
{	
	AHT10_ReadyMeasureData() ;
	sysTickDelay_ms(75);// 发出测量命令 75ms以上 再判断busy标志位
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


