#include "adc.h"
#include "stm32f10x.h"
#include "systick.h"
#include "stdio.h"

/*
 

  //��ͨ����DMA  ��ͨ��    

 
*/

u16   gADC1_DMA_VAL[2] = {0};
 

void DMA1_Channel1_IRQHandler_CALLBACK(void)  //  DMAͨ���жϻص�����������ͨ�������ô���������ʹ��ͨ����
{
	

	DMA_ClearITPendingBit(DMA1_IT_TC1); 
 
	 printf("%f,%f \r\n",  ((  gADC1_DMA_VAL[0]     )*3.3)/0Xfff        ,((  gADC1_DMA_VAL[1]     )*3.3)/0Xfff  );
	 

 
	
 
	
	
		
}

void adc1_SoftwareStartConv(void)
{
 ADC_SoftwareStartConvCmd(ADC1,ENABLE); //������� ʹ�� ,��������ø�λ������ת����ת����ʼ��Ӳ�����������λ��
}

void adc_PA1_init(void) //PA1---ADC123_IN1/  
{
	//����ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1,ENABLE);
	
	//��λadc1 ����adc1ʱ�� ���12M ���� 6��Ƶ,72/6 =12
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	
	ADC_DeInit(ADC1);
	 
	
	//����gpio
	 GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;               //PA1
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init( GPIOA,   &GPIO_InitStruct);
	
	//��ʼ��adc
	ADC_InitTypeDef  ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;  //������ת��        
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //������ʽsofoware��
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;     //����ģʽ
	ADC_InitStruct.ADC_NbrOfChannel = 1; //ͨ����                   //PA1
	ADC_InitStruct.ADC_ScanConvMode =  DISABLE;  //��ʹ��ɨ��
	 ADC_Init(ADC1,  &ADC_InitStruct);
	
	
	//ʹ��adc ��У׼
	ADC_Cmd(ADC1,ENABLE);
	 
	 
	 ADC_ResetCalibration(  ADC1); //��λУ׼
	while(  ADC_GetResetCalibrationStatus( ADC1) ){} ;//Ӳ���Զ���λ

    ADC_StartCalibration(  ADC1);//��ʼУ׼
	while(  ADC_GetCalibrationStatus(  ADC1 ) ){};//Ӳ���Զ���λ
	 
	
}

void adc1_init(void) //PA1---ADC123_IN1/  //PA4---ADC12_IN4/   //��ͨ����DMA   
{
	//����ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1,ENABLE);
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	 
	 
	
	
	
	//��λadc1 ����adc1ʱ�� ���12M ���� 6��Ƶ,72/6 =12
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	
	ADC_DeInit(ADC1);
	 
	
	//����gpio  PA1---ADC123_IN1/ //PA4---ADC12_IN4/
	 GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init( GPIOA,   &GPIO_InitStruct);
	
	//��ʼ��adc  
	ADC_InitTypeDef  ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;                            //  ������ת��:ʹ�����ÿ1s ����һ�� /  
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //������ʽ��
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;     //����ģʽ
	ADC_InitStruct.ADC_NbrOfChannel = 2; //ͨ����  PA1---ADC123_IN1/ //PA4---ADC12_IN4/
	ADC_InitStruct.ADC_ScanConvMode =  ENABLE;  //   ʹ��ɨ��   //��ͨ���� 
	ADC_Init(ADC1,  &ADC_InitStruct);
	

	// ʹ��dma  ADC1 ��Ӧ DMA1 ͨ�� 1�� ADC3 ��Ӧ DMA2 ͨ�� 5�� ADC2 û�� DMA ����
  DMA_InitTypeDef  DMA_InitStruct;
	
	
  DMA_InitStruct.DMA_BufferSize =  2;//sizeof(gADC1_DMA_VAL)/sizeof(gADC1_DMA_VAL[0]) ;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular; //DMA_Mode_Circular:һ��֮�󣬴��������Զ�����,, DMA_Mode_Normal:һ�κ�������Ϊ0 ����Ҫ����ͨ�����ظ�ֵ�鷳
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
   
  DMA_InitStruct.DMA_MemoryBaseAddr = (u32)&gADC1_DMA_VAL;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  
  DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&(ADC1->DR);
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  
  DMA_Init(  DMA1_Channel1, &DMA_InitStruct);
 
  
  //// �����ж����ȼ�  dma�ж�,����len=2 ����
 NVIC_InitTypeDef NVIC_InitStruct;
 NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn ; //DMA1����ж�
 NVIC_InitStruct.NVIC_IRQChannelCmd =  ENABLE;
 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
 NVIC_InitStruct.NVIC_IRQChannelSubPriority= 2 ;
 NVIC_Init(&NVIC_InitStruct);
  
   //����DMAͨ�� �ж�
   DMA_ITConfig(DMA1_Channel1,   DMA_IT_TC,   ENABLE);
  
 
	
	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
 
	 //ʹ��dac dma
	 ADC_DMACmd(ADC1,ENABLE);
	 
	 //ʹ��DMA1-ͨ��
	 DMA_Cmd(DMA1_Channel1,ENABLE);
	 
	 //ʹ��adc ��У׼
	ADC_Cmd(ADC1,ENABLE);
	
	 ADC_ResetCalibration(  ADC1); //��λУ׼
	while(  ADC_GetResetCalibrationStatus( ADC1) ){} ;//Ӳ���Զ���λ

    ADC_StartCalibration(  ADC1);//��ʼУ׼
	while(  ADC_GetCalibrationStatus(  ADC1 ) ){};//Ӳ���Զ���λ
	 	
		ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);  //�������β�Ϊͨ����ת��˳�򣬿�ѡΪ 1 �� 16�� //PA1---ADC123_IN1/ //PA4---ADC12_IN4/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,2,ADC_SampleTime_239Cycles5);  //�������β�Ϊͨ����ת��˳�򣬿�ѡΪ 1 �� 16�� //PA1---ADC123_IN1/ //PA4---ADC12_IN4/
	
		//ADC_SoftwareStartConvCmd(ADC1,ENABLE); //������� ʹ��
}

 


u16 adc_get_PA1(void) //12λ��Чֵ���Ҷ���
{
	
	//���ù���ͨ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);  //ת�����1����Ϊֻ��һ��ͨ����
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE); //������� ʹ��
	
	//��ѯ״̬
	while ( RESET == ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)) {};
	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);  //���״̬
				
	return ADC_GetConversionValue(ADC1); //��ת�����;
		
}


 

float adc_get_average_PA1_voltage(u16 time)
{
	u32 sum = 0 ;
	 for(u16 i = 0 ;i < time; i++)
	{
		sum += (u32)adc_get_PA1();
		sysTickDelay_ms( 5); 
	}


return    ((1.0*sum/time)*3.3)/0Xfff    ;

}



void ADC1_2_IRQHandler_CALLBACK(void)
{
		u32 static count = 0;
		count++;
		ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);

		printf("ADC1_2_IRQHandler  :%d \r\n",count);

	 
	
	
}
















