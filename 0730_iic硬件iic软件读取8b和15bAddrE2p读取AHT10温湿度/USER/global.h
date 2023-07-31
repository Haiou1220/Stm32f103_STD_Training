#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "stm32f10x.h"
#include "stdio.h"


#define  NumCountZero_1s   (100000*2 )   


#define  Num_DownToZero_1s   (100000 *2)   
 

extern u8 Debug_count;
extern u16 Debug_count_WordAddrToWrite;

#ifdef  USE_FULL_ASSERT
	#define ErrorInfo(messsge)  printf("Error:flle:%s,line:%d,Info:%s\r\n",__FILE__,__LINE__,#messsge)
	#define Info(messsge)  printf("Info:%s\r\n",#messsge)
#else
	#define ErrorInfo(messsge)  ((void)0)
	#define Info(messsge)  ((void)0)
#endif /* USE_FULL_ASSERT */


#endif


