#include "pwm_servo.h"
#include "stm32f10x.h"
#include "stdio.h"
/*


SG90 �����������
����90�Ƚ�   20ms���ڣ�  1.5ms��׼��    1.5ms�Ӽ�1ms ��Ӧ 0�ȼӼ�90��
��ʱ��5
ʹ�ã�TIM5_CH2 pa1,   ��ʹ��---����PA0-WKUP��TIM5_CH1 pa0,          ����2��TIM5_CH3 pa2,TIM5_CH4 pa3

//TIM6TIM7 �ǻ�����ʱ��
20MS = 50 hz,72M hz/1440K hz = 50hz,,,,,,,,��Ҫ1440K��Ƶ->1000*1440

1000->20ms  ==>>1ms==50��λ,,,,,      0.5ms==25��λ    2.5ms==125��λ,,,,,
*/

u16 gServo_setAngle = 90;//ȫ�ֱ��� Ĭ�϶���Ƕ�

void Servo_tim5ch2pa1_init(void) //
{
	//��ʱ��3ʱ�ӿ�����gpio��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	//GPIO ���ã�����
	GPIO_InitTypeDef  GPIO_InitStruct ; 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//��ʱ��3����ʱ��
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 1440-1;
	TIM_TimeBaseInitStruct.TIM_Period = 1000-1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStruct);//50HZ����20ms
	
	//���ñȽ����CH2
	 TIM_OCInitTypeDef  TIM_OCInitStruct;
	 TIM_OCInitStruct.TIM_OCMode =  TIM_OCMode_PWM1;//���ϼ������ڱȽ�ֵ �����Ч��ƽ,,,, // TIM_OCIdleState �߼���ʱ��ʹ��
	 TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//��Ч��ƽ
	 TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Disable;//-------------�ر�ͨ��
	 TIM_OCInitStruct.TIM_Pulse = 0;//��СΪ0
	
     TIM_OC2Init( TIM5,   &TIM_OCInitStruct);
	 
	 //����Ԥװ��
	 TIM_ARRPreloadConfig(TIM5,ENABLE);
	 TIM_OC2PreloadConfig(TIM5,TIM_OCPreload_Enable);
	 
	 
	 //��ʹ��DMA����
	
	//--------------------�رն�ʱ��
	TIM_Cmd(TIM5,DISABLE);
	
}


void Servo_tim5ch2pa1_setAngle(u16 angle) //
{
	u16 compare;
	if(angle>180) angle = 180;
	
	//0-180�ȣ�1000->20ms  ==>>1ms==50��λ,,,,,    ����  0.5ms==25��λ    2.5ms==125��λ,,,,,
	//�������� ʵ�� ��С25->0.5ms      ���122->2.44ms
	   
		compare = (angle /180.0)*(122-25) + 25;        
	  
	    //����ͨ��2�ıȽ�ֵ
		TIM_SetCompare2(TIM5,compare); // 	compare = 1000 ;ռ�ձ�100,,,;;;compare = 1000-1; ռ�ձ�99.9,�����÷ǼĴ���װ��ֵ��������ȷ��
	    //��������Ƚ�ͨ��2
		TIM_CCxCmd(TIM5,TIM_Channel_2,TIM_CCx_Enable);
		//������ʱ��5
		TIM_Cmd(TIM5,ENABLE);
	



}	




