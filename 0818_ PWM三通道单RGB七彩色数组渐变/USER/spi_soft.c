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


// CS�͵�ƽѡ��
#define SPI_CS_LOW	     GPIO_WriteBit( CS_GPIO,  CS_PIN,  Bit_RESET)
#define SPI_CS_HIGH	     GPIO_WriteBit( CS_GPIO,  CS_PIN,  Bit_SET)
	//Ĭ�ϵ�ƽ spiģʽ0 ����ʱ�Ӽ���0��ʱ����λ��0��sck����Ϊ0��sck��һ�����ز���
#define SPI_SCK_HIGH		 GPIO_WriteBit( SCK_GPIO,  SCK_PIN,  Bit_SET)
#define SPI_SCK_LOW	          GPIO_WriteBit( SCK_GPIO,  SCK_PIN,  Bit_RESET)

#define SPI_MO_HIGH		 GPIO_WriteBit( MOSI_GPIO,  MOSI_PIN,  Bit_SET)


void spi_soft_init_mode0(void);
void spi_soft_init_mode3(void);	
u8 spi_soft_swapByte_mode3(u8 sendByte); //ģʽ3����1������ ������ ����2������ �ɼ����ݣ�SCK����=1
u8 spi_soft_swapByte_mode0(u8 sendByte); //ģʽ0��receiveByte sendByte //spi��λ�Ĵ�������λ����    //ģʽ0����1�����ز����������أ�����0������CS�����ݣ��½��أ�

u8 SPI_SOFT_MODE = 0xff;  //ֻ���ڱ��ļ�ʹ����




void spi_soft_CS(u8 bit) //CS�ź�
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
		temp = spi_soft_swapByte_mode0(Byte) ;//ģʽ0��receiveByte sendByte //spi��λ�Ĵ�������λ����    //ģʽ0����1�����ز����������أ�����0������CS�����ݣ��½��أ�

			break;
		case 1:
			
			break;
		case 2:
			
			break;
	
		case 3:
			temp = spi_soft_swapByte_mode3(Byte); //ģʽ3����1������ ������ ����2������ �ɼ����ݣ�SCK����=1
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
	//������
	if((mode<=3) && (mode>=0))
	{
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,  ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//cs -PC4 Ƭѡ 
	GPIO_InitStruct.GPIO_Pin = CS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( CS_GPIO, &GPIO_InitStruct);

	
	//PA5-SCK  �����ֲ�������ԣ� Tsclh = Tsckl >=45%PC ;PC= 1/Fc = 1/133M = 7518us ;3383us 
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
	//Ĭ�ϵ�ƽ spiģʽ0 ����ʱ�Ӽ���0��ʱ����λ��0��sck����Ϊ0��sck��1�����ز���
	SPI_CS_HIGH; //ʧ�ܣ��͵�ƽ��Ч 
	SPI_SCK_LOW; //sck����Ϊ0��
	SPI_MO_HIGH; //MO ��ƽ ��ʼ����
}
	
void spi_soft_init_mode3(void)
{
	// spiģʽ3 ����ʱ�Ӽ���1��ʱ����λ1 ��sck����Ϊ1��  sck��2�����ز���
	SPI_CS_HIGH; //ʧ�ܣ��͵�ƽ��Ч 
	SPI_SCK_HIGH; //sck����Ϊ1��
	SPI_MO_HIGH; //MO ��ƽ ��ʼ����
}


u8 spi_soft_swapByte_mode3(u8 sendByte) //ģʽ3����1������ ������ ����2������ �ɼ����ݣ�SCK����=1
{													
	for(u8 i = 0;i < 8;i++)
	{
	
	//�½��أ���һ������ ������
	SPI_SCK_LOW;
	spi_MOSI_SendBit(  sendByte & 0X80);//������,MSB
	sendByte <<= 1;
	
	//�����أ��ڶ����زɼ� ��
	SPI_SCK_HIGH;
	if( spi_MISO_ReceiveBit())  sendByte |= 0x01;
		
	//���������ѭ��
		
	}
		
	return sendByte;
}

u8 spi_soft_swapByte_mode0(u8 sendByte) //ģʽ0��receiveByte sendByte //spi��λ�Ĵ�������λ����    //ģʽ0����1�����ز����������أ�����0������CS�����ݣ��½��أ�
{
	for(u8 i = 0;i < 8;i++)
	{
	//������,MSB
	 spi_MOSI_SendBit(  sendByte & 0X80);
	sendByte <<= 1;
	//�����أ���һ�����ز���
	SPI_SCK_HIGH;
	if( spi_MISO_ReceiveBit())  sendByte |= 0x01;
	
	//�½��ؽ���������ѭ��
	SPI_SCK_LOW;
	}
	
	return sendByte;
}


