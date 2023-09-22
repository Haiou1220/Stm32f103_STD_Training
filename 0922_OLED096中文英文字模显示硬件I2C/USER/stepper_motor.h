#ifndef __STEPPER_MOTOR_H__
#define __STEPPER_MOTOR_H__

#include "stm32f10x.h"







void gpio_output_steperMotor_Uln2003_init(void)	; //PE0_1_2_3 

void steperMotor_Uln2003_SET_istep(u8 dir);	 //PE0_1_2_3
	
  

void steperMotor_Uln2003_run_Nstep_Blocking (u32 Nstep,u8 dir,u16 us_delay_speed);


void steperMotor_Uln2003_run_Angle_StartTask(u32 angle,u8 dir);//
 
 



void steperMotor_Uln2003_run_timerUp_interrupt_init(void);	  //定时器7 定时中断 执行步进电机步数

 


void steperMotor_Uln2003_run_timerUpINT_callBack(void);	  //定时器7 定时中断 执行步进电机步数
	
extern  u32 gStepperMotorInputNstep;
 extern u16 gStepperMotorInput_minHz  ;//1us 计数频率即1M * 最大计数oxffff ,得出15hz 
extern u16  gStepperMotorInput_usdelay_16bit;//全局变量 步进电机目标步数  延时，速度
#endif

