#include "stepper_motor.h"
#include "stm32f10x.h"
#include "systick.h"
#include "stdio.h"

		/*
		
		//65535最大，65535us =  65ms(15hz) 经过gpio翻转波形验证 是65ms
			牵入频率500Hz(频率太大的话启动不了)
	牵出频率800hz (工作时候最大的速度)
		// 启动100HZ -> 10000us
		
		
		*/
		
		

#define ULN2003_SM_GPIO       GPIOF
#define ON_A 	 GPIO_SetBits( ULN2003_SM_GPIO,   GPIO_Pin_0 )
#define ON_B	 GPIO_SetBits( ULN2003_SM_GPIO,   GPIO_Pin_1 )
#define ON_C	 GPIO_SetBits( ULN2003_SM_GPIO,   GPIO_Pin_2 )
#define ON_D	 GPIO_SetBits( ULN2003_SM_GPIO,   GPIO_Pin_3 )

#define OFF_A	      GPIO_ResetBits( ULN2003_SM_GPIO  ,   GPIO_Pin_0 )
#define OFF_B		  GPIO_ResetBits( ULN2003_SM_GPIO  ,   GPIO_Pin_1 )
#define OFF_C		  GPIO_ResetBits( ULN2003_SM_GPIO  ,   GPIO_Pin_2 )
#define OFF_D			  GPIO_ResetBits( ULN2003_SM_GPIO  ,   GPIO_Pin_3 )
 

u32 gStepperMotorInputNstep; //全局变量 步进电机目标步数
u8 gStepperMotorInput_dir; //全局变量 步进电机目标步数方向
u16 gStepperMotorInput_usdelay_16bit; //全局变量 步进电机目标步数  延时，速度
u16 gStepperMotorInput_minHz =  1000000/0XFFFF;//1us 计数频率即1M

void gpio_output_steperMotor_Uln2003_init(void)	 //PF0_1_2_3
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	 
    GPIO_Init(  ULN2003_SM_GPIO,  & GPIO_InitStruct);
	
	 
	
	
}


void steperMotor_Uln2003_outputClose(void)	
{

  OFF_A ;
  OFF_B	;
  OFF_C	;
  OFF_D	;
	
}






void steperMotor_Uln2003_OneStep(u8 dir)	 
{
	static u8 step_memory  = 0;
/*
	ABCD线圈公阳极,Uln2003输入反向，所以in1 = 1,out1 = 0, A线圈 得电
	8步：  
 ON_A;	OFF_B;	OFF_C;	OFF_D;
 ON_A; 	ON_B;	OFF_C;	OFF_D;
OFF_A;	ON_B;	OFF_C;	OFF_D;
OFF_A;	ON_B;	ON_C;	OFF_D;
OFF_A;	OFF_B;	ON_C;	OFF_D;
OFF_A;	OFF_B;	ON_C;	ON_D;
OFF_A;	OFF_B;	OFF_C;	ON_D;
ON_A;	OFF_B;	OFF_C;	ON_D;
*/
	
	//printf("step_memory:%d\r\n",step_memory);调试OK
	
	switch(step_memory)
	{
		case 0:
		{
			 ON_A;	OFF_B;	OFF_C;	OFF_D;
			break;
		}
		case 1:
		{
			 ON_A; 	ON_B;	OFF_C;	OFF_D;
			break;
		}
		case 2:
		{
			OFF_A;	ON_B;	OFF_C;	OFF_D;
			break;
		}
		case 3:
		{
			OFF_A;	ON_B;	ON_C;	OFF_D;
			break;
		}
		case 4:
		{
			OFF_A;	OFF_B;	ON_C;	OFF_D;
			break;
		}
		case 5:
		{
			OFF_A;	OFF_B;	ON_C;	ON_D;
			break;
		}
		case 6:
		{
			OFF_A;	OFF_B;	OFF_C;	ON_D;
			break;
		}
		case 7:
		{
			ON_A;	OFF_B;	OFF_C;	ON_D;
			break;	
		}

		default:
			break;

	}

	if(dir )//正向
	{
	step_memory++;
	if(step_memory >7) step_memory = 0;
	}else
	{
	
		if(step_memory == 0)
		{
		step_memory = 7;
		}else
		{
		step_memory--;
		
		}
	
	}


}

void steperMotor_Uln2003_run_timerUp_interrupt_init(void)	  //定时器7 定时中断 执行步进电机步数
{	
	//开启时钟
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct ;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;   //1us 计数频率,,72M /72 = 1M -> 计数频率1M 时间1us
	TIM_TimeBaseInitStruct.TIM_Period =    1000-1;              //65535最大，65535us =  65ms(15hz) 经过gpio翻转波形验证 是65ms
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1; ;// 72-1;
	
	TIM_TimeBaseInit( TIM7, &TIM_TimeBaseInitStruct);
	

	//中断管理
	NVIC_InitTypeDef  NVIC_InitStruct; 
	NVIC_InitStruct.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(& NVIC_InitStruct);
	
	TIM_ARRPreloadConfig(  TIM7,  ENABLE);//预装载 为了更平滑//取消预装载，立即更新重载值

	
	 //清除中断标志位
 	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
 
 	//开启中断
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
	
	//定时器使能
	TIM_Cmd(TIM7,DISABLE); //不使能
}



