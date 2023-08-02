

 #ifndef __I2C_HARD_H__
#define __I2C_HARD_H__

#include "stm32f10x.h"
 
 
#define I2CError(message)    I2CErrorHandle((uint8_t*)__FILE__,__LINE__,#message) 

u8 I2CErrorHandle(uint8_t* file, uint32_t line,char* message); //#define Error   Print

 u8 I2cHard_buf_read_NoAddr(uint8_t id,uint8_t* pbuf,uint8_t num_byte_to_read); //没地址读出buf

u8 I2cHard_buf_write_NoAddr(uint8_t id,uint8_t* pbuf,uint8_t num_byte_to_write); //没地址写入buf
	
	

u8 eeprom_byteRead_byteAddr(uint8_t id,uint8_t addrToRead,uint8_t *store_buf,uint8_t count_read);  //
u8 eeprom_byteWrite_byteAddr(uint8_t id,uint8_t addrToWrite,uint8_t Data);  //
u8 eeprom_bufWrite_byteAddr(uint8_t id,uint8_t addrToWrite,uint8_t *bufWriteData,uint8_t count);  

u8 eeprom_byteRead_wordAddr(uint8_t id,uint16_t WordaddrToRead,uint8_t *store_buf,uint8_t count) ;
u8 eeprom_byteWrite_wordAddr(uint8_t id,uint16_t WordAddrToWrite,uint8_t byteData);  
u8 eeprom_bufWrite_wordAddr(uint8_t id,uint16_t WordAddrToWrite,uint8_t *bufWriteData,uint8_t count)  ;
u8 eeprom_bufRead_wordAddr(uint8_t id,uint16_t WordAddrToRead,uint8_t *bufRead_Store,uint8_t count) ;// 
#endif 

