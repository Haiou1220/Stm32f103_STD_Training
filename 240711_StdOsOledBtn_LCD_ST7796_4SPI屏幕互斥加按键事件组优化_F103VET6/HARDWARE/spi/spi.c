#include "spi.h"
#include "stm32f10x.h"
#include "print.h"

/**
 * @brief spi 收发交换函数
 * 
 * @param byte 
 * @return uint8_t 
 */
uint8_t spi1_SwapByte( uint8_t byte)
{
	uint32_t TimeOut;
	
	//等待空闲结束 ,注意：不要使用BSY标志处理每一个数据项的发送和接收，最好使用TXE和RXNE标志。
	  //  等待发送寄存器为空 TXE  
	TimeOut = 0x10000;
	while( !SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) )
	{	 
	 	if(!(TimeOut--)) 
		{ 
		ASSERT(ERROR);	
		break;
		}
	}
	SPI_I2S_SendData( SPI1,  byte);//8bit，。数据从发送缓冲器传输到移位寄存器时TXE标志将被置位，在试图写发送缓冲器之前，需确认TXE标志应该为’1’。
	//等待发送完成
	TimeOut = 0x10000;
	while( !SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) )   //收到数据
	{
		 if(!(TimeOut--))   
		{ 
	    ASSERT(ERROR);	
		break; 
		}
	}
	
	//接收到了数据，并返回
	return  SPI_I2S_ReceiveData(SPI1);//读SPI数据寄存器可以清除此标志 ;读SPI_DR寄存器将清除RXNE位。
}
