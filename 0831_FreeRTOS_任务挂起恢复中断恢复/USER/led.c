#include "led.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"



#define Led0_On GPIO_WriteBit( GPIOB,GPIO_Pin_5, Bit_RESET );
#define Led0_Off GPIO_WriteBit( GPIOB,GPIO_Pin_5, Bit_SET );
#define Led1_On GPIO_WriteBit( GPIOE,GPIO_Pin_5, Bit_RESET );
#define Led1_Off GPIO_WriteBit( GPIOE,GPIO_Pin_5, Bit_SET );






void led_init(void )//led0 led1 pb5 pe5
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOB,&GPIO_InitStruct);//pb5
  GPIO_Init(GPIOE,&GPIO_InitStruct);//pb5

}



void led0_switch(u8 state)//led0 led1 pb5 pe5
{
	if(state == 1)
	{
		Led0_On;
	}else
	{
	Led0_Off;
	
	}
}





void led1_switch(u8 state )//led0 led1 pb5 pe5
{
	if(state == 1)
	{
		Led1_On;
	}else
	{
	Led1_Off;
	
	}

}

void led1_blink_task(void* pvParameters)
{

	while(1){
	
			led1_switch(1);
	vTaskDelay(500);//1000ms

	led1_switch(0);
	vTaskDelay(500);//1000ms
		

	
	}
}

void led0_blink_task(void* pvParameters)
{

	while(1){
	


	led0_switch(0);
	vTaskDelay(1000);//1000ms
		
			led0_switch(1);
	vTaskDelay(1000);//1000ms
		
	
	}
}

