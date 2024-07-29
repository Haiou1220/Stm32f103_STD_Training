#include "print.h"
#include "stdio.h"
#include "stdarg.h"
#include "FreeRTOS.h"
#include "stm32f10x.h"


int print(const char *format, ...)
{
    portENTER_CRITICAL();
    int ret;
    va_list argv;
    va_start(argv, format); // format是可变参数列表最后一个参数(栈低),是第一个函数的参数
    ret = vprintf(format, argv);
    va_end(argv);
    portEXIT_CRITICAL();
    return ret;
}

int fputc(int c,FILE* f)//重定向标准库printf输出
{	
  uint32_t count = 0xffff; //超时退出																																			  
	while ( (USART_GetFlagStatus(USART1,USART_FLAG_TXE) != SET)  && (count--)) {}; //等待TXE=1发送寄存器空，表示发送完成，由硬件复位，TC与TXE复位值为1
	USART_SendData(  USART1,  c);	
	return c;
}

  void assert_check(uint8_t* file_name,uint32_t line)
  {
		//portENTER_CRITICAL();	
		print("*** error:file:%s,line:%d\r\n",(char*)file_name,line);	
		//portEXIT_CRITICAL();
		while(1)
    {
     //do nothing ,方便debug
      file_name = file_name;
      line = line;
    };	//有错误就在此循环
  }

#ifdef  USE_FULL_ASSERT
  void assert_failed(uint8_t* file, uint32_t line)
  {
    assert_check(file,line);
  }
#endif /* USE_FULL_ASSERT */

