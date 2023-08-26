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
#include "main.h" //使用全局变量
#include "stdio.h"
#include "systick.h" //使用全局变量
#include "stepper_motor.h" //call back
 
/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */


 
 extern  u8  gDma_test_u8Buf_dest[];


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
	  gFlagDelay_SysTickInt = 1;//systick定时标志位	
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
			 gFlagUsartCMD_0 = SET;
		}	
		if(falg == '1')
		{ 
			 gFlagUsartCMD_1 = SET;
		}
		if(falg == '2')
		{
			gFlagUsartCMD_2  = SET;
			 
		}
		if(falg == '3')
		{
			gFlagUsartCMD_3  = SET;
			 
		}
		if(falg == '4')
		{
			gFlagUsartCMD_4  = SET;
			 
		}
	
	
		if(falg == '5')
		{
			gFlagUsartCMD_5  = SET;
			 
		}
		if(falg == '6')
		{
			gFlagUsartCMD_6  = SET;
			 
		}
		
		if(falg == '7')
		{
			gFlagUsartCMD_7  = SET;
			 
		}
		
			if(falg == '8')
		{
			gFlagUsartCMD_8  = SET;
			 
		}
		if(falg == '9')
		{
			gFlagUsartCMD_9  = SET;
			 
		}
		
	}
	
}


void TIM2_IRQHandler(void) //定时器2-1ms中断
{
  
	if(TIM_GetITStatus(  TIM2,  TIM_IT_Update))
	{
		
		
		TIM_ClearITPendingBit(  TIM2,   TIM_IT_Update);
		gFlagTime_1MS = SET;
		
		 
		
	}

}
void TIM4_IRQHandler(void)
{
	/*
编码器AB 都是下降沿有效,捕获是A通道，，  
顺时针 B A       ,   A的下降沿 B 是低电平    
逆时针 A B      ,      A的下降沿 B 是高电平   

*/
	
	  static s32 count = 10;
	  static u8 Flag_clockwise = 0;
		extern u16 gServo_setAngle ;
	//编码器是20个单位
	
	
	 if(TIM_GetITStatus(TIM4,TIM_IT_CC4))
	 {
	 
		TIM_ClearITPendingBit(TIM4,TIM_IT_CC4);
	 
		  

		 
		 
		 
		 
		if( GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0) ) //pE0
		{	//逆时针
			count--; if(count<0) count = 0;
			Flag_clockwise = 0;
		}else
		{	//顺时针
			count++;if(count> 20 ) count = 20;
				Flag_clockwise = 1;
		}
		 
		
		
		
	//    gServo_setAngle = (u16)   ( (float)count/20.0 ) *180 ;  //应该0-180，但是为什么会错误，	因为把小于1的系数转换成u16 结果总是小于0    
		
		
		
		
	 	gServo_setAngle = ((u16)count)*9; //编码器一圈20个计数，每一个数可以是9，那么一周9*20 = 180 刚刚好是舵机的范围
		
			printf("Flag_clockwise:%4d ",Flag_clockwise);//调试 计数 方向
		 	printf("count:%4d",count);//调试 计数 编码器个数
		printf(" gServo_setAngle:%4d\r\n",gServo_setAngle);//调试
		
		
		 
	 }

}





void DMA1_Channel1_IRQHandler(void)  // 
{


	 
	
 
}

void DMA1_Channel2_IRQHandler(void)  // // 
{

	if(DMA_GetITStatus( DMA1_IT_TC2) )
	{
		
	  DMA_ClearITPendingBit(  DMA1_IT_TC2);

	// TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Disable);// 输出比较 关闭 ,定时器不关闭，因为用比较事件作为事件申请
		
		
		//TIM_ClearFlag(TIM3,TIM_FLAG_CC3);
	   
		// TIM_GenerateEvent(TIM3,TIM_EventSource_Update); // 手动产生更新事件 ,清除寄存器
		 //TIM_SetCompare3(TIM3,0);
									
		  TIM_Cmd(TIM3,DISABLE);
		
   //   printf("DMA1_CH2:TIM3_OCR3 request-----------------------complete \r\n" ) ;
	   
		
		
	
	}

}


void DMA1_Channel3_IRQHandler(void)  // //DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE); 
{

//	if(DMA_GetITStatus( DMA1_IT_TC3) ) pwm-dma不适合使用定时器更新事件请求
//	{  
//	    DMA_ClearITPendingBit(  DMA1_IT_TC3);
//																								 
//		TIM_SetCompare3(TIM3,255);
//		//TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Disable);
//		TIM_Cmd(TIM3,DISABLE);//关闭定时器，因为用更新事件 作为事件请求
//		
//		printf("DMA1_CH3:TIM3_UP request--------------------complete \r\n" ) ;
////	 ;
////	    
////		
//	}

}


void DMA1_Channel4_IRQHandler(void)  //串口TX 中断 请求，完成
{

	/*
	1.清除通道中断，1.关闭串口
	*/
	if(DMA_GetITStatus( DMA1_IT_TC4) )
	{	
			printf("Usart TX  DMA send complete \r\n");
			DMA_ClearITPendingBit(DMA1_IT_TC4  );
		
		    DMA_Cmd(DMA1_Channel4,DISABLE);//发送完成 关闭通道 ，如果接收不要关闭
		
	 
		 
	
	}

	

	
	

}


void TIM7_IRQHandler(void)
{

	 if(TIM_GetITStatus(TIM7,TIM_IT_Update))
	 {
	 	// printf("TIM7_IRQHandler befor TIM_ClearITPendingBit ：TIM7->ARR%#06x ,TIM7->CNT:%#06x，TIM7->SR:%#06x \r\n", TIM7->ARR,TIM7->CNT,TIM7->SR);

		 TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
	 
	 	// printf("TIM7_IRQHandler afftor TIM_ClearITPendingBit ：TIM7->ARR%#06x ,TIM7->CNT:%#06x，TIM7->SR:%#06x \r\n", TIM7->ARR,TIM7->CNT,TIM7->SR);
		
			  steperMotor_Uln2003_run_timerUpINT_callBack( );	  //定时器7 回调函数
		 
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

