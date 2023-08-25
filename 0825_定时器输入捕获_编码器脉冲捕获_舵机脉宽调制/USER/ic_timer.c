#include "ic_timer.h"
#include "stm32f10x.h"

//
void gpio_pe0_input_ForEncoderB_init(void)
{
	
	//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);//pE0

		//����gpio 
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;// ����,����Ҫ����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(  GPIOE, & GPIO_InitStruct);

}

void input_captrue_tim4Ch4_pb9_ForEncoderA_init(void) //��ʱ��4 ���벶�� ͨ�� PB9 TIM4_CH4
{
	
/*
������AB �����½�����Ч,������Aͨ������  
˳ʱ�� B A       ,   A���½��� B �ǵ͵�ƽ    
��ʱ�� A B      ,      A���½��� B �Ǹߵ�ƽ   

*/	
	//����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//pb9

		//����gpio 
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;// ����,����Ҫ����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(  GPIOB, & GPIO_InitStruct);
	
	
	//����ʱ��YIM4,,  CCR4Ϊ����ʱ�̵� ����ֵ
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
               //1ms Ƶ��= 1000Hz,72MHZ/1000HZ=72000;(72*1000)
	
	TIM_TimeBaseInitStruct.TIM_Period =    1000-1;        //���Զ���װ�ص�ֵΪ��ʱ������������������������0�������Զ�����ֵ(TIMx_ARR������������)����Ϊ������0������Ҫ��һ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;         //ck_nt = 72M/72 = 1MHZ,
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; //����Ƶ�ʲ���Ƶ
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM4,   &TIM_TimeBaseInitStruct);
	
	//���� ��ʱ��4ͨ��4 ���벶��
	TIM_ICInitTypeDef  TIM_ICInitStruct;
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_4; 
	TIM_ICInitStruct.TIM_ICFilter = 2;                 //�˲�2�� 0010������Ƶ��fSAMPLING=fCK_INT�� N=4 ���˲�3�� 0011������Ƶ��fSAMPLING=fCK_INT�� N=8�� //ck_nt = 72M/72 = 1MHZ
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;// �½��� TIM_ICPolarity_Rising;//�����ز���  ///  ����Ƚ�ʹ�ܼĴ���CCER��ͨ�� ������ ������
	TIM_ICInitStruct.TIM_ICPrescaler =  TIM_ICPSC_DIV1;		//��������Ƶ
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;//ֱ��ӳ��
	
	TIM_ICInit(TIM4, &TIM_ICInitStruct);
	
	  //ʹ���жϣ���ʹ�� dma
	 TIM_ITConfig( TIM4,   TIM_IT_CC4, ENABLE );
	
		 	
	//�����жϹ���
	NVIC_InitTypeDef  NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority =1;
	NVIC_Init(&NVIC_InitStruct);
	
	
	
	TIM_CCxCmd(TIM4,TIM_Channel_4,TIM_CCx_Enable);        ///  ����Ƚ�ʹ�ܼĴ���CCER��ʹ��ͨ��
	
	//������ʱ������
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




