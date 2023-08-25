/**
  ******************************************************************************
 
 项目介绍：
 1.使用定时器输入捕获功能，对旋转编码器的ab相其中的一相进行脉冲进行采集，捕获功能有滤波功能，最后得出脉冲有有效边沿，，
 在脉冲的边沿对另外一相进行高低电平判断，可以读出方向，脉冲数可以得出脉冲个数
 2.获得编码器的控制信号从而 实现对 pwm脉冲宽度调制从而实现舵机SG90的角度控制
 
 3.默认角度复位90度，在指定0-180角度快速定位，用50ms 定时刷新舵机角度的pwm比较值
控制脉冲一直维持，以保证有维持的力矩输出，及时修正外力的影响。					 
编码器顺时针增加角度值，逆时针减少角度值。
4.串口可以发 0命令 1命令 减少或者增加角度

  
   
 如果定时器 时钟72M,预分频器 72-1;
 定时器分辨率是1M
  
 技术实现：
 1. 定时器的捕获功能，gpio的输入功能
  
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "global.h"
#include "systick.h"//延时定时器
#include "timer.h"//定时器2 1ms中断 作为任务定时
#include "usart.h"//串口1 串口助手
#include "led.h"
#include "dma_usart.h"  //串口1dma
#include "pwm_servo.h"  //串口1dma
#include "ic_timer.h"


/*Compiler definec : USE_STDPERIPH_DRIVER,STM32F10X_MD */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/




/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 

  
 //------------------------系统全局变量头文件对外声明---------------------------------------- 
//  串口命令任务 ，串口命令 字符标志位
 u8 gFlagUsartCMD_0,gFlagUsartCMD_1,gFlagUsartCMD_2,gFlagUsartCMD_3,gFlagUsartCMD_4,gFlagUsartCMD_5,gFlagUsartCMD_6,gFlagUsartCMD_7,gFlagUsartCMD_8,gFlagUsartCMD_9;
   // 定时任务标志位
  uint8_t gFlagTime_1MS;
  
  u8 gDebug_u8;  //调试
  u16 gDebug_u16; //调试 
  
  
  
   
u16 gServo_setAngle = 90;
 //------------------------系统全局变量头文件对外声明----------------------------------------
 
 

//--------------------------------------全局静态static变量区 	---------------------------------------------------

 


//--------------------------------------全局静态static变量区 	---------------------------------------------------
	



