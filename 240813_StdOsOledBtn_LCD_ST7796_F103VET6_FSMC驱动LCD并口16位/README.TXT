项目:LCD ST7796 fsmc 驱动  并口16位

改善地方:
硬件i2c 与 fsmc 有冲突,目前关闭oled模块,lcd才能正常工作
下一步工作:
1_fsmc驱动lcd的基础上,移植lvgl
2_触摸芯片的驱动
3_i2c改为dma,再尝试是否还有冲突

笔记:
CS  => FSMC_NE1_PD7 :

FSMC_NE1 0X6000,0000~63FF,FFFF
[27:26]=[0,0] FSMC_A[25:0]

RS 对应 PD11 即 FSMC_A16;
//使用NOR/SRAM的 Bank1.sector1,地址位HADDR[27,26]=00 A16作为数据命令区分线
//不论外部接 8 位/16 位宽设备， FSMC_A[0]永远接在外部设备地址 A[0] ,由于内部硬件u16的寻址原因 ,舍弃bit0,硬件把bit1解析为bit0,相当于addr右偏移一位(既是乘以2)
//u16访问地址访问是2n,不关心bit0
//RS_BIT_A16 =0 是命令,RS_BIT_A16 =1 是数据

RS_BIT=0 是命令
RS_BIT=1 是数据
不论外部接 8 位/16 位宽设备， FSMC_A[0]永远接在外部设备地址 A[0]
16位时候,访问是u16,所以访问地址访问是2n,不关心bit0

fsmc主要配置 三个寄存器: FSMC_BCRx(片选控制)、 FSMC_BTRx(读)、 FSMC_BWTRx(写) 
FSMC_NORSRAMInit

对于[fsmc写时间配置 主要是 AddressSetupTime 与 DataSetupTime] ,[fsmc读时间配置 主要是 AddressSetupTime 与 DataSetupTime ]
其中的fsmc写时间配置我都是一个一个修改才能有好的显示效果,不然就花屏或者死屏,不知道如何准确配置























{                  
https://freertoskernel.asicfans.com/
 /*等待所有任务到达各自的同步点。*/
        xEventGroupSync(/* 用于同步的事件组。 */
                        xEventGroup,
            
                        /* 该任务设置的位，表示它已到达同步点。 */
                        uxThisTasksSyncBit,
            
                        /*需要等待的比特，每个参与同步的任务都有一个位*/
                        (mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainTHIRD_TASK_BIT),
            
                        /*无限等待所有三个任务到达同步点*/
                        portMAX_DELAY);

    /*取递归互斥锁。 */
    if( xSemaphoreTakeRecursive( xRecursiveMutex, xMaxBlock20ms ) == pdPASS )
    {
        /*成功获取递归互斥锁。任务现在可以访问互斥锁保护的资源。此时，递归调用计数(即对xSemaphoreTakeRecursive()的嵌套调用的数量)为1，因为递归互斥对象只被取过一次。* /
        
        /*当它已经持有递归的互斥对象时，任务再次接受互斥对象。在实际的应用程序中这只可能发生在该任务调用的子函数中，因为没有实际的理由要多次使用同一个互斥锁。\
        调用任务已经是互斥锁的持有者，所以对xSemaphorerakeRecursive()的第二次调用只是将递归调用的计数增加到2。*/
        xSemaphoreTakeRecursive( xRecursiveMutex, xMaxBlock20ms );
        
        /* ... */
        /*任务在访问了互斥锁保护的资源后返回互斥锁。此时，递归调用计数为2，因此对xSemaphoreGiveRecursive()的第一次调用不返回互斥锁。相反它只是将递归调用计数减回1。*/
        xSemaphoreGiveRecursive( xRecursiveMutex );
        
        /*再次调用xSemaphoreGiveRecursive()时，返回的调用计数减少为0。所以这次返回的是递归互斥锁。*/
        xSemaphoreGiveRecursive( xRecursiveMutex );
        
        /*现在开始每次调用 xSemaphoreTakeRecursive()都会执行获取互斥锁的动作，因为任务不再是互斥锁的持有者。*/
    }                    
}        

