
#include "pwm.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "systick.h"
#include "global.h"



RGBcolor_t rainbow_color_buf[RAINBOW_COLOR_BUF_COUNT]={ //赤色,橙色,黄色绿色青色蓝色紫色
{255,0,0  },

{255,165,0  },

{255,255,0  },

{0,255,0  },

{0,127,255  },

{0,0,255  },

{139,0,255  }

};

//RGBcolor_t rainbow_color_buf[RAINBOW_COLOR_BUF_COUNT]={ // 调试

//{0,0,0  } ,//7--
//{255,255,128  },//1 -r---------------'

//{255,255,0  },//2  --rg         rgb  rg rb gb  

//{255,255,255  },//4 -- gb

//{255,255,0  },//5-b

//{255,0,0  },//2  --rb  

//{0,0,0  } ,//7--------------------------------



//};
 

 

u8 time3_PWM_ch3_init(void)  // ,时基配置  设置了ARR:1000  PB0
{	
	  u8 ret;
	 
		//开启时钟 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3,  ENABLE);

	
	
		//配置时基单元time3
		TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode =  TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_Prescaler =  72-1; //1M ->1*10^-6S(分频要减一)
		TIM_TimeBaseInitStruct.TIM_Period =  1000-1  ;    // (1*10^-6S)*1*10^3 -> 1ms（装载值要大于0，cn工作）
		
	TIM_TimeBaseInit(  TIM3, &TIM_TimeBaseInitStruct);
	TIM_ARRPreloadConfig( TIM3, ENABLE); // 时基单元3   开启 arr 预装载
 
		//配置 PB0 复用 
		GPIO_InitTypeDef  GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		//配置 比较输出
		TIM_OCInitTypeDef TIM_OCInitStruct;
		
		TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1; //向上计数 小于目标值 输出1
		TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStruct.TIM_Pulse = 0;//CCRx 比较值
			
		  //TIM_OCInitStruct.TIM_OCNIdleState = 高级定时器用
		   //TIM_OCInitStruct.TIM_OCIdleState = 高级定时器用 ;
		 //TIM_OCInitStruct.TIM_OutputNState =高级定时器用
		//TIM_OCInitStruct.TIM_OCNPolarity = 高级定时器用
	   TIM_OC3Init( TIM3, &TIM_OCInitStruct); 	// 配置  ccmr 模式寄存器 ，cce 使能寄存器  ，ccrx
	   TIM_OC3PreloadConfig( TIM3, TIM_OCPreload_Enable);  // 配置ccmrx_oc2pe 输出比较 开启 ccrx 预装载

	

		 TIM_Cmd( TIM3, ENABLE );
		
		
		ret =  0 ;

	 
		
		//ErrorInfo(timeX_PWM_chX_init error);
		//ret = 1;
	 	
		//TIM_SetCompare1(TIM3,  0);//配置比较值CCR1 ,             定时器配置时候 设置了ARR:1000
		//TIM_CCxCmd( TIM3,TIM_Channel_0,  TIM_CCx_Enable); //配置CCER使能寄存器 的输出使能
return ret;	
}




 void time4_PWM_ch1to3_Switch(u8 on_off)   //
 {
	if (on_off )
	{
	 TIM_CCxCmd( TIM4,TIM_Channel_1,  TIM_CCx_Enable);
	TIM_CCxCmd( TIM4,TIM_Channel_2,  TIM_CCx_Enable);
	TIM_CCxCmd( TIM4,TIM_Channel_3,  TIM_CCx_Enable);
	 
	 }else
	{
	
	 TIM_CCxCmd( TIM4,TIM_Channel_1,  TIM_CCx_Disable);
	TIM_CCxCmd( TIM4,TIM_Channel_2,  TIM_CCx_Disable);
	TIM_CCxCmd( TIM4,TIM_Channel_3,  TIM_CCx_Disable);
	}

}


 void time4_PWM_ch1to3_init(void)   //完全重映射 pd12-13-14,,,,,,,,,, 1ms周期
{
	//开启时钟
	RCC_APB2PeriphClockCmd(  RCC_APB2Periph_GPIOD,   ENABLE);
	RCC_APB2PeriphClockCmd(  RCC_APB2Periph_AFIO,   ENABLE);
	RCC_APB1PeriphClockCmd(  RCC_APB1Periph_TIM4,   ENABLE);
	
	//配置gpio_d
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(  GPIOD,   &GPIO_InitStruct);
	//配置afio 重映射
	GPIO_PinRemapConfig( GPIO_Remap_TIM4,  ENABLE);
	
	//配置时基4
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; //72M
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;// 1M
	TIM_TimeBaseInitStruct.TIM_Period = 1000-1;//  1K,1MS
	TIM_TimeBaseInit( TIM4,  &TIM_TimeBaseInitStruct);
	
	//配置时基4 其他，预装载
    TIM_ARRPreloadConfig( TIM4,  ENABLE);
	
	//配置oc 输出比较
	 TIM_OCInitTypeDef  TIM_OCInitStruct;
	 TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	 TIM_OCInitStruct.TIM_OCPolarity =TIM_OCPolarity_High;
	 TIM_OCInitStruct.TIM_OutputState =TIM_OutputState_Disable;
	 TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC1Init(  TIM4,  &TIM_OCInitStruct);
	TIM_OC2Init(  TIM4, &TIM_OCInitStruct);
	TIM_OC3Init(  TIM4, &TIM_OCInitStruct);
	
	////配置oc 输出比较，其他 ，预装载
	  TIM_OC1PreloadConfig(  TIM4,   TIM_OCPreload_Enable);
	  TIM_OC2PreloadConfig(  TIM4,   TIM_OCPreload_Enable);
	  TIM_OC3PreloadConfig(  TIM4,   TIM_OCPreload_Enable);
	  
	 //使能时间基本单元4
	 TIM_Cmd(TIM4,ENABLE);
	 
	
	 
}

 u16 convert_255To999(u8 color)
{

	return color*(999.0)/255.0;

}

