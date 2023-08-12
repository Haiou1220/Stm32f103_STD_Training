#include "spi_hard.h"
#include "stm32f10x.h"
#include "global.h"


#define CS_PIN 						GPIO_Pin_4
#define CS_PORT						GPIOC // pa-4 SPI1_NCC不能使用，原因不知道
#define CS_RCC_APB2Periph_GPIOX    RCC_APB2Periph_GPIOC // RCC_APB2Periph_GPIOA //RCC_APB2Periph_GPIOC

/*
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, uint16_t SPI_NSSInternalSoft); //不使用
*/ 

//
/*
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);  //常用
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);

ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
*/
//



void spi_hard_init(void) // spi1硬件初始化,模式0  NSS 软件模式 ,//PA4 -NSS //PA5-SCK  //PA6-MI  //PA7-MO
{
	//开启外设时钟 SPI  GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(CS_RCC_APB2Periph_GPIOX,ENABLE);
	//配置gpio ,复用
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = CS_PIN; ////SPI1_NCC不能使用，原因不知道, 软件模式下，， 未用，可作为通用I/O
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; //
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init( CS_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;  //PA5-SCK   //PA7-MO
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz; //50Mhz读取失败
	GPIO_Init( GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;	            //PA6-MI 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init( GPIOA, &GPIO_InitStruct);
	
	//配置spi1
	SPI_InitTypeDef  SPI_InitStruct;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge; //模式0 
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low; //模式0 
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master; //主机
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //72/2 = 36m 大于8M
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;  //不使用硬件NSS多主机模式，没必要一定接NSS脚，当然可以接Nss，SPI_NSS_Hard
	SPI_InitStruct.SPI_CRCPolynomial = 7;//不使用,填默认值7

   	SPI_Init( SPI1,&SPI_InitStruct);
	
	//配置中断  NVIC_Init(&NVIC_InitStruct);SPI_I2S_ITConfig  不需要中断
	
	//启用SPI
	  SPI_Cmd(SPI1,  ENABLE);
	  
	  //CS上电默认为高电平
	  spi_hard_CS(1);
 
}

u8 spi_hard_swapByte( u8 byte)  // 交换字节
{
	u32 TimeOut;
	
	//等待空闲结束 ,注意：不要使用BSY标志处理每一个数据项的发送和接收，最好使用TXE和RXNE标志。
	  //  等待发送寄存器为空 TXE  
	TimeOut = 0x10000;
	while( !SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) )
	{	 
	 	if(!(TimeOut--)) 
		{ 
		ErrorInfo( SPI1_I2S_GetFlagStatus TXE TimeOut ); 
		break;
		}
	}
	SPI_I2S_SendData( SPI1,  byte);//8bit，。数据从发送缓冲器传输到移位寄存器时TXE标志将被置位，在试图写发送缓冲器之前，需确认TXE标志应该为’1’。
	//等待发送完成
	TimeOut = 0x10000;
	while( !SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) )   // //收到数据
	{
		 if(!(TimeOut--))   
		{ 
		ErrorInfo( SPI1_I2S_GetFlagStatus RXNE TimeOut); 
		break; 
		}
	}
	
	//接收到了数据，并返回
	return  SPI_I2S_ReceiveData(SPI1);//读SPI数据寄存器可以清除此标志 ;读SPI_DR寄存器将清除RXNE位。
}

void spi_hard_CS(u8 cs_bit)
{
	if(cs_bit)
	{
		GPIO_WriteBit(CS_PORT,CS_PIN,Bit_SET);
	 
	}else{
		GPIO_WriteBit(CS_PORT,CS_PIN,Bit_RESET);
	}
}


//void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
//uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);


//SPI_Init(  SPI1, SPI_InitTypeDef* SPI_InitStruct);
// SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);

//void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);

//void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
//uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);

//FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
//void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);

//ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
//void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);






