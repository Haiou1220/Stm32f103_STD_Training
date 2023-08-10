/**
  ******************************************************************************
  * @file    GPIO/IOToggle/main.c 
  * @author  MCD Application Team
  * @version V3.6.0
  * @date    20-September-2021
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2011 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stdio.h"
#include "global.h"
#include "systick.h"
#include "i2c_hard.h"
#include "iic_soft.h"
#include "usart.h"
#include "timer.h"
#include "aht10.h"
#include "eeprom.h"
#include "iic_soft_eeprom.h"
#include "lm75b.h"
#include "led.h"
#include "w25q128.h"
 

 u8 Debug_count;
u16 Debug_count_WordAddrToWrite;
/*Compiler definec : USE_STDPERIPH_DRIVER,STM32F10X_MD */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 u8 FlagUsartCMD_1,FlagUsartCMD_2,FlagUsartCMD_3,FlagUsartCMD_4,FlagUsartCMD_5,FlagUsartCMD_6,FlagUsartCMD_7,FlagUsartCMD_8;

 uint8_t FlagTime_1ms,FlagTime_1Sec,FlagTime_3Sec;
 u32 us_LEDDelayChange = 1000*1800 ;


u32 w25q128_Sector_addr0    = W25Q128_Sector_4k_OFFSET_ADDR*0;//w25q128 �����洢��ַ0  ���洢  
u32 w25q128_Sector_addr1 = W25Q128_Sector_4k_OFFSET_ADDR*1;//w25q128 �����洢��ַ1    ���洢  
u32 w25q128_Sector_addr2 = W25Q128_Sector_4k_OFFSET_ADDR*1;//w25q128 �����洢��ַ2��   ���洢  






//����num  u16_buf 
u16 buf_num_u16[300] ;
u16 temp_buf_num_u16[300];
//�¶�  double_buf
double Buf_temperature_double[4] = {23.12, 24.68, 78.96, 99.9999};  	//8 �ֽ� double��float 4�ֽ�
double temp_Buf_temperature_double[4] ;  	// 



//#define OLED_ADDR 0x78
//uint8_t bit_led1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//	void  ws2812b_timer3_init()
//	{
//		//ws2812 1.25us  0.8M=800kb  72M  /0.8M = 90  //apb2-72M,apb1-36M
//		//���ö�ʱ��1
////		RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM,  ENABLE);
////		TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;
////		TIM_TimeBaseInitStruct.TIM_Prescaler = 90-1;
////			TIM_TimeBaseInitStruct.TIM_Period = 1;
////		TIM_TimeBaseInit(  TIM1, &TIM_TimeBaseInitStruct);
//		
//		//����cc
//	//	tim_
//		
//	
//	}
   		 
//	//ws2812_
//	void ws2812_refresh(u32 number )
//	{
//		
//	
//	}




		
		

		
		
	/*
	�������� �� �������� ���spi д�뵽����0 ����1
	������ʽ������ д��洢���ĸ�ʽ�����������
	��page ��page д��
	��ӡ�����п��ƺ���
	
	
	*/	
		
	
