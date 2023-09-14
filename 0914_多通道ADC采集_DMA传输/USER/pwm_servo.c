#include "pwm_servo.h"
#include "stm32f10x.h"
#include "stdio.h"
/*


SG90 舵机驱动程序
正负90度角   20ms周期，  1.5ms基准，    1.5ms加减1ms 对应 0度加减90度
定时器5
使用：TIM5_CH2 pa1,   不使用---》》PA0-WKUP：TIM5_CH1 pa0,          串口2：TIM5_CH3 pa2,TIM5_CH4 pa3

//TIM6TIM7 是基本定时器
20MS = 50 hz,72M hz/1440K hz = 50hz,,,,,,,,需要1440K分频->1000*1440

1000->20ms  ==>>1ms==50单位,,,,,      0.5ms==25单位    2.5ms==125单位,,,,,
*/

u16 gServo_setAngle = 90;//全局变量 默认舵机角度

void Servo_tim5ch2pa1_init(void) //
{
	//定时器3时钟开启，gpio口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	//GPIO 配置，复用
	GPIO_InitTypeDef  GPIO_InitStruct ; 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//定时器3配置时基
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 1440-1;
	TIM_TimeBaseInitStruct.TIM_Period = 1000-1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStruct);//50HZ既是20ms
	
	//配置比较输出CH2
	 TIM_OCInitTypeDef  TIM_OCInitStruct;
	 TIM_OCInitStruct.TIM_OCMode =  TIM_OCMode_PWM1;//向上计数大于比较值 输出有效电平,,,, // TIM_OCIdleState 高级定时器使用
	 TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平
	 TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Disable;//-------------关闭通道
	 TIM_OCInitStruct.TIM_Pulse = 0;//最小为0
	
     TIM_OC2Init( TIM5,   &TIM_OCInitStruct);
	 
	 //配置预装载
	 TIM_ARRPreloadConfig(TIM5,ENABLE);
	 TIM_OC2PreloadConfig(TIM5,TIM_OCPreload_Enable);
	 
	 
	 //不使用DMA请求
	
	//--------------------关闭定时器
	TIM_Cmd(TIM5,DISABLE);
	
}


void Servo_tim5ch2pa1_setAngle(u16 angle) //
{
	u16 compare;
	if(angle>180) angle = 180;
	
	//0-180度，1000->20ms  ==>>1ms==50单位,,,,,    理论  0.5ms==25单位    2.5ms==125单位,,,,,
	//经过测试 实际 最小25->0.5ms      最大122->2.44ms
	   
		compare = (angle /180.0)*(122-25) + 25;        
	  
	    //设置通道2的比较值
		TIM_SetCompare2(TIM5,compare); // 	compare = 1000 ;占空比100,,,;;;compare = 1000-1; 占空比99.9,所以用非寄存器装载值计算是正确的
	    //开启输出比较通道2
		TIM_CCxCmd(TIM5,TIM_Channel_2,TIM_CCx_Enable);
		//开启定时器5
		TIM_Cmd(TIM5,ENABLE);
	



}	




