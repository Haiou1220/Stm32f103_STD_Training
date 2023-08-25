#include "spi_soft.h"
#include "stm32f10x.h"
#include "global.h"

#define CS_PIN			       GPIO_Pin_4   //pc4-CS
#define CS_GPIO			       GPIOC   //pc4-CS

#define MOSI_PIN 		 		GPIO_Pin_7 //PA7-MO
#define MOSI_GPIO		 		GPIOA //PA7-MO

#define MISO_PIN				GPIO_Pin_6  //PA6-MI
#define MISO_GPIO				GPIOA  //PA6-MI

#define SCK_PIN					GPIO_Pin_5  //PA5-SCK
#define SCK_GPIO				GPIOA  //PA5-SCK


// CS低电平选中
#define SPI_CS_LOW	     GPIO_WriteBit( CS_GPIO,  CS_PIN,  Bit_RESET)
#define SPI_CS_HIGH	     GPIO_WriteBit( CS_GPIO,  CS_PIN,  Bit_SET)
	//默认电平 spi模式0 ：：时钟极性0，时钟相位：0；sck空闲为0，sck第一个边沿采样
#define SPI_SCK_HIGH		 GPIO_WriteBit( SCK_GPIO,  SCK_PIN,  Bit_SET)
#define SPI_SCK_LOW	          GPIO_WriteBit( SCK_GPIO,  SCK_PIN,  Bit_RESET)

#define SPI_MO_HIGH		 GPIO_WriteBit( MOSI_GPIO,  MOSI_PIN,  Bit_SET)


void spi_soft_init_mode0(void);
void spi_soft_init_mode3(void);	
u8 spi_soft_swapByte_mode3(u8 sendByte); //模式3：第1个边沿 放数据 ，第2个边沿 采集数据，SCK空闲=1
u8 spi_soft_swapByte_mode0(u8 sendByte); //模式0：receiveByte sendByte //spi移位寄存器：高位先行    //模式0，第1个边沿采样（上升沿），第0个边沿CS发数据（下降沿）

u8 SPI_SOFT_MODE = 0xff;  //只能在本文件使用吗




void spi_soft_CS(u8 bit) //CS信号
{
	
	if(bit)
	{	
		SPI_CS_HIGH;
	
	}else
	{	
		SPI_CS_LOW;
	}
}


void spi_MOSI_SendBit(u8 bit)
{
	if(bit)
	{
		GPIO_WriteBit( MOSI_GPIO,  MOSI_PIN,  Bit_SET);
	}else
	{
		GPIO_WriteBit( MOSI_GPIO,  MOSI_PIN,  Bit_RESET);
	}	
		
}

BitAction spi_MISO_ReceiveBit(void)
{
	if(    GPIO_ReadInputDataBit(MISO_GPIO,MISO_PIN)   )
	{
		return Bit_SET;
	}else
	{
		return Bit_RESET;
	}
	
}



u8 spi_soft_swapByte(u8 Byte)
{
	u8 temp ;
	switch(SPI_SOFT_MODE)
	{
		case 0:
		temp = spi_soft_swapByte_mode0(Byte) ;//模式0：receiveByte sendByte //spi移位寄存器：高位先行    //模式0，第1个边沿采样（上升沿），第0个边沿CS发数据（下降沿）

			break;
		case 1:
			
			break;
		case 2:
			
			break;
	
		case 3:
			temp = spi_soft_swapByte_mode3(Byte); //模式3：第1个边沿 放数据 ，第2个边沿 采集数据，SCK空闲=1
			break;
		
		default: 

		temp = 0;

	} 
	return  temp;

}





u8 spi_soft_init(s8 mode)  //MODE; 0 1 2 3
{
	u8 ret;
	SPI_SOFT_MODE = mode;
	//检查参数
	if((mode<=3) && (mode>=0))
	{
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,  ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//cs -PC4 片选 
	GPIO_InitStruct.GPIO_Pin = CS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( CS_GPIO, &GPIO_InitStruct);

	
	//PA5-SCK  数据手册电气特性： Tsclh = Tsckl >=45%PC ;PC= 1/Fc = 1/133M = 7518us ;3383us 
	GPIO_InitStruct.GPIO_Pin = SCK_PIN;        //                      
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init( SCK_GPIO, &GPIO_InitStruct);
	
	//PA7-MO OUT-PP
	GPIO_InitStruct.GPIO_Pin = MOSI_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( MOSI_GPIO, &GPIO_InitStruct);
	
	////PA6-MI  INPUT-UP
	GPIO_InitStruct.GPIO_Pin = MISO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( MISO_GPIO, &GPIO_InitStruct);
	
	}
	
		  if(mode == 0)
		  {
			spi_soft_init_mode0( );
			  ret = 0;
		  }else if(mode == 1)
		  {
			 ret = 1;
		  
		  }else if(mode == 2)
		  {
			ret = 1;
		  }else if(mode ==3 )
		  {
			spi_soft_init_mode3( );
				ret = 0;
		  }else
		  {
			Info( ERROR:SPI soft mode arg is out of range);
		  }
		 
			if(ret)
			{
				  Info( ERROR:SPI soft mode init mistake);
			}
			
			
	return ret;
}

void spi_soft_init_mode0(void)
{
	//默认电平 spi模式0 ：：时钟极性0，时钟相位：0；sck空闲为0，sck第1个边沿采样
	SPI_CS_HIGH; //失能，低电平有效 
	SPI_SCK_LOW; //sck空闲为0，
	SPI_MO_HIGH; //MO 电平 初始化高
}
	
void spi_soft_init_mode3(void)
{
	// spi模式3 ：：时钟极性1，时钟相位1 ；sck空闲为1，  sck第2个边沿采样
	SPI_CS_HIGH; //失能，低电平有效 
	SPI_SCK_HIGH; //sck空闲为1，
	SPI_MO_HIGH; //MO 电平 初始化高
}


u8 spi_soft_swapByte_mode3(u8 sendByte) //模式3：第1个边沿 放数据 ，第2个边沿 采集数据，SCK空闲=1
{													
	for(u8 i = 0;i < 8;i++)
	{
	
	//下降沿，第一个边沿 放数据
	SPI_SCK_LOW;
	spi_MOSI_SendBit(  sendByte & 0X80);//放数据,MSB
	sendByte <<= 1;
	
	//上升沿，第二边沿采集 ，
	SPI_SCK_HIGH;
	if( spi_MISO_ReceiveBit())  sendByte |= 0x01;
		
	//或结束，或循环
		
	}
		
	return sendByte;
}

u8 spi_soft_swapByte_mode0(u8 sendByte) //模式0：receiveByte sendByte //spi移位寄存器：高位先行    //模式0，第1个边沿采样（上升沿），第0个边沿CS发数据（下降沿）
{
	for(u8 i = 0;i < 8;i++)
	{
	//放数据,MSB
	 spi_MOSI_SendBit(  sendByte & 0X80);
	sendByte <<= 1;
	//上升沿，第一个边沿采样
	SPI_SCK_HIGH;
	if( spi_MISO_ReceiveBit())  sendByte |= 0x01;
	
	//下降沿结束，或者循环
	SPI_SCK_LOW;
	}
	
	return sendByte;
}


