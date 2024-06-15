#include "stepper_drv.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "print.h"

//引脚定义
/*******************************************************/
//STEPPER 方向
#define STEPPER_DIR_PIN                       GPIO_Pin_11
#define STEPPER_DIR_GPIO_PORT                 GPIOB
#define STEPPER_DIR_GPIO_CLK_ENABLE()         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE)

//STEPPER 使能
#define STEPPER_EN_PIN                        GPIO_Pin_10
#define STEPPER_EN_GPIO_PORT                  GPIOB
#define STEPPER_EN_GPIO_CLK_ENABLE()           RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE)

//STEPPER 脉冲

#define STEPPER_PUL_PIN                       GPIO_Pin_1
#define STEPPER_PUL_GPIO_PORT                 GPIOB
#define STEPPER_PUL_GPIO_CLK_ENABLE()          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE)


/************************************************************/
static uint16_t s_TIM2_OC1_CCR_VAL =  0xffff /*默认最大 */; //500*1us =>500us高电平,500us翻转,1000us周期
                                                       //250*1us => 500us周期
                                                       //最大启动频率: 1.5k HZ = 0.66ms = 660 us
                                                       // 最大空载频率: 空载:1.9k Hz = 0.52ms = 520us

 inline void STEPPER_DIR(FlagStatus level) {GPIO_WriteBit(STEPPER_DIR_GPIO_PORT,STEPPER_DIR_PIN,(BitAction)((level == STEPPER_DIR_CW)?SET:RESET));}
 inline void STEPPER_EN(FlagStatus level) { GPIO_WriteBit(STEPPER_EN_GPIO_PORT,STEPPER_EN_PIN,(BitAction)((level == STEPPER_EN_ON)?RESET:SET));}
 inline void STEPPER_PUL(FlagStatus level) {GPIO_WriteBit(STEPPER_PUL_GPIO_PORT,STEPPER_PUL_PIN,(BitAction)level);    }

void stepper_drv_gpioCtlPin_init(void) //gpio en pin ,dir pin
{
    STEPPER_DIR_GPIO_CLK_ENABLE();
    STEPPER_EN_GPIO_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStruct.GPIO_Pin = STEPPER_DIR_PIN;
    GPIO_Init(STEPPER_DIR_GPIO_PORT,&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = STEPPER_EN_PIN;
    GPIO_Init(STEPPER_EN_GPIO_PORT,&GPIO_InitStruct);
    STEPPER_EN(STEPPER_EN_OFF);  
}

void stepper_drv_gpioSimulation_init(void) //gpio 模拟 ,摒弃
{
    STEPPER_PUL_GPIO_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStruct.GPIO_Pin = STEPPER_PUL_PIN;
    GPIO_Init(STEPPER_PUL_GPIO_PORT,&GPIO_InitStruct);
}

void stepper_drv_gpioSimulation_RotateAngle(uint32_t Angle,uint8_t subdivide, uint8_t direction) //gpio模拟,摒弃
{
    int32_t pulse = 0;
    pulse = Angle/1.8/subdivide;
    
    if((FlagStatus)direction == STEPPER_DIR_CW)
    {
        STEPPER_DIR(STEPPER_DIR_CW); 
    }else
    {
        STEPPER_DIR(STEPPER_DIR_ACW); 
    }
 
    STEPPER_EN(STEPPER_EN_ON);
  
    for(int i = 0;i < pulse;i++)
    {
                 
        STEPPER_PUL(RESET);
        vTaskDelay(pdMS_TO_TICKS(1));
        STEPPER_PUL(SET);
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    STEPPER_EN(STEPPER_EN_OFF);

}

void stepper_drv_timerOC_init(void) //timer2,channel1,pa0;
{ 
    //时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    //端口复用
     GPIO_InitTypeDef GPIO_InitStruct;
     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    //开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    /*时基配置*/
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1; //72M 定时器时钟频率 ,分频
    TIM_TimeBaseInitStruct.TIM_Period = 0xffff  /* 1000 */ /* 0xffff */;
    // 设置 arr重载值最大  ,计数器从0计数到自动加载值(TIMx_ARR计数器的内容)，然后重新从0开始
        //计数并且产生一个计数器溢出事件
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);

    //使能ARR 预装载
    TIM_ARRPreloadConfig(TIM2,ENABLE);    //区别 oc的ccr捕获比较寄存器与 ARR

    /*OC配置*/
    /*
    翻转模式:当Cnt = CCR(输出比较寄存器) 时候翻转电平,
    ARR 决定输出频率 ,CCRx 决定每个通道的初始相位.,重复修改ccr,在规定相位(间隔)发送翻转
    */
    TIM_OCInitTypeDef TIM_OCInitStruct;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle; 

    TIM_OCInitStruct.TIM_Pulse = s_TIM2_OC1_CCR_VAL; //设置ccr
         
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Disable /*  需要手动再开启 */;

    TIM_OC1Init(TIM2,&TIM_OCInitStruct);

    //TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);//CCR预装载,不要等待等更新触发事件,要立即更新
  
   //开启oc中断
    TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);

    //配置nvic
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn; //TIM2_IRQHandler
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; //优先级要最高
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    //定时器启动
    TIM_Cmd(TIM2,ENABLE);

}

void stepper_drv_timerOC_start(void) //timer2,channel1,pa0;
{
    TIM_CCxCmd(TIM2,TIM_Channel_1,TIM_CCx_Enable);          
}

void stepper_drv_timerOC_stop(void) //timer2,channel1,pa0;
{
    TIM_CCxCmd(TIM2,TIM_Channel_1,TIM_CCx_Disable);
    stepper_drv_timerOC_set_ccr((660*1.5)/2);//          
}

/*
 //500*1us =>500us高电平,500us翻转,1000us周期
//250*1us => 500us周期
//最大启动频率: 1.5k HZ = 0.66ms = 660 us =>  ccr=660/2
// 最大空载频率: 空载:1.9k Hz = 0.52ms = 520us
*/
void stepper_drv_timerOC_set_ccr(uint16_t ccr)
{   
    s_TIM2_OC1_CCR_VAL = ccr;         
}


void TIM2_IRQHandler(void)
{
   static  uint16_t tim2_cnt = 0;
   extern  uint32_t g_oled_count;

   
    if(SET == TIM_GetITStatus(TIM2,TIM_IT_CC1)) //oc中断 ch1 
    {        
        TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);
        
        tim2_cnt = TIM_GetCounter(TIM2);
        //s_TIM2_OC1_CCR_VAL       
        TIM_SetCompare1(TIM2,tim2_cnt + s_TIM2_OC1_CCR_VAL);
      
    }
}

