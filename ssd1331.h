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
void ssd1331_drawPixel(uint8_t x, uint8_t y, uint16_t color);
void ssd1331_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);
void ssd1331_drawRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t fill, uint16_t outcolor, uint16_t fillcolor);
void ssd1331_hsv2rgb(uint8_t rgb[], uint8_t hsv[]);
void ssd1331_drawchar(uint8_t x, uint8_t y, char chr, uint16_t fgcolor, uint16_t bgcolor);
void ssd1331_drawstr(uint8_t x, uint8_t y, char *str, uint16_t fgcolor, uint16_t bgcolor);
void ssd1331_scrolltest(void);
void ssd1331_copyRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t xd, uint8_t yd);

#endif

