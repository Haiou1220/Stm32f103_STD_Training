

 #ifndef __IIC_SOFT_H__
#define  __IIC_SOFT_H__

#include "stm32f10x.h"
 
void iic_soft_initialize(void) ;

void iic_delay_init(void);

void iic_delay(void);
void IIC_SDA(u8 state) ;

void IIC_SCL(u8 state);


//SCL�ߵ�ƽ�ڼ䣬�ǲɼ������ڼ䣬Ҫ���ƽ����
// ���SCL�ߵ�ƽ�ڼ䣬��ƽ�����ı䣬����Ϊ��ʼ�źź�ֹͣ�ź�
// ��׼������ �� ����

void iic_start(void) ;

void iic_stop(void);

u8 iic_wait_ack(void) ;

void iic_ack(void);
void iic_nack(void);
  
  
void iic_send_byte(u8 byte);	//����һ��byte
u8 iic_read_byte(u8 ack);
u8 iic_soft_eeprom_byteRead_wordAddr(uint8_t id,uint16_t WordaddrToRead,uint8_t *store_buf,uint8_t count) ;//


#endif 