void show_RGB_LED_3Channel( RGBcolor_t RGBcolor) //设置 ccrX  与 时间基本单位aar 比较 
{
	

	 TIM_SetCompare1(TIM4,   convert_255To999( RGBcolor.R  )  );//最大1000-1
	TIM_SetCompare2(TIM4,  convert_255To999(RGBcolor.G  )  );
	TIM_SetCompare3(TIM4,   convert_255To999(  RGBcolor.B)  );
	
	
}

void show_RGB_LED_3Channel_colorBuf_sequence( RGBcolor_t* p_RGBcolor_t,  u8 count)
{
	static u8 i = 0,buf_index;
	 static u8 dirUp = 1;
 
	if(dirUp == 1) 
	{
		i++;
		if(i > count ) //到达外界  //1 2 3 4 5 6 7
		{
			dirUp = 0;
			i = count-1;
		}
		
	}else{
		i--;
		if(i < 1 ) ////到达外界  //1 2 3 4 5 6 7
		{
			dirUp = 1;
			i = 2;
		}
	}
	
	buf_index = i-1;
	
    show_RGB_LED_3Channel( p_RGBcolor_t[buf_index]) ;

	//printf("index:%d,R:%d,G:%d,B:%d\r\n",buf_index,p_RGBcolor_t[buf_index].R,p_RGBcolor_t[buf_index].G,p_RGBcolor_t[buf_index].B); //调试

}


