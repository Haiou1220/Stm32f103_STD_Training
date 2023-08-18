

 #ifndef __IIC_SOFT_H__
#define  __IIC_SOFT_H__

#include "stm32f10x.h"
 
void iic_soft_initialize(void) ;

void iic_delay_init(void);

void iic_delay(void);
void IIC_SDA(u8 state) ;

void IIC_SCL(u8 state);


//SCL高电平期间，是采集数据期间，要求电平不变
// 如果SCL高电平期间，电平发生改变，定义为开始信号和停止信号
// 先准备数据 再 采样

void iic_start(void) ;

void iic_stop(void);

u8 iic_wait_ack(void) ;

void iic_ack(void);
void iic_nack(void);
  
  
void iic_send_byte(u8 byte);	//发送一个byte
u8 iic_read_byte(u8 ack);
 
void iic_send_ID_R(u8 id );	//发送ID

void iic_send_ID_W(u8 id );	//发送ID


 

#endif 





