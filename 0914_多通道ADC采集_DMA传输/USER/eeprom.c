
#include "eeprom.h"
#include "global.h"
#include "systick.h"
#include "i2c_hard.h"
#include "main.h"
 


u8 eeprom_byteRead_byteAddr(uint8_t id,uint8_t addrToRead,uint8_t *store_buf,uint8_t count_read)  //ok
{	
	u32 FlagTimeoutReset = NumCountZero_1s; 
	u32 FlagTimeout ;
	 
	//是否忙线
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:空闲  1：忙碌
	{	 
		return I2CError();
	}
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//发送id地址 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 
	}
	
	//发送 要读或写的寄存器地址 
	I2C_SendData( I2C1,  addrToRead);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}

	//第二次通讯
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while (!I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT))  // 通讯开始 ev5
	{
		if((FlagTimeout--) == 0) return I2CError();  
	}

	//发送id地址,读方向
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Receiver);// 1，读方向
	//Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
	while ( !I2C_CheckEvent(  I2C1,   I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // 地址应答 ev6
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	
		//读字节，//接收完成 ，回复 ACK / noAck
	for(u8 i = 0;i<count_read ; i++)
	{
		    FlagTimeout = FlagTimeoutReset;
		if(i == count_read - 1  ){ //最后一个就 No ack
			I2C_AcknowledgeConfig(	I2C1,  DISABLE);
		}else{
			I2C_AcknowledgeConfig(	I2C1,  ENABLE);
		}	
		while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_RECEIVED)	 ) 
		{
			if((FlagTimeout--) == 0) return I2CError();
		}
		store_buf[i] = I2C_ReceiveData(I2C1);	
	}		
			
	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ;//成功
}


u8 eeprom_byteWrite_byteAddr(uint8_t id,uint8_t addrToWrite,uint8_t byteData)  //ok
{	
	u32 FlagTimeoutReset = NumCountZero_1s;
	u32 FlagTimeout ;
	 
	//是否忙线
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:空闲  1：忙碌
	{	 
		return I2CError();
	}
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//发送id地址 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 
	}
	
	//发送 要写的寄存器地址 
	I2C_SendData( I2C1,  addrToWrite);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}

	//写字节
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,  byteData);	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				if((FlagTimeout--) == 0) return I2CError();
			}
	
	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //成功
}


u8 eeprom_bufWrite_byteAddr(uint8_t id,uint8_t addrToWrite,uint8_t *bufWriteData,uint8_t count)  //  要修改 翻页的地址
{	
	u32 FlagTimeoutReset = NumCountZero_1s;
	u32 FlagTimeout ;
	 
	while(count>0)
	{
	//是否忙线
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:空闲  1：忙碌
	{	 
		return I2CError();
	}
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//发送id地址 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 
	}
	
	//发送 要写的寄存器地址 
	I2C_SendData( I2C1,  addrToWrite);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	while(count>0) //发送缓冲器内部的8个字节的范围
	{
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,  *bufWriteData  );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				if((FlagTimeout--) == 0) return I2CError();
			}
			//一个字节完成
			count--;//计数减一
			bufWriteData++;//源地址加一
			addrToWrite++; //目的地址递增,加一结果大于零
			if((addrToWrite % 8) == 0)  break; //目前目的地址，不在缓冲区范围，  0 - 7;  8-15,16-23,24-31,248-255 
			
	
	}//while 发送缓冲区的8个字节
	
	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);
	//延时6ms 写周期
	 sysTickDelay_ms(6);
	

	}//while 等待全部发送完成
	return 	0 ;//成功

}






