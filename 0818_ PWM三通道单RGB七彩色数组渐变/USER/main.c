/**
  ******************************************************************************
 1����ɵ������˿��߲�ɫrgbLED��������
ͨ������rgb�ṹ�����飬ǰ����������ݱȽϣ���ɫͬ��ˢ�£���ͷ��β�ٷ��ص������ѯ 
 2��ʱ����Ԫ���ã��Ƚ�������ã��˿���ӳ��
 ����1ms��Ƶ�ʣ��ı�ccrxֵ��arr,ccrx��װ��
 
3����ͨ��pwm������������pinLed������������RGB��ɫ����
10ms����ִ�У�ˢ�µ��飬��ѭ������Ӱ�죬�����Զ�����ɫ����
	
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
#include "spi_hard.h"
#include "adxl345.h"
#include "adxl345_instruction.h"
 #include "math.h"
 
#include "pwm.h"
  
  
  
#define PI (3.141592654)
#define Rad2Ang  (180/PI)



u8 Debug_count;
u16 Debug_count_WordAddrToWrite;
/*Compiler definec : USE_STDPERIPH_DRIVER,STM32F10X_MD */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 u8 FlagUsartCMD_0,FlagUsartCMD_1,FlagUsartCMD_2,FlagUsartCMD_3,FlagUsartCMD_4,FlagUsartCMD_5,FlagUsartCMD_6,FlagUsartCMD_7,FlagUsartCMD_8,FlagUsartCMD_9;

 uint8_t FlagTime_1ms,FlagTime_10MS,FlagTime_500MS,FlagTime_1Sec,FlagTime_3Sec;
 u32 us_LEDDelayChange = 1000*1800 ;


//u32 w25q128_Sector_addr0    = W25Q128_Sector_4k_OFFSET_ADDR*0;//w25q128 �����洢��ַ0  ���洢 u16��������
//u32 w25q128_Sector_addr1 = W25Q128_Sector_4k_OFFSET_ADDR*1;//w25q128 �����洢��ַ1    ���洢 u16��������
//u32 w25q128_Sector_addr2 = W25Q128_Sector_4k_OFFSET_ADDR*2;//w25q128 �����洢��ַ2��   ���洢 double ��������8�ֽ�
//u32 w25q128_Sector_addr3 = W25Q128_Sector_4k_OFFSET_ADDR*3;//w25q128 �����洢��ַ2��   ���洢 double ��������8�ֽ�





//����num  u16_buf 
u16 buf_num_u16[300] ;
u16 temp_buf_num_u16[300];
//�¶�  double_buf
double Buf_temperature_double[4] = {23.12, 24.68, 78.96, 99.9999};  	//8 �ֽ� double��float 4�ֽ�
double temp_Buf_temperature_double[4] ;  	// 

//Ӳ��spi ����buf
u32 buf_hard_spi_u32[300];
u32 buf_temp_hard_spi_u32[300];

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

