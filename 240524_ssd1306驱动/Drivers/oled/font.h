
#include "ssd1306.h"

/**
  * ���ݴ洢��ʽ��
  * ����8�㣬��λ���£��ȴ����ң��ٴ��ϵ���
  * ÿһ��Bit��Ӧһ�����ص�
  * 
  *      B0 B0                  B0 B0
  *      B1 B1                  B1 B1
  *      B2 B2                  B2 B2
  *      B3 B3  ------------->  B3 B3 --
  *      B4 B4                  B4 B4  |
  *      B5 B5                  B5 B5  |
  *      B6 B6                  B6 B6  |
  *      B7 B7                  B7 B7  |
  *                                    |
  *  -----------------------------------
  *  |   
  *  |   B0 B0                  B0 B0
  *  |   B1 B1                  B1 B1
  *  |   B2 B2                  B2 B2
  *  --> B3 B3  ------------->  B3 B3
  *      B4 B4                  B4 B4
  *      B5 B5                  B5 B5
  *      B6 B6                  B6 B6
  *      B7 B7                  B7 B7
  * 
  */

/*ASCII��ģ����*********************/

/*��8���أ���16����*/
extern const uint8_t ascii_font[128][16];
extern const uint8_t ascii_font_L[][16] ;