u8 eeprom_byteRead_wordAddr(uint8_t id,uint16_t WordaddrToRead,uint8_t *store_buf,uint8_t count) //
{

	u32 FlagTimeoutReset = NumCountZero_1s; 
	u32 FlagTimeout ;
	u8 addrByteLow =  WordaddrToRead & 0XFF ;
	u8 addrByteHigh = (WordaddrToRead >> 8) & 0X7F;
	
	//判断 addrWord 范围
	if( !(WordaddrToRead < (1<<15)) ) { return I2CError(); }
	//是否忙线
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:空闲  1：忙碌
	{	 
		return I2CError();
	}
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	
	//发送id地址 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 
	}
	
	//发送 要读或写的寄存器地址 高地址
	I2C_SendData( I2C1,  addrByteHigh);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	//发送 要读或写的寄存器地址 低地址
	I2C_SendData( I2C1,  addrByteLow);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	
	

	//第二次通讯
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while (!I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT))  // 通讯开始 ev5
	{
		if((FlagTimeout--) == 0) return I2CError();  
	}

	//发送id地址,读方向
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Receiver);// 1，读方向
	//Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
	while ( !I2C_CheckEvent(  I2C1,   I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // 地址应答 ev6
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	
		//读字节，//接收完成 ，回复 ACK / noAck
	for(u8 i = 0;i<count ; i++)
	{
		    FlagTimeout = FlagTimeoutReset;
		if(i == count - 1  ){ //最后一个就 No ack
			I2C_AcknowledgeConfig(	I2C1,  DISABLE);
		}else{
			I2C_AcknowledgeConfig(	I2C1,  ENABLE);
		}	
		while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_RECEIVED)	 ) 
		{
			if((FlagTimeout--) == 0) return I2CError();
		}
		store_buf[i] = I2C_ReceiveData(I2C1);	
	}		
			
	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ;//成功





}
u8 eeprom_byteWrite_wordAddr(uint8_t id,uint16_t WordAddrToWrite,uint8_t byteData)  //
{
	
	u32 FlagTimeoutReset = NumCountZero_1s;
	u32 FlagTimeout ;
	u8 addrByteLow =  WordAddrToWrite & 0XFF ;
	u8 addrByteHigh = (WordAddrToWrite >> 8) & 0X7F;
	 
	//判断 addrWord 范围
	if( !(WordAddrToWrite < (1<<15)) ) { return I2CError(); }
	//是否忙线
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:空闲  1：忙碌
	{	 
		return I2CError();
	}
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//发送id地址 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 
	}
	
	//发送 要写的寄存器地址  高地址
	I2C_SendData( I2C1,  addrByteHigh);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	//发送 要写的寄存器地址 低地址
	I2C_SendData( I2C1,  addrByteLow);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}

	//写字节
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,  byteData);	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				if((FlagTimeout--) == 0) return I2CError();
			}
	
	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //成功


}
u8 eeprom_bufWrite_wordAddr(uint8_t id,uint16_t WordAddrToWrite,uint8_t *bufWriteData,uint8_t count)  
{
	u32 FlagTimeoutReset = NumCountZero_1s;
	u32 FlagTimeout ;
	 
	//判断 addrWord 范围
	if( !(WordAddrToWrite < (1<<15)) ) { return I2CError(); }  
	
	while(count>0)
	{
	//是否忙线
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:空闲  1：忙碌
	{	 
		return I2CError();
	}
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//发送id地址 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 	}
	
	//发送 要写的寄存器地址  高地址   (WordAddrToWrite >> 8) & 0X7F;
	I2C_SendData( I2C1,  (WordAddrToWrite >> 8) & 0X7F);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
		//发送 要写的寄存器地址  低地址 //	 WordAddrToWrite & 0XFF ;

	I2C_SendData( I2C1,  WordAddrToWrite & 0XFF);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	
	while(count>0) //发送 page 的字节书目    64个字节   
	{
		    FlagTimeout = FlagTimeoutReset;
			//判断 addrWord 范围
			//if( !(WordAddrToWrite < (1<<15)) ) { return I2CError(); } 
			
			I2C_SendData(  I2C1,  *bufWriteData  );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				if((FlagTimeout--) == 0) return I2CError();
			}
			//一个字节完成
			count--;//计数减一
														gDebug_u8 = count; //调试使用
			bufWriteData++;//数据源地址加一
			WordAddrToWrite++; //目的地址递增,加一结果大于零 
																gDebug_u16= WordAddrToWrite;//调试
			
			if(((WordAddrToWrite & 0x003f) == 0)   )  //  11 1111(6位) 0-63 
			{
				break;//下一个地址 ，到达边界  ，跳出
			}

	
	}//while 发送缓冲区的64个字节
	
	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);
	//延时6ms 写入页周期
	sysTickDelay_ms(10);
	

	}//while 等待全部发送完成
	return 	0 ;//成功


}
 u8 eeprom_bufRead_wordAddr(uint8_t id,uint16_t WordAddrToRead,uint8_t *bufRead_Store,uint8_t count) //ing
{
	u32 FlagTimeoutReset = NumCountZero_1s; 
	u32 FlagTimeout ;
	 
	//判断 addrWord 范围
	if( !(WordAddrToRead < (1<<15)) ) { return I2CError(); }  
	
	//是否忙线
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:空闲  1：忙碌
	{	 
		return I2CError();
	}
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	
		//发送id地址 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 	
	}
	
	//发送 要读的寄存器地址  高地址   (WordAddrToWrite >> 8) & 0X7F;
	I2C_SendData( I2C1,  (WordAddrToRead >> 8) & 0X7F);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	
	//发送 要读的寄存器地址  低地址 //	 WordAddrToWrite & 0XFF ;
	I2C_SendData( I2C1,  WordAddrToRead & 0XFF);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // 主发送模式 ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	
	//第二次产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while (!I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT))  // 通讯开始 ev5
	{
		if((FlagTimeout--) == 0) return I2CError();  
	}

	//发送id地址,读方向
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Receiver);// 1，读方向
	//Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
	while ( !I2C_CheckEvent(  I2C1,   I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // 地址应答 ev6 //等待ack(0)
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
////////////////////////////////////////////////////////////////////////////////////////////	
	//序列读出 多个字节，配置：回复ACK 或者 no Ack
	for(u8 i = 0;i<count ; i++)
	{
		FlagTimeout = FlagTimeoutReset;
		if(i == count - 1  ){ //最后一个就 No ack//	(i == count - 1 )?I2C_AcknowledgeConfig(I2C1,DISABLE):I2C_AcknowledgeConfig(I2C1, ENABLE) ;
			I2C_AcknowledgeConfig(	I2C1,  DISABLE);
		}else{
			I2C_AcknowledgeConfig(	I2C1,  ENABLE);
		}
		
		//等待 接收完成 信号
		while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_RECEIVED)	 ) 
		{
			if((FlagTimeout--) == 0) return I2CError();
		}
		//存储收到的数据
		bufRead_Store[i] = I2C_ReceiveData(I2C1);	
	}		
			
	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);
	
	return 	0 ;//成功
}	

