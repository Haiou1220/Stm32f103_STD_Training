#ifndef __PWM_SERVO_H__
#define __PWM_SERVO_H__

#include "stm32f10x.h"

void Servo_tim5ch2pa1_init(void) ;//
	
 


void Servo_tim5ch2pa1_setAngle(u16 angle); //

extern u16 gServo_setAngle  ;//全局变量 默认舵机角度


#endif

