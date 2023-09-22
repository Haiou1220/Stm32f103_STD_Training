#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "stm32f10x.h"

#define ID_Device_EEPROM_24C02 0xA0 //// 内部 24c02模块 id ok ,page:256 ,每page：8字节
#define ID_Device_EEPROM_256K  0xAE ////// 24c246模块  , page:512   每page:64字节 ，WordAddr15Bit

void eeprom_init(void);  //i2c1   pb6-scl pb7-sda 

u8 eeprom_byteRead_byteAddr(uint8_t id,uint8_t addrToRead,uint8_t *store_buf,uint8_t count_read);  //
u8 eeprom_byteWrite_byteAddr(uint8_t id,uint8_t addrToWrite,uint8_t Data);  //
u8 eeprom_bufWrite_byteAddr(uint8_t id,uint8_t addrToWrite,uint8_t *bufWriteData,uint8_t count);  

u8 eeprom_byteRead_wordAddr(uint8_t id,uint16_t WordaddrToRead,uint8_t *store_buf,uint8_t count) ;
u8 eeprom_byteWrite_wordAddr(uint8_t id,uint16_t WordAddrToWrite,uint8_t byteData);  
u8 eeprom_bufWrite_wordAddr(uint8_t id,uint16_t WordAddrToWrite,uint8_t *bufWriteData,uint8_t count)  ;
u8 eeprom_bufRead_wordAddr(uint8_t id,uint16_t WordAddrToRead,uint8_t *bufRead_Store,uint8_t count) ;// 



 




#endif
