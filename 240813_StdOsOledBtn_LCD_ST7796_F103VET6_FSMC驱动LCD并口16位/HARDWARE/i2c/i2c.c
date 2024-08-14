#include "i2c.h"
#include "stm32f10x.h"

int I2CError(I2C_TypeDef *I2C_instance)
{
	I2C_GenerateSTOP(I2C_instance, ENABLE);
	return -1;
}

int I2C_Mem_Write(I2C_TypeDef *I2C_instance, uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	uint32_t timeout = 0;

	if ((MemAddSize != 8) && (MemAddSize != 16))
	{
		return -1;
	}

	// 是否忙线
	{
		FlagStatus status;
		timeout = Timeout;
		do
		{
			status = I2C_GetFlagStatus(I2C_instance, I2C_FLAG_BUSY); // 0:空闲  1：忙碌
			if ((timeout--) == 0)
				return -1;
		} while (status == SET);
	}
	// 产生开始信号
	timeout = Timeout;
	I2C_GenerateSTART(I2C_instance, ENABLE);
	while (!I2C_CheckEvent(I2C_instance, I2C_EVENT_MASTER_MODE_SELECT)) ////  等待ev5
	{
		if ((timeout--) == 0)
			return I2CError(I2C_instance);
	}
	// 发送id地址
	timeout = Timeout;
	I2C_Send7bitAddress(I2C_instance, DevAddress, I2C_Direction_Transmitter);		  // 0，写方向
	while (!I2C_CheckEvent(I2C_instance, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) // 等待ev6
	{
		if ((timeout--) == 0)
			return I2CError(I2C_instance);
	}

	// 写字节地址
	/* If Memory address size is 8Bit */
	if (MemAddSize == 8)
	{
		/* Send LSB Memory Address */
		timeout = Timeout;
		I2C_SendData(I2C_instance, MemAddress & 0x00ff);
		while (!I2C_CheckEvent(I2C_instance, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 等待应答
		{
			if ((timeout--) == 0)
				return I2CError(I2C_instance);
		}
	}
	/* If Memory address size is 16Bit */
	else if (MemAddSize == 16)
	{
		/* Send MSB of Memory Address */
		timeout = Timeout;
		I2C_SendData(I2C_instance, (MemAddress >> 8) & 0x00ff);
		while (!I2C_CheckEvent(I2C_instance, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 等待应答
		{
			if ((timeout--) == 0)
				return I2CError(I2C_instance);
		}
		/* Wait until TXE flag is set */
		/* Send LSB of Memory Address */
		timeout = Timeout;
		I2C_SendData(I2C_instance, MemAddress & 0x00ff);
		while (!I2C_CheckEvent(I2C_instance, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 等待应答
		{
			if ((timeout--) == 0)
				return I2CError(I2C_instance);
		}
	}

	// 写字节数据
	for (int i = 0; i < Size; i++)
	{
		timeout = Timeout;
		I2C_SendData(I2C_instance, pData[i]);
		while (!I2C_CheckEvent(I2C_instance, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 等待应答
		{
			if ((timeout--) == 0)
				return I2CError(I2C_instance);
		}
	}

	// 停止信号
	I2C_GenerateSTOP(I2C_instance, ENABLE);

	return 0; // 成功
}

int I2C_Master_Transmit(I2C_TypeDef *I2C_instance, uint8_t DevAddress, uint8_t *pBuf, uint16_t Size, uint32_t Timeout)
{
	uint32_t timeout = 0;
	// 是否忙线
	{
		FlagStatus status;
		timeout = Timeout;
		do
		{
			status = I2C_GetFlagStatus(I2C_instance, I2C_FLAG_BUSY); // 0:空闲  1：忙碌
			if ((timeout--) == 0)
				return -1;
		} while (status == SET);
	}
	// 产生开始信号
	timeout = Timeout;
	I2C_GenerateSTART(I2C_instance, ENABLE);
	while (!I2C_CheckEvent(I2C_instance, I2C_EVENT_MASTER_MODE_SELECT)) ////  等待ev5
	{
		if ((timeout--) == 0)
			return I2CError(I2C_instance);
	}
	// 发送id地址
	timeout = Timeout;
	I2C_Send7bitAddress(I2C_instance, DevAddress, I2C_Direction_Transmitter);		  // 0，写方向
	while (!I2C_CheckEvent(I2C_instance, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) // 等待ev6
	{
		if ((timeout--) == 0)
			return I2CError(I2C_instance);
	}

	// 写字节
	for (int i = 0; i < Size; i++)
	{
		timeout = Timeout;
		I2C_SendData(I2C_instance, pBuf[i]);
		while (!I2C_CheckEvent(I2C_instance, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // 等待应答
		{
			if ((timeout--) == 0)
				return I2CError(I2C_instance);
		}
	}
	// 停止信号
	I2C_GenerateSTOP(I2C_instance, ENABLE);

	return 0; // 成功
}
