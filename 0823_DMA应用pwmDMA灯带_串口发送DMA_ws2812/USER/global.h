#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "stm32f10x.h"
#include "stdio.h"


#define  NumCountZero_1s   (100000*2 )   


#define  Num_DownToZero_1s   (100000 *2)   
 
#define  FUNC_FAULT  1
#define  FUNC_SUCCESS   0


	
	

void  print_unsignedNum_BufShow_SetColumnm( u8* Buf ,u8 UnitOfByte ,u16 Unit_count,u8 column);  //¶Á³öÔ¤ÀÀ
	
void  print_floatDouble_BufShow_SetColumnm( u8* Buf ,u8 UnitOfByte ,u16 Unit_count,u8 column);  //¶Á³öÔ¤ÀÀ


void PrintPinStatus(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin ,char* str);


#ifdef  USE_FULL_ASSERT
	#define ErrorInfo(messsge)  printf("Error:flle:%s,line:%d,Info:%s\r\n",__FILE__,__LINE__,#messsge)
	#define Info(messsge)  printf("Info:%s\r\n",#messsge)
#else
	#define ErrorInfo(messsge)  ((void)0)
	#define Info(messsge)  ((void)0)
#endif /* USE_FULL_ASSERT */


#endif


