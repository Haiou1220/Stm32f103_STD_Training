#ifndef __PRINT_H
#define __PRINT_H
#include "stdint.h"

#define ASSERT(x) ((x)?((void)0):(assert_check((uint8_t *)__FILE__, __LINE__)))

int print(const char *__format, ...);
void assert_check(uint8_t* file_name,uint32_t line);

#endif
