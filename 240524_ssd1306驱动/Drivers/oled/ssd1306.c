
#include "ssd1306.h"
#include "stm32f1xx_hal.h"

#define SSD1306_I2C_ADDR 0x78 // 0 1 1 1 1 0 SA0:0 R/W:#  SA0电阻拉低 R/W:0写 1读
#define OLED_TIMEOUT 500

typedef enum
{
    H_ADDR_MODE = 0,    // 水平地址模式
    V_ADDR_MODE = 1,    // 垂直地址模式
    PAGE_ADDR_MODE = 2, // 页地址模式
} MEM_MODE;             // 内存地址模式

static MEM_MODE mem_mode = PAGE_ADDR_MODE; // 静态局部变量，保存OLED的地址模式的

extern I2C_HandleTypeDef hi2c1;

static I2C_HandleTypeDef *g_pHL_OLED = &hi2c1;

/* 控制字节: Co  D/C# 0 0 0 0 0 0 */
int SSD1306_WriteCmd(uint8_t cmd)
{
    uint8_t byte_buf[2] = {0};
    uint8_t *p_ctrl_byte = &byte_buf[0];
    uint8_t *p_data_byte = &byte_buf[1];

    *p_ctrl_byte = 0; // Co=0:仅发送数据字节,控制字节不发送       D/C#=0;发命令
    *p_data_byte = cmd;

    return HAL_I2C_Master_Transmit(g_pHL_OLED, SSD1306_I2C_ADDR, byte_buf, sizeof(byte_buf), OLED_TIMEOUT);
    // HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmd,1,0x100);
}

int SSD1306_WriteData(uint8_t data)
{
    uint8_t byte_buf[2] = {0};
    uint8_t *p_ctrl_byte = &byte_buf[0];
    uint8_t *p_data_byte = &byte_buf[1];

    *p_ctrl_byte = 0x40; // D/C#=1;发数据
    *p_data_byte = data;

    return HAL_I2C_Master_Transmit(g_pHL_OLED, SSD1306_I2C_ADDR, byte_buf, sizeof(byte_buf), OLED_TIMEOUT);
}

int SSD1306_WriteNBytes(uint8_t *buf, uint16_t length)
{
    // byte[0]是CtrlByte ,其中D/C#=1;发数据
    return HAL_I2C_Mem_Write(g_pHL_OLED, SSD1306_I2C_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, buf, length, OLED_TIMEOUT);
}

/*
 点阵:128*64 bits ,一共8个page
1个page:
seg0       seg127
D0           D0   COM0
.						.
.						.
D7           D7   COM7
(取模时候低位在前)
page addressing mode:
col_pointer 列地址,自动加一,到头自动复位
page_pointer 不会变,修改page的命令是B0--B7


1page的bits 等于 8 COM * 128 SEG
 显示高为16bit的字模需要 跨越2 page
*/
/*page addressing mode:下的设置命令:地址模式设置:  地址设置:B0-B7,00-0F,10-1F*/

void OLED_SetMemAddrMode(void)
{
    MEM_MODE mode = PAGE_ADDR_MODE;
    if ((mode != H_ADDR_MODE) && (mode != V_ADDR_MODE) && (mode != PAGE_ADDR_MODE))
        return;
    SSD1306_WriteCmd(0x20);
    SSD1306_WriteCmd(mode);
}

void OLED_SetStart_PageAddr(uint8_t addr)
{
    if (mem_mode != PAGE_ADDR_MODE)
        return;
    if (addr > 7)
        return;
    SSD1306_WriteCmd(0xB0 + addr);
}

void OLED_SetStart_ColumnAddr(uint8_t addr)
{
    uint8_t lower, upper;
    lower = addr & 0x0F;
    upper = (addr >> 4) & 0x0F;

    if (mem_mode != PAGE_ADDR_MODE)
        return;
    if (addr > 127)
        return;
    SSD1306_WriteCmd(0X00 + lower);
    SSD1306_WriteCmd(0X10 + upper);
}

void OLED_SetPosition(uint8_t page, uint8_t col)
{
    OLED_SetStart_PageAddr(page);
    OLED_SetStart_ColumnAddr(col);
}

//
void OLED_Clear(void)
{
    uint8_t i = 0;
    uint8_t buf[128] = {0};

    for (i = 0; i < 8; i++)
    {
        OLED_SetPosition(i, 0);
        SSD1306_WriteNBytes(&buf[0], 128);
    }
}

