#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "stm32f10x.h"

#define ID_Device_EEPROM_24C02 0xA0 //// �ڲ� 24c02ģ�� id ok ,page:256 ,ÿpage��8�ֽ�
#define ID_Device_EEPROM_256K  0xAE ////// 24c246ģ��  , page:512   ÿpage:64�ֽ� ��WordAddr15Bit

void eeprom_init(void);  //i2c1   pb6-scl pb7-sda 

u8 eeprom_byteRead_byteAddr(uint8_t id,uint8_t addrToRead,uint8_t *store_buf,uint8_t count_read);  //
u8 eeprom_byteWrite_byteAddr(uint8_t id,uint8_t addrToWrite,uint8_t Data);  //
u8 eeprom_bufWrite_byteAddr(uint8_t id,uint8_t addrToWrite,uint8_t *bufWriteData,uint8_t count);  

u8 eeprom_byteRead_wordAddr(uint8_t id,uint16_t WordaddrToRead,uint8_t *store_buf,uint8_t count) ;
u8 eeprom_byteWrite_wordAddr(uint8_t id,uint16_t WordAddrToWrite,uint8_t byteData);  
u8 eeprom_bufWrite_wordAddr(uint8_t id,uint16_t WordAddrToWrite,uint8_t *bufWriteData,uint8_t count)  ;
u8 eeprom_bufRead_wordAddr(uint8_t id,uint16_t WordAddrToRead,uint8_t *bufRead_Store,uint8_t count) ;// 



 




#endif
