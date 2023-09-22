 #ifndef __IIC_HARD_H__
#define  __IIC_HARD_H__

#include "stm32f10x.h"
 
 
 
#define I2CError(message)    I2CErrorHandle((uint8_t*)__FILE__,__LINE__,#message) 

void i2c1_init(void);  ////i2c1   pb6-scl pb7-sda   

u8 I2CErrorHandle(uint8_t* file, uint32_t line,char* message); //#define Error   Print

 u8 I2cHard_buf_read_NoAddr(uint8_t id,uint8_t* pbuf,uint8_t num_byte_to_read); //没地址读出buf

u8 I2cHard_buf_write_NoAddr(uint8_t id,uint8_t* pbuf,uint8_t num_byte_to_write); //没地址写入buf
	
	



#endif 
