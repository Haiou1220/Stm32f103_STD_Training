/**
  ******************************************************************************
  
1.DMA 的应用，实现 dma内存到外设的数据搬运：定时器的pwm 比较寄存器的dma操作，请求标志位：输出比较匹配标志位
2.dma的应用 ，实现 dma 内存到外设的数据搬运：串口1的DR 寄存器的 dma操作 ， 请求标志位：TXE:发送数据寄存器空
3.dma应用，dma的内存到内存 操作。
4.ws2812灯带的零一码与pwm频率的对应、颜色数组的转码操作，颜色数组的修改用于产生不同效果
5.dma外设与通道的配置，dma中断函数
6.用逻辑分析仪看波形数据，和串口打印写入输出比较寄存器的数据，两者匹配

本项目，实现了ws2812灯带的动态效果点灯，有移动效果有跑马灯效果有亮度调节效果有环形跑效果
可以串口打印比较输出寄存器的写入数据，用dma实现了3万多个字节的秒传，如果串口要10多秒

用串口助手，发送命令0-9数组，可以查看运行指定任务，如查看串口串口寄存器，查看dma通道寄存器等，方便调试
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  

  
  注意事项：1.global头文件：不要把main文件的全局变量 声明在global头文件，因为global头文件是大部分c文件  引用的公共头文件 ，
		如果 mian全局变量 修改，global头文件没有修改(也没道理要修改global.h，而是要修改main.h文件)，引用的文件编译就报错，
		  改进办法：谁使用就自己 在引用main.h 文件，因为main.h里面有对外开放的变量声明
	2.在单个文件内定义全局变量时使用static修饰
	3.不论全局变量或局部变量都应初始化后再使用。
	4.定时器dma中 如果使用定时器更新作为申请的话，每一次传输完成要设置比较值为0 如果大于某个数 ，值定时器开启时已经是输出高电平了。这个用法不妥，发送完比较数据后 还在比较输出为高电平， 在完成传输时设置比较值为0，但会多出一点高电平，
	  如果完成后关闭通道也不能解决。结论：不使用更新事件作为申请dma的事件请求
	
	4.应使用大括号 来 对 数组及结构体的非0初始化；   初始化为0或NULL时，只用一层大括号即可。
	5.无符号数做减法，减数不能大于被减数，可以使用三目运算符解决
	6.仅允许在函数体内使用goto 语句，用于出现错误时统一处理，其他情况则禁止使用goto 
	7. 存在中文时，使用UTF-8 编码可以避免在不同编辑器上显示乱码的问题。
	8.命名前缀 ,指针 pFile, 函数 fnCallBack ,非静态全局变量 gNumCount,静态全局变量 s_iCount 
	9.静态全局变量区，自己使用， 非静态全局变量 外部文件引用
	
  不理解地方：1.TIM_GenerateEvent(TIM3,TIM_EventSource_Update);// 因此在计数器开始计数之前，必须通过设置TIMx_EGR寄存器中的UG位来初始化所有的寄存器。 的用法
	2.定时器 dma的通道请求 其中更新标志位请求与比较输出标志位请求 的区别， 
   dma 发送的时候注意 外设寄存器的的数据宽度（虽然地址不自增，但是如果发的宽度是实际的宽度的一半，那么这个寄存器就存储两次的结果，原理不太清楚,我猜应该dma写寄存器有计数功能知道还没有写满） 

	 

	
	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "global.h"
#include "systick.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
 
#include "ws2812.h"
#include "pwm_ws2812.h" 
#include "dma_ws2812.h"  
#include "dma_usart.h"  



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
 //------------------------系统全局变量头文件对外声明----------------------------------------
 
 

//--------------------------------------变量区0	---------------------------------------------------

 //dma内存 搬运数据 test数据
static u8 s_Dma_test_u8_source[10]={1,2,3,4,5,6,7,8,9,10};
u8 gDma_test_u8Buf_dest[10]  ; 
 
     
 
// //灯板数据
//GRB_CCRx_t GRB_CCRx_buf[ 64+1 ] = {0};//最后一组是 DUMMY_COLOR    ,灯板 64个led
//RGB24BIT_t RGB24bitColor_Buf[64+1]={
//0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,//1*8
//0x000080,0x000080,0x000080,0x000080,0x000080,0x000080,0x000080,0x000080,//2
//0x556B2F,0x556B2F,0x556B2F,0x556B2F,0x556B2F,0x556B2F,0x556B2F,0x556B2F,//3	
//0xFFE4E1,0xFFE4E1,0xFFE4E1,0xFFE4E1,0xFFE4E1,0xFFE4E1,0xFFE4E1,0xFFE4E1,//4
//0x1E90FF,0x1E90FF,0x1E90FF,0x1E90FF,0x1E90FF,0x1E90FF,0x1E90FF,0x1E90FF,	//5
//0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,//6
//0x8B4513,0x8B4513,0x8B4513,0x8B4513,0x8B4513,0x8B4513,0x8B4513,0x8B4513,//7
//0xFF1493,0xFF1493,0xFF1493,0xFF1493,0xFF1493,0xFF1493,0xFF1493,0xFF1493,	//8  
//DUMMY_COLOR	
//};

//灯带 数据
GRB_CCRx_t GRB_CCRx_buf[ 300+1 ] = {0};//最后一组是 DUMMY_COLOR:reset码    ,灯带300个led
RGB24BIT_t RGB24bitColor_Buf1[300+1]; 
RGB24BIT_t RGB24bitColor_Buf[300+1]={
	0xFFFAFA,0xF8F8FF,0xF5F5F5,0xDCDCDC,0xFFFAF0,0xFDF5E6,0xFAF0E6,0xFAEBD7,0xFFEFD5,0xFFEBCD,0xFFE4C4,0xFFDAB9,0xFFDEAD,0xFFE4B5,0xFFF8DC,0xFFFFF0,0xFFFACD,0xFFF5EE,0xF0FFF0,0xF5FFFA,0xF0FFFF,0xF0F8FF,
	0xE6E6FA,0xFFF0F5,0xFFE4E1,0xFFFFFF,0x2F4F4F,0x696969,0x708090,0x778899,0xBEBEBE,0xD3D3D3,0x191970,0x000080,0x6495ED,0x483D8B,0x6A5ACD,0x7B68EE,0x8470FF,0x0000CD,0x4169E1,0x0000FF,0x1E90FF,0x00BFFF,
	0x87CEEB,0x87CEFA,0x4682B4,0xB0C4DE,0xADD8E6,0xB0E0E6,0xAFEEEE,0x00CED1,0x48D1CC,0x40E0D0,0x00FFFF,0xE0FFFF,0x5F9EA0,0x66CDAA,0x7FFFD4,0x006400,0x556B2F,0x8FBC8F,0x2E8B57,0x3CB371,0x20B2AA,0x98FB98,
	0x00FF7F,0x7CFC00,0x00FF00,0x7FFF00,0x00FA9A,0xADFF2F,0x32CD32,0x9ACD32,0x228B22,0x6B8E23,0xBDB76B,0xEEE8AA,0xFAFAD2,0xFFFFE0,0xFFFF00,0xFFD700,0xEEDD82,0xDAA520,0xB8860B,0xBC8F8F,0xCD5C5C,0x8B4513,
	0xA0522D,0xCD853F,0xDEB887,0xF5F5DC,0xF5DEB3,0xF4A460,0xD2B48C,0xD2691E,0xB22222,0xA52A2A,0xE9967A,0xFA8072,0xFFA07A,0xFFA500,0xFF8C00,0xFF7F50,0xF08080,0xFF6347,0xFF4500,0xFF0000,0xFF69B4,0xFF1493,
	0xFFC0CB,0xFFB6C1,0xDB7093,0xB03060,0xC71585,0xD02090,0xFF00FF,0xEE82EE,0xDDA0DD,0xDA70D6,0xBA55D3,0x9932CC,0x9400D3,0x8A2BE2,0xA020F0,0x9370DB,0xD8BFD8,0xFFFAFA,0xEEE9E9,0xCDC9C9,0x8B8989,0xFFF5EE,
	0xEEE5DE,0xCDC5BF,0x8B8682,0xFFEFDB,0xEEDFCC,0xCDC0B0,0x8B8378,0xFFE4C4,0xEED5B7,0xCDB79E,0x8B7D6B,0xFFDAB9,0xEECBAD,0xCDAF95,0x8B7765,0xFFDEAD,0xEECFA1,0xCDB38B,0x8B795E,0xFFFACD,0xEEE9BF,0xCDC9A5,
	0x8B8970,0xFFF8DC,0xEEE8CD,0xCDC8B1,0x8B8878,0xFFFFF0,0xEEEEE0,0xCDCDC1,0x8B8B83,0xF0FFF0,0xE0EEE0,0xC1CDC1,0x838B83,0xFFF0F5,0xEEE0E5,0xCDC1C5,0x8B8386,0xFFE4E1,0xEED5D2,0xCDB7B5,0x8B7D7B,0xF0FFFF,
	0xE0EEEE,0xC1CDCD,0x838B8B,0x836FFF,0x7A67EE,0x6959CD,0x473C8B,0x4876FF,0x436EEE,0x3A5FCD,0x27408B,0x0000FF,0x0000EE,0x0000CD,0x00008B,0x1E90FF,0x1C86EE,0x1874CD,0x104E8B,0x63B8FF,0x5CACEE,0x4F94CD,
	0x36648B,0x00BFFF,0x00B2EE,0x009ACD,0x00688B,0x87CEFF,0x7EC0EE,0x6CA6CD,0x4A708B,0xB0E2FF,0xA4D3EE,0x8DB6CD,0x607B8B,0xC6E2FF,0xB9D3EE,0x9FB6CD,0x6C7B8B,0xCAE1FF,0xBCD2EE,0xA2B5CD,0x6E7B8B,0xBFEFFF,
	0xB2DFEE,0x9AC0CD,0x68838B,0xE0FFFF,0xD1EEEE,0xB4CDCD,0x7A8B8B,0xBBFFFF,0xAEEEEE,0x96CDCD,0x668B8B,0x98F5FF,0x8EE5EE,0x7AC5CD,0x53868B,0x00F5FF,0x00E5EE,0x00C5CD,0x00868B,0x00FFFF,0x00EEEE,0x00CDCD,
	0x008B8B,0x97FFFF,0x8DEEEE,0x79CDCD,0x528B8B,0x7FFFD4,0x76EEC6,0x66CDAA,0x458B74,0xC1FFC1,0xB4EEB4,0x9BCD9B,0x698B69,0x54FF9F,0x4EEE94,0x43CD80,0x2E8B57,0x9AFF9A,0x90EE90,0x7CCD7C,0x548B54,0x00FF7F,
	0x00EE76,0x00CD66,0x008B45,0x00FF00,0x00EE00,0x00CD00,0x008B00,0x7FFF00,0x76EE00,0x66CD00,0x458B00,0xC0FF3E,0xB3EE3A,0x9ACD32,0x698B22,0xCAFF70,0xBCEE68,0xA2CD5A,0x6E8B3D,0xFFF68F,0xEEE685,0xCDC673,
	0x8B864E,0xFFEC8B,0xEEDC82,0xCDBE70,0x8B814C,0xFFFFE0,0xEEEED1,0xCDCDB4,0x8B8B7A,0xFFFF00,0xEEEE00,0xCDCD00,0x8B8B00,0xFF000A,DUMMY_COLOR//0xFFD700
	
};

char str[] = "1234567890"  ; 

char sprintf_str[32000]={0};
u32 sprintf_str_out_num = 0;


//--------------------------------------变量区0	---------------------------------------------------
	



int main(void)
{
//------------------------变量区1----------------------  
//定时任务  计时器 标志位
 uint8_t  FlagTime_nMS,FlagTime_500MS,FlagTime_1Sec,FlagTime_3Sec;
  // s8 Light_per = 0; //ws2812 的亮度百分百0-100
 
	 
	
	
//------------------------变量区1----------------------
	
	
 

////////////////////////////   
  NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//配置中断分组
  sysTick_init();//初始化systick
  usart1_init(115200 ) ;//pa9 tx,  pa10 rx
  timer2_countdown_init();   //配置定时器2countdown中断
  Led1_Off;	
  Led0_Off;	
/////////////////////
	Info( MAIN FUN STAR UP); //打印系统启动提示
////////////////////
	
	//DMA1内存到内存 发送 buf ,初始化 ，串口命令0 触发软件请求，dma 中断打印结果
    dma1_ch1_mem2mem_init();
 
    // 定时器3配置 ，pwm ws2812 配置  PB0,开启ocr dma 请求
    ws2812_PwmTIM3CH3_init( ); 
	
	 //初始化DMA1CH2,请求是 CCR3 比较标志位
	     ws2812_DMA1CH2_to_Tim3OCR3_init( );// tim3_ch2 	// (u32)&GRB_CCRx_buf ,  (sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0]))
	

	 

	//初始默认 转码并发数据 
//	RGB24BIT_To_TIMxCCRx_Codebuf_AddResCode(  RGB24bitColor_Buf,GRB_CCRx_buf ,  (sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0])) ,100);// 转码 
	// ws2812_DMA1CH2_for_TIM3OCR3_Send(  (u32)&GRB_CCRx_buf ,  ( sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0]) ) );//   1s 间隔刷新
 


 dma1CH4_USART1_TX_init();
 while ( 1 ) {
	 
  
	
	 
	//////////////////////-----------------------串口命令任务---------------------------------------//////////////////
 	if(gFlagUsartCMD_0 == SET)
 	{
		gFlagUsartCMD_0 = RESET;
	////////////////
		//------------------------------------------------------改进，因为有占用cpu ，需要改为串口dma 搬运----------------------------------------
		
	  print_CCRxBuf_column24(GRB_CCRx_buf ,sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0])); //打印  转码值 展示
	  printf("\r\n");
		
		
		
    ///////////////////////////////////////////////////////////////////////////////////
	}

	if(gFlagUsartCMD_1 == SET)
 	{
		gFlagUsartCMD_1 = RESET;
	/////////////////////////////////////////////////
		
		//----------------------------------------------------------内存到内存 dma 数据搬运----------------------------
		printf("FUN:dma_mem2men sending u8_buf  ;\r\n");
		 //DMA MEM2MEM 发送数据，dma1ch1中断回应
		 dma1_ch1_mem2mem_u8_send((u32)&s_Dma_test_u8_source   ,  (u32)&gDma_test_u8Buf_dest ,sizeof(gDma_test_u8Buf_dest)/sizeof(gDma_test_u8Buf_dest[0])); //DMA MEM2MEM 开启完成中断
		
		   //改变原数据
		 	 for(u16 i = 0 ; i<255; i++)
		 	{
		 	   s_Dma_test_u8_source[i]++ ;

		 	}
		
		

	///////////////////////////////////////////////////////////////////////////////////	
	}
	
	if(gFlagUsartCMD_2 == SET)
 	{
		gFlagUsartCMD_2 = RESET;
		//////////////////////////////
			//------------------------------------串口1 dma 数据搬运-------------------------------------------
		
	  printf("sprintf for CCRxBuf starting\r\n" )	;
	   	
	 sprint_CCRxBuf_column24(  sprintf_str,sizeof(sprintf_str)/sizeof(sprintf_str[0]) ,\
								 GRB_CCRx_buf ,  sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0]) ,\
									  &sprintf_str_out_num);    //打印  转码值 到字符串
								 
		 printf("sprintf for CCRxBuf :out_num:%d,str_capacity:%d\r\n",sprintf_str_out_num,sizeof(sprintf_str)/sizeof(sprintf_str[0]) )	;						 
								 
		printf(" usart1_sendBuf_dma1CH4_start: SEND LEN :%d \r\n ",    sizeof(sprintf_str)/sizeof(sprintf_str[0])     );   
		usart1_sendBuf_dma1CH4_start( (u32)&sprintf_str , sizeof(sprintf_str)/sizeof(sprintf_str[0])            );//函数 //使能串口 发送DMA请求
		
								 
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
	printf("------------------gFlagUsartCMD_4\r\n");
	 
		printf("get now USART1->CR3:%#2x \r\n",USART1->CR3);
		printf("USART1->DR:%#x \r\n ",USART1->CR3 );   //调试
		 printf("DMA1_Channel4->CNDTR:%d \r\n ",DMA1_Channel4->CNDTR );   //调试   
	 
		
   ///////////////////////////////////////////////////////////////////////////////////		
	}
	
	if(gFlagUsartCMD_5 == SET) //
	{
		gFlagUsartCMD_5 = RESET;
		/////
		printf("(------------------gFlagUsartCMD_5\r\n");
		static u8 SwitchBit = 0;
		
		if( SwitchBit)
		{
		printf("DMA USART TX DISABLE\r\n");
		USART1->CR3 &= ~(0x80);
			SwitchBit = ~SwitchBit;
			
		}else{
		
		printf("DMA USART TX ENABLE\r\n"); 
		USART1->CR3 |= (0x80);
		SwitchBit = ~SwitchBit;
		}
		
		
		
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
	
  
	
	
	
	
	
/***************************************定时 任务*************************************************************/ 
	if(FlagTime_nMS)
	{
		FlagTime_nMS = RESET;  		////////// Nms
		////////////////
 
		       
 		        RGB24bitColorBuf_effect__sequence_add_del_color1( RGB24bitColor_Buf1, sizeof(RGB24bitColor_Buf1)/sizeof(RGB24bitColor_Buf1[0])   );// 配色方案2 : 
 		      //  RGB24bitColorBuf_effect__sequence_add_del_color( RGB24bitColor_Buf1, sizeof(RGB24bitColor_Buf1)/sizeof(RGB24bitColor_Buf1[0])   );// 配色方案1:添加 顺序颜色 颜色 效果
		 		 //  RGB24bitColorBuf_effect_move(  RGB24bitColor_Buf,  (sizeof(RGB24bitColor_Buf)/sizeof(RGB24bitColor_Buf[0])  )  ,1 );			//   配色方案0:移动效果
		
		
		
 	  	   RGB24BIT_To_TIMxCCRx_Codebuf_AddResCode(  RGB24bitColor_Buf1,GRB_CCRx_buf ,  (sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0])) ,100);// 转码 ,亮度100
//        //	RGB24BIT_To_TIMxCCRx_Codebuf_AddResCode(  RGB24bitColor_Buf,GRB_CCRx_buf ,  (sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0])) ,10);// 转码 ，亮度10
   
		
 	  ws2812_DMA1CH2_for_TIM3OCR3_Send(  (u32)&GRB_CCRx_buf ,  ( sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0]) ) );//    间隔刷新 CCRx 数据

	
		
		
		
		
		
		
		
		
		
	}
/***************************************定时 任务*************************************************************/ 	
	////0.5s-任务
	if(FlagTime_500MS)
	{
	FlagTime_500MS = RESET;
		//////////
	
	
	}
/***************************************定时 任务*************************************************************/ 
	////1s-任务
	if(FlagTime_1Sec)
	{	
		static uint8_t data_count; //默认为0
		FlagTime_1Sec = RESET;
		data_count++;
		if(data_count % 3 == 2 ){  FlagTime_3Sec = SET;  }   //余 0 1 2
		if(data_count>=255) { data_count = 0; }
		////////////////////////////////////////////////////
 
	}	
/***************************************定时 任务*************************************************************/ 		
	
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
	
 
 
	 

 }
   
   
} 




