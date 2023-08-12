/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.6.0
  * @date    20-September-2021
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2011 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "global.h"
 
 
 
/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

extern u8 FlagDelay_SysTickInt;//systick定时标志位
extern u32 us_LEDDelayChange ;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{	
	  FlagDelay_SysTickInt = 1;//systick定时标志位	
}

void USART1_IRQHandler(void)
{	
	static u8 falg;
	if(USART_GetITStatus( USART1,USART_IT_RXNE) == SET )
	{
		USART_ClearITPendingBit( USART1, USART_IT_RXNE);
		falg = (u8)USART_ReceiveData(USART1);
	//	USART_SendData(USART1,falg  );
		if(falg == '0')
		{ 
			 FlagUsartCMD_0 = SET;
		}	
		if(falg == '1')
		{ 
			 FlagUsartCMD_1 = SET;
		}
		if(falg == '2')
		{
			FlagUsartCMD_2  = SET;
			 
		}
		if(falg == '3')
		{
			FlagUsartCMD_3  = SET;
			 
		}
		if(falg == '4')
		{
			FlagUsartCMD_4  = SET;
			 
		}
	
	
		if(falg == '5')
		{
			FlagUsartCMD_5  = SET;
			 
		}
		if(falg == '6')
		{
			FlagUsartCMD_6  = SET;
			 
		}
		
		if(falg == '7')
		{
			FlagUsartCMD_7  = SET;
			 
		}
		
			if(falg == '8')
		{
			FlagUsartCMD_8  = SET;
			 
		}
		if(falg == '9')
		{
			FlagUsartCMD_9  = SET;
			 
		}
		
	}
	
}


void TIM2_IRQHandler(void) //定时器2-1ms中断
{

	if(TIM_GetITStatus(  TIM2,  TIM_IT_Update))
	{
		TIM_ClearITPendingBit(  TIM2,   TIM_IT_Update);
		FlagTime_1ms = SET;
		
	}

}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