u16 StepForSpeed_algorithm(u32 now_step,u32 step_require)	  //
{
	const u16 slowest_usdelay = 0xffff;//15hz  1us计数频率推到来源
	const u16 fastest_usdelay = 1250;//最高频率800hz   ，1000 000/x Hz 
	u32 now_step_usdelay;
	
	//
	if(now_step<=50) //小于 度即 内， (1024是90度)
	{
	
	now_step_usdelay = slowest_usdelay  - (64285)*(   (1.0*now_step)/50  );    //65535 - 1250最高 = 64,285,  64285 = 125 *512
	
	}else if(now_step+50 >= step_require   )//接近目标 度时候
	{
	
	now_step_usdelay = fastest_usdelay +  64285 * ((1.0*now_step)/step_require)  ;    //1250最高 + 64,285= 65535,
	
	}else
	{
	now_step_usdelay = fastest_usdelay;
	}
	

return  now_step_usdelay;

}
	


void steperMotor_Uln2003_run_timerUpINT_callBack(void)	  //定时器7 定时中断 执行步进电机步数
{
	
static u16 old_usdelay  = 0;
static u32 now_task_setp_i  = 0;
static u32 require_step = 0;
static u8 now_task_finsh_flag = 0;	
	
    // gStepperMotorInputNstep 是 全局变量 步进电机目标步数
	
	//gStepperMotorInput_dir
	
	//载入步进电机 启动速度 定时器arr
	//1us 计数频率,启动100HZ -> 10000us
	//TIM_SetAutoreload( TIM7,  2000);//500HZ
	//65535最大，65535us =  65ms(15hz) 
	if(require_step == 0) { require_step = gStepperMotorInputNstep;}

	
	if(now_task_finsh_flag == 1) //执行完 
	{	
		now_task_finsh_flag = 0;//复位
		
		TIM_Cmd(TIM7,DISABLE); //执行完  停止定时器 计时
		
			require_step = 0; ////  执行完了，  复位
		
		 steperMotor_Uln2003_outputClose( );	//执行完 关闭线圈
		now_task_setp_i = 0;//执行完  复位计数步数
		gStepperMotorInputNstep = 0; //执行完  复位要求的步数
		 
		
	}  if(require_step > 0){ //还未执行完 
		
			if(require_step == 1)
			{ 
			  
				now_task_finsh_flag = 1; //完成 即将标志位
			}else
			{
				require_step --;
			}  
			
			
	 	now_task_setp_i++;
			
		gStepperMotorInput_usdelay_16bit = StepForSpeed_algorithm(  now_task_setp_i, gStepperMotorInputNstep);	  //
				
		if( gStepperMotorInput_usdelay_16bit  != old_usdelay )//更新了 延时时间 ，就写ARR寄存器
		{	
			old_usdelay  = gStepperMotorInput_usdelay_16bit;
			TIM_SetAutoreload(TIM7,gStepperMotorInput_usdelay_16bit);//为了不要频繁且无意义的写入
		}
		
		
		
	 steperMotor_Uln2003_OneStep( gStepperMotorInput_dir);	 
	
	
	}
	
	//printf("in--------steperMotor_Uln2003_run_timerUpINT_callBack\r\n");
	
	

	
	
}









void steperMotor_Uln2003_run_Nstep_Blocking (u32 Nstep,u8 dir,u16 us_delay_speed)	// //  1250 MS = 800HZ ,2000MS = 500HZ, 10000 MS=1OOHZ    
{
/*28BYJ-48
通过关闭延时，改变转速
不宜过快或者过慢
完成步数后 关闭线圈输出，减少发热
	
	牵入频率500Hz(频率太大的话启动不了)
	牵出频率800hz (工作时候最大的速度)
	
	步距角  5.625/64
	
	提示;步进电机启动时用低频率，之后用高频率工作
	90度 1024
	
	
*/
	
	for( u32 i = 0;i < Nstep;i++)
	{

	 steperMotor_Uln2003_OneStep( dir ); 
		
		
	 sysTickDelay_us_Block( us_delay_speed) ;   // //  1250 MS = 800HZ ,2000MS = 500HZ, 10000 MS=1OOHZ  
													
	}

	  //关闭线圈输出， 
	 steperMotor_Uln2003_outputClose( );	//执行完 关闭线圈
	
}



void steperMotor_Uln2003_run_Angle_StartTask(u32 angle,u8 dir)//
{

    
	
	gStepperMotorInputNstep = (angle * 64.0)/5.625 ;//全局 变量  计算需要的步数
	gStepperMotorInput_dir = dir; 

	TIM_Cmd(TIM7,ENABLE); //使能
	

}




