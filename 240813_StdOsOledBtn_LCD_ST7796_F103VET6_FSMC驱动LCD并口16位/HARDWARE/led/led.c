#include "stm32f10x.h"
#include "led.h"

void led_init(void)
{
	RCC_APB2PeriphClockCmd(LED_LED0_RCC_APB_Periph, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = LED_LED0_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_LED0_PORN, &GPIO_InitStruct); // PC13
}
