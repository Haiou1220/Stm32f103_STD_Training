/**
  ******************************************************************************
 
 1.spi 接口 驱动axdl345
 2.配置axdl 寄存器
 3.读取 三轴 g值 
4.通过g值 计算三轴倾斜角  需要三角几何知识
5.通过 z轴g值 计数 跑步步数
   6.发送 串口命令0 进行三轴零校对
	7. 未完成 滤波 
	
	
	 在xy平面上，x轴发生倾斜a角度，  那么 sin_a = 对边/斜边
	 对边 = x轴的g分量，斜边 = g
	 因为 可以把三轴空间的g分量 即 Ax Ay Az 组成的矢量 等效 为1g,因为它们满足勾股定理： Ax平方 + Ay平方 +  Az平方 = 1g 平方 = 1
	
	那么 sin_a = 对边/斜边 = Ax /1 =Ax
	那么 cos_a = 邻边/斜边 = ( 1平方-Ax平方   ) /1 = sqrt(Ay平方 + Az平方 )
	那么 tan_a = sin_a/cos_a = Ax/sqrt(Ay平方 + Az平方 )
	
	那么 角度a = atan(   Ax/sqrt(Ay平方 + Az平方 )  )
	其他两轴 同理
	
	 
	
	
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

 uint8_t FlagTime_1ms,FlagTime_200MS,FlagTime_1Sec,FlagTime_3Sec;
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


 adxl345_init();  //加速度模块 初始化
 



	



	 





//数据初始化/////////////
for( u16 i =0 ;i < sizeof(buf_num_u16)/sizeof(buf_num_u16[0]) ;i++)
{
	buf_num_u16[i] = i;
}
for( u16 i =0 ;i < ( sizeof(buf_hard_spi_u32)/sizeof(buf_hard_spi_u32[0])) ;i++)
{
	buf_hard_spi_u32[i] = i;
}

//数据初始化//////////////

u8 buf_byte_read_axis_xyz[6];

u32  step_num = 0;

//三轴g值分量
 float gForge_1g_axis_x,gForge_1g_axis_y,gForge_1g_axis_z;
s32 gForge_mg_axis_x,gForge_mg_axis_y,gForge_mg_axis_z;

	
static float atan_par_x ,atan_par_y,atan_par_z;
	


 while (1) {
 	if(FlagUsartCMD_0 == SET)
 	{
		FlagUsartCMD_0 = RESET;
	//////////////////////////////////
		
	
		printf("------------------writing adxl345 3axis Offset register  \r\n");
		
		 adxl345_3axis_Offset_check(  ) ;//, 三轴 计算并写入 偏差寄存器的值

	 

	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_1 == SET)
 	{
		FlagUsartCMD_1 = RESET;
	///////////////////////////////////////
		//打印 adxl345 ID号默认   速率 默认
	 printf("adxl345_ReadReg( ADXL345_DEVID ):%#x\r\n",adxl345_ReadReg( ADXL345_DEVID ));; //0XE5 ID号默认
	 	printf("adxl345_ReadReg( ADXL345_BW_RATE ):%#x\r\n",adxl345_ReadReg( ADXL345_BW_RATE ));; //0X0A 速率 默认
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	
	if(FlagUsartCMD_2 == SET)
 	{
		FlagUsartCMD_2 = RESET;
		

  
			
		
			
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_3 == SET)
	{
		FlagUsartCMD_3 = RESET;
	 
		
		
	}
	///////////////////////////////////////////////////////////////////////////////////
	
	if(FlagUsartCMD_4 == SET)
	{
		FlagUsartCMD_4 = RESET;
		
	 	  
		
		
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
		
   
		
 	 
		 
	}

///////////////////////////////////////////////////////////////////////////////////	
	
	if(FlagUsartCMD_8 == SET) //// 
	{
		FlagUsartCMD_8 = RESET;
		
	 
			
	}
	
	///////////////////////////////////////////////////////////////////////////////////
	if(FlagUsartCMD_9 == SET) //// 
	{
		FlagUsartCMD_9 = RESET;
	//////////////////////////////////////////////
 	
		 
			
	}
	///////////////////////////////////////////////////////////////////////////////////
	if(FlagTime_200MS)
	{
		FlagTime_200MS = 0;
		////////////////////////////////////////////////
		static u32 old_step_num = 0;
		
		 //200ms计算一次 三轴xzy g值 ,ok
		adxl345_Buf_Read(  ADXL345_DATAX0 ,  buf_byte_read_axis_xyz,  6) ;
		 
		gForge_mg_axis_x = adxl345_axis_dataTransform(buf_byte_read_axis_xyz[0],buf_byte_read_axis_xyz[1])  ;
		gForge_mg_axis_y = adxl345_axis_dataTransform( buf_byte_read_axis_xyz[2],buf_byte_read_axis_xyz[3])  ;
		gForge_mg_axis_z = adxl345_axis_dataTransform(buf_byte_read_axis_xyz[4],buf_byte_read_axis_xyz[5])   ;
		
	   gForge_1g_axis_x =	gForge_mg_axis_x / 1000.0;
		gForge_1g_axis_y =	gForge_mg_axis_y / 1000.0;
	    gForge_1g_axis_z =  gForge_mg_axis_z / 1000.0;
		
		
		// 执行 //Z轴mg 是否 发生超过或者低于指定数值表示 发送 上升动作 和下降下降， 表示完成一步
	   stepping_occur_Z_axis(  gForge_mg_axis_z ,&step_num) ;
		//步数更新了
		if( step_num != old_step_num )
		{	
			old_step_num = step_num;
			printf("--------------------------------step_num:%d\r\n", step_num );
		}
		

		
		
		
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
	
		// printf("IN WHILE:%d\r\n",data_count);
			
	     //增加三角函数 输出角度
		
		printf("%d,%d,%d\r\n",gForge_mg_axis_x,gForge_mg_axis_y,gForge_mg_axis_z) ; //调试
		
		//计算三轴 反tan的参数
		atan_par_x =  gForge_1g_axis_x/ sqrt( pow(gForge_1g_axis_y , 2) +pow(gForge_1g_axis_z , 2) );
		atan_par_y =  gForge_1g_axis_y/ sqrt( pow(gForge_1g_axis_x , 2) +pow(gForge_1g_axis_z , 2)  );
		atan_par_z =  gForge_1g_axis_z/ sqrt( pow(gForge_1g_axis_x , 2) +pow(gForge_1g_axis_y , 2)  );
		///输出三轴倾斜角
		 printf("angle X:%0.1f,angle Y:%0.1f,angle Z:%0.1f\r\n", Rad2Ang*atan(atan_par_x)  ,Rad2Ang*atan(atan_par_y)   ,  Rad2Ang*atan(atan_par_z)         );
		
		 
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
 
	
//		printf("X:%d,Y:%d,Z:%d\r\n",	
//			adxl345_axis_dataTransform(buf_byte_read_axis_xyz[0],buf_byte_read_axis_xyz[1]) ,
//			adxl345_axis_dataTransform( buf_byte_read_axis_xyz[2],buf_byte_read_axis_xyz[3]),
//	      	adxl345_axis_dataTransform(buf_byte_read_axis_xyz[4],buf_byte_read_axis_xyz[5])   );
		
	     
	}
	 
	
	//定时器计数任务
	if( FlagTime_1ms == SET ) 
	{	
		static uint16_t count1000,count200; //局部静态默认为0
		FlagTime_1ms = RESET;
		count1000++;
		count200++;
		if(count200>=200)  //不包含0
		{
			count200 = 0;
			FlagTime_200MS = SET;
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