void OLED_init(void)
{

    /*关闭显示*/
    SSD1306_WriteCmd(0xAE); //--turn off oled panel

    //  (0x00); //---set low column address
    //  (0x10); //---set high column address

    /*3. 设置起始行*/
    SSD1306_WriteCmd(0x40); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)

    /*7. 设置对比度*/
    SSD1306_WriteCmd(0x81); //--set contrast control register
    SSD1306_WriteCmd(0x7F); // Set SEG Output Current Brightness  //0xCF

    /*4. 行翻转 column address 0 is mapped to SEG0 (RESET)*/
    SSD1306_WriteCmd(0xA0); //--Set SEG/Column Mapping
                            // 0xA1  //左右镜像0xA0 0xA1
    /* 5. 正常扫描 normal mode (RESET) Scan from COM0 to COM[N –1]*/
    SSD1306_WriteCmd(0xC0); // Set COM/Row Scan Direction
                            // 0xC0 上下镜像0xC80xC0   A1&C8  A0&C0

    // LCD_WRITE_CMD(0xA6); //--set normal display

    /*1. 设置多路复用率*/
    SSD1306_WriteCmd(0xA8); //--set multiplex ratio(1 to 64)
    SSD1306_WriteCmd(0x3f); //--1/64 duty

    /*2. 设置显示的偏移值*/
    SSD1306_WriteCmd(0xD3); //-set display offset  Shift Mapping RAM Counter (0x00~0x3F)
    SSD1306_WriteCmd(0x00); //-not offset

    /*10. 设置分频系数和频率增值*/
    SSD1306_WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    SSD1306_WriteCmd(0x80); //--set divide ratio, Set Clock as 100 Frames/Sec

    SSD1306_WriteCmd(0xD9); //--set pre-charge period
    SSD1306_WriteCmd(0xF1); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

    /* 6. COM 引脚设置 ,0xC0时:Scan from COM0 to COM[N –1],A5 remap,A4 alt,A5A4=00,A5A4=01,A5A4=10,A5A4=11,*/
    SSD1306_WriteCmd(0xDA);                       //--set com pins hardware configuration
    SSD1306_WriteCmd(0x02 + (0 << 5) + (1 << 4)); // (0x12);

    // LCD_WRITE_CMD(0xDB); //--set vcomh
    //  LCD_WRITE_CMD(0x40); //Set VCOM Deselect Level

    /* 0. 设置地址模式*/
    // (0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
    //  (0x02);
    OLED_SetMemAddrMode();

    /*11. 使能电荷碰撞*/
    SSD1306_WriteCmd(0x8D); //--set Charge Pump enable/disable
    SSD1306_WriteCmd(0x14); //--set(0x10) disable

    /*8. 全屏熄灭*/
    SSD1306_WriteCmd(0xA4); // Disable Entire Display On (0xa4/0xa5)

    /*9. 显示模式,阳码显示*/
    SSD1306_WriteCmd(0xA6); // Disable Inverse Display On (0xa6 reset, /a7)

    /*打开显示*/
    SSD1306_WriteCmd(0xAF); //--turn on oled panel
}

void OLED_ShowChar(uint8_t x, uint8_t y, char c)
{
    /*8*16 font  max:y=8,x=16*/

    extern const uint8_t ascii_font[128][16];
    if (y > 7 || x > 15)
        return;

    uint8_t page = y;
    uint8_t col = x * 8;

    OLED_SetPosition(page, col);
    SSD1306_WriteNBytes((uint8_t *)&ascii_font[c][0], 8);
    OLED_SetPosition(page + 1, col);
    SSD1306_WriteNBytes((uint8_t *)&ascii_font[c][0] + 8, 8);

    //  OLED_SetPosition(page, col);
    // SSD1306_WriteNBytes((uint8_t*)&ascii_font[c][0], 8);
    // OLED_SetPosition(page + 1, col);
    // SSD1306_WriteNBytes((uint8_t*)&ascii_font[c][8], 8);
}

int OLED_ShowString(uint8_t x, uint8_t y, const char *buf)
{

    int8_t i = 0;

    while (buf[i])
    {

        OLED_ShowChar(x, y, buf[i]);
        x++;
        if (x > 15)
        {
            x = 0;
            y += 2;
        }
        i++;
    }
    return i;
}

int OLED_PrintSignedVal(uint8_t x, uint8_t y, int32_t val)
{
    char str[16];
    char revert_str[16];
    int i = 0, j = 0;
    int mod;

    if (val < 0)
    {
        str[i++] = '-';
        val = 0 - val;
    }
    else if (val == 0)
    {
        str[i++] = '0';
    }

    while (val)
    {
        mod = val % 10;
        revert_str[j++] = mod + '0';
        val = val / 10;
    }

    while (j)
    {
        str[i++] = revert_str[j - 1];
        j--;
    }

    str[i] = '\0';
    OLED_ShowString(x, y, str);
    return i;
}

void OLED_ClearLine(uint8_t x, uint8_t y)
{
    for (; x < 16; x++)
    {
        OLED_ShowChar(x, y, ' ');
    }
}

/*
用字母L测试字模
*/
void OLED_ShowChar_L(uint8_t num)
{
    extern const uint8_t ascii_font_L[][16];
    uint8_t x = 0;
    uint8_t y = 0;

    uint8_t page = y;
    uint8_t col = x * 8;

    if (y > 7 || x > 15)
        return;

    if (num >= 4)
        return;

    OLED_SetPosition(page, col);
    SSD1306_WriteNBytes((uint8_t *)&ascii_font_L[num][0], 8);
    OLED_SetPosition(page + 1, col);
    SSD1306_WriteNBytes((uint8_t *)&ascii_font_L[num][0] + 8, 8);
}
