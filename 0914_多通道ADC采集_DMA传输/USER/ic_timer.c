#include "ic_timer.h"
#include "stm32f10x.h"

//
void gpio_pe0_input_ForEncoderB_init(void)
{
	
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);//pE0

		//配置gpio 
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;// 复用,输入要浮空
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(  GPIOE, & GPIO_InitStruct);

}

void input_captrue_tim4Ch4_pb9_ForEncoderA_init(void) //定时器4 输入捕获 通道 PB9 TIM4_CH4
{
	
/*
编码器AB 都是下降沿有效,捕获是A通道，，  
顺时针 B A       ,   A的下降沿 B 是低电平    
逆时针 A B      ,      A的下降沿 B 是高电平   

*/	
	//开启时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//pb9

		//配置gpio 
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;// 复用,输入要浮空
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(  GPIOB, & GPIO_InitStruct);
	
	
	//配置时基YIM4,,  CCR4为捕获时刻的 计数值
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
               //1ms 频率= 1000Hz,72MHZ/1000HZ=72000;(72*1000)
	
	TIM_TimeBaseInitStruct.TIM_Period =    1000-1;        //当自动重装载的值为空时，计数器不工作。计数器从0计数到自动加载值(TIMx_ARR计数器的内容)，因为包含了0，所以要减一
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;         //ck_nt = 72M/72 = 1MHZ,
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; //采样频率不分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM4,   &TIM_TimeBaseInitStruct);
	
	//配置 定时器4通道4 输入捕获
	TIM_ICInitTypeDef  TIM_ICInitStruct;
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_4; 
	TIM_ICInitStruct.TIM_ICFilter = 2;                 //滤波2： 0010：采样频率fSAMPLING=fCK_INT， N=4 ，滤波3： 0011：采样频率fSAMPLING=fCK_INT， N=8， //ck_nt = 72M/72 = 1MHZ
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;// 下降沿 TIM_ICPolarity_Rising;//上升沿捕获  ///  捕获比较使能寄存器CCER，通道 捕获极性 上升沿
	TIM_ICInitStruct.TIM_ICPrescaler =  TIM_ICPSC_DIV1;		//采样不分频
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;//直接映射
	
	TIM_ICInit(TIM4, &TIM_ICInitStruct);
	
	  //使能中断，不使能 dma
	 TIM_ITConfig( TIM4,   TIM_IT_CC4, ENABLE );
	
		 	
	//开启中断管理
	NVIC_InitTypeDef  NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority =1;
	NVIC_Init(&NVIC_InitStruct);
	
	
	
	TIM_CCxCmd(TIM4,TIM_Channel_4,TIM_CCx_Enable);        ///  捕获比较使能寄存器CCER，使能通道
	
	//开启定时器计数
	TIM_Cmd(TIM4,ENABLE);

}


void  EncoderAB_init(void) // 
{

  gpio_pe0_input_ForEncoderB_init( );

  input_captrue_tim4Ch4_pb9_ForEncoderA_init( ); 


}
	













u8 read_pB9_output(void)
{
 
 
	
	return 0;
}




