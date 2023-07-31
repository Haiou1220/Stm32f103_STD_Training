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
#include "aht10.h"
#include "eeprom.h"
#include "iic_soft.h"

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


#define Led0_On GPIO_WriteBit( GPIOB,GPIO_Pin_5, Bit_RESET )
#define Led0_Off GPIO_WriteBit( GPIOB,GPIO_Pin_5, Bit_SET )
#define Led1_On GPIO_WriteBit( GPIOE,GPIO_Pin_5, Bit_RESET )
#define Led1_Off GPIO_WriteBit( GPIOE,GPIO_Pin_5, Bit_SET )
#define Led1_Switch GPIO_WriteBit( GPIOE,GPIO_Pin_5, (BitAction)!GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_5) ) 


#define Device_EEPROM 0xA0 //// �ڲ� 24c02ģ�� id ok ,page:256 ,ÿpage��8�ֽ�
#define Device_EEPROM_256K  0xAE ////// 24c246ģ��  , page:512   ÿpage:64�ֽ� ��WordAddr15Bit
#define Device_LM75 0x90 //// LM75�¶�ģ�� id ok
#define EE_READ_ADDR 0xf0 
#define OLED_ADDR 0x78

//uint8_t bit_led1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void led_init(void )//led0 led1 pb5 pe5
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStruct);//pb5
  GPIO_Init(GPIOE,&GPIO_InitStruct);//pb5

}

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
   

	
	
	void led_blink(u32 us) 
	{	
		static u32 temp = 0;
		if (temp != us) { temp = us; SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; sysTickDelay_us_NonBlock(us);  }      //������us
				
			
	 	 if(getFlagSysTickDelayOut() )
		{
		Led1_Switch; 
		}
	}
		 
//	//ws2812_
//	void ws2812_refresh(u32 number )
//	{
//		
//	
//	}
	
