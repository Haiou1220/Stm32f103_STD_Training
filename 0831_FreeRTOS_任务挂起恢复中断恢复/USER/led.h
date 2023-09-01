#ifndef __LED_H__
#define __LED_H__
#include "stm32f10x.h"



void led_init(void );//led0 led1 pb5 pe5
void led0_switch(u8 state);//led0 led1 pb5 pe5


void led1_switch(u8 state );//led0 led1 pb5 pe5

void led_blink_task(void);
void led0_blink_task(void* pvParameters);
void led1_blink_task(void* pvParameters);
#endif






