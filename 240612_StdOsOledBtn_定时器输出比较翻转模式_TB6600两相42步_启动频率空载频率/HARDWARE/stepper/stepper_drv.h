#ifndef __STEPPER_DRV_H
#define __STEPPER_DRV_H
#include "stdint.h"
#include "stm32f10x.h"

#define STEPPER_EN_ON       RESET                 //开
#define STEPPER_EN_OFF      SET                  //关
#define STEPPER_DIR_CW  SET                        //顺时针
#define STEPPER_DIR_ACW RESET                       //逆时针

void stepper_drv_gpioCtlPin_init(void); //gpio en pin ,dir pin
void stepper_drv_gpioSimulation_init(void); //gpio 模拟 ,摒弃
  void STEPPER_DIR(FlagStatus level);
  void STEPPER_EN(FlagStatus level);
  void STEPPER_PUL(FlagStatus level);

void stepper_drv_gpioSimulation_RotateAngle(uint32_t Angle,uint8_t subdivide, uint8_t direction); //gpio模拟,摒弃

void stepper_drv_timerOC_init(void); //timer2,channel1,pa0;
void stepper_drv_timerOC_start(void); //timer2,channel1,pa0;
void stepper_drv_timerOC_stop(void); //timer2,channel1,pa0;
void stepper_drv_timerOC_set_ccr(uint16_t ccr);

#endif

