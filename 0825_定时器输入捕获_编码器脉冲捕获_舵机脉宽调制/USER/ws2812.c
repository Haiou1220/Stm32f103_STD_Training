#include "ws2812.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "systick.h"
#include "global.h"
#include "string.h"
#include "stdlib.h"
 
  
 


	/*
��ɫRGB��ɫ
Blue RGB code = 0*65536+0*256+255 = #000000FF         0*65536+0*256+255 = #000000FF
��ɫRGB��ɫ
Red RGB code = 255*65536+0*256+0 = #00FF0000
��ɫRGB��ɫ
Green RGB code = 0*65536+255*256+0 = #0000FF00
*/

u8 get_B_color(RGB24BIT_t  rgb24bit)
{
	return (u8)rgb24bit ; 
}
u8 get_R_color(RGB24BIT_t rgb24bit )
{
	return (u8)(rgb24bit>>16) ; 
}
u8 get_G_color(RGB24BIT_t rgb24bit )
{
	return  (u8)(rgb24bit>>8);
}




   






void RGB24bitColorBuf_effect__sequence_add_del_color(RGB24BIT_t* RGB24bitcolor, u16 led_count)//  ˳�������ɫ  ˳��ɾ����ɫ
{
	//������һ��  
	static u8 R_numcode,G_numcode,B_numcode;
	static s32 color_add_loc;
 	static u8  FLAG_SWITCH_NUM = 0 ;
	
	const  u8  FALG_negativeDIR_add = 2,FLAG_negativeDIR_del = 3 ,FALG_positiveDIR_add = 0,FALG_positiveDIR_del= 1 ;


	
	u8 u8_numcode  = rand()% 256;
	u8 u8_numcode1  = rand()% 256;
	u8 u8_numcode2  = rand()% 256;
	
//	
	u8 u8_num_order = rand()% 3;
	
	R_numcode  = (u8_num_order==0)? 0 : u8_numcode;   G_numcode = (u8_num_order==1)? 0 : u8_numcode1;      B_numcode = (u8_num_order==2)? 0 :  u8_numcode2;
	
	switch(FLAG_SWITCH_NUM)
	{
	
		case   0://FALG_positiveDIR_add														//111111111111111111111111111111111111111111
		
		RGB24bitcolor[color_add_loc ] = R_numcode*65536+G_numcode*256+B_numcode;	
			
		color_add_loc++;
		if(color_add_loc  >= led_count)//���˱߽�  ��Чֵ
		{
			
			FLAG_SWITCH_NUM = FALG_positiveDIR_del  ;//��һ�� ���� ˳��ɾ��
			color_add_loc = 0;
		
		}			
			break;
		
		
		
		
		
		case   1: //FALG_positiveDIR_del ���� ˳��ɾ��
			
				RGB24bitcolor[color_add_loc ] = 0;
		     color_add_loc++;
		
				if(color_add_loc  >= led_count)//���˱߽�  ��Чֵ
				{
					
					FLAG_SWITCH_NUM = FALG_negativeDIR_add  ;//                 //��һ�� ���� ˳�� ��� //
					color_add_loc = led_count - 1;
				
				}	

		
			break;
				
				
				
				
		case   2://FALG_negativeDIR_add  ;//  ���� ˳�� ���
			
			RGB24bitcolor[color_add_loc ] = R_numcode*65536+G_numcode*256+B_numcode;	
			color_add_loc--;
		
				if(color_add_loc  < 0 )//���˱߽�  ��Чֵ
				{
					
					FLAG_SWITCH_NUM = FLAG_negativeDIR_del ;//��һ�� ���� ˳�� ɾ�� 
					color_add_loc = led_count - 1;
				
				}	
				
				
		
			break;
		case   3: //FLAG_negativeDIR_del ;//  ���� ˳�� ɾ�� 
			RGB24bitcolor[color_add_loc ] = 0;
		color_add_loc--;
						if(color_add_loc  < 0 )//���˱߽�  ��Чֵ
				{
					
					FLAG_SWITCH_NUM = FALG_positiveDIR_add  ;//��һ�� ���� ˳�� ��� 
					color_add_loc = 0;
				
				}	
	
			break;
				

	
	}
	
	
	



}

