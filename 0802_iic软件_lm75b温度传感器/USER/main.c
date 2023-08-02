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

#include "aht10.h"
#include "eeprom.h"
#include "iic_soft_eeprom.h"
#include "lm75b.h"
#include "led.h"


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


#define Device_EEPROM 0xA0 //// �ڲ� 24c02ģ�� id ok ,page:256 ,ÿpage��8�ֽ�
#define Device_EEPROM_256K  0xAE ////// 24c246ģ��  , page:512   ÿpage:64�ֽ� ��WordAddr15Bit
#define Device_LM75 0x90 //// LM75�¶�ģ�� id ok
#define EE_READ_ADDR 0xf0 
#define OLED_ADDR 0x78

//uint8_t bit_led1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void usart1_init(uint32_t BaudRate) //pa9 tx,  pa10 rx �жϽ���
{

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,   ENABLE);//��������ʱ��
	//����gpio
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, & GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, & GPIO_InitStruct);
	//����usart1
	USART_InitTypeDef  USART_InitStruct;
	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(  USART1, &USART_InitStruct);
	//���ô����ж�
	NVIC_InitTypeDef  NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(& NVIC_InitStruct);
	//���������ж�
	USART_ITConfig( USART1,   USART_IT_RXNE,  ENABLE );
	//��������
	USART_Cmd( USART1, ENABLE );

}

int fputc(int c,FILE* f)
{
																																				  
	while ( USART_GetFlagStatus(   USART1,  USART_FLAG_TXE ) != SET) {}; //�ȴ�TXE=1���ͼĴ����գ���ʾ������ɣ���Ӳ����λ��TC��TXE��λֵΪ1
	USART_SendData(  USART1,  c);
	
	return c;
}
void assert_failed(uint8_t* file, uint32_t line)
{
	printf("Error:%s:%d\r\n",file,line);
	while(1){};
}





void i2c1_init(void) //i2c1 ��ʪ�ȴ����� pb6-scl pb7-sda   24c02 a0=a1=a2=0 id=0XA0
{
	//ʱ�ӿ���
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1,  ENABLE);
	//gpio���� pb6-scl         pb7-sda 
	GPIO_InitTypeDef   GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;// ȫ�����ÿ�©�������ֹ��·
	GPIO_Init(  GPIOB,&GPIO_InitStruct);//pb6-pb7
	//i2c1����
	I2C_InitTypeDef   I2C_InitStruct;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = 100000;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1 = 0x01;
	I2C_Init(  I2C1,& I2C_InitStruct);
	//ʹ��
	I2C_Cmd(  I2C1,  ENABLE);

}


void  timer2_countdown_init(void)        //���ö�ʱ��2��ʱ1ms�ж�
{
	//�򿪶�ʱ��2ʱ��
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2,  ENABLE);
	//���ö�ʱ��2
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode =  TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_Period =  1000  ;    // 1*10-3 -> 1ms��װ��ֵҪ����0��cn������
		TIM_TimeBaseInitStruct.TIM_Prescaler =  72-1; //1M ->1*10-6(��ƵҪ��һ)
	TIM_TimeBaseInit(  TIM2, &TIM_TimeBaseInitStruct);
	//���ö�ʱ��2�ж�
	TIM_ITConfig(  TIM2,  TIM_IT_Update,  ENABLE);
	//��ʱ��2�ж����ȼ�
	NVIC_InitTypeDef    NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel =  TIM2_IRQn; 
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =  2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(  &NVIC_InitStruct);
 	//��ʱ��2 ʹ��
 	TIM_Cmd(  TIM2,  ENABLE);

 
}

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
	
int main(void)
{
  
//	u8 e2p_8bitAddr = 0Xff;
//	u16 e2p_15bitAddr = 0X7fff;
 
	   
  NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//�����жϷ���
  led_init( );//led0 led1 pb5 pe5
  usart1_init(115200 ) ;//pa9 tx,  pa10 rx
  i2c1_init( ) ;//i2c1  pb6-scl pb7-sda
  timer2_countdown_init();   //���ö�ʱ��2countdown�ж�
  Led1_Off;	
  Led0_Off;
	
 sysTick_init();//��ʼ��systick
 	
 Info(IN MAIN);

  //AHT10_init( );   //AHT10_��ʼ��

  iic_soft_initialize( ) ;//���iic ��ʼ��




  while (1) {
 	if(FlagUsartCMD_1 == SET)
 	{
		FlagUsartCMD_1 = RESET;
		
		
	
	}
	
	
	
	if(FlagUsartCMD_2 == SET)
 	{
		FlagUsartCMD_2 = RESET;
	
 	
		
		
		
	}
	
	if(FlagUsartCMD_3 == SET)
 	{
		FlagUsartCMD_3 = RESET;
		
	
	
	}
	
	if(FlagUsartCMD_4 == SET)
	{
		FlagUsartCMD_4 = RESET;
		
		
		
	}
	
	if(FlagUsartCMD_5 == SET)
	{
		FlagUsartCMD_5 = RESET;
		
		

	}
	
	if(FlagUsartCMD_6 == SET) //
	{
		FlagUsartCMD_6 = RESET;
	


	}
	
	
	if(FlagUsartCMD_7 == SET) ////
	{
		FlagUsartCMD_7 = RESET;

	//����lm75b ,���¹�ͣ�Ĵ������ָ��¶ȼĴ��� ���¶�
		 lm75b_set_temper_over( 34); 
		printf("lm75b_set_temper_over  ing \r\n");
		 lm75b_set_temper_hyster(31);
		printf("lm75b_set_temper_hyster  ing \r\n");
		
	
	}
	
	if(FlagUsartCMD_8 == SET) //// 
	{
		FlagUsartCMD_8 = RESET;

 
		
		
		u8  conf_reg;
	 //��lm75b ����conf�Ĵ���
		lm75b_read_conf( &conf_reg );
		printf("lm75b_read_conf:0x%02x\r\n",conf_reg);
		
		//��lm75b ���¹�ͣ�Ĵ������ָ��¶ȼĴ���
		u16  Tover_reg_9b,Thys_reg_9b;
		lm75b_read_Tover_reg( &Tover_reg_9b ) ;
		printf("lm75b_read_Tover_reg_9b:%d,read set over_temper:%0.2f\r\n",Tover_reg_9b,Tover_reg_9b*0.5);
		 lm75b_read_Thys_reg_9b( &Thys_reg_9b )  ;
		printf("lm75b_read_Thys_reg_9b:read set hyster_temper:%0.2f\r\n",Thys_reg_9b*0.5);
	
	}
	
	
	
	
	
	////1s-����
	if(FlagTime_1Sec)
	{	
		static uint8_t data_count; //Ĭ��Ϊ0
		FlagTime_1Sec = RESET;
		data_count++;
		if(data_count % 3 == 2 ){  FlagTime_3Sec = SET;  }   //�� 0 1 2
		if(data_count>=255) { data_count = 0; }
		
	
		 printf("IN WHILE:%d\r\n",data_count);
		
		 
	}
	////3s-����
	if(FlagTime_3Sec)
	{
		 
		
		FlagTime_3Sec = RESET;
		
		//iic ����� LM75B 
		float  temper;
		u16    temp_reg;
		lm75b_read_temp(&temper,&temp_reg  )  ;
		printf("REG:%d\r\n",temp_reg);
		printf("lm75b_read_temper:%f\r\n",temper);
		
		
	     
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




