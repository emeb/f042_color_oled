/*
 * SSD1331 OLED test for STM32F042
 * E. Brombaugh 08-16-2016
 */

#include <stdlib.h>
#include "stm32f0xx.h"
#include "led.h"
#include "systick.h"
#include "ssd1331.h"

int main(void)
{
	uint8_t x, y, x0, y0, x1, y1, r, g, b;
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
	/* test area fill */
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
		ssd1331_fillRect(x0, y0, x1, y1, color, 0);
		systick_delayms(20);
	}
#endif
	
#if 1
	/* clear and draw lines */
	for(;;)
	{
		for(x0=0;x0<8;x0++)
		{
			ssd1331_fillRect(0, 0, SSD1331_WIDTH, SSD1331_HEIGHT, 0, 0);
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
