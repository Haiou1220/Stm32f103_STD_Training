/**
  ******************************************************************************
  
1.DMA ��Ӧ�ã�ʵ�� dma�ڴ浽��������ݰ��ˣ���ʱ����pwm �ȽϼĴ�����dma�����������־λ������Ƚ�ƥ���־λ
2.dma��Ӧ�� ��ʵ�� dma �ڴ浽��������ݰ��ˣ�����1��DR �Ĵ����� dma���� �� �����־λ��TXE:�������ݼĴ�����
3.dmaӦ�ã�dma���ڴ浽�ڴ� ������
4.ws2812�ƴ�����һ����pwmƵ�ʵĶ�Ӧ����ɫ�����ת���������ɫ������޸����ڲ�����ͬЧ��
5.dma������ͨ�������ã�dma�жϺ���
6.���߼������ǿ��������ݣ��ʹ��ڴ�ӡд������ȽϼĴ��������ݣ�����ƥ��

����Ŀ��ʵ����ws2812�ƴ��Ķ�̬Ч����ƣ����ƶ�Ч���������Ч�������ȵ���Ч���л�����Ч��
���Դ��ڴ�ӡ�Ƚ�����Ĵ�����д�����ݣ���dmaʵ����3�����ֽڵ��봫���������Ҫ10����

�ô������֣���������0-9���飬���Բ鿴����ָ��������鿴���ڴ��ڼĴ������鿴dmaͨ���Ĵ����ȣ��������
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  

  
  ע�����1.globalͷ�ļ�����Ҫ��main�ļ���ȫ�ֱ��� ������globalͷ�ļ�����Ϊglobalͷ�ļ��Ǵ󲿷�c�ļ�  ���õĹ���ͷ�ļ� ��
		��� mianȫ�ֱ��� �޸ģ�globalͷ�ļ�û���޸�(Ҳû����Ҫ�޸�global.h������Ҫ�޸�main.h�ļ�)�����õ��ļ�����ͱ���
		  �Ľ��취��˭ʹ�þ��Լ� ������main.h �ļ�����Ϊmain.h�����ж��⿪�ŵı�������
	2.�ڵ����ļ��ڶ���ȫ�ֱ���ʱʹ��static����
	3.����ȫ�ֱ�����ֲ�������Ӧ��ʼ������ʹ�á�
	4.��ʱ��dma�� ���ʹ�ö�ʱ��������Ϊ����Ļ���ÿһ�δ������Ҫ���ñȽ�ֵΪ0 �������ĳ���� ��ֵ��ʱ������ʱ�Ѿ�������ߵ�ƽ�ˡ�����÷����ף�������Ƚ����ݺ� ���ڱȽ����Ϊ�ߵ�ƽ�� ����ɴ���ʱ���ñȽ�ֵΪ0��������һ��ߵ�ƽ��
	  �����ɺ�ر�ͨ��Ҳ���ܽ�������ۣ���ʹ�ø����¼���Ϊ����dma���¼�����
	
	4.Ӧʹ�ô����� �� �� ���鼰�ṹ��ķ�0��ʼ����   ��ʼ��Ϊ0��NULLʱ��ֻ��һ������ż��ɡ�
	5.�޷��������������������ܴ��ڱ�����������ʹ����Ŀ��������
	6.�������ں�������ʹ��goto ��䣬���ڳ��ִ���ʱͳһ��������������ֹʹ��goto 
	7. ��������ʱ��ʹ��UTF-8 ������Ա����ڲ�ͬ�༭������ʾ��������⡣
	8.����ǰ׺ ,ָ�� pFile, ���� fnCallBack ,�Ǿ�̬ȫ�ֱ��� gNumCount,��̬ȫ�ֱ��� s_iCount 
	9.��̬ȫ�ֱ��������Լ�ʹ�ã� �Ǿ�̬ȫ�ֱ��� �ⲿ�ļ�����
	
  �����ط���1.TIM_GenerateEvent(TIM3,TIM_EventSource_Update);// ����ڼ�������ʼ����֮ǰ������ͨ������TIMx_EGR�Ĵ����е�UGλ����ʼ�����еļĴ����� ���÷�
	2.��ʱ�� dma��ͨ������ ���и��±�־λ������Ƚ������־λ���� ������ 
   dma ���͵�ʱ��ע�� ����Ĵ����ĵ����ݿ�ȣ���Ȼ��ַ������������������Ŀ����ʵ�ʵĿ�ȵ�һ�룬��ô����Ĵ����ʹ洢���εĽ����ԭ��̫���,�Ҳ�Ӧ��dmaд�Ĵ����м�������֪����û��д���� 

	 

	
	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "global.h"
#include "systick.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
 
#include "ws2812.h"
#include "pwm_ws2812.h" 
#include "dma_ws2812.h"  
#include "dma_usart.h"  



/*Compiler definec : USE_STDPERIPH_DRIVER,STM32F10X_MD */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/




