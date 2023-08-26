#ifndef __STEPPER_MOTOR_H__
#define __STEPPER_MOTOR_H__

#include "stm32f10x.h"







void gpio_output_steperMotor_Uln2003_init(void)	; //PE0_1_2_3 

void steperMotor_Uln2003_SET_istep(u8 dir);	 //PE0_1_2_3
	
  

void steperMotor_Uln2003_run_Nstep_Blocking (u32 Nstep,u8 dir,u16 us_delay_speed);


void steperMotor_Uln2003_run_Angle_StartTask(u32 angle,u8 dir);//
 
 



void steperMotor_Uln2003_run_timerUp_interrupt_init(void);	  //��ʱ��7 ��ʱ�ж� ִ�в����������

 


void steperMotor_Uln2003_run_timerUpINT_callBack(void);	  //��ʱ��7 ��ʱ�ж� ִ�в����������
	
extern  u32 gStepperMotorInputNstep;
 extern u16 gStepperMotorInput_minHz  ;//1us ����Ƶ�ʼ�1M * ������oxffff ,�ó�15hz 
extern u16  gStepperMotorInput_usdelay_16bit;//ȫ�ֱ��� �������Ŀ�경��  ��ʱ���ٶ�
#endif

