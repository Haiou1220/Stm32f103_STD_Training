#ifndef __LED_H
#define __LED_H
#include "stdint.h"

#define Led0_On() do {GPIO_WriteBit( GPIOC,GPIO_Pin_13, Bit_RESET );}while (0)
#define Led0_Off() do {GPIO_WriteBit( GPIOC,GPIO_Pin_13, Bit_SET );}while (0)

void led_init(void);

#endif
