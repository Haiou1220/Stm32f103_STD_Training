#ifndef __LED_H
#define __LED_H
#include "stdint.h"


#define LED_LED0_RCC_APB_Periph   RCC_APB2Periph_GPIOC
#define LED_LED0_PIN              GPIO_Pin_0
#define LED_LED0_PORN             GPIOC

#define Led0_On() do {GPIO_WriteBit( LED_LED0_PORN,LED_LED0_PIN, Bit_RESET );}while (0)
#define Led0_Off() do {GPIO_WriteBit( LED_LED0_PORN,LED_LED0_PIN, Bit_SET );}while (0)


void led_init(void);

#endif