void RGB24bitColorBuf_effect__sequence_add_del_color1(RGB24BIT_t* RGB24bitcolor, u16 led_count)//  ˳�������ɫ  ˳��ɾ����ɫ
{
	//������һ��  
	static u8 R_numcode,G_numcode,B_numcode;
	static s32 color_add_loc;
 	static u8  FLAG_SWITCH_NUM = 0 ;
	
	const  u8  FALG_negativeDIR_add = 2,FLAG_negativeDIR_del = 3 ,FALG_positiveDIR_add = 0,FALG_positiveDIR_del= 1 ;


	
	u8 u8_numcode  = rand()% 256;
	u8 u8_numcode1  = rand()% 256;
	u8 u8_numcode2  = rand()% 256;
	
//	
	u8 u8_num_order = rand()% 3;
	
	R_numcode  = (u8_num_order==0)? 0 : u8_numcode;   G_numcode = (u8_num_order==1)? 0 : u8_numcode1;      B_numcode = (u8_num_order==2)? 0 :  u8_numcode2;
	
	switch(FLAG_SWITCH_NUM)
	{
	
		case   0://FALG_positiveDIR_add		 ���� ˳�� ���											//111111111111111111111111111111111111111111
		
		RGB24bitcolor[color_add_loc ] = R_numcode*65536+G_numcode*256+B_numcode;	
			
		color_add_loc++;
		if(color_add_loc  >= led_count)//���˱߽�  ��Чֵ
		{
			
			FLAG_SWITCH_NUM = FALG_positiveDIR_del  ;//��һ�� ���� ˳��ɾ��
			color_add_loc = 0;
		
		}			
			break;
		
		
		
		
		
		case   1: //FALG_positiveDIR_del ���� ˳��ɾ��
			
				RGB24bitcolor[color_add_loc ] = 0;
		     color_add_loc++;
		
				if(color_add_loc  >= led_count)//���˱߽�  ��Чֵ
				{
					
					FLAG_SWITCH_NUM = FALG_positiveDIR_add  ;// ��һ�� ������ ���              
					color_add_loc = 0; //index
				
				}	

		
			break;

	
	}
	
	
	



}











void RGB24bitColorBuf_effect_move(RGB24BIT_t* RGB24bitcolor, u16 led_count,u8 flag_dir_ForHead)//��������ǰ���γ�����Ч��
{
	u32 real_led_count = led_count-1;//���һ����dummy
	
	RGB24BIT_t headmost_LED,tail_LED;
	headmost_LED = RGB24bitcolor[real_led_count-1];// ������0..........real_led_count������         ��ǰ��� led��������
	tail_LED = RGB24bitcolor[0];//��һ��led
	
		if(flag_dir_ForHead)  //��ͷ��ǰ��
		{
			for(u16 i = real_led_count ;i > 0;i--) //������real_led_count........0������
			{
				RGB24bitcolor[i] = RGB24bitcolor[i-1]; //ǰ�� = ����
			
			}
			RGB24bitcolor[0]  = headmost_LED;
		}else
		{//��β�� �˻�
		
			for(u16 i = 0 ;i < real_led_count ;i++) //������0.......................real_led_count������
			{
				RGB24bitcolor[i] = RGB24bitcolor[i+1]; //���� = ǰ��  
			
			}
			RGB24bitcolor[real_led_count-1]  = tail_LED;
		
		}
		
}







void RGB24BIT_To_TIMxCCRx_Codebuf_AddResCode( RGB24BIT_t* rgb24bit  ,GRB_CCRx_t* GRB_CCRx_buf ,u16 LED_count, u8 light_percent )
{
 	const u8 code0 =  0X12;  //0;   //0.2*90 = 18 ��
    const u8 code1  = 0X48;  //1;   //0.8*90 = 72 ��
	 const u8 reset = 0x00;
 
	
	if(light_percent >100) {   light_percent = 100;}
	

		for(u16 i = 0 ;i<LED_count-1; i++) //һ��led
		{
			static u8 R,G,B;
			R = rgb24bit[i]>>16 ;//get_R_color(rgb24bit[i]);
			G = rgb24bit[i]>>8;            //get_G_color(rgb24bit[i]);
			B = rgb24bit[i] ;//get_B_color(rgb24bit[i]);
		
			for(u8 j = 0; j<8;j++)//  bit 
			{	
					GRB_CCRx_buf[i].R[j] = (  ( (u8)(  R * light_percent*0.01) )&( 0x80>>j ) )? code1:code0 ; //R
					GRB_CCRx_buf[i].G[j] = (  ( (u8)(  G * light_percent*0.01) )&( 0x80>>j ) )? code1:code0 ; //G
					GRB_CCRx_buf[i].B[j] = (  ( (u8)(  B * light_percent*0.01) )&( 0x80>>j ) )? code1:code0 ; //B	
			}
		}
		
		//����res ��
		
			for(u8 j = 0 ; j<8;j++)//  ���һ�� ���reset ��
			{		
					GRB_CCRx_buf[ LED_count-1 ].R[j] =  reset ; //R
					GRB_CCRx_buf[ LED_count-1 ].G[j] =  reset ; //G
					GRB_CCRx_buf[ LED_count-1 ].B[j] =  reset  ; //B
			}
		
	

}


