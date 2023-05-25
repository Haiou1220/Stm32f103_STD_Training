#ifndef _MAIN_H_
#define _MAIN_H_
#include "stm32f10x.h"
typedef struct
{
	char recvBuf[50];
	u8 recvLen;
	u8 FlagRecvCompl;
}RS485Recv_t;

#endif

