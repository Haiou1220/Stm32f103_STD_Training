

 #ifndef __I2C_HARD_H__
#define __I2C_HARD_H__

#include "stm32f10x.h"
 
 
#define I2CError(message)    I2CErrorHandle((uint8_t*)__FILE__,__LINE__,#message) 

void i2c1_init(void);  //i2c1 温湿度传感器 pb6-scl pb7-sda   24c02 a0=a1=a2=0 id=0XA0

u8 I2CErrorHandle(uint8_t* file, uint32_t line,char* message); //#define Error   Print

 u8 I2cHard_buf_read_NoAddr(uint8_t id,uint8_t* pbuf,uint8_t num_byte_to_read); //没地址读出buf

u8 I2cHard_buf_write_NoAddr(uint8_t id,uint8_t* pbuf,uint8_t num_byte_to_write); //没地址写入buf
	
	


#endif 