void print_CCRxBuf_column24(GRB_CCRx_t* GRB_CCRx_buf ,u32 led_count) //��ӡ  ת��ֵ
{	
	 
	u8* p_CCRx =(u8*) GRB_CCRx_buf; // ������u8
	u32	led_showed_count = 0;
	
	for(u32 i =0 ; i < led_count*24; i++)
	{ 
		
		
		printf("%3d,",p_CCRx[i]   );
		
		if((i+1)%24 == 0)
		{		
			led_showed_count++;
			printf(" LED:%3d\r\n",led_showed_count);//һ��led �����

		}
	
		
	}


}
	
void sprint_CCRxBuf_column24( char* sprintf_str_buf,u32 sprintf_str_buf_len , GRB_CCRx_t* GRB_CCRx_buf ,u32 GRB_CCRx_buf_len,u32* sprintf_output_len) //��ӡ  ת��ֵ ���ַ���
{	
	 
	u32 temp_count;
	// char* old_sprintf_str_buf = sprintf_str_buf;
	u32  sprint_out_count = 0;
	u8* p_CCRx =(u8*) GRB_CCRx_buf; // ������u8
	u32	led_showed_count = 0;
	u32 led_count = GRB_CCRx_buf_len;
	
	
//	printf("old_sprintf_str_buf:%d\r\n",(u32)old_sprintf_str_buf);
//	printf("GRB_CCRx_buf_len:%d\r\n",GRB_CCRx_buf_len);
	
	for(u32 i =0 ; i < led_count*24; i++)
	{ 
		
		
//		printf("led:%d,sprint_out_count:%d,sprintf_str_buf ADDR:%d,sprintf_str_buf_len:%d\r\n",\
//					led_showed_count+1,sprint_out_count,       (u32)sprintf_str_buf,     sprintf_str_buf_len);
		
		temp_count = sprintf( sprintf_str_buf ,"%3d,",p_CCRx[i]   );
		sprint_out_count += temp_count ;
		sprintf_str_buf += temp_count;
		
		
		if((i+1)%24 == 0)
		{		
			led_showed_count++;
			temp_count = sprintf(sprintf_str_buf," LED:%3d\r\n",led_showed_count);//һ��led �����
			sprint_out_count += temp_count; 
			sprintf_str_buf += temp_count;
		}
	
		
	}
	
	temp_count =   sprintf(sprintf_str_buf,"\r\n");//һ��led �����
	sprint_out_count += temp_count; 
	sprintf_str_buf += temp_count;
	
	*sprintf_output_len = sprint_out_count;
	 
//	printf("new_sprintf_str_buf:%d\r\n",(u32)sprintf_str_buf);
//	printf("led:%d,sprint_out_count:%d,sprintf_str_buf ADDR:%d,sprintf_str_buf_len:%d\r\n",\
//					led_showed_count+1,sprint_out_count,       (u32)sprintf_str_buf,     sprintf_str_buf_len);
//	
	
	
	
	
}














