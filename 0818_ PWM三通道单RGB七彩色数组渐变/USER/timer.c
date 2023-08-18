#include "timer.h"
#include "stm32f10x.h"


void  timer2_countdown_init(void)        //���ö�ʱ��2��ʱ1ms�ж�
{
	//�򿪶�ʱ��2ʱ��
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2,  ENABLE);
	//���ö�ʱ��2
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode =  TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_Period =  1000  ;    // 1*10-3 -> 1ms��װ��ֵҪ����0��cn������
		TIM_TimeBaseInitStruct.TIM_Prescaler =  72-1; //1M ->1*10-6(��ƵҪ��һ)
	TIM_TimeBaseInit(  TIM2, &TIM_TimeBaseInitStruct);
	
	TIM_ARRPreloadConfig( TIM2, ENABLE); // ʱ����Ԫ2    ���� arr Ԥװ��
	
	//���ö�ʱ��2�ж�
	TIM_ITConfig(  TIM2,  TIM_IT_Update,  ENABLE);
	//��ʱ��2�ж����ȼ�
	NVIC_InitTypeDef    NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel =  TIM2_IRQn; 
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =  2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(  &NVIC_InitStruct);
 	//��ʱ��2 ʹ��
 	TIM_Cmd(  TIM2,  ENABLE);

 
}