int main(void)
{
  
	u8 e2p_8bitAddr = 0Xff;
	u16 e2p_15bitAddr = 0X7fff;

	u8 FlagUsartCMD_2_copy = SET ,FlagUsartCMD_1_copy = SET;
	
	
	 
	   
  NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//�����жϷ���
  led_init( );//led0 led1 pb5 pe5
  usart1_init(115200 ) ;//pa9 tx,  pa10 rx
  i2c1_init( ) ;//i2c1 ��ʪ�ȴ����� pb6-scl pb7-sda
  timer2_countdown_init();   //���ö�ʱ��2countdown�ж�
  Led1_Off;	
  Led0_Off;
	
 sysTick_init();//��ʼ��systick
 	
 Info(IN MAIN);

  AHT10_init( );   //AHT10_��ʼ��

  iic_soft_initialize( ) ;//���iic ��ʼ��

//
 //eeprom_byteWrite_byteAddr( Device_EEPROM, e2p_8bitAddr,Device_EEPROM); //д����
//eeprom_byteWrite_wordAddr( Device_EEPROM_256K, e2p_15bitAddr,Device_EEPROM_256K); //д����
//


  while (1) {
 	if(FlagUsartCMD_1 == SET)
 	{
		FlagUsartCMD_1 = RESET;
		 FlagUsartCMD_1_copy = SET;
		
		static u8  date_write = 0;
		date_write++;
		
		printf("e2p_8bit byte date writing:%d \r\n",date_write);
		
		sysTickDelay_ms(100);
		eeprom_byteWrite_byteAddr( Device_EEPROM, e2p_8bitAddr,date_write); //д����
		
		printf("date of to write:%d \r\n",date_write);
		
	
	}
	
	
	
	if(FlagUsartCMD_2 == SET)
 	{
		FlagUsartCMD_2 = RESET;
	    FlagUsartCMD_2_copy = SET;
 	
		
		static u8  buf_date_write[]  = {0,0,0};	
		
		buf_date_write[0]++,buf_date_write[1]++,buf_date_write[2]++;
		printf("e2p_8bit buf date writing:" );
		for(u8 i = 0 ;i < (sizeof(buf_date_write)/sizeof(buf_date_write[0]));  i++ )
		{
			printf("%d , ",buf_date_write[i]);
		}
		printf("  \r\n" );
		
	 sysTickDelay_ms(100);
		
	 eeprom_bufWrite_byteAddr( Device_EEPROM,  e2p_8bitAddr+1, buf_date_write,  sizeof(buf_date_write)/sizeof(buf_date_write[0]));  //ing

		
		
	}
	
	if(FlagUsartCMD_3 == SET)
 	{
		FlagUsartCMD_3 = RESET;
		
		static u8 buf_date_read[3] ={0xff,0xff,0xff} ,date_read = 0xff;
		

		printf(" e2p_8bit date  reading   \r\n" );
		
		
	 
	 {
		
		 sysTickDelay_ms(100);
		eeprom_byteRead_byteAddr(Device_EEPROM,e2p_8bitAddr+1,buf_date_read,3) ;//��e2p����
		
	 	printf("buf date of to read :" );
		for(u8 i = 0 ;i < 3;  i++ )
		{
			printf("%d , ",buf_date_read[i]);
		}
		printf("  \r\n" );
			 
	 }
	 
	 {
		 
		 sysTickDelay_ms(100);
	 	eeprom_byteRead_byteAddr(Device_EEPROM,e2p_8bitAddr,&date_read,1) ;//��e2p���� 	
		  printf("byte date of to read :%d\r\n",date_read);
	 }
	
	}
	
	if(FlagUsartCMD_4 == SET)
	{
		FlagUsartCMD_4 = RESET;
		
		static u8  date_write = 0;
		date_write++;

		sysTickDelay_ms(100);
		
		 printf("e2p_15bit byte date writing:%d \r\n",date_write);
		 //15bitAddr  byte write
		 if( !eeprom_byteWrite_wordAddr( Device_EEPROM_256K, e2p_15bitAddr, date_write) ) 
		 {
			printf("date of to write:%d \r\n",date_write);
		 
		 }			 
		
	}
	
	if(FlagUsartCMD_5 == SET)
	{
		FlagUsartCMD_5 = RESET;
		static u8 Flag_startup;
		static u8  buf_date_write[70] ;	
		
		//ֻ��һ�Σ���ʼ������
		if(Flag_startup == 0)
		{	
			Flag_startup = 1;
			for(u8 i = 0 ;i < (sizeof(buf_date_write)/sizeof(buf_date_write[0]));  i++ ) 
			{
				buf_date_write[i] = i+1;
			}
		}
	
		sysTickDelay_ms(100); 
		
		printf("e2p_15bit buf date writing:\r\n" );//��ӡ��ʾ
		//15bitAddr buf write
		if( !eeprom_bufWrite_wordAddr( \
		Device_EEPROM_256K,\
		e2p_15bitAddr-100,\
		buf_date_write, \
		(sizeof(buf_date_write)/sizeof(buf_date_write[0])) ) )  
		{
			//��ӡд�������
			for(u8 i = 0+1 ;i < (sizeof(buf_date_write)/sizeof(buf_date_write[0]))+1;  i++ )
			{			
				printf("%03d,",buf_date_write[i-1]);
				if((i%16 == 0)) printf("\r\n" );
			}
			printf("\r\n" );
			
			//ÿһ���Լ�1
			for(u8 i = 0 ;i < 70;  i++ ) 
			{
				buf_date_write[i]++ ;
			}
		
		}
				 
						
						
	

	}
	
	if(FlagUsartCMD_6 == SET) //e2p_15bitAddr  Device_EEPROM_256K
	{
		FlagUsartCMD_6 = RESET;
		u8 store_byte = 0;
		u8 store_buf[70] ; 

		
		//byte read e2p_15bitAddr
		printf(" #e2p_15bit byte  reading   \r\n" );
		if( !eeprom_byteRead_wordAddr( Device_EEPROM_256K, e2p_15bitAddr, &store_byte, 1) )
		{
			printf("byte date of to read :%d\r\n",store_byte);
		}
		 
		
          //buf read ,e2p_15bitAddr
		 printf(" #e2p_15bit buf  reading   \r\n" );		
		 if( !eeprom_bufRead_wordAddr(Device_EEPROM_256K,e2p_15bitAddr-100,store_buf,70) )  
		 {
			
				//��ӡ��ʾ ���		 
				for(u8 i = 0+1 ;i < 70+1;  i++ )
				{			
					printf("%03d,",store_buf[i-1]);
					if((i%16 == 0)) printf("\r\n" );
				}
				printf("\r\n" );
		 }
	}
	
	
	if(FlagUsartCMD_7 == SET) ////iic soft,����256kb eeprom 
	{
		FlagUsartCMD_7 = RESET;

		u8 store_byte = 0;
		//iic soft,byte read e2p_15bitAddr
		printf("##iic soft, e2p_15bit byte  reading   \r\n" );
		//if( iic_soft_eeprom_respond )
		if(	 ! iic_soft_eeprom_byteRead_wordAddr( Device_EEPROM_256K, e2p_15bitAddr, &store_byte, 1) )
		{
			 printf("##byte date of to read :%d\r\n",store_byte);
		}
	
	}
	
		if(FlagUsartCMD_8 == SET) ////iic soft,����256kb eeprom 
	{
		FlagUsartCMD_8 = RESET;

		printf("---------------------- Temper:%.2f\r\n",  AHT10_Task_GetTemperature( )   );
		printf("###################### Humi:%.2f%%\r\n",  ATH10_Task_GetHumidity( )*100  );
	
	}
	
	
	
	
	
	////1s-����
	if(FlagTime_1Sec)
	{	
		static uint8_t data_count; //Ĭ��Ϊ0
		FlagTime_1Sec = RESET;
		data_count++;
		if(data_count % 3 == 2 ){  FlagTime_3Sec = SET;  }   //�� 0 1 2
		if(data_count>=255) { data_count = 0; }
		
		// i2c1_byte_write( Device_EEPROM, 0x01,data_count); //E2PRд����
		 printf("IN WHILE:%d\r\n",data_count);
		
		 
	}
	////3s-����
	if(FlagTime_3Sec)
	{
		
		FlagTime_3Sec = RESET;

	    
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




