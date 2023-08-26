#include "ws2812.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "systick.h"
#include "global.h"
#include "string.h"
#include "stdlib.h"
 
  
 


	/*
蓝色RGB颜色
Blue RGB code = 0*65536+0*256+255 = #000000FF         0*65536+0*256+255 = #000000FF
红色RGB颜色
Red RGB code = 255*65536+0*256+0 = #00FF0000
绿色RGB颜色
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




   






void RGB24bitColorBuf_effect__sequence_add_del_color(RGB24BIT_t* RGB24bitcolor, u16 led_count)//  顺序添加颜色  顺序删除颜色
{
	//记忆上一次  
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
		if(color_add_loc  >= led_count)//过了边界  无效值
		{
			
			FLAG_SWITCH_NUM = FALG_positiveDIR_del  ;//下一步 正向 顺序删除
			color_add_loc = 0;
		
		}			
			break;
		
		
		
		
		
		case   1: //FALG_positiveDIR_del 正向 顺序删除
			
				RGB24bitcolor[color_add_loc ] = 0;
		     color_add_loc++;
		
				if(color_add_loc  >= led_count)//过了边界  无效值
				{
					
					FLAG_SWITCH_NUM = FALG_negativeDIR_add  ;//                 //下一步 反向 顺序 添加 //
					color_add_loc = led_count - 1;
				
				}	

		
			break;
				
				
				
				
		case   2://FALG_negativeDIR_add  ;//  反向 顺序 添加
			
			RGB24bitcolor[color_add_loc ] = R_numcode*65536+G_numcode*256+B_numcode;	
			color_add_loc--;
		
				if(color_add_loc  < 0 )//过了边界  无效值
				{
					
					FLAG_SWITCH_NUM = FLAG_negativeDIR_del ;//下一步 反向 顺序 删除 
					color_add_loc = led_count - 1;
				
				}	
				
				
		
			break;
		case   3: //FLAG_negativeDIR_del ;//  反向 顺序 删除 
			RGB24bitcolor[color_add_loc ] = 0;
		color_add_loc--;
						if(color_add_loc  < 0 )//过了边界  无效值
				{
					
					FLAG_SWITCH_NUM = FALG_positiveDIR_add  ;//下一步 正向 顺序 添加 
					color_add_loc = 0;
				
				}	
	
			break;
				

	
	}
	
	
	



}

void RGB24bitColorBuf_effect__sequence_add_del_color1(RGB24BIT_t* RGB24bitcolor, u16 led_count)//  顺序添加颜色  顺序删除颜色
{
	//记忆上一次  
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
	
		case   0://FALG_positiveDIR_add		 正向 顺序 添加											//111111111111111111111111111111111111111111
		
		RGB24bitcolor[color_add_loc ] = R_numcode*65536+G_numcode*256+B_numcode;	
			
		color_add_loc++;
		if(color_add_loc  >= led_count)//过了边界  无效值
		{
			
			FLAG_SWITCH_NUM = FALG_positiveDIR_del  ;//下一步 正向 顺序删除
			color_add_loc = 0;
		
		}			
			break;
		
		
		
		
		
		case   1: //FALG_positiveDIR_del 正向 顺序删除
			
				RGB24bitcolor[color_add_loc ] = 0;
		     color_add_loc++;
		
				if(color_add_loc  >= led_count)//过了边界  无效值
				{
					
					FLAG_SWITCH_NUM = FALG_positiveDIR_add  ;// 下一步 正方向 添加              
					color_add_loc = 0; //index
				
				}	

		
			break;

	
	}
	
	
	



}











void RGB24bitColorBuf_effect_move(RGB24BIT_t* RGB24bitcolor, u16 led_count,u8 flag_dir_ForHead)//搬运数据前后，形成流动效果
{
	u32 real_led_count = led_count-1;//最后一个是dummy
	
	RGB24BIT_t headmost_LED,tail_LED;
	headmost_LED = RGB24bitcolor[real_led_count-1];// 闭区间0..........real_led_count开区间         最前面的 led保存下来
	tail_LED = RGB24bitcolor[0];//第一个led
	
		if(flag_dir_ForHead)  //向头部前进
		{
			for(u16 i = real_led_count ;i > 0;i--) //闭区间real_led_count........0开区间
			{
				RGB24bitcolor[i] = RGB24bitcolor[i-1]; //前面 = 后面
			
			}
			RGB24bitcolor[0]  = headmost_LED;
		}else
		{//向尾部 退回
		
			for(u16 i = 0 ;i < real_led_count ;i++) //闭区间0.......................real_led_count开区间
			{
				RGB24bitcolor[i] = RGB24bitcolor[i+1]; //后面 = 前面  
			
			}
			RGB24bitcolor[real_led_count-1]  = tail_LED;
		
		}
		
}







void RGB24BIT_To_TIMxCCRx_Codebuf_AddResCode( RGB24BIT_t* rgb24bit  ,GRB_CCRx_t* GRB_CCRx_buf ,u16 LED_count, u8 light_percent )
{
 	const u8 code0 =  0X12;  //0;   //0.2*90 = 18 短
    const u8 code1  = 0X48;  //1;   //0.8*90 = 72 长
	 const u8 reset = 0x00;
 
	
	if(light_percent >100) {   light_percent = 100;}
	

		for(u16 i = 0 ;i<LED_count-1; i++) //一组led
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
		
		//增加res 码
		
			for(u8 j = 0 ; j<8;j++)//  最后一组 添加reset 码
			{		
					GRB_CCRx_buf[ LED_count-1 ].R[j] =  reset ; //R
					GRB_CCRx_buf[ LED_count-1 ].G[j] =  reset ; //G
					GRB_CCRx_buf[ LED_count-1 ].B[j] =  reset  ; //B
			}
		
	

}


void print_CCRxBuf_column24(GRB_CCRx_t* GRB_CCRx_buf ,u32 led_count) //打印  转码值
{	
	 
	u8* p_CCRx =(u8*) GRB_CCRx_buf; // 类型是u8
	u32	led_showed_count = 0;
	
	for(u32 i =0 ; i < led_count*24; i++)
	{ 
		
		
		printf("%3d,",p_CCRx[i]   );
		
		if((i+1)%24 == 0)
		{		
			led_showed_count++;
			printf(" LED:%3d\r\n",led_showed_count);//一个led 码完成

		}
	
		
	}


}
	
void sprint_CCRxBuf_column24( char* sprintf_str_buf,u32 sprintf_str_buf_len , GRB_CCRx_t* GRB_CCRx_buf ,u32 GRB_CCRx_buf_len,u32* sprintf_output_len) //打印  转码值 到字符串
{	
	 
	u32 temp_count;
	// char* old_sprintf_str_buf = sprintf_str_buf;
	u32  sprint_out_count = 0;
	u8* p_CCRx =(u8*) GRB_CCRx_buf; // 类型是u8
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
			temp_count = sprintf(sprintf_str_buf," LED:%3d\r\n",led_showed_count);//一个led 码完成
			sprint_out_count += temp_count; 
			sprintf_str_buf += temp_count;
		}
	
		
	}
	
	temp_count =   sprintf(sprintf_str_buf,"\r\n");//一个led 码完成
	sprint_out_count += temp_count; 
	sprintf_str_buf += temp_count;
	
	*sprintf_output_len = sprint_out_count;
	 
//	printf("new_sprintf_str_buf:%d\r\n",(u32)sprintf_str_buf);
//	printf("led:%d,sprint_out_count:%d,sprintf_str_buf ADDR:%d,sprintf_str_buf_len:%d\r\n",\
//					led_showed_count+1,sprint_out_count,       (u32)sprintf_str_buf,     sprintf_str_buf_len);
//	
	
	
	
	
}














void RGB24bitColorBuf_modifier_location_breathe(RGB24BIT_t* RGB24bitcolor,RGB24BIT_t* Return_RGB24bitcolor, u16 led_count  ,u32  location_1_0To299,float  loc1_breathe_gradually_0To1,u32  loc1_breathe_range)//固定或者动态位置，呼吸效果
{
	 
//	
////	static u32  location_2;
////	static u32  location_3;
//	//static float  loc1_breathe_gradually_0To1;//呼吸小到大再小  0-1
////	static float  loc2_breathe_gradually;
//////	static float  loc3_breathe_gradually;
////	static u32  loc1_breathe_range;//呼吸 范围 小到大再小,大于=0
////	static u32  loc2_breathe_range;//呼吸 范围 小到大再小
////	static u32  loc3_breathe_range;//呼吸 范围 小到大再小
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

////遍历全部位置
//for(u32 x = 0 ;x < (led_count-1) ;x++)   //(led_count-1是除去dummy led)
//{
//	
// 	 Return_RGB24bitcolor[x] = 0;//提前修改 ，因为最后修改 麻烦 区间找不到
//	
//	//找到需要修改的位置
//	if(location_1_0To299 == x)
//	{			
//					//在 该位置进行 呼吸效果修改
//				for(u32 i = 0;i < loc1_breathe_range;i++)//呼吸 范围 对称
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
//							//正方向
//							temp_RGB =RGB24bitcolor[location_1_0To299 + i] ;
//		
//		                    temp_R    = ( (temp_RGB & 0X00FF0000)>>16 )* ( loc1_breathe_gradually_0To1*(1.0*(i+1)/loc1_breathe_range )    );//越远影响越小,影响系数0-1 , Red RGB code = 255*65536+0*256+0 = #00FF0000
//							temp_G = ( (temp_RGB & 0X0000FF00 )>>8 )* ( loc1_breathe_gradually_0To1*(1.0*(i+1)/loc1_breathe_range )   ); //Green RGB code = 0*65536+255*256+0 = #0000FF00
//					    	temp_B = ( (temp_RGB & 0X000000FF )>>0 )* ( loc1_breathe_gradually_0To1*(1.0*(i+1)/loc1_breathe_range )    ); //Blue RGB code = 0*65536+0*256+255 = #000000FF
//							//得出修改值再，修改该位置
//							Return_RGB24bitcolor[location_1_0To299 + i] = temp_R*65536+temp_G*256+temp_B;
//					 
//				         	//正方向
//							}
//		
//							negative_direction :
//				        	//反方向
//					
//							if( (location_1_0To299 < i  )) //负，不作处理,(反方向)  2 < 2 假的
//							{
//							         continue;//继续下一个循环
//							 
//								
//							}else{
//								
//								temp_RGB =RGB24bitcolor[location_1_0To299 - i] ;
//								
//							// loc1_breathe_gradually_0To1*(i/loc1_breathe_range) 
//						    temp_R    = ( (temp_RGB & 0X00FF0000)>>16 )*loc1_breathe_gradually_0To1*( (1.0*(i+1)/loc1_breathe_range)  )  ;//越远影响越小,影响系数0-1 , Red RGB code = 255*65536+0*256+0 = #00FF0000
//							temp_G = ( (temp_RGB & 0X0000FF00 )>>8 )* loc1_breathe_gradually_0To1*(    (1.0*(i+1)/loc1_breathe_range) ); //Green RGB code = 0*65536+255*256+0 = #0000FF00vvvvv----------------------------------(float)(i/loc1_breathe_range)
//					    	temp_B = ( (temp_RGB & 0X000000FF )>>0 )* loc1_breathe_gradually_0To1*( (1.0*(i+1)/loc1_breathe_range )    ); //Blue RGB code = 0*65536+0*256+255 = #000000FF
//							//得出修改值再，修改该位置
//							Return_RGB24bitcolor[ location_1_0To299 - i] = temp_R*65536+temp_G*256+temp_B;
//							//Return_RGB24bitcolor[ location_1_0To299 - i] = 0XFF*65536+0XFF*256+0XFF;

//								
//								
//								
//							}
////																						
////					//反方向
////			  }
////	
////	
////    } 
////	
////	

//////	if(! (    ( x>= judge_rang  ) &&  ( x<= location_1_0To299+loc1_breathe_range  )     ))	// 不是要修改的位置，，，    修改了的位置：开区间(location_1_0To299-loc1_breathe_range......................location_1_0To299+loc1_breathe_range）开区间..............................................  
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





