extern RGBcolor_t rainbow_color_buf[RAINBOW_COLOR_BUF_COUNT]; ////��ɫ,��ɫ,��ɫ��ɫ��ɫ��ɫ��ɫ;



 
   

     
  
  
int main(void)
{
  
//	u8 e2p_8bitAddr = 0Xff;
//	u16 e2p_15bitAddr = 0X7fff;
 
	   
  NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//�����жϷ���
  //led_init( );//led0 led1 pb5 pe5
  usart1_init(115200 ) ;//pa9 tx,  pa10 rx
 // i2c1_init( ) ;//i2c1  pb6-scl pb7-sda
  timer2_countdown_init();   //���ö�ʱ��2countdown�ж�
  Led1_Off;	
  Led0_Off;	
	
 sysTick_init();//��ʼ��systick
 Info(IN MAIN);

  //AHT10_init( );   //AHT10_��ʼ��
 // iic_soft_initialize( ) ;//���iic ��ʼ��
 //    if(  W25Q128_soft_init( ) )   ErrorInfo( spi_soft_init  error occurs  );//���spi ��ʼ�� ��ģʽ0
// W25Q128_hard_init( ); //Ӳ��spi ��ʼ�� ��ģʽ0
 //adxl345_init();  //���ٶ�ģ�� ��ʼ��
 
 // time3_PWM_ch3_init() ; // ʱ������  ������ARR:1000

 // time2_PWM_ch2_pa1_init();   //PA1,�Ѿ�������ʱ��tim2,  ��������ARR:1000��ʱ�䣺1*10^-6  *1*10^3 = 1*10^-3 = 1ms    
   time4_PWM_ch1to3_init( );   //��ȫ��ӳ�� pd12-13-14
	
  time4_PWM_ch1to3_Switch(1) ;  //




 while (1) {
	 
	
	 
	 
 	if(FlagUsartCMD_0 == SET)
 	{
		FlagUsartCMD_0 = RESET;
	////////////////
		
	  time4_PWM_ch1to3_Switch(0) ;  //

	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_1 == SET)
 	{
		FlagUsartCMD_1 = RESET;
	/////////////////////
		
	   time4_PWM_ch1to3_Switch(1) ;  //
			
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	
	if(FlagUsartCMD_2 == SET)
 	{
		FlagUsartCMD_2 = RESET;
		////////////
	
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_3 == SET)
	{
		FlagUsartCMD_3 = RESET;
	 //////
	
		
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_4 == SET)
	{
		FlagUsartCMD_4 = RESET;
		//////
	 	  
		
		
	}
	
	///////////////////////////////////////////////////////////////////////////////////

	if(FlagUsartCMD_5 == SET) //
	{
		FlagUsartCMD_5 = RESET;
		
 	
			
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
		//////
		
   
		  
 	 
		 
	}

///////////////////////////////////////////////////////////////////////////////////	
	
	if(FlagUsartCMD_8 == SET) //// 
	{
		FlagUsartCMD_8 = RESET;
		//////

	 
		

		
		
		
	}
	
	///////////////////////////////////////////////////////////////////////////////////
	if(FlagUsartCMD_9 == SET) 
	{
		FlagUsartCMD_9 = RESET;
	//////////////////
 
			
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagTime_500MS)
	{
	FlagTime_500MS = 0;
		//////////
		
		
	  
	
	
	
	}
	
	
	
	if(FlagTime_10MS)
	{
		FlagTime_10MS = 0;
		////////// 
		
	//�ʺ罥��rgbɫ��10ms ˢ��
	show_RGB_LED_3Channel_colorBuf_sequence_gradient( (RGBcolor_t* )rainbow_color_buf,    RAINBOW_COLOR_BUF_COUNT);	 //�ʺ罥��rgbɫ

		
		
	}
	
	////1s-����
	if(FlagTime_1Sec)
	{	
		static uint8_t data_count; //Ĭ��Ϊ0
		FlagTime_1Sec = RESET;
		data_count++;
		if(data_count % 3 == 2 ){  FlagTime_3Sec = SET;  }   //�� 0 1 2
		if(data_count>=255) { data_count = 0; }
		////////////////////////////////////////////////////
	
		   printf("IN WHILE:%d\r\n",data_count);
			

		 
	}
	////3s-����
	if(FlagTime_3Sec)
	{
		FlagTime_3Sec = RESET;
		/////////////////////////////////////////////////
		static uint8_t count; //Ĭ��Ϊ0
		count++;
		if(count>=255) { count = 0; }
		// printf("IN WHILE:%d\r\n",count);
 
	     
	}
	 
	
	//��ʱ����������
	if( FlagTime_1ms == SET ) 
	{	
		static uint16_t count1000,count200,count500; //�ֲ���̬Ĭ��Ϊ0
		FlagTime_1ms = RESET;
		count1000++;
		count200++;
		count500++;
		if(count200>=10)  //������0
		{
			count200 = 0;
			FlagTime_10MS = SET;
		}
		if(count500>=500)  //������0
		{
			count500 = 0;
			FlagTime_500MS = SET;
		}
		if(count1000>=1000)  //������0
		{
			count1000 = 0;
			FlagTime_1Sec = SET;
		}
	 }
	
	
		

	
	//led1 blink ���� led_blink(us_LEDDelayChange);////(1000)*100*(1-18),
	   
	
	 

 }
   
   
} 