void show_RGB_LED_3Channel_colorBuf_sequence_gradient( RGBcolor_t* p_RGBcolor_t,  u8 count) //渐变色 序列
{	
	static u8 compare_now,compare_next;
	static s8 i = -1; //保存 上一次结果
	 static u8 now_group_R_val ,now_group_G_val ,now_group_B_val; //保存当前组 RGB 的 val
	 static u8 dirUp = 1; //保存 当前方向
	static u8 flag_finish_one_group_gradient_task = 1; //保存 渐变色 任务
 
	
	 
	
	if(flag_finish_one_group_gradient_task == 1)
	{
		flag_finish_one_group_gradient_task = 0;// 复位
		
		
		
	if(dirUp == 1) // //得出 当前 index  与渐变 目标index
	{
		i++;
		if(i >= count ) //到达外界  //0 1 2 3 4 5 6 
		{
			dirUp = 0;
			i = count-2;
			
			compare_now = i;
		    compare_next = i-1;
			
		}else
		{
		   if( i == count -1 ) //到达最后一个有效下标
		   {
			   compare_now = i;
		      compare_next = count -2;
		   }else
		   {
			compare_now = i;
		    compare_next = i+1;
		   }
		}
		
	}else{
		i--;
		if(i <= -1 ) ////到达外界  //0 1 2 3 4 5 6  
		{
			dirUp = 1;
			i = 1;
			
			compare_now = i;
		    compare_next = i+1;
			
		}else
		 {
		   if( i == 0 ) //到达最前 一个 有效下标
		   {
			  compare_now = i;
		      compare_next = i+1;
			   
		   }else
		   {
			compare_now = i;
		    compare_next = i-1;
		   } 
		 }
    	}
	
	now_group_R_val = p_RGBcolor_t[compare_now].R;//获得初始 比较值 
	now_group_G_val = p_RGBcolor_t[compare_now].G;//获得初始 比较值 
	now_group_B_val = p_RGBcolor_t[compare_now].B;//获得初始 比较值 
	}
			
	
   //  根据 比较RGB 两组数据，  开始值 渐变 到目标值 ， 并写入 timx_ocx   ccrx寄存器,完成退出，到下一个颜色组

			if( now_group_R_val != p_RGBcolor_t[compare_next].R )
			{	
				 
				if(now_group_R_val <  p_RGBcolor_t[compare_next].R )
				{
					now_group_R_val++;
				}else
				{
					now_group_R_val--;
				}
				
				 TIM_SetCompare1(TIM4,   convert_255To999( now_group_R_val  )  );//最大1000-1

				 //	printf("index:%d,R:%d,G:%d,B:%d\r\n",compare_now, now_group_R_val , now_group_G_val  , now_group_B_val  ); //调试
				
			
	
			}
		
		
		   if(  now_group_G_val != p_RGBcolor_t[compare_next].G   )
			{	
				if(now_group_G_val <  p_RGBcolor_t[compare_next].G )
				{
					now_group_G_val++;
				}else
				{
					now_group_G_val--;
				}
			 
				TIM_SetCompare2(TIM4,  convert_255To999(now_group_G_val )  );//最大1000-1
				
               // printf("index:%d,R:%d,G:%d,B:%d\r\n",compare_now, p_RGBcolor_t[compare_now].R, now_group_G_val , p_RGBcolor_t[compare_now].B ); //调试
		
				
			}
		
		   
			if( now_group_B_val != p_RGBcolor_t[compare_next].B   )
			{	
				if(now_group_B_val <  p_RGBcolor_t[compare_next].B )
				{
					now_group_B_val++;
				}else
				{
					 now_group_B_val--;
				}
				 
		
				TIM_SetCompare3(TIM4,   convert_255To999(   now_group_B_val )  );
				
			 //	printf("index:%d,R:%d,G:%d,B:%d\r\n",compare_now, p_RGBcolor_t[compare_now].R,p_RGBcolor_t[compare_now].G,now_group_B_val); //调试
			 	
		
			} 
			
				// 是否RGB 都完成 渐变了
				if((now_group_R_val == p_RGBcolor_t[compare_next].R)&&(now_group_G_val == p_RGBcolor_t[compare_next].G)&&( now_group_B_val == p_RGBcolor_t[compare_next].B) )
				{
						flag_finish_one_group_gradient_task = 1;//RGB 都完成 渐变
				}
	
	
	
	  //   printf("R:%d,G:%d,B:%d___index:%d\r\n", now_group_R_val , now_group_G_val  , now_group_B_val,compare_now  ); //调试OK
	

}





 void time2_PWM_ch2_pa1_init(void)   //PA1,已经配置了时基tim2,  并设置了ARR:1000，时间：1*10^-6  *1*10^3 = 1*10^-3 = 1ms    
{
	
	//开启时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	
	//配置pa1 复用
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStruct);
	
	//配置比较输出(不使能)
	TIM_OCInitTypeDef  TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCMode =TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity =TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState =TIM_OutputState_Disable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC2Init(TIM2, &TIM_OCInitStruct);
	
	//使能 预装载
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	//使能比较输出引脚
	TIM_CCxCmd( TIM2,TIM_Channel_2,  TIM_CCx_Enable);
	
}
