/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 

  
 //------------------------ϵͳȫ�ֱ���ͷ�ļ���������---------------------------------------- 
//  ������������ ���������� �ַ���־λ
 u8 gFlagUsartCMD_0,gFlagUsartCMD_1,gFlagUsartCMD_2,gFlagUsartCMD_3,gFlagUsartCMD_4,gFlagUsartCMD_5,gFlagUsartCMD_6,gFlagUsartCMD_7,gFlagUsartCMD_8,gFlagUsartCMD_9;
   // ��ʱ�����־λ
  uint8_t gFlagTime_1MS;
  
  u8 gDebug_u8;  //����
  u16 gDebug_u16; //���� 
 //------------------------ϵͳȫ�ֱ���ͷ�ļ���������----------------------------------------
 
 

//--------------------------------------������0	---------------------------------------------------

 //dma�ڴ� �������� test����
static u8 s_Dma_test_u8_source[10]={1,2,3,4,5,6,7,8,9,10};
u8 gDma_test_u8Buf_dest[10]  ; 
 
     
 
// //�ư�����
//GRB_CCRx_t GRB_CCRx_buf[ 64+1 ] = {0};//���һ���� DUMMY_COLOR    ,�ư� 64��led
//RGB24BIT_t RGB24bitColor_Buf[64+1]={
//0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,//1*8
//0x000080,0x000080,0x000080,0x000080,0x000080,0x000080,0x000080,0x000080,//2
//0x556B2F,0x556B2F,0x556B2F,0x556B2F,0x556B2F,0x556B2F,0x556B2F,0x556B2F,//3	
//0xFFE4E1,0xFFE4E1,0xFFE4E1,0xFFE4E1,0xFFE4E1,0xFFE4E1,0xFFE4E1,0xFFE4E1,//4
//0x1E90FF,0x1E90FF,0x1E90FF,0x1E90FF,0x1E90FF,0x1E90FF,0x1E90FF,0x1E90FF,	//5
//0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,//6
//0x8B4513,0x8B4513,0x8B4513,0x8B4513,0x8B4513,0x8B4513,0x8B4513,0x8B4513,//7
//0xFF1493,0xFF1493,0xFF1493,0xFF1493,0xFF1493,0xFF1493,0xFF1493,0xFF1493,	//8  
//DUMMY_COLOR	
//};

