#include "spi_hard.h"
#include "stm32f10x.h"
#include "global.h"


#define CS_PIN 						GPIO_Pin_4
#define CS_PORT						GPIOC // pa-4 SPI1_NCC����ʹ�ã�ԭ��֪��
#define CS_RCC_APB2Periph_GPIOX    RCC_APB2Periph_GPIOC // RCC_APB2Periph_GPIOA //RCC_APB2Periph_GPIOC

/*
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, uint16_t SPI_NSSInternalSoft); //��ʹ��
*/ 

//
/*
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);  //����
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);

ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
*/
//



void spi_hard_init(void) // spi1Ӳ����ʼ��,ģʽ0  NSS ���ģʽ ,//PA4 -NSS //PA5-SCK  //PA6-MI  //PA7-MO
{
	//��������ʱ�� SPI  GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(CS_RCC_APB2Periph_GPIOX,ENABLE);
	//����gpio ,����
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = CS_PIN; ////SPI1_NCC����ʹ�ã�ԭ��֪��, ���ģʽ�£��� δ�ã�����Ϊͨ��I/O
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; //
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init( CS_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;  //PA5-SCK   //PA7-MO
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz; //50Mhz��ȡʧ��
	GPIO_Init( GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;	            //PA6-MI 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init( GPIOA, &GPIO_InitStruct);
	
	//����spi1
	SPI_InitTypeDef  SPI_InitStruct;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge; //ģʽ0 
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low; //ģʽ0 
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master; //����
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //72/2 = 36m ����8M
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;  //��ʹ��Ӳ��NSS������ģʽ��û��Ҫһ����NSS�ţ���Ȼ���Խ�Nss��SPI_NSS_Hard
	SPI_InitStruct.SPI_CRCPolynomial = 7;//��ʹ��,��Ĭ��ֵ7

   	SPI_Init( SPI1,&SPI_InitStruct);
	
	//�����ж�  NVIC_Init(&NVIC_InitStruct);SPI_I2S_ITConfig  ����Ҫ�ж�
	
	//����SPI
	  SPI_Cmd(SPI1,  ENABLE);
	  
	  //CS�ϵ�Ĭ��Ϊ�ߵ�ƽ
	  spi_hard_CS(1);
 
}

u8 spi_hard_swapByte( u8 byte)  // �����ֽ�
{
	u32 TimeOut;
	
	//�ȴ����н��� ,ע�⣺��Ҫʹ��BSY��־����ÿһ��������ķ��ͺͽ��գ����ʹ��TXE��RXNE��־��
	  //  �ȴ����ͼĴ���Ϊ�� TXE  
	TimeOut = 0x10000;
	while( !SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) )
	{	 
	 	if(!(TimeOut--)) 
		{ 
		ErrorInfo( SPI1_I2S_GetFlagStatus TXE TimeOut ); 
		break;
		}
	}
	SPI_I2S_SendData( SPI1,  byte);//8bit�������ݴӷ��ͻ��������䵽��λ�Ĵ���ʱTXE��־������λ������ͼд���ͻ�����֮ǰ����ȷ��TXE��־Ӧ��Ϊ��1����
	//�ȴ��������
	TimeOut = 0x10000;
	while( !SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) )   // //�յ�����
	{
		 if(!(TimeOut--))   
		{ 
		ErrorInfo( SPI1_I2S_GetFlagStatus RXNE TimeOut); 
		break; 
		}
	}
	
	//���յ������ݣ�������
	return  SPI_I2S_ReceiveData(SPI1);//��SPI���ݼĴ�����������˱�־ ;��SPI_DR�Ĵ��������RXNEλ��
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






