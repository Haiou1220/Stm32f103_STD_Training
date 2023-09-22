#include "stepper_motor.h"
#include "stm32f10x.h"
#include "systick.h"
#include "stdio.h"

		/*
		
		//65535���65535us =  65ms(15hz) ����gpio��ת������֤ ��65ms
			ǣ��Ƶ��500Hz(Ƶ��̫��Ļ���������)
	ǣ��Ƶ��800hz (����ʱ�������ٶ�)
		// ����100HZ -> 10000us
		
		
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
 

u32 gStepperMotorInputNstep; //ȫ�ֱ��� �������Ŀ�경��
u8 gStepperMotorInput_dir; //ȫ�ֱ��� �������Ŀ�경������
u16 gStepperMotorInput_usdelay_16bit; //ȫ�ֱ��� �������Ŀ�경��  ��ʱ���ٶ�
u16 gStepperMotorInput_minHz =  1000000/0XFFFF;//1us ����Ƶ�ʼ�1M

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
	ABCD��Ȧ������,Uln2003���뷴������in1 = 1,out1 = 0, A��Ȧ �õ�
	8����  
 ON_A;	OFF_B;	OFF_C;	OFF_D;
 ON_A; 	ON_B;	OFF_C;	OFF_D;
OFF_A;	ON_B;	OFF_C;	OFF_D;
OFF_A;	ON_B;	ON_C;	OFF_D;
OFF_A;	OFF_B;	ON_C;	OFF_D;
OFF_A;	OFF_B;	ON_C;	ON_D;
OFF_A;	OFF_B;	OFF_C;	ON_D;
ON_A;	OFF_B;	OFF_C;	ON_D;
*/
	
	//printf("step_memory:%d\r\n",step_memory);����OK
	
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

	if(dir )//����
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

void steperMotor_Uln2003_run_timerUp_interrupt_init(void)	  //��ʱ��7 ��ʱ�ж� ִ�в����������
{	
	//����ʱ��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct ;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;   //1us ����Ƶ��,,72M /72 = 1M -> ����Ƶ��1M ʱ��1us
	TIM_TimeBaseInitStruct.TIM_Period =    1000-1;              //65535���65535us =  65ms(15hz) ����gpio��ת������֤ ��65ms
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1; ;// 72-1;
	
	TIM_TimeBaseInit( TIM7, &TIM_TimeBaseInitStruct);
	

	//�жϹ���
	NVIC_InitTypeDef  NVIC_InitStruct; 
	NVIC_InitStruct.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(& NVIC_InitStruct);
	
	TIM_ARRPreloadConfig(  TIM7,  ENABLE);//Ԥװ�� Ϊ�˸�ƽ��//ȡ��Ԥװ�أ�������������ֵ

	
	 //����жϱ�־λ
 	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
 
 	//�����ж�
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
	
	//��ʱ��ʹ��
	TIM_Cmd(TIM7,DISABLE); //��ʹ��
}



u16 StepForSpeed_algorithm(u32 now_step,u32 step_require)	  //
{
	const u16 slowest_usdelay = 0xffff;//15hz  1us����Ƶ���Ƶ���Դ
	const u16 fastest_usdelay = 1250;//���Ƶ��800hz   ��1000 000/x Hz 
	u32 now_step_usdelay;
	
	//
	if(now_step<=50) //С�� �ȼ� �ڣ� (1024��90��)
	{
	
	now_step_usdelay = slowest_usdelay  - (64285)*(   (1.0*now_step)/50  );    //65535 - 1250��� = 64,285,  64285 = 125 *512
	
	}else if(now_step+50 >= step_require   )//�ӽ�Ŀ�� ��ʱ��
	{
	
	now_step_usdelay = fastest_usdelay +  64285 * ((1.0*now_step)/step_require)  ;    //1250��� + 64,285= 65535,
	
	}else
	{
	now_step_usdelay = fastest_usdelay;
	}
	

return  now_step_usdelay;

}
	


