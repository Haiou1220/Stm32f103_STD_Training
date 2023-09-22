#ifndef __FONTS_H__
#define __FONTS_H__
#include "stdint.h"

//typedef struct {
//  const uint8_t FontWidth;    /*!< Font width in pixels */
//  uint8_t FontHeight;         /*!< Font height in pixels */
//  const unsigned char  *data;       /*!< Pointer to data font data array */
//} FontDef;


//
// De 3 fonts
//
//extern FontDef Font_7x10;
//extern FontDef Font_0806;
//extern FontDef Font_asc2_1608 ;
//extern FontDef Font_16x16 ; 
//extern FontDef Font_11x18;
//extern FontDef Font_16x26;

extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];


 extern const unsigned char asc2_2412[95][36];
extern const char zh_font_1616[][16];
extern const char zh_font_2424[][24];
	
#endif










