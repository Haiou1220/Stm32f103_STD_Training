#include "spi_hard.h"
#include "stm32f10x.h"
#include "global.h"



#define SPIHard_CS_PIN			        GPIO_Pin_4   //pc4-CS
#define SPIHard_CS_GPIO			        GPIOC   //pc4-CS

#define SPIHard_MOSI_PIN 		 		GPIO_Pin_7 //PA7-MO
#define SPIHard_MOSI_GPIO		 		GPIOA //PA7-MO

#define SPIHard_MISO_PIN				GPIO_Pin_6  //PA6-MI
#define SPIHard_MISO_GPIO				GPIOA  //PA6-MI

#define SPIHard_SCK_PIN					GPIO_Pin_5  //PA5-SCK
#define SPIHard_SCK_GPIO				GPIOA  //PA5-SCK




//void spi_hard_init(void) // spi1硬件初始化,模式0  （MO  MI SCL）硬件    CS软件
//{
//	
//	SPI_InitTypeDef  SPI_InitStruct;
//	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge; //模式0 
//	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low; //模式0 
//	SPI_InitStruct.SPI_Mode = SPI_Mode_Master; //主机
//	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //72/2 = 36m
//	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
//	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
//	SPI_InitStruct.SPI_NSS = SPI_NSS_Hard;
//	//SPI_InitStruct.SPI_CRCPolynomial = SPI_CRCPolynomial
//	
//	
//	SPI_InitStruct.
//	SPI_Init( SPI1, SPI_InitTypeDef* SPI_InitStruct)

//	
//	


//}


