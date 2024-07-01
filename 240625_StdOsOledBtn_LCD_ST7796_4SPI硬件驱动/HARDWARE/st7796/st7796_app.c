#include "st7796_app.h"
#include "st7796.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"


TaskHandle_t g_st7796Task_Handler; //export

//英文测试
void English_Font_test(void)
{	
    u8 size; 
    //st7796_set_back_color(RGB_WHITE);

   // st7796_set_paint_color(RGB_RED);	  
	//st7796_ShowString(10,30,200,12,12,"6X12:abcdefghijklmnopqrstuvwxyz");
	//st7796_ShowString(10,45,200,12,12,"6X12:ABCDEFGHIJKLMNOP0123456789");

	//st7796_set_paint_color(RGB_RED);
	//st7796_ShowString(10,60,8*strlen("8X16:abcdefghijklmnopqrstuvwx"),16,\
                                16,"8X16:abcdefghijklmnopqrstuvwxyz");
	//st7796_ShowString(10,80,8*strlen("8X16:ABCDEFGHIJKLMNOP0123456789"),16,\
                                16,"8X16:ABCDEFGHIJKLMNOP0123456789");

	st7796_set_paint_color(RGB_RED);
    size = 24;
	st7796_ShowString(10,((100+100+5+50)+5),(size/2)*18,size*1,\
            size,"12X24:7890123456789");
	st7796_ShowString(10,((100+100+5+50)+5)+size+3,(size/2)*strlen("12X24:ABCDEFGHIJKLMNOP0123456789"),size*1,\
             size,"12X24:ABCDEFGHIJKLMNOP0123456789");
	
}

//读颜色 测试
void Read_color_Test(void)
{
    char str[50];
    u16 point1_color,point2_color;
    u16 x0,x1,y0,y1;

    x0 = 100,y0=100;
    x1 = st7796_get_lcddev_width()-x0;
    y1= y0+50;
	st7796_ScreenFill(x0, y0,\
     x1,\
     y1,\
    RGB_RED);
    point1_color = st7796_ReadPoint_color(100,100);


    y0+=50;
    y1+=50;
    st7796_ScreenFill(x0, y0,\
     x1,\
     y1,\
    RGB_BLUE);
    point2_color = st7796_ReadPoint_color(x0,y0);

    vTaskDelay(pdMS_TO_TICKS(500));
       
    snprintf(str,sizeof(str),"p1_color:0x%04x,p2_color:0x%04x",point1_color,point2_color);

	st7796_set_paint_color(RGB_RED);
	st7796_ShowString(10,60,(16/2)*strlen(str),16*2,\
                                16,(u8*)str);

}

//读id 测试
void Read_id_Test(void)
{
    char str[50];
    u8 id[3]={0};

    id[0]=st7796_ReadIDn(1);
    vTaskDelay(pdMS_TO_TICKS(200));

    id[1]=st7796_ReadIDn(2);
    vTaskDelay(pdMS_TO_TICKS(200));

    id[2]=st7796_ReadIDn(3);
    vTaskDelay(pdMS_TO_TICKS(200));

    snprintf(str,sizeof(str),"id1:0x%02x,id2:0x%02x,id3:0x%02x",id[0],id[1],id[2]);

	st7796_set_paint_color(RGB_RED);
	st7796_ShowString(10,60+16+2,(16/2)*strlen(str),16*2,\
                                16,(u8*)str);

}

//绘图 测试
void paint_pic_Test(void)
{
    st7796_DrawRectangle_color(100,100+100+5,40,40,0XFC07);
     //st7796_DrawPic_bmp(100,100+100,gImage_1); //显示40*40图片,空间不够
}


void st7796_task(void *pvarg)
{
    
    st7796_bsp_init();
    st7796_init();
    st7796_Display_device_init(0);
    st7796_ScreenClear(RGB_BLACK);

    vTaskDelay(pdMS_TO_TICKS(1000)); 

     Read_color_Test();
    Read_id_Test();
    
    paint_pic_Test();
    English_Font_test();

    while (1)
    {        

     vTaskDelay(pdMS_TO_TICKS(1000));     
     printf(" in st7796_task \r\n");              

    }
}

