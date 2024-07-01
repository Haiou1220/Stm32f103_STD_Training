#ifndef __KEY_H
#define __KEY_H
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"


void key_init_gpio(void);
void key_init_exti(void);

uint8_t key_getValue(void);

#define KEY_PORT GPIOB
#define KEY_PIN  GPIO_Pin_0
#define KEY_RCC_APB2Periph_GPIOx RCC_APB2Periph_GPIOB
#define KEY_GPIO_PortSourceGPIOx GPIO_PortSourceGPIOB
#define KEY_GPIO_PinSourcex GPIO_PinSource0
#define KEY_GPIO_Mode  GPIO_Mode_IPU
#define KEY_EXTI_Linex EXTI_Line0
#define KEY_EXTIx_IRQn EXTI0_IRQn

#define KEY_EXTI_Line_NUM  0


#endif

