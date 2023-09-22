#include "stm32f10x.h"



//配置periph2 时钟

void clock_config_periph2(void) //配置apb2外设 pclk2时钟 =  高性能总线AHB
{

RCC_PCLK2Config(RCC_HCLK_Div1);
 
 
}

//配置periph1 时钟

void clock_config_periph1(void) //配置apb1外设 pclk1时钟 =  高性能总线AHB
{

RCC_PCLK1Config(RCC_HCLK_Div1);
 
 
}

//配置高性能总线 时钟 ahb
void clock_config_ahb(void) //配置 AHB clock = SYSCLK 
{

//RCC_SYSCLK_Div1: AHB clock = SYSCLK
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
 
}


//配置系统 时钟 
void clock_config_sysclk(void) //配置 SYSCLK  = 锁相环
{
 /*
	  * @arg RCC_SYSCLKSource_HSI: HSI selected as system clock
  *     @arg RCC_SYSCLKSource_HSE: HSE selected as system clock
  *     @arg RCC_SYSCLKSource_PLLCLK: PLL selected as system clock
	
	*/
	
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
 
}

void clock_config_pllclk(void) //配置 锁相环
{
 
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); //8*9 = 72
	RCC_PLLCmd(ENABLE);
 
}


