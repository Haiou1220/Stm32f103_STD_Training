/**
  ******************************************************************************
 1：完成单个三端口七彩色rgbLED渐变驱动
通过给定rgb结构体数组，前数据与后数据比较，三色同步刷新，从头到尾再返回的数组查询 
 2：时基单元配置，比较输出配置，端口重映射
 给定1ms的频率，改变ccrx值，arr,ccrx重装载
 
3：三通道pwm函数，单个三pinLed灯驱动，渐变RGB颜色函数
10ms任务执行，刷新灯珠，主循环不受影响，可以自定义颜色数组
	
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


//u32 w25q128_Sector_addr0    = W25Q128_Sector_4k_OFFSET_ADDR*0;//w25q128 扇区存储地址0  ，存储 u16编码数据
//u32 w25q128_Sector_addr1 = W25Q128_Sector_4k_OFFSET_ADDR*1;//w25q128 扇区存储地址1    ，存储 u16编码数据
//u32 w25q128_Sector_addr2 = W25Q128_Sector_4k_OFFSET_ADDR*2;//w25q128 扇区存储地址2，   ，存储 double 编码数据8字节
//u32 w25q128_Sector_addr3 = W25Q128_Sector_4k_OFFSET_ADDR*3;//w25q128 扇区存储地址2，   ，存储 double 编码数据8字节





//序列num  u16_buf 
u16 buf_num_u16[300] ;
u16 temp_buf_num_u16[300];
//温度  double_buf
double Buf_temperature_double[4] = {23.12, 24.68, 78.96, 99.9999};  	//8 字节 double；float 4字节
double temp_Buf_temperature_double[4] ;  	// 

//硬件spi 测试buf
u32 buf_hard_spi_u32[300];
u32 buf_temp_hard_spi_u32[300];

//#define OLED_ADDR 0x78
//uint8_t bit_led1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//	void  ws2812b_timer3_init()
//	{
//		//ws2812 1.25us  0.8M=800kb  72M  /0.8M = 90  //apb2-72M,apb1-36M
//		//配置定时器1
////		RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM,  ENABLE);
////		TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;
////		TIM_TimeBaseInitStruct.TIM_Prescaler = 90-1;
////			TIM_TimeBaseInitStruct.TIM_Period = 1;
////		TIM_TimeBaseInit(  TIM1, &TIM_TimeBaseInitStruct);
//		
//		//配置cc
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

extern RGBcolor_t rainbow_color_buf[RAINBOW_COLOR_BUF_COUNT]; ////赤色,橙色,黄色绿色青色蓝色紫色;



 
   

     
  
  
int main(void)
{
  
//	u8 e2p_8bitAddr = 0Xff;
//	u16 e2p_15bitAddr = 0X7fff;
 
	   
  NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//配置中断分组
  //led_init( );//led0 led1 pb5 pe5
  usart1_init(115200 ) ;//pa9 tx,  pa10 rx
 // i2c1_init( ) ;//i2c1  pb6-scl pb7-sda
  timer2_countdown_init();   //配置定时器2countdown中断
  Led1_Off;	
  Led0_Off;	
	
 sysTick_init();//初始化systick
 Info(IN MAIN);

  //AHT10_init( );   //AHT10_初始化
 // iic_soft_initialize( ) ;//软件iic 初始化
 //    if(  W25Q128_soft_init( ) )   ErrorInfo( spi_soft_init  error occurs  );//软件spi 初始化 ，模式0
// W25Q128_hard_init( ); //硬件spi 初始化 ，模式0
 //adxl345_init();  //加速度模块 初始化
 
 // time3_PWM_ch3_init() ; // 时基配置  设置了ARR:1000

 // time2_PWM_ch2_pa1_init();   //PA1,已经配置了时基tim2,  并设置了ARR:1000，时间：1*10^-6  *1*10^3 = 1*10^-3 = 1ms    
   time4_PWM_ch1to3_init( );   //完全重映射 pd12-13-14
	
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
		
	//彩虹渐变rgb色，10ms 刷新
	show_RGB_LED_3Channel_colorBuf_sequence_gradient( (RGBcolor_t* )rainbow_color_buf,    RAINBOW_COLOR_BUF_COUNT);	 //彩虹渐变rgb色

		
		
	}
	
	////1s-任务
	if(FlagTime_1Sec)
	{	
		static uint8_t data_count; //默认为0
		FlagTime_1Sec = RESET;
		data_count++;
		if(data_count % 3 == 2 ){  FlagTime_3Sec = SET;  }   //余 0 1 2
		if(data_count>=255) { data_count = 0; }
		////////////////////////////////////////////////////
	
		   printf("IN WHILE:%d\r\n",data_count);
			

		 
	}
	////3s-任务
	if(FlagTime_3Sec)
	{
		FlagTime_3Sec = RESET;
		/////////////////////////////////////////////////
		static uint8_t count; //默认为0
		count++;
		if(count>=255) { count = 0; }
		// printf("IN WHILE:%d\r\n",count);
 
	     
	}
	 
	
	//定时器计数任务
	if( FlagTime_1ms == SET ) 
	{	
		static uint16_t count1000,count200,count500; //局部静态默认为0
		FlagTime_1ms = RESET;
		count1000++;
		count200++;
		count500++;
		if(count200>=10)  //不包含0
		{
			count200 = 0;
			FlagTime_10MS = SET;
		}
		if(count500>=500)  //不包含0
		{
			count500 = 0;
			FlagTime_500MS = SET;
		}
		if(count1000>=1000)  //不包含0
		{
			count1000 = 0;
			FlagTime_1Sec = SET;
		}
	 }
	
	
		

	
	//led1 blink 任务 led_blink(us_LEDDelayChange);////(1000)*100*(1-18),
	   
	
	 

 }
   
   
} 