int main(void)
{
  
//	u8 e2p_8bitAddr = 0Xff;
//	u16 e2p_15bitAddr = 0X7fff;
 
	   
  NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//�����жϷ���
  led_init( );//led0 led1 pb5 pe5
  usart1_init(115200 ) ;//pa9 tx,  pa10 rx
 // i2c1_init( ) ;//i2c1  pb6-scl pb7-sda
  timer2_countdown_init();   //���ö�ʱ��2countdown�ж�
  Led1_Off;	
  Led0_Off;
	
 sysTick_init();//��ʼ��systick
 Info(IN MAIN);

  //AHT10_init( );   //AHT10_��ʼ��
 // iic_soft_initialize( ) ;//���iic ��ʼ��


////spi
//SPI_InitTypeDef  SPI_InitStruct;
//SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;        //ǰ��
//SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;          //�͵�ƽ
//SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;   //
//SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
//SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
//SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
//SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;

//SPI_Init(  SPI1, SPI_InitTypeDef* SPI_InitStruct);
// SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
//void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);
//void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
//uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);
//FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
//void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
//ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
//void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);







	


//���spi ��ʼ�� ��ģʽ0
if( w25q128_soft_spi_init(0))   ErrorInfo( w25q128_soft_spi_init  error occurs  );
	 





//���ݳ�ʼ��/////////////
for( u16 i =0 ;i < sizeof(buf_num_u16)/sizeof(buf_num_u16[0]) ;i++)
{
	buf_num_u16[i] = i;
}
//���ݳ�ʼ��//////////////



 while (1) {
 	if(FlagUsartCMD_1 == SET)
 	{
		FlagUsartCMD_1 = RESET;
				
		//һPageд��addr0  --buf_num_u16
		W25Q128_SOFT_Page_Program(  w25q128_Sector_addr0    , (u8*)buf_num_u16, 2*sizeof(buf_num_u16)/sizeof(buf_num_u16[0]) ) ; //02h  A23-A16 A15-A8 A7-A0 D7-D0 D7-D0(3) 
	
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_2 == SET)
 	{
		FlagUsartCMD_2 = RESET;
	
		//��Pageд�룬д��addr0  --buf_num_u16
	 	W25Q128_SOFT_Sector_Write_4K(  w25q128_Sector_addr0    , (u8*)buf_num_u16, 2*sizeof(buf_num_u16)/sizeof(buf_num_u16[0])  );
		
		
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	
	if(FlagUsartCMD_3 == SET)
 	{
		FlagUsartCMD_3 = RESET;
		

	   //��addr0 ���� --buf_num_u16
	    w25q128_soft_Read_Data( w25q128_Sector_addr0    ,(u8*)temp_buf_num_u16, 2* sizeof(temp_buf_num_u16)/sizeof(temp_buf_num_u16[0]) );
		
	  
		//����Ԥ��
	    print_unsignedNum_BufShow_SetColumnm( (u8*)temp_buf_num_u16 ,  2 ,sizeof(temp_buf_num_u16)/sizeof(temp_buf_num_u16[0])  ,15) ; 
			
		
			
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_4 == SET)
	{
		FlagUsartCMD_4 = RESET;
			
		//��Pageд�룬д��addr1  ---Buf_temperature_double
	 	W25Q128_SOFT_Sector_Write_4K( w25q128_Sector_addr1    ,  (u8*) Buf_temperature_double,  8*sizeof(Buf_temperature_double)/sizeof(Buf_temperature_double[0]) );
		
		
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_5 == SET)
	{
		FlagUsartCMD_5 = RESET;
		
	 	  

		//��addr1 ���� ---temp_Buf_temperature_double
	    w25q128_soft_Read_Data( w25q128_Sector_addr1    ,(u8*)temp_Buf_temperature_double, 8* sizeof(temp_Buf_temperature_double)/sizeof(temp_Buf_temperature_double[0]) );
		
		//����Ԥ��
		printf("%f,%f,%f,%f\r\n",temp_Buf_temperature_double[0],temp_Buf_temperature_double[1],temp_Buf_temperature_double[2],temp_Buf_temperature_double[3]);
		
		
		
		
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_6 == SET) //
	{
		FlagUsartCMD_6 = RESET;
	
		
		
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	
	if(FlagUsartCMD_7 == SET) ////
	{
		FlagUsartCMD_7 = RESET;
		
	
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_8 == SET) //// 
	{
		FlagUsartCMD_8 = RESET;
		
			u8  MANUFACTURER_ID ;
		u16 Device_ID;
		
		//�������豸��
		 if(   !W25Q128_soft_Get_JEDEC_ID(&MANUFACTURER_ID,&Device_ID ) ) 
		 {
			 printf("W25Q128_Get_ MANUFACTURER_ID:0X%04X,Device_ID:0X%04X\r\n" ,   MANUFACTURER_ID, Device_ID ) ;

		 }
					
			
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	
	////1s-����
	if(FlagTime_1Sec)
	{	
		static uint8_t data_count; //Ĭ��Ϊ0
		FlagTime_1Sec = RESET;
		data_count++;
		if(data_count % 3 == 2 ){  FlagTime_3Sec = SET;  }   //�� 0 1 2
		if(data_count>=255) { data_count = 0; }
		
	
		// printf("IN WHILE:%d\r\n",data_count);
		
		 
	}
	////3s-����
	if(FlagTime_3Sec)
	{
		FlagTime_3Sec = RESET;
		
		static uint8_t count; //Ĭ��Ϊ0
		count++;
		if(count>=255) { count = 0; }
		 printf("IN WHILE:%d\r\n",count);
	
		
		
	     
	}
	 
	
	//��ʱ����������
	if( FlagTime_1ms == SET ) 
	{	
		static uint16_t count; //�ֲ���̬Ĭ��Ϊ0
		FlagTime_1ms = RESET;
		count++;
		if(count>=1000)  //������0
		{
			count = 0;
			FlagTime_1Sec = SET;
		}
	 }
	
	
		

	
	//led1 blink ���� led_blink(us_LEDDelayChange);////(1000)*100*(1-18),
	   
	
	 

 }
   
   
} 




