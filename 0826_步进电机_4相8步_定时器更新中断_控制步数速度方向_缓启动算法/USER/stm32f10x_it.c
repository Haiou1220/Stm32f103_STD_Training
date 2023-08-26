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
#include "main.h" //ʹ��ȫ�ֱ���
#include "stdio.h"
#include "systick.h" //ʹ��ȫ�ֱ���
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
	  gFlagDelay_SysTickInt = 1;//systick��ʱ��־λ	
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


void TIM2_IRQHandler(void) //��ʱ��2-1ms�ж�
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
������AB �����½�����Ч,������Aͨ������  
˳ʱ�� B A       ,   A���½��� B �ǵ͵�ƽ    
��ʱ�� A B      ,      A���½��� B �Ǹߵ�ƽ   

*/
	
	  static s32 count = 10;
	  static u8 Flag_clockwise = 0;
		extern u16 gServo_setAngle ;
	//��������20����λ
	
	
	 if(TIM_GetITStatus(TIM4,TIM_IT_CC4))
	 {
	 
		TIM_ClearITPendingBit(TIM4,TIM_IT_CC4);
	 
		  

		 
		 
		 
		 
		if( GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0) ) //pE0
		{	//��ʱ��
			count--; if(count<0) count = 0;
			Flag_clockwise = 0;
		}else
		{	//˳ʱ��
			count++;if(count> 20 ) count = 20;
				Flag_clockwise = 1;
		}
		 
		
		
		
	//    gServo_setAngle = (u16)   ( (float)count/20.0 ) *180 ;  //Ӧ��0-180������Ϊʲô�����	��Ϊ��С��1��ϵ��ת����u16 �������С��0    
		
		
		
		
	 	gServo_setAngle = ((u16)count)*9; //������һȦ20��������ÿһ����������9����ôһ��9*20 = 180 �ոպ��Ƕ���ķ�Χ
		
			printf("Flag_clockwise:%4d ",Flag_clockwise);//���� ���� ����
		 	printf("count:%4d",count);//���� ���� ����������
		printf(" gServo_setAngle:%4d\r\n",gServo_setAngle);//����
		
		
		 
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

	// TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Disable);// ����Ƚ� �ر� ,��ʱ�����رգ���Ϊ�ñȽ��¼���Ϊ�¼�����
		
		
		//TIM_ClearFlag(TIM3,TIM_FLAG_CC3);
	   
		// TIM_GenerateEvent(TIM3,TIM_EventSource_Update); // �ֶ����������¼� ,����Ĵ���
		 //TIM_SetCompare3(TIM3,0);
									
		  TIM_Cmd(TIM3,DISABLE);
		
   //   printf("DMA1_CH2:TIM3_OCR3 request-----------------------complete \r\n" ) ;
	   
		
		
	
	}

}


void DMA1_Channel3_IRQHandler(void)  // //DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE); 
{

//	if(DMA_GetITStatus( DMA1_IT_TC3) ) pwm-dma���ʺ�ʹ�ö�ʱ�������¼�����
//	{  
//	    DMA_ClearITPendingBit(  DMA1_IT_TC3);
//																								 
//		TIM_SetCompare3(TIM3,255);
//		//TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Disable);
//		TIM_Cmd(TIM3,DISABLE);//�رն�ʱ������Ϊ�ø����¼� ��Ϊ�¼�����
//		
//		printf("DMA1_CH3:TIM3_UP request--------------------complete \r\n" ) ;
////	 ;
////	    
////		
//	}

}


void DMA1_Channel4_IRQHandler(void)  //����TX �ж� �������
{

	/*
	1.���ͨ���жϣ�1.�رմ���
	*/
	if(DMA_GetITStatus( DMA1_IT_TC4) )
	{	
			printf("Usart TX  DMA send complete \r\n");
			DMA_ClearITPendingBit(DMA1_IT_TC4  );
		
		    DMA_Cmd(DMA1_Channel4,DISABLE);//������� �ر�ͨ�� ��������ղ�Ҫ�ر�
		
	 
		 
	
	}

	

	
	

}


void TIM7_IRQHandler(void)
{

	 if(TIM_GetITStatus(TIM7,TIM_IT_Update))
	 {
	 	// printf("TIM7_IRQHandler befor TIM_ClearITPendingBit ��TIM7->ARR%#06x ,TIM7->CNT:%#06x��TIM7->SR:%#06x \r\n", TIM7->ARR,TIM7->CNT,TIM7->SR);

		 TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
	 
	 	// printf("TIM7_IRQHandler afftor TIM_ClearITPendingBit ��TIM7->ARR%#06x ,TIM7->CNT:%#06x��TIM7->SR:%#06x \r\n", TIM7->ARR,TIM7->CNT,TIM7->SR);
		
			  steperMotor_Uln2003_run_timerUpINT_callBack( );	  //��ʱ��7 �ص�����
		 
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

