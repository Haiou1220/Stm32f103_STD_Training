#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "stm32f10x.h"
#include "stdio.h"


#define  NumCountZero_1s   (100000*2 )   


#define  Num_DownToZero_1s   (100000 *2)   
 
#define  FUNC_FAULT  1
#define  FUNC_SUCCESS   0

//全局工程 变量
extern u8 Debug_count; //调试用
extern u16 Debug_count_WordAddrToWrite; //调试用

extern u8 FlagUsartCMD_0,FlagUsartCMD_1,FlagUsartCMD_2,FlagUsartCMD_3,FlagUsartCMD_4,FlagUsartCMD_5,FlagUsartCMD_6,FlagUsartCMD_7,FlagUsartCMD_8,FlagUsartCMD_9;
 
extern uint8_t FlagTime_1ms,FlagTime_1Sec,FlagTime_3Sec;





//void  printShow_column_u16ArryBufNum( u16* u16Buf ,u16 count,u8 column);  //读出预览 old

		
	

void  print_unsignedNum_BufShow_SetColumnm( u8* Buf ,u8 UnitOfByte ,u16 Unit_count,u8 column);  //读出预览
	
void  print_floatDouble_BufShow_SetColumnm( u8* Buf ,u8 UnitOfByte ,u16 Unit_count,u8 column);  //读出预览


void PrintPinStatus(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin ,char* str);


#ifdef  USE_FULL_ASSERT
	#define ErrorInfo(messsge)  printf("Error:flle:%s,line:%d,Info:%s\r\n",__FILE__,__LINE__,#messsge)
	#define Info(messsge)  printf("Info:%s\r\n",#messsge)
#else
	#define ErrorInfo(messsge)  ((void)0)
	#define Info(messsge)  ((void)0)
#endif /* USE_FULL_ASSERT */


#endif


