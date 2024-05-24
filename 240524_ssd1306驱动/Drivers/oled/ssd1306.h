/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SSD1306_H
#define __SSD1306_H
#include "stm32f1xx_hal.h"

int SSD1306_WriteCmd(uint8_t cmd);

int SSD1306_WriteData(uint8_t data);

void OLED_Clear(void);
void OLED_init(void);

void OLED_ShowChar(uint8_t x, uint8_t y, char c);
void OLED_ShowChar_L(uint8_t num);

int OLED_ShowString(uint8_t x, uint8_t y, const char *buf);
int OLED_PrintSignedVal(uint8_t x, uint8_t y, int32_t val);
void OLED_ClearLine(uint8_t x, uint8_t y);
#endif
