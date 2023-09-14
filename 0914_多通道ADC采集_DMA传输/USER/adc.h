#ifndef __ADC_H__
#define __ADC_H__
#include "stm32f10x.h"

void adc_PA1_init(void); //PA1---ADC123_IN1/
u16 adc_get_PA1(void);
float adc_get_average_PA1_voltage(u16 time);

 
void adc1_init(void); //PA1---ADC123_IN1/  //PA4---ADC12_IN4/
	
void DMA1_Channel1_IRQHandler_CALLBACK(void) ; // 
void ADC1_2_IRQHandler_CALLBACK(void);
void adc1_SoftwareStartConv(void);


#endif