//�ƴ� ����
GRB_CCRx_t GRB_CCRx_buf[ 300+1 ] = {0};//���һ���� DUMMY_COLOR:reset��    ,�ƴ�300��led
RGB24BIT_t RGB24bitColor_Buf1[300+1]; 
RGB24BIT_t RGB24bitColor_Buf[300+1]={
	0xFFFAFA,0xF8F8FF,0xF5F5F5,0xDCDCDC,0xFFFAF0,0xFDF5E6,0xFAF0E6,0xFAEBD7,0xFFEFD5,0xFFEBCD,0xFFE4C4,0xFFDAB9,0xFFDEAD,0xFFE4B5,0xFFF8DC,0xFFFFF0,0xFFFACD,0xFFF5EE,0xF0FFF0,0xF5FFFA,0xF0FFFF,0xF0F8FF,
	0xE6E6FA,0xFFF0F5,0xFFE4E1,0xFFFFFF,0x2F4F4F,0x696969,0x708090,0x778899,0xBEBEBE,0xD3D3D3,0x191970,0x000080,0x6495ED,0x483D8B,0x6A5ACD,0x7B68EE,0x8470FF,0x0000CD,0x4169E1,0x0000FF,0x1E90FF,0x00BFFF,
	0x87CEEB,0x87CEFA,0x4682B4,0xB0C4DE,0xADD8E6,0xB0E0E6,0xAFEEEE,0x00CED1,0x48D1CC,0x40E0D0,0x00FFFF,0xE0FFFF,0x5F9EA0,0x66CDAA,0x7FFFD4,0x006400,0x556B2F,0x8FBC8F,0x2E8B57,0x3CB371,0x20B2AA,0x98FB98,
	0x00FF7F,0x7CFC00,0x00FF00,0x7FFF00,0x00FA9A,0xADFF2F,0x32CD32,0x9ACD32,0x228B22,0x6B8E23,0xBDB76B,0xEEE8AA,0xFAFAD2,0xFFFFE0,0xFFFF00,0xFFD700,0xEEDD82,0xDAA520,0xB8860B,0xBC8F8F,0xCD5C5C,0x8B4513,
	0xA0522D,0xCD853F,0xDEB887,0xF5F5DC,0xF5DEB3,0xF4A460,0xD2B48C,0xD2691E,0xB22222,0xA52A2A,0xE9967A,0xFA8072,0xFFA07A,0xFFA500,0xFF8C00,0xFF7F50,0xF08080,0xFF6347,0xFF4500,0xFF0000,0xFF69B4,0xFF1493,
	0xFFC0CB,0xFFB6C1,0xDB7093,0xB03060,0xC71585,0xD02090,0xFF00FF,0xEE82EE,0xDDA0DD,0xDA70D6,0xBA55D3,0x9932CC,0x9400D3,0x8A2BE2,0xA020F0,0x9370DB,0xD8BFD8,0xFFFAFA,0xEEE9E9,0xCDC9C9,0x8B8989,0xFFF5EE,
	0xEEE5DE,0xCDC5BF,0x8B8682,0xFFEFDB,0xEEDFCC,0xCDC0B0,0x8B8378,0xFFE4C4,0xEED5B7,0xCDB79E,0x8B7D6B,0xFFDAB9,0xEECBAD,0xCDAF95,0x8B7765,0xFFDEAD,0xEECFA1,0xCDB38B,0x8B795E,0xFFFACD,0xEEE9BF,0xCDC9A5,
	0x8B8970,0xFFF8DC,0xEEE8CD,0xCDC8B1,0x8B8878,0xFFFFF0,0xEEEEE0,0xCDCDC1,0x8B8B83,0xF0FFF0,0xE0EEE0,0xC1CDC1,0x838B83,0xFFF0F5,0xEEE0E5,0xCDC1C5,0x8B8386,0xFFE4E1,0xEED5D2,0xCDB7B5,0x8B7D7B,0xF0FFFF,
	0xE0EEEE,0xC1CDCD,0x838B8B,0x836FFF,0x7A67EE,0x6959CD,0x473C8B,0x4876FF,0x436EEE,0x3A5FCD,0x27408B,0x0000FF,0x0000EE,0x0000CD,0x00008B,0x1E90FF,0x1C86EE,0x1874CD,0x104E8B,0x63B8FF,0x5CACEE,0x4F94CD,
	0x36648B,0x00BFFF,0x00B2EE,0x009ACD,0x00688B,0x87CEFF,0x7EC0EE,0x6CA6CD,0x4A708B,0xB0E2FF,0xA4D3EE,0x8DB6CD,0x607B8B,0xC6E2FF,0xB9D3EE,0x9FB6CD,0x6C7B8B,0xCAE1FF,0xBCD2EE,0xA2B5CD,0x6E7B8B,0xBFEFFF,
	0xB2DFEE,0x9AC0CD,0x68838B,0xE0FFFF,0xD1EEEE,0xB4CDCD,0x7A8B8B,0xBBFFFF,0xAEEEEE,0x96CDCD,0x668B8B,0x98F5FF,0x8EE5EE,0x7AC5CD,0x53868B,0x00F5FF,0x00E5EE,0x00C5CD,0x00868B,0x00FFFF,0x00EEEE,0x00CDCD,
	0x008B8B,0x97FFFF,0x8DEEEE,0x79CDCD,0x528B8B,0x7FFFD4,0x76EEC6,0x66CDAA,0x458B74,0xC1FFC1,0xB4EEB4,0x9BCD9B,0x698B69,0x54FF9F,0x4EEE94,0x43CD80,0x2E8B57,0x9AFF9A,0x90EE90,0x7CCD7C,0x548B54,0x00FF7F,
	0x00EE76,0x00CD66,0x008B45,0x00FF00,0x00EE00,0x00CD00,0x008B00,0x7FFF00,0x76EE00,0x66CD00,0x458B00,0xC0FF3E,0xB3EE3A,0x9ACD32,0x698B22,0xCAFF70,0xBCEE68,0xA2CD5A,0x6E8B3D,0xFFF68F,0xEEE685,0xCDC673,
	0x8B864E,0xFFEC8B,0xEEDC82,0xCDBE70,0x8B814C,0xFFFFE0,0xEEEED1,0xCDCDB4,0x8B8B7A,0xFFFF00,0xEEEE00,0xCDCD00,0x8B8B00,0xFF000A,DUMMY_COLOR//0xFFD700
	
};

