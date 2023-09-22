#include "adc.h"
#include "stm32f10x.h"
#include "systick.h"
#include "stdio.h"

/*
 

  //多通道数DMA  单通道    

 
*/

u16   gADC1_DMA_VAL[2] = {0};
 

void DMA1_Channel1_IRQHandler_CALLBACK(void)  //  DMA通道中断回调函数，禁用通道再设置传输数量再使能通道，
{
	

	DMA_ClearITPendingBit(DMA1_IT_TC1); 
 
	 printf("%f,%f \r\n",  ((  gADC1_DMA_VAL[0]     )*3.3)/0Xfff        ,((  gADC1_DMA_VAL[1]     )*3.3)/0Xfff  );
	 

 
	
 
	
	
		
}

void adc1_SoftwareStartConv(void)
{
 ADC_SoftwareStartConvCmd(ADC1,ENABLE); //软件触发 使能 ,由软件设置该位以启动转换，转换开始后硬件马上清除此位。
}

void adc_PA1_init(void) //PA1---ADC123_IN1/  
{
	//开启时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1,ENABLE);
	
	//复位adc1 设置adc1时钟 最大12M 所以 6分频,72/6 =12
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	
	ADC_DeInit(ADC1);
	 
	
	//配置gpio
	 GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;               //PA1
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init( GPIOA,   &GPIO_InitStruct);
	
	//初始化adc
	ADC_InitTypeDef  ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;  //非连续转换        
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //触发方式sofoware无
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;     //独立模式
	ADC_InitStruct.ADC_NbrOfChannel = 1; //通道数                   //PA1
	ADC_InitStruct.ADC_ScanConvMode =  DISABLE;  //不使用扫描
	 ADC_Init(ADC1,  &ADC_InitStruct);
	
	
	//使能adc 并校准
	ADC_Cmd(ADC1,ENABLE);
	 
	 
	 ADC_ResetCalibration(  ADC1); //复位校准
	while(  ADC_GetResetCalibrationStatus( ADC1) ){} ;//硬件自动复位

    ADC_StartCalibration(  ADC1);//开始校准
	while(  ADC_GetCalibrationStatus(  ADC1 ) ){};//硬件自动复位
	 
	
}

void adc1_init(void) //PA1---ADC123_IN1/  //PA4---ADC12_IN4/   //多通道数DMA   
{
	//开启时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1,ENABLE);
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	 
	 
	
	
	
	//复位adc1 设置adc1时钟 最大12M 所以 6分频,72/6 =12
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	
	ADC_DeInit(ADC1);
	 
	
	//配置gpio  PA1---ADC123_IN1/ //PA4---ADC12_IN4/
	 GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init( GPIOA,   &GPIO_InitStruct);
	
	//初始化adc  
	ADC_InitTypeDef  ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;                            //  非连续转换:使用软件每1s 触发一次 /  
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //触发方式无
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;     //独立模式
	ADC_InitStruct.ADC_NbrOfChannel = 2; //通道数  PA1---ADC123_IN1/ //PA4---ADC12_IN4/
	ADC_InitStruct.ADC_ScanConvMode =  ENABLE;  //   使用扫描   //多通道数 
	ADC_Init(ADC1,  &ADC_InitStruct);
	

	// 使用dma  ADC1 对应 DMA1 通道 1， ADC3 对应 DMA2 通道 5， ADC2 没有 DMA 功能
  DMA_InitTypeDef  DMA_InitStruct;
	
	
  DMA_InitStruct.DMA_BufferSize =  2;//sizeof(gADC1_DMA_VAL)/sizeof(gADC1_DMA_VAL[0]) ;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular; //DMA_Mode_Circular:一次之后，传输数量自动重载,, DMA_Mode_Normal:一次后传输数量为0 ，需要禁用通道再重赋值麻烦
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
   
  DMA_InitStruct.DMA_MemoryBaseAddr = (u32)&gADC1_DMA_VAL;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  
  DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&(ADC1->DR);
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  
  DMA_Init(  DMA1_Channel1, &DMA_InitStruct);
 
  
  //// 配置中断优先级  dma中断,传输len=2 结束
 NVIC_InitTypeDef NVIC_InitStruct;
 NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn ; //DMA1完成中断
 NVIC_InitStruct.NVIC_IRQChannelCmd =  ENABLE;
 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
 NVIC_InitStruct.NVIC_IRQChannelSubPriority= 2 ;
 NVIC_Init(&NVIC_InitStruct);
  
   //配置DMA通道 中断
   DMA_ITConfig(DMA1_Channel1,   DMA_IT_TC,   ENABLE);
  
 
	
	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
 
	 //使能dac dma
	 ADC_DMACmd(ADC1,ENABLE);
	 
	 //使能DMA1-通道
	 DMA_Cmd(DMA1_Channel1,ENABLE);
	 
	 //使能adc 并校准
	ADC_Cmd(ADC1,ENABLE);
	
	 ADC_ResetCalibration(  ADC1); //复位校准
	while(  ADC_GetResetCalibrationStatus( ADC1) ){} ;//硬件自动复位

    ADC_StartCalibration(  ADC1);//开始校准
	while(  ADC_GetCalibrationStatus(  ADC1 ) ){};//硬件自动复位
	 	
		ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);  //第三个形参为通道的转换顺序，可选为 1 到 16； //PA1---ADC123_IN1/ //PA4---ADC12_IN4/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,2,ADC_SampleTime_239Cycles5);  //第三个形参为通道的转换顺序，可选为 1 到 16； //PA1---ADC123_IN1/ //PA4---ADC12_IN4/
	
		//ADC_SoftwareStartConvCmd(ADC1,ENABLE); //软件触发 使能
}

 


u16 adc_get_PA1(void) //12位有效值，右对齐
{
	
	//配置规则通道
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);  //转换序号1：因为只有一个通道数
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE); //软件触发 使能
	
	//查询状态
	while ( RESET == ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)) {};
	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);  //清除状态
				
	return ADC_GetConversionValue(ADC1); //读转换结果;
		
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
















