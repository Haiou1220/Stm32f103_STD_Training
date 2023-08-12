#include "global.h"
#include "stdio.h"
#include "stm32f10x.h"
 


void assert_failed(uint8_t* file, uint32_t line)
{
	printf("Error:%s:%d\r\n",file,line);
	while(1){};
}






//void  printShow_column_u16ArryBufNum( u16* u16Buf ,u16 count,u8 column)  //读出预览 old
//{
//	
////	printf("%s",charchar);
//u16 count_print_char = 0;	
//	
//for( u16 i = 0;i<count ;i++)
//{	
//	printf("%5d",u16Buf[i]);// 如何空格占位符
//	printf(" ");
//	count_print_char++;
//	if( ((i+1)%column == 0) ){ printf("\r\n");}
//}
//printf("\r\n");

//}

void  print_unsignedNum_BufShow_SetColumnm( u8* Buf ,u8 UnitOfByte ,u16 Unit_count,u8 column)  //读出预览
{
	

	
u8* Buf_u8 = (u8*)Buf;;
u16* Buf_u16 = (u16*)Buf;;
u32* Buf_u32 = (u32*)Buf;
	
u16 count_print_char = 0;	
	
	for( u16 i = 0;i<Unit_count ;i++)
	{	
		switch(UnitOfByte)
		{
			case 1:  //u8 
				printf("%5u",Buf_u8[i]);
				break;
			
			case 2:  //u16
				printf("%5u",Buf_u16[i]);
				break;
			
			case 4:  //u32
			printf("%5u",Buf_u32[i]);
				break;
			
			default:
				printf("Error UnitOfByte:%d",UnitOfByte );
			    i = Unit_count;
				break;
		}
		printf(" ");
		count_print_char++;
		if( ((i+1)%column == 0) ){ printf("\r\n");}
	}
	printf("\r\n");




}
		
void  print_floatDouble_BufShow_SetColumnm( u8* Buf ,u8 UnitOfByte ,u16 Unit_count,u8 column)  //读出预览
{
	
float* Buf_float = (float*)Buf;;
double* Buf_double = (double*)Buf;
u16 count_print_char = 0;	
	
	for( u16 i = 0;i<Unit_count ;i++)
	{	
		switch(UnitOfByte)
		{
			case 4:  //u8 
				printf("%0.5f",Buf_float[i]);
				break;
			
			case 8:  //u16
				printf("%0.5f",Buf_double[i]);
				break;
			
			default:
				printf("Error UnitOfByte:%d",UnitOfByte );
			    i = Unit_count;
				break;
		}
		printf(" ");
		count_print_char++;
		if( ((i+1)%column == 0) ){ printf("\r\n");}
	}
	printf("\r\n");

}

void PrintPinStatus(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin ,char* str)
{

	printf("%s,PinStatus:%d\r\n",str,GPIO_ReadInputDataBit( GPIOx, GPIO_Pin)); //输出模式下只能是开漏模式，才能读入
}





