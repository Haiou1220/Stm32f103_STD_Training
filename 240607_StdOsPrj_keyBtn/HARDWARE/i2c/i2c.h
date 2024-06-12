#ifndef __I2C_H
#define __I2C_H
#include "stdint.h"
#include "stm32f10x.h"
// 写命令:////写字节 CTRL_BYTE  + //写字节 cmd
// 写数据 : 写字节 CTRL_BYTE  +  写字节 data
int I2C_Master_Transmit(I2C_TypeDef *I2C_instance, uint8_t DevAddress, uint8_t *pBuf, uint16_t Size, uint32_t Timeout);
int I2C_Mem_Write(I2C_TypeDef *I2C_instance, uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);


#endif

