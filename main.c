/*
 * SSD1331 OLED test for STM32F042
 * E. Brombaugh 08-16-2016
 */

#include <stdlib.h>
#include "stm32f0xx.h"
#include "led.h"
#include "systick.h"
#include "ssd1331.h"

#define NUM_LINES 64

int main(void)
{
	//uint8_t x, y, x0, y0, x1, y1, r, g, b;
	uint8_t x0[NUM_LINES], y0[NUM_LINES], x1[NUM_LINES], y1[NUM_LINES];
	uint8_t dx0, dy0, dx1, dy1, idx, nidx, erase;
	uint8_t rgb[3], hsv[3];
	uint16_t color;
	
	/* initialize the hardware */
	led_init();
	systick_init();
	ssd1331_init();
	
#if 0
	/* clear screen */
	color = ssd1331_getcolor(0,0,0);
	for(x=0;x<SSD1331_WIDTH;x++)
	{
		for(y=0;y<SSD1331_HEIGHT;y++)
		{
			ssd1331_xy(x, y);
			color = ssd1331_getcolor(x<<1,y<<2,0);
			ssd1331_setcolor(color);
			systick_delayms(1);
		}
	}
#endif

#if 0
	/* test random line drawing */
	for (;;)
	{
		r = rand()&0xff;
		g = rand()&0xff;
		b = rand()&0xff;
		x0 = rand()%SSD1331_WIDTH;
		x1 = rand()%SSD1331_WIDTH;
		y0 = rand()%SSD1331_HEIGHT;
		y1 = rand()%SSD1331_HEIGHT;
		color = ssd1331_getcolor(r,g,b);
		ssd1331_drawLine(x0, y0, x1, y1, color);
	}
#endif
	
#if 0
	/* test random rects */
	for (;;)
	{
		r = rand()&0xff;
		g = rand()&0xff;
		b = rand()&0xff;
		x0 = rand()%SSD1331_WIDTH;
		x1 = rand()%SSD1331_WIDTH;
		y0 = rand()%SSD1331_HEIGHT;
		y1 = rand()%SSD1331_HEIGHT;
		color = ssd1331_getcolor(r,g,b);
		ssd1331_drawRect(x0, y0, x1, y1, 1, color, color);
		systick_delayms(20);
	}
#endif
	
#if 0
	/* clear and draw lines */
	for(;;)
	{
		for(x0=0;x0<8;x0++)
		{
			ssd1331_drawRect(0, 0, SSD1331_WIDTH, SSD1331_HEIGHT, 0, 0);
			for(x=x0;x<SSD1331_WIDTH;x+=8)
			{
				color = ssd1331_getcolor(x<<1,255-(x<<1),0);
				ssd1331_drawLine(0, 0, x, SSD1331_HEIGHT, color);
			}
			for(y=x0;y<SSD1331_HEIGHT;y+=8)
			{
				color = ssd1331_getcolor(y<<2,0,255-(y<<2));
				ssd1331_drawLine(0, SSD1331_HEIGHT, SSD1331_WIDTH, SSD1331_HEIGHT-y, color);
			}
			for(x=x0;x<SSD1331_WIDTH;x+=8)
			{
				color = ssd1331_getcolor(0,x<<1,255-(x<<1));
				ssd1331_drawLine(SSD1331_WIDTH, SSD1331_HEIGHT, x, 0, color);
			}
			for(y=x0;y<SSD1331_HEIGHT;y+=8)
			{
				color = ssd1331_getcolor(y<<2,0,255-(y<<2));
				ssd1331_drawLine(SSD1331_WIDTH, 0, 0, y, color);
			}
			systick_delayms(100);
		}
	}
#endif

#if 0
	/* qix */
	ssd1331_drawRect(0, 0, SSD1331_WIDTH, SSD1331_HEIGHT, 1, 0, 0);
	hsv[0] = 0;
	hsv[1] = 255;
	hsv[2] = 255;
	idx = 0;
	erase = 0;
	x0[0] = rand()%SSD1331_WIDTH;
	x1[0] = rand()%SSD1331_WIDTH;
	y0[0] = rand()%SSD1331_HEIGHT;
	y1[0] = rand()%SSD1331_HEIGHT;
	dx0 = (rand()&1)*2-1;
	dy0 = (rand()&1)*2-1;
	dx1 = (rand()&1)*2-1;
	dy1 = (rand()&1)*2-1;
	for(;;)
	{
		ssd1331_hsv2rgb(rgb, hsv);
		color = ssd1331_getcolor(rgb[0], rgb[1], rgb[2]);
		//ssd1331_drawLine(x0[idx], y0[idx], x1[idx], y1[idx], color);
		ssd1331_drawRect(x0[idx], y0[idx], x1[idx], y1[idx], 0, color, 0);
		nidx = (idx+1)%NUM_LINES;
		systick_delayms(10);
		if(nidx == 0) erase = 1;
		if(erase)
		{
			//ssd1331_drawLine(x0[nidx], y0[nidx], x1[nidx], y1[nidx], 0);
			ssd1331_drawRect(x0[nidx], y0[nidx], x1[nidx], y1[nidx], 0, 0, 0);
		}
		hsv[0]++;
		x0[nidx] = x0[idx]+dx0;
		y0[nidx] = y0[idx]+dy0;
		x1[nidx] = x1[idx]+dx1;
		y1[nidx] = y1[idx]+dy1;
		if(x0[nidx] == 0) dx0 = -dx0;
		if(x1[nidx] == 0) dx1 = -dx1;
		if(x0[nidx] == SSD1331_WIDTH-1) dx0 = -dx0;
		if(x1[nidx] == SSD1331_WIDTH-1) dx1 = -dx1;
		if(y0[nidx] == 0) dy0 = -dy0;
		if(y1[nidx] == 0) dy1 = -dy1;
		if(y0[nidx] == SSD1331_HEIGHT-1) dy0 = -dy0;
		if(y1[nidx] == SSD1331_HEIGHT-1) dy1 = -dy1; // 1 l 
		idx = nidx;
	}
#endif

#if 1
	/* test text */
	ssd1331_drawRect(0, 0, SSD1331_WIDTH, SSD1331_HEIGHT, 1, 0, 0);
	color = ssd1331_getcolor(255, 0, 0);
	ssd1331_drawstr(0,  0, "Hello World!", color, 0);
	color = ssd1331_getcolor(0, 255, 0);
	ssd1331_drawstr(0,  8, "This is a   ", color, 0);
	color = ssd1331_getcolor(0, 255, 0);
	ssd1331_drawstr(0, 16, "test.       ", color, 0);
	color = ssd1331_getcolor(0, 0, 255);
	ssd1331_drawstr(0, 24, "123456789ABC", color, 0);
	color = ssd1331_getcolor(0, 255, 255);
	ssd1331_drawstr(0, 32, "inverse.....", 0, color);
#endif

	/* Loop forever */
	while(1)
	{
		/* wait a bit & flash light */
		systick_delayms(190);
		led_on(LED1);
		systick_delayms(10);
		led_off(LED1);
	}
}