char str[] = "1234567890"  ; 

char sprintf_str[32000]={0};
u32 sprintf_str_out_num = 0;


//--------------------------------------������0	---------------------------------------------------
	



int main(void)
{
//------------------------������1----------------------  
//��ʱ����  ��ʱ�� ��־λ
 uint8_t  FlagTime_nMS,FlagTime_500MS,FlagTime_1Sec,FlagTime_3Sec;
  // s8 Light_per = 0; //ws2812 �����Ȱٷְ�0-100
 
	 
	
	
//------------------------������1----------------------
	
	
 

////////////////////////////   
  NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//�����жϷ���
  sysTick_init();//��ʼ��systick
  usart1_init(115200 ) ;//pa9 tx,  pa10 rx
  timer2_countdown_init();   //���ö�ʱ��2countdown�ж�
  Led1_Off;	
  Led0_Off;	
/////////////////////
	Info( MAIN FUN STAR UP); //��ӡϵͳ������ʾ
////////////////////
	
	//DMA1�ڴ浽�ڴ� ���� buf ,��ʼ�� ����������0 �����������dma �жϴ�ӡ���
    dma1_ch1_mem2mem_init();
 
    // ��ʱ��3���� ��pwm ws2812 ����  PB0,����ocr dma ����
    ws2812_PwmTIM3CH3_init( ); 
	
	 //��ʼ��DMA1CH2,������ CCR3 �Ƚϱ�־λ
	     ws2812_DMA1CH2_to_Tim3OCR3_init( );// tim3_ch2 	// (u32)&GRB_CCRx_buf ,  (sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0]))
	

	 

	//��ʼĬ�� ת�벢������ 
//	RGB24BIT_To_TIMxCCRx_Codebuf_AddResCode(  RGB24bitColor_Buf,GRB_CCRx_buf ,  (sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0])) ,100);// ת�� 
	// ws2812_DMA1CH2_for_TIM3OCR3_Send(  (u32)&GRB_CCRx_buf ,  ( sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0]) ) );//   1s ���ˢ��
 


 dma1CH4_USART1_TX_init();
 while ( 1 ) {
	 
  
	
	 
	//////////////////////-----------------------������������---------------------------------------//////////////////
 	if(gFlagUsartCMD_0 == SET)
 	{
		gFlagUsartCMD_0 = RESET;
	////////////////
		//------------------------------------------------------�Ľ�����Ϊ��ռ��cpu ����Ҫ��Ϊ����dma ����----------------------------------------
		
	  print_CCRxBuf_column24(GRB_CCRx_buf ,sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0])); //��ӡ  ת��ֵ չʾ
	  printf("\r\n");
		
		
		
    ///////////////////////////////////////////////////////////////////////////////////
	}

	if(gFlagUsartCMD_1 == SET)
 	{
		gFlagUsartCMD_1 = RESET;
	/////////////////////////////////////////////////
		
		//----------------------------------------------------------�ڴ浽�ڴ� dma ���ݰ���----------------------------
		printf("FUN:dma_mem2men sending u8_buf  ;\r\n");
		 //DMA MEM2MEM �������ݣ�dma1ch1�жϻ�Ӧ
		 dma1_ch1_mem2mem_u8_send((u32)&s_Dma_test_u8_source   ,  (u32)&gDma_test_u8Buf_dest ,sizeof(gDma_test_u8Buf_dest)/sizeof(gDma_test_u8Buf_dest[0])); //DMA MEM2MEM ��������ж�
		
		   //�ı�ԭ����
		 	 for(u16 i = 0 ; i<255; i++)
		 	{
		 	   s_Dma_test_u8_source[i]++ ;

		 	}
		
		

	///////////////////////////////////////////////////////////////////////////////////	
	}
	
	if(gFlagUsartCMD_2 == SET)
 	{
		gFlagUsartCMD_2 = RESET;
		//////////////////////////////
			//------------------------------------����1 dma ���ݰ���-------------------------------------------
		
	  printf("sprintf for CCRxBuf starting\r\n" )	;
	   	
	 sprint_CCRxBuf_column24(  sprintf_str,sizeof(sprintf_str)/sizeof(sprintf_str[0]) ,\
								 GRB_CCRx_buf ,  sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0]) ,\
									  &sprintf_str_out_num);    //��ӡ  ת��ֵ ���ַ���
								 
		 printf("sprintf for CCRxBuf :out_num:%d,str_capacity:%d\r\n",sprintf_str_out_num,sizeof(sprintf_str)/sizeof(sprintf_str[0]) )	;						 
								 
		printf(" usart1_sendBuf_dma1CH4_start: SEND LEN :%d \r\n ",    sizeof(sprintf_str)/sizeof(sprintf_str[0])     );   
		usart1_sendBuf_dma1CH4_start( (u32)&sprintf_str , sizeof(sprintf_str)/sizeof(sprintf_str[0])            );//���� //ʹ�ܴ��� ����DMA����
		
								 
    ///////////////////////////////////////////////////////////////////////////////////
	}

	
	if(gFlagUsartCMD_3 == SET)
	{
		gFlagUsartCMD_3 = RESET;
	 //////


	
		
		
		
 
		
		
		
    //////////////////////////////////////////////////////////////////////////////////////////		
	}
	
	if(gFlagUsartCMD_4 == SET)
	{
		gFlagUsartCMD_4 = RESET;
		//////////////////
	printf("------------------gFlagUsartCMD_4\r\n");
	 
		printf("get now USART1->CR3:%#2x \r\n",USART1->CR3);
		printf("USART1->DR:%#x \r\n ",USART1->CR3 );   //����
		 printf("DMA1_Channel4->CNDTR:%d \r\n ",DMA1_Channel4->CNDTR );   //����   
	 
		
   ///////////////////////////////////////////////////////////////////////////////////		
	}
	
	if(gFlagUsartCMD_5 == SET) //
	{
		gFlagUsartCMD_5 = RESET;
		/////
		printf("(------------------gFlagUsartCMD_5\r\n");
		static u8 SwitchBit = 0;
		
		if( SwitchBit)
		{
		printf("DMA USART TX DISABLE\r\n");
		USART1->CR3 &= ~(0x80);
			SwitchBit = ~SwitchBit;
			
		}else{
		
		printf("DMA USART TX ENABLE\r\n"); 
		USART1->CR3 |= (0x80);
		SwitchBit = ~SwitchBit;
		}
		
		
		
	///////////////////////////////////////////////////////////////////////////////////
	}
	
	if(gFlagUsartCMD_6 == SET) //
	{
		gFlagUsartCMD_6 = RESET;
		////
		
		
		
		
		
		
	///////////////////////////////////////////////////////////////////////////////////		
	}
	if(gFlagUsartCMD_7 == SET) ////
	{
		gFlagUsartCMD_7 = RESET;
		//////
 
		
///////////////////////////////////////////////////////////////////////////////////			 
	}	
	if(gFlagUsartCMD_8 == SET) //// 
	{
		gFlagUsartCMD_8 = RESET;
		//////
		
		
		
		
///////////////////////////////////////////////////////////////////////////////////		 
	}
	if(gFlagUsartCMD_9 == SET) 
	{
		gFlagUsartCMD_9 = RESET;
	//////////////////
    		

		
		
		
	 
////////////////////////////////////////////////////////////////////////////////////////////////	
	}
	
  
	
	
	
	
	