void RGB24bitColorBuf_modifier_location_breathe(RGB24BIT_t* RGB24bitcolor,RGB24BIT_t* Return_RGB24bitcolor, u16 led_count  ,u32  location_1_0To299,float  loc1_breathe_gradually_0To1,u32  loc1_breathe_range)//�̶����߶�̬λ�ã�����Ч��
{
	 
//	
////	static u32  location_2;
////	static u32  location_3;
//	//static float  loc1_breathe_gradually_0To1;//����С������С  0-1
////	static float  loc2_breathe_gradually;
//////	static float  loc3_breathe_gradually;
////	static u32  loc1_breathe_range;//���� ��Χ С������С,����=0
////	static u32  loc2_breathe_range;//���� ��Χ С������С
////	static u32  loc3_breathe_range;//���� ��Χ С������С
//	
//	u32 temp_RGB,temp_R,temp_G,temp_B;
//	
////	u32 judge_rang ; 
//	
////	judge_rang = (location_1_0To299 >= loc1_breathe_range )?( location_1_0To299 - loc1_breathe_range):(0);
//	
////	u32 up_rang,down_rang;
// 
//if(location_1_0To299>299) location_1_0To299 = 299;

////����ȫ��λ��
//for(u32 x = 0 ;x < (led_count-1) ;x++)   //(led_count-1�ǳ�ȥdummy led)
//{
//	
// 	 Return_RGB24bitcolor[x] = 0;//��ǰ�޸� ����Ϊ����޸� �鷳 �����Ҳ���
//	
//	//�ҵ���Ҫ�޸ĵ�λ��
//	if(location_1_0To299 == x)
//	{			
//					//�� ��λ�ý��� ����Ч���޸�
//				for(u32 i = 0;i < loc1_breathe_range;i++)//���� ��Χ �Գ�
//				{
//				 
//		
//							if((location_1_0To299 + i) >  299 )
//							{
//							
//								goto negative_direction; 
//								
//							
//							}else
//							{
//							
//							//������
//							temp_RGB =RGB24bitcolor[location_1_0To299 + i] ;
//		
//		                    temp_R    = ( (temp_RGB & 0X00FF0000)>>16 )* ( loc1_breathe_gradually_0To1*(1.0*(i+1)/loc1_breathe_range )    );//ԽԶӰ��ԽС,Ӱ��ϵ��0-1 , Red RGB code = 255*65536+0*256+0 = #00FF0000
//							temp_G = ( (temp_RGB & 0X0000FF00 )>>8 )* ( loc1_breathe_gradually_0To1*(1.0*(i+1)/loc1_breathe_range )   ); //Green RGB code = 0*65536+255*256+0 = #0000FF00
//					    	temp_B = ( (temp_RGB & 0X000000FF )>>0 )* ( loc1_breathe_gradually_0To1*(1.0*(i+1)/loc1_breathe_range )    ); //Blue RGB code = 0*65536+0*256+255 = #000000FF
//							//�ó��޸�ֵ�٣��޸ĸ�λ��
//							Return_RGB24bitcolor[location_1_0To299 + i] = temp_R*65536+temp_G*256+temp_B;
//					 
//				         	//������
//							}
//		
//							negative_direction :
//				        	//������
//					
//							if( (location_1_0To299 < i  )) //������������,(������)  2 < 2 �ٵ�
//							{
//							         continue;//������һ��ѭ��
//							 
//								
//							}else{
//								
//								temp_RGB =RGB24bitcolor[location_1_0To299 - i] ;
//								
//							// loc1_breathe_gradually_0To1*(i/loc1_breathe_range) 
//						    temp_R    = ( (temp_RGB & 0X00FF0000)>>16 )*loc1_breathe_gradually_0To1*( (1.0*(i+1)/loc1_breathe_range)  )  ;//ԽԶӰ��ԽС,Ӱ��ϵ��0-1 , Red RGB code = 255*65536+0*256+0 = #00FF0000
//							temp_G = ( (temp_RGB & 0X0000FF00 )>>8 )* loc1_breathe_gradually_0To1*(    (1.0*(i+1)/loc1_breathe_range) ); //Green RGB code = 0*65536+255*256+0 = #0000FF00vvvvv----------------------------------(float)(i/loc1_breathe_range)
//					    	temp_B = ( (temp_RGB & 0X000000FF )>>0 )* loc1_breathe_gradually_0To1*( (1.0*(i+1)/loc1_breathe_range )    ); //Blue RGB code = 0*65536+0*256+255 = #000000FF
//							//�ó��޸�ֵ�٣��޸ĸ�λ��
//							Return_RGB24bitcolor[ location_1_0To299 - i] = temp_R*65536+temp_G*256+temp_B;
//							//Return_RGB24bitcolor[ location_1_0To299 - i] = 0XFF*65536+0XFF*256+0XFF;

//								
//								
//								
//							}
////																						
////					//������
////			  }
////	
////	
////    } 
////	
////	

//////	if(! (    ( x>= judge_rang  ) &&  ( x<= location_1_0To299+loc1_breathe_range  )     ))	// ����Ҫ�޸ĵ�λ�ã�����    �޸��˵�λ�ã�������(location_1_0To299-loc1_breathe_range......................location_1_0To299+loc1_breathe_range��������..............................................  
//////	{
//////	
//////	
//////	Return_RGB24bitcolor[x] = RGB24bitcolor[x];
//////	
//////	
//////	
//////	}
////	
//	
//	
//}

}





























