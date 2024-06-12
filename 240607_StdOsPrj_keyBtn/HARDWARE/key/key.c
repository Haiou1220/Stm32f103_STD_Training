#include "key.h"
#include "stm32f10x.h"
#include "stdio.h"


void key_init_gpio(void)
{
//时钟
RCC_APB2PeriphClockCmd(KEY_RCC_APB2Periph_GPIOx,ENABLE);

GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Pin = KEY_PIN;
GPIO_InitStruct.GPIO_Mode = KEY_GPIO_Mode;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

GPIO_Init(KEY_PORT, &GPIO_InitStruct);

}

void key_init_exti(void)
{

//使能中断
//外部中断需要开启AFIO时钟
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

//配置EXIT中断线的映射
GPIO_EXTILineConfig(KEY_GPIO_PortSourceGPIOx,KEY_GPIO_PinSourcex);

//配置外部中断触发事件,
EXTI_InitTypeDef EXTI_InitStruct;
EXTI_InitStruct.EXTI_Line = KEY_EXTI_Linex;
EXTI_InitStruct.EXTI_LineCmd = ENABLE;
EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //边沿触发模式
EXTI_Init(&EXTI_InitStruct);

//配置中断优先级
NVIC_InitTypeDef NVIC_InitStruct;
NVIC_InitStruct.NVIC_IRQChannel = KEY_EXTIx_IRQn;
NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;

NVIC_Init(&NVIC_InitStruct);

}


uint8_t key_getValue(void)
{
    return GPIO_ReadInputDataBit(KEY_PORT,KEY_PIN);
}

