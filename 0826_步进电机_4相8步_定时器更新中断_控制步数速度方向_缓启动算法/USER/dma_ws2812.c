#include "dma_ws2812.h"
#include "stm32f10x.h"


void TIM_SelectOutputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource);//CR2

void TIM_UpdateRequestConfig(TIM_TypeDef* TIMx, uint16_t TIM_UpdateSource_);//CR1   TIM_UpdateSource_Global
void TIM_UpdateDisableConfig(TIM_TypeDef* TIMx, FunctionalState NewState);//CR1

 

void TIM_DMAConfig(TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength);//DCR
void TIM_DMACmd(TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState);//DIER

void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState);//DIER

 
 
 
 
 
 
 
 
 
 
void ws2812_DMA1CH2_to_Tim3OCR3_init(void )//DMA1ch2初始化_外设地址是tim3ch3_比较输出寄存器
{
		   //  DMA1时钟 
	 RCC_AHBPeriphClockCmd(  RCC_AHBPeriph_DMA1,   ENABLE);
	/*
	要开始新的DMA传输，需要在关闭DMA通道的情况下，在DMA_CNDTRx寄存器中重新写入传输数目。
	*/
    u32 PeripheralBaseAddr_dest = (u32) &(TIM3->CCR3);
	
	DMA_InitTypeDef  DMA_InitStruct;
	DMA_InitStruct.DMA_BufferSize  = 1 ;//骗过检查
	
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;// 数据方向  
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable; //非 M2M
	
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Low;
	
	DMA_InitStruct.DMA_MemoryBaseAddr = 0;
	DMA_InitStruct.DMA_MemoryDataSize =DMA_MemoryDataSize_Byte; //         
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; //来源地址递增u8数据
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = PeripheralBaseAddr_dest;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// 16位      
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 不递增
	
    DMA_Init( DMA1_Channel2, &DMA_InitStruct);
	
	//配置传输完成中断
	 NVIC_InitTypeDef NVIC_InitStruct;//配置中断管理器
	 NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	 NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	 NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	 NVIC_Init(& NVIC_InitStruct);
	//开启传输完成中断
	DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);
 
	DMA_Cmd( DMA1_Channel2,    DISABLE );//发送触发再使能    

}





 
void ws2812_DMA1CH3_to_TIM3UP_init(u32 u8_buf_addr_Sourse ,u32 led_count)//DMA1ch2初始化_外设地址是tim3ch3_比较输出寄存器
{
		   //  DMA1时钟 
	 RCC_AHBPeriphClockCmd(  RCC_AHBPeriph_DMA1,   ENABLE);
	/*
	要开始新的DMA传输，需要在关闭DMA通道的情况下，在DMA_CNDTRx寄存器中重新写入传输数目。
	*/
    u32 PeripheralBaseAddr_dest = (u32) &(TIM3->CCR3);
	
	DMA_InitTypeDef  DMA_InitStruct;
	DMA_InitStruct.DMA_BufferSize  = led_count*24;
	
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;// 数据方向  
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable; //非 M2M
	
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Low;
	
	DMA_InitStruct.DMA_MemoryBaseAddr = u8_buf_addr_Sourse;
	DMA_InitStruct.DMA_MemoryDataSize =DMA_MemoryDataSize_Byte; // 
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; //来源地址递增
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = PeripheralBaseAddr_dest;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// 16位
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 不递增
	
    DMA_Init( DMA1_Channel3, &DMA_InitStruct);
	
	//配置传输完成中断
	 NVIC_InitTypeDef NVIC_InitStruct;//配置中断管理器
	 NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	 NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	 NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	 NVIC_Init(& NVIC_InitStruct);
	//开启传输完成中断
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
 
	DMA_Cmd( DMA1_Channel3,    DISABLE );//使能 ，等待请求

}

void ws2812_DMA1CH3_for_TIM3UP__Send(u32 buf_addr ,u32  LED_count)// 
{			 
		 
		//PWM - 定时器不适合用 更新事件作为申请请求
//	
//		DMA_Cmd( DMA1_Channel3,    DISABLE );// 
//	
//	    DMA1_Channel3->CNDTR =  LED_count*24;
//		DMA1_Channel3->CMAR = buf_addr;
//	
//	    TIM_Cmd(TIM3,ENABLE);
//		DMA_Cmd( DMA1_Channel3,    ENABLE );//使能 ，等待请求
		 
}



void ws2812_DMA1CH2_for_TIM3OCR3_Send(u32 buf_addr ,u32 LED_count)// (u32)&GRB_CCRx_buf ,  (sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0]))
{
		u32 zeroOneCode_u8buf_count = (  LED_count )*24;
		
		DMA_Cmd( DMA1_Channel2,    DISABLE );// 
		
		DMA1_Channel2->CMAR = buf_addr;
	    DMA1_Channel2->CNDTR =  zeroOneCode_u8buf_count;
		TIM_GenerateEvent(TIM3,TIM_EventSource_Update); // 手动产生更新事件 ,清除寄存器？？？用法不清楚，感觉要这样使用
	 
	
	  TIM_Cmd(TIM3,ENABLE);
		DMA_Cmd( DMA1_Channel2,    ENABLE );//通道使能 ，等待请求
}