int main(void)
{
//------------------------主函数局部变量区----------------------  
//定时任务  计时器 标志位
 uint8_t  FlagTime_nMS,FlagTime_500MS,FlagTime_1Sec,FlagTime_3Sec;
 

 
	
//------------------------主函数局部变量区----------------------
	
	
 

////////////////////////////   
  NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//配置中断分组2(2bit:2bit)
  sysTick_init();//初始化systick
	usart1_init(115200 ) ;//pa9 tx,  pa10 rx,,,接收中断:
  timer2_countdown_init();   //配置定时器2为基准的任务计时
  Led1_Off;	
  Led0_Off;	
/////////////////////
	Info( SYS IS STAR UP); //打印系统启动提示
////////////////////
//------------------------启动函数区----------------------	
	  Servo_tim5ch2pa1_init( ) ;//定时器5 ch2 比较输出
	
 
   
 

 //Servo_tim5ch2pa1_setAngle(  90); //20ms 周期正确，，
   EncoderAB_init( ); // 编码器初始化，定时器4通道4输入捕获，A相下降沿输入，B相 pe0电平输入
	

//------------------------启动函数区----------------------	
 while ( 1 ) {
	 
  
	
	 
	//////////////////////-----------------------串口命令任务---------------------------------------//////////////////
 	if(gFlagUsartCMD_0 == SET)
 	{
		gFlagUsartCMD_0 = RESET;
	////////////////
	 //----------------------------------串口0命令 减 舵机角度
		if(gServo_setAngle == 0)
		{ gServo_setAngle = 0;
		}else{
			gServo_setAngle--;
		}
	
		printf("-gServo_setAngle:%d\r\n",gServo_setAngle);
		
    ///////////////////////////////////////////////////////////////////////////////////
	}

	if(gFlagUsartCMD_1 == SET)
 	{
		gFlagUsartCMD_1 = RESET;
	/////////////////////////////////////////////////
		
	 //----------------------------------串口1命令 加 舵机角度
		gServo_setAngle++;
		if(gServo_setAngle > 180) gServo_setAngle =180;
		printf("+gServo_setAngle:%d\r\n",gServo_setAngle);

	///////////////////////////////////////////////////////////////////////////////////	
	}
	
	if(gFlagUsartCMD_2 == SET)
 	{
		gFlagUsartCMD_2 = RESET;
		//////////////////////////////
			
		//
									
    ///////////////////////////////////////////////////////////////////////////////////
	}

	
	if(gFlagUsartCMD_3 == SET)
	{
		gFlagUsartCMD_3 = RESET;
	 //////


	
		
		
		
    //////////////////////////////////////////////////////////////////////////////////////////		
	}
	
	if(gFlagUsartCMD_4 == SET)
	{
		gFlagUsartCMD_4 = RESET;
		//////////////////
	
	 
		
   ///////////////////////////////////////////////////////////////////////////////////		
	}
	
	if(gFlagUsartCMD_5 == SET) //
	{
		gFlagUsartCMD_5 = RESET;
		/////
	
		
		
	///////////////////////////////////////////////////////////////////////////////////
	}
	
	if(gFlagUsartCMD_6 == SET) //
	{
		gFlagUsartCMD_6 = RESET;
		////
		
		
		
		
		
		
	///////////////////////////////////////////////////////////////////////////////////		
	}
	if(gFlagUsartCMD_7 == SET) ////
	{
		gFlagUsartCMD_7 = RESET;
		//////
 
		
///////////////////////////////////////////////////////////////////////////////////			 
	}	
	if(gFlagUsartCMD_8 == SET) //// 
	{
		gFlagUsartCMD_8 = RESET;
		//////
		
 
		
		
///////////////////////////////////////////////////////////////////////////////////		 
	}
	if(gFlagUsartCMD_9 == SET) 
	{
		gFlagUsartCMD_9 = RESET;
	//////////////////
    		
 
		
	 
////////////////////////////////////////////////////////////////////////////////////////////////	
	}
	
  
	
	
	
	
	
/***************************************定时 Nms任务*************************************************************/ 
	if(FlagTime_nMS)
	{
		FlagTime_nMS = RESET;  		////////// Nms
		////////////////
		//50ms
		    
		
		
	
						  Servo_tim5ch2pa1_setAngle( gServo_setAngle) ;
		
		
		
		
	}
/***************************************定时0.5S 任务*************************************************************/ 	
	////0.5s-任务
	if(FlagTime_500MS)
	{
	FlagTime_500MS = RESET;
		//////////
	
	
	}
/***************************************定时 1S任务*************************************************************/ 
	////1s-任务
	if(FlagTime_1Sec)
	{	
		static uint8_t data_count; //默认为0
		FlagTime_1Sec = RESET;
		data_count++;
		if(data_count % 3 == 2 ){  FlagTime_3Sec = SET;  }   //余 0 1 2
		if(data_count>=255) { data_count = 0; }
		////////////////////////////////////////////////////
		 
 	

 
		
		
 /***************************************定时 3S任务*************************************************************/ 		
	}	
	////3s-任务
	if(FlagTime_3Sec)
	{
		FlagTime_3Sec = RESET;
		/////////////////////////////////////////////////
 
	     
	}
	 
/***************************************定时 任务*************************************************************/ 	
	//定时器2计数任务
	if( gFlagTime_1MS == SET ) 
	{	
		static uint16_t count1000,countN,count500; //局部静态默认为0
		gFlagTime_1MS = RESET;
		count1000++;
		countN++;
		count500++;
		if(countN>=   10  )  //自定义 ms
		{
			countN = 0;
			FlagTime_nMS = SET;
		}
		if(count500>=500)  //0.5S任务标志位
		{
			count500 = 0;
			FlagTime_500MS = SET;
		}
		if(count1000>=1000)  ////1S任务标志位，，，闭区间0................1000开区间
		{
			count1000 = 0;
			FlagTime_1Sec = SET;
		}
	 }
	
 
 
	 

 }//WHILE
}//MAIN




