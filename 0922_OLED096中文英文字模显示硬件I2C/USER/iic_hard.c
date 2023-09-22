#include "iic_hard.h"
#include "stm32f10x.h"


#include "global.h"
#include "systick.h"
#include "stdio.h"

/**/


void i2c1_init(void) //i2c1   pb6-scl pb7-sda    
{


	//时钟开启
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1,  ENABLE);
	//gpio配置 pb6-scl         pb7-sda 
	GPIO_InitTypeDef   GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;// 全部配置开漏输出，防止短路
	GPIO_Init(  GPIOB,&GPIO_InitStruct);//pb6-pb7
	//i2c1配置
	I2C_InitTypeDef   I2C_InitStruct;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = 100000;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1 = 0x01;
	I2C_Init(  I2C1,& I2C_InitStruct);
	//使能
	I2C_Cmd(  I2C1,  ENABLE);



}





u8 I2CErrorHandle(uint8_t* file, uint32_t line ,char* message ) //#define Error ErrorPrint((uint8_t*)__FILE__,__LINE__);
{
	printf("I2C Error:%s,%d,%s\r\n",file,line,message);
	I2C_GenerateSTOP(  I2C1,  ENABLE);
	return 1;//失败
}
u8 I2cHard_buf_read_NoAddr(uint8_t id,uint8_t* pbuf,uint8_t num_byte_to_read) //没地址读出buf
{	
	//u32 FlagTimeoutReset = Num_DownToZero_1s;
	//u32 FlagTimeout ;
	//是否忙线
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:空闲  1：忙碌
	{	 
		return I2CError();
	}
	//产生开始信号
	//FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		//if((FlagTimeout--) == 0) return I2CError();
		sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
		if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
	}
	//发送id地址,读方向
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Receiver);// 1，读方向
	//Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
	while ( !I2C_CheckEvent(  I2C1,   I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // 地址应答 ev6
	{
		//if((FlagTimeout--) == 0) return I2CError();
		sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
		if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
	}

	for(u8 i = 0 ;i < num_byte_to_read; i++) ////读字节
	{
		    //FlagTimeout = FlagTimeoutReset;
		//最后一个接收字节，回复NACK
		if(i == num_byte_to_read - 1)
		{	
			I2C_AcknowledgeConfig(	I2C1,  DISABLE);
		}else
		{
			I2C_AcknowledgeConfig(	I2C1,  ENABLE);
		}	
		//等待接收完成
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_RECEIVED)	 ) 
			{
				//if((FlagTimeout--) == 0) return I2CError();
				sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
				if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
			}
			////接收
		pbuf[i] = I2C_ReceiveData(I2C1);
	}
			
	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //成功
			
}



u8  I2cHard_buf_write_NoAddr(uint8_t id,uint8_t* pbuf,uint8_t num_byte_to_write) //没地址写入buf
{
	 u32 FlagTimeoutReset = Num_DownToZero_1s;
	 u32 FlagTimeout ;
	 
	//是否忙线
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:空闲  1：忙碌
	{	 
		return I2CError();
	}
	//产生开始信号
	//FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		//if((FlagTimeout--) == 0) return I2CError();
		sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
		if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
	}
	//发送id地址 
	 FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{	
		
		 if((FlagTimeout--) == 0) return I2CError();
	}
	
	for(int i = 0;i < num_byte_to_write;i++ )
	{
		//写字节
		    //FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,  pbuf[i]);	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				//if((FlagTimeout--) == 0) return I2CError();
				sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
				if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
			}
	}

	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //成功
						
}

 