/***************************************��ʱ ����*************************************************************/ 
	if(FlagTime_nMS)
	{
		FlagTime_nMS = RESET;  		////////// Nms
		////////////////
 
		       
 		        RGB24bitColorBuf_effect__sequence_add_del_color1( RGB24bitColor_Buf1, sizeof(RGB24bitColor_Buf1)/sizeof(RGB24bitColor_Buf1[0])   );// ��ɫ����2 : 
 		      //  RGB24bitColorBuf_effect__sequence_add_del_color( RGB24bitColor_Buf1, sizeof(RGB24bitColor_Buf1)/sizeof(RGB24bitColor_Buf1[0])   );// ��ɫ����1:��� ˳����ɫ ��ɫ Ч��
		 		 //  RGB24bitColorBuf_effect_move(  RGB24bitColor_Buf,  (sizeof(RGB24bitColor_Buf)/sizeof(RGB24bitColor_Buf[0])  )  ,1 );			//   ��ɫ����0:�ƶ�Ч��
		
		
		
 	  	   RGB24BIT_To_TIMxCCRx_Codebuf_AddResCode(  RGB24bitColor_Buf1,GRB_CCRx_buf ,  (sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0])) ,100);// ת�� ,����100
//        //	RGB24BIT_To_TIMxCCRx_Codebuf_AddResCode(  RGB24bitColor_Buf,GRB_CCRx_buf ,  (sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0])) ,10);// ת�� ������10
   
		
 	  ws2812_DMA1CH2_for_TIM3OCR3_Send(  (u32)&GRB_CCRx_buf ,  ( sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0]) ) );//    ���ˢ�� CCRx ����

	
		
		
		
		
		
		
		
		
		
	}
