

 #ifndef __IIC_SOFT_EEPROM_H__
#define  __IIC_SOFT_EEPROM_H__

#include "stm32f10x.h"
 
 
u8 iic_soft_eeprom_byteRead_wordAddr(uint8_t id,uint16_t WordaddrToRead,uint8_t *store_buf,uint8_t count) ;//


#endif 