void steperMotor_Uln2003_run_timerUpINT_callBack(void)	  //��ʱ��7 ��ʱ�ж� ִ�в����������
{
	
static u16 old_usdelay  = 0;
static u32 now_task_setp_i  = 0;
static u32 require_step = 0;
static u8 now_task_finsh_flag = 0;	
	
    // gStepperMotorInputNstep �� ȫ�ֱ��� �������Ŀ�경��
	
	//gStepperMotorInput_dir
	
	//���벽����� �����ٶ� ��ʱ��arr
	//1us ����Ƶ��,����100HZ -> 10000us
	//TIM_SetAutoreload( TIM7,  2000);//500HZ
	//65535���65535us =  65ms(15hz) 
	if(require_step == 0) { require_step = gStepperMotorInputNstep;}

	
	if(now_task_finsh_flag == 1) //ִ���� 
	{	
		now_task_finsh_flag = 0;//��λ
		
		TIM_Cmd(TIM7,DISABLE); //ִ����  ֹͣ��ʱ�� ��ʱ
		
			require_step = 0; ////  ִ�����ˣ�  ��λ
		
		 steperMotor_Uln2003_outputClose( );	//ִ���� �ر���Ȧ
		now_task_setp_i = 0;//ִ����  ��λ��������
		gStepperMotorInputNstep = 0; //ִ����  ��λҪ��Ĳ���
		 
		
	}  if(require_step > 0){ //��δִ���� 
		
			if(require_step == 1)
			{ 
			  
				now_task_finsh_flag = 1; //��� ������־λ
			}else
			{
				require_step --;
			}  
			
			
	 	now_task_setp_i++;
			
		gStepperMotorInput_usdelay_16bit = StepForSpeed_algorithm(  now_task_setp_i, gStepperMotorInputNstep);	  //
				
		if( gStepperMotorInput_usdelay_16bit  != old_usdelay )//������ ��ʱʱ�� ����дARR�Ĵ���
		{	
			old_usdelay  = gStepperMotorInput_usdelay_16bit;
			TIM_SetAutoreload(TIM7,gStepperMotorInput_usdelay_16bit);//Ϊ�˲�ҪƵ�����������д��
		}
		
		
		
	 steperMotor_Uln2003_OneStep( gStepperMotorInput_dir);	 
	
	
	}
	
	//printf("in--------steperMotor_Uln2003_run_timerUpINT_callBack\r\n");
	
	

	
	
}









void steperMotor_Uln2003_run_Nstep_Blocking (u32 Nstep,u8 dir,u16 us_delay_speed)	// //  1250 MS = 800HZ ,2000MS = 500HZ, 10000 MS=1OOHZ    
{
/*28BYJ-48
ͨ���ر���ʱ���ı�ת��
���˹�����߹���
��ɲ����� �ر���Ȧ��������ٷ���
	
	ǣ��Ƶ��500Hz(Ƶ��̫��Ļ���������)
	ǣ��Ƶ��800hz (����ʱ�������ٶ�)
	
	�����  5.625/64
	
	��ʾ;�����������ʱ�õ�Ƶ�ʣ�֮���ø�Ƶ�ʹ���
	90�� 1024
	
	
*/
	
	for( u32 i = 0;i < Nstep;i++)
	{

	 steperMotor_Uln2003_OneStep( dir ); 
		
		
	 sysTickDelay_us_Block( us_delay_speed) ;   // //  1250 MS = 800HZ ,2000MS = 500HZ, 10000 MS=1OOHZ  
													
	}

	  //�ر���Ȧ����� 
	 steperMotor_Uln2003_outputClose( );	//ִ���� �ر���Ȧ
	
}



void steperMotor_Uln2003_run_Angle_StartTask(u32 angle,u8 dir)//
{

    
	
	gStepperMotorInputNstep = (angle * 64.0)/5.625 ;//ȫ�� ����  ������Ҫ�Ĳ���
	gStepperMotorInput_dir = dir; 

	TIM_Cmd(TIM7,ENABLE); //ʹ��
	

}