/***************************************��ʱ ����*************************************************************/ 	
	////0.5s-����
	if(FlagTime_500MS)
	{
	FlagTime_500MS = RESET;
		//////////
	
	
	}
/***************************************��ʱ ����*************************************************************/ 
	////1s-����
	if(FlagTime_1Sec)
	{	
		static uint8_t data_count; //Ĭ��Ϊ0
		FlagTime_1Sec = RESET;
		data_count++;
		if(data_count % 3 == 2 ){  FlagTime_3Sec = SET;  }   //�� 0 1 2
		if(data_count>=255) { data_count = 0; }
		////////////////////////////////////////////////////
 
	}	
/***************************************��ʱ ����*************************************************************/ 		
	
	////3s-����
	if(FlagTime_3Sec)
	{
		FlagTime_3Sec = RESET;
		/////////////////////////////////////////////////
 
	     
	}
	 
/***************************************��ʱ ����*************************************************************/ 	
	//��ʱ��2��������
	if( gFlagTime_1MS == SET ) 
	{	
		static uint16_t count1000,countN,count500; //�ֲ���̬Ĭ��Ϊ0
		gFlagTime_1MS = RESET;
		count1000++;
		countN++;
		count500++;
		if(countN>=   10  )  //�Զ��� ms
		{
			countN = 0;
			FlagTime_nMS = SET;
		}
		if(count500>=500)  //0.5S�����־λ
		{
			count500 = 0;
			FlagTime_500MS = SET;
		}
		if(count1000>=1000)  ////1S�����־λ������������0................1000������
		{
			count1000 = 0;
			FlagTime_1Sec = SET;
		}
	 }
	
 
 
	 

 }
   
   
} 




