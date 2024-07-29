StdOsOledBtn_flash片内读写操作
    key_task,发送信号量,更新按键次数 KEY_PRESS_COUNT_STORE 的值到flash最后一个页区
    flash_internal_task,上电执行一次更新芯片复位次数 MCU_RESET_COUNT_STORE 数值
    定时器循环回调flashPrintvTimerCallback打印KEY_PRESS_COUNT_STORE,MCU_RESET_COUNT_STORE

FLASH_STM_SIZE 64(KB);
只能半字读写u16,解锁再上锁
组织块:中容量:64个1K字节/页(中容量)     IROM1: start:0x800 0000 size:0x1 0000(64KB)
最小擦除单位:页;每页1K字节(1024字节)
最小写入数据单位:半字
最大擦除单位:整片擦除
写入前,必须是已经擦除的,否则写入失败,并产生错误状态标志位