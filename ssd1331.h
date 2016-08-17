/*
 * ssd1331.h - init & comm api for the SSD1331 SPI Color OLED
 * mostly kanged from Adafruit Arduino / ESP8266 driver
 * 08-16-16 E. Brombaugh
 */

#ifndef __ssd1331__
#define __ssd1331__

#include "stm32f0xx.h"

/* display characteristics */
#define SSD1331_WIDTH 96
#define SSD1331_HEIGHT 64

void ssd1331_init(void);
void ssd1331_xy(uint8_t x, uint8_t y);
uint16_t ssd1331_getcolor(uint8_t r, uint8_t g, uint8_t b);
void ssd1331_setcolor(uint16_t color);
void ssd1331_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void ssd1331_fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t outcolor, uint16_t fillcolor);

#endif

