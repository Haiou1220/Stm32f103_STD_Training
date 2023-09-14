#include "stm32f10x.h"



//����periph2 ʱ��

void clock_config_periph2(void) //����apb2���� pclk2ʱ�� =  ����������AHB
{

RCC_PCLK2Config(RCC_HCLK_Div1);
 
 
}

//����periph1 ʱ��

void clock_config_periph1(void) //����apb1���� pclk1ʱ�� =  ����������AHB
{

RCC_PCLK1Config(RCC_HCLK_Div1);
 
 
}

//���ø��������� ʱ�� ahb
void clock_config_ahb(void) //���� AHB clock = SYSCLK 
{

//RCC_SYSCLK_Div1: AHB clock = SYSCLK
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
 
}


//����ϵͳ ʱ�� 
void clock_config_sysclk(void) //���� SYSCLK  = ���໷
{
 /*
	  * @arg RCC_SYSCLKSource_HSI: HSI selected as system clock
  *     @arg RCC_SYSCLKSource_HSE: HSE selected as system clock
  *     @arg RCC_SYSCLKSource_PLLCLK: PLL selected as system clock
	
	*/
	
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
 
}

void clock_config_pllclk(void) //���� ���໷
{
 
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); //8*9 = 72
	RCC_PLLCmd(ENABLE);
 
}


