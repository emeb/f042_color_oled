/*
 * ssd1331.h - init & comm api for the SSD1331 SPI Color OLED
 * mostly kanged from Adafruit Arduino / ESP8266 driver
 * 08-16-16 E. Brombaugh
 */

#include "ssd1331.h"
#include "systick.h"

/* macros to define pins */
#define OLED_CS_PIN                        GPIO_Pin_3
#define OLED_CS_GPIO_PORT                  GPIOA
#define OLED_CS_GPIO_CLK                   RCC_AHBPeriph_GPIOA
#define OLED_DC_PIN                        GPIO_Pin_4
#define OLED_DC_GPIO_PORT                  GPIOA
#define OLED_DC_GPIO_CLK                   RCC_AHBPeriph_GPIOA
#define OLED_RST_PIN                       GPIO_Pin_6
#define OLED_RST_GPIO_PORT                 GPIOA
#define OLED_RST_GPIO_CLK                  RCC_AHBPeriph_GPIOA

/* macros for GPIO setting */
#define OLED_CS_LOW()     GPIO_ResetBits(OLED_CS_GPIO_PORT,OLED_CS_PIN)
#define OLED_CS_HIGH()    GPIO_SetBits(OLED_CS_GPIO_PORT,OLED_CS_PIN)
#define OLED_DC_LOW()     GPIO_ResetBits(OLED_DC_GPIO_PORT,OLED_DC_PIN)
#define OLED_DC_HIGH()    GPIO_SetBits(OLED_DC_GPIO_PORT,OLED_DC_PIN)
#define OLED_RST_LOW()    GPIO_ResetBits(OLED_RST_GPIO_PORT,OLED_RST_PIN)
#define OLED_RST_HIGH()   GPIO_SetBits(OLED_RST_GPIO_PORT,OLED_RST_PIN)

/* SSD1331 commands */
#define SSD1331_CMD_DRAWLINE 		0x21
#define SSD1331_CMD_DRAWRECT 		0x22
#define SSD1331_CMD_FILL 			0x26
#define SSD1331_CMD_SETCOLUMN 		0x15
#define SSD1331_CMD_SETROW    		0x75
#define SSD1331_CMD_CONTRASTA 		0x81
#define SSD1331_CMD_CONTRASTB 		0x82
#define SSD1331_CMD_CONTRASTC		0x83
#define SSD1331_CMD_MASTERCURRENT 	0x87
#define SSD1331_CMD_SETREMAP 		0xA0
#define SSD1331_CMD_STARTLINE 		0xA1
#define SSD1331_CMD_DISPLAYOFFSET 	0xA2
#define SSD1331_CMD_NORMALDISPLAY 	0xA4
#define SSD1331_CMD_DISPLAYALLON  	0xA5
#define SSD1331_CMD_DISPLAYALLOFF 	0xA6
#define SSD1331_CMD_INVERTDISPLAY 	0xA7
#define SSD1331_CMD_SETMULTIPLEX  	0xA8
#define SSD1331_CMD_SETMASTER 		0xAD
#define SSD1331_CMD_DISPLAYOFF 		0xAE
#define SSD1331_CMD_DISPLAYON     	0xAF
#define SSD1331_CMD_POWERMODE 		0xB0
#define SSD1331_CMD_PRECHARGE 		0xB1
#define SSD1331_CMD_CLOCKDIV 		0xB3
#define SSD1331_CMD_PRECHARGEA 		0x8A
#define SSD1331_CMD_PRECHARGEB 		0x8B
#define SSD1331_CMD_PRECHARGEC 		0x8C
#define SSD1331_CMD_PRECHARGELEVEL 	0xBB
#define SSD1331_CMD_VCOMH 			0xBE

/* color order */
#define SSD1331_COLORORDER_RGB

/* Timing Delays in ms */
#define SSD1331_DELAYS_HWFILL 3
#define SSD1331_DELAYS_HWLINE 1

/*
 * Initialize the RCC, GPIO, SPI interfaces to the DAC 
 */
void spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	/* Enable the SPI peripheral */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* Enable SCK, MOSI, MISO and NSS GPIO clocks */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);

	/* SPI pin mappings */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);  // SCK	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);  // MOSI
  
	/* SPI SCK, MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* CS, DC, RST pin configuration */
	GPIO_InitStructure.GPIO_Pin = OLED_CS_PIN | OLED_DC_PIN | OLED_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  
	/* SPI configuration -------------------------------------------------*/
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI1, &SPI_InitStructure);

	/* Enable the SPI peripheral */
	SPI_Cmd(SPI1, ENABLE);
}

/*
 * send a byte via spi
 */
void spi_tx(uint8_t data)
{
	/* wait for TXE */
	while((SPI1->SR & SPI_SR_TXE) == 0);
		
	/* write data as 8-bit value */
	*(__IO uint8_t *)((uint32_t)SPI1+0x0c) = data;

	/* wait for finished */
	while((SPI1->SR & SPI_SR_BSY));
}

/*
 * send a command byte
 */
void spi_command_byte(uint8_t cmd)
{
	/* set DC low for command */
	OLED_DC_LOW();
	
	/* set CS low */
	OLED_CS_LOW();
	
	/* send a byte */
	spi_tx(cmd);
	
	/* set CS high */
	OLED_CS_HIGH();
}

/*
 * send a data byte
 */
void spi_data_byte(uint8_t data)
{
	/* set DC high for data */
	OLED_DC_HIGH();
	
	/* set CS low */
	OLED_CS_LOW();
	
	/* send a byte */
	spi_tx(data);
	
	/* set CS high */
	OLED_CS_HIGH();
}

void ssd1331_init(void)
{
	/* init the hardware interfaces */
	spi_init();
	
	/* set CS high */
	OLED_CS_HIGH();	
	
	/* reset */
	OLED_RST_HIGH();
	systick_delayms(10);
	OLED_RST_LOW();
	systick_delayms(10);
	OLED_RST_HIGH();

	/* send init sequence to display */
    spi_command_byte(SSD1331_CMD_DISPLAYOFF);  	// 0xAE
    spi_command_byte(SSD1331_CMD_SETREMAP); 	// 0xA0
#if defined SSD1331_COLORORDER_RGB
    spi_command_byte(0x72);				// RGB Color
#else
    spi_command_byte(0x76);				// BGR Color
#endif
    spi_command_byte(SSD1331_CMD_STARTLINE); 	// 0xA1
    spi_command_byte(0x0);
    spi_command_byte(SSD1331_CMD_DISPLAYOFFSET); 	// 0xA2
    spi_command_byte(0x0);
    spi_command_byte(SSD1331_CMD_NORMALDISPLAY);  	// 0xA4
    spi_command_byte(SSD1331_CMD_SETMULTIPLEX); 	// 0xA8
    spi_command_byte(0x3F);  			// 0x3F 1/64 duty
    spi_command_byte(SSD1331_CMD_SETMASTER);  	// 0xAD
    spi_command_byte(0x8E);
    spi_command_byte(SSD1331_CMD_POWERMODE);  	// 0xB0
    spi_command_byte(0x0B);
    spi_command_byte(SSD1331_CMD_PRECHARGE);  	// 0xB1
    spi_command_byte(0x31);
    spi_command_byte(SSD1331_CMD_CLOCKDIV);  	// 0xB3
    spi_command_byte(0xF0);  // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    spi_command_byte(SSD1331_CMD_PRECHARGEA);  	// 0x8A
    spi_command_byte(0x64);
    spi_command_byte(SSD1331_CMD_PRECHARGEB);  	// 0x8B
    spi_command_byte(0x78);
    spi_command_byte(SSD1331_CMD_PRECHARGEA);  	// 0x8C
    spi_command_byte(0x64);
    spi_command_byte(SSD1331_CMD_PRECHARGELEVEL);  	// 0xBB
    spi_command_byte(0x3A);
    spi_command_byte(SSD1331_CMD_VCOMH);  		// 0xBE
    spi_command_byte(0x3E);
    spi_command_byte(SSD1331_CMD_MASTERCURRENT);  	// 0x87
    spi_command_byte(0x06);
    spi_command_byte(SSD1331_CMD_CONTRASTA);  	// 0x81
    spi_command_byte(0x91);
    spi_command_byte(SSD1331_CMD_CONTRASTB);  	// 0x82
    spi_command_byte(0x50);
    spi_command_byte(SSD1331_CMD_CONTRASTC);  	// 0x83
    spi_command_byte(0x7D);
    spi_command_byte(SSD1331_CMD_DISPLAYON);	//--turn on oled panel    
}

/*
 * set x/y address of next operation
 */
void ssd1331_xy(uint8_t x, uint8_t y)
{
  if ((x >= SSD1331_WIDTH) || (y >= SSD1331_HEIGHT)) return;
  
  // set x and y coordinate
  spi_command_byte(SSD1331_CMD_SETCOLUMN);
  spi_command_byte(x);
  spi_command_byte(SSD1331_WIDTH-1);

  spi_command_byte(SSD1331_CMD_SETROW);
  spi_command_byte(y);
  spi_command_byte(SSD1331_HEIGHT-1);
}

/*
 * compute 16-bit color value
 */
uint16_t ssd1331_getcolor(uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t c;

	c = r >> 3;
	c <<= 6;
	c |= g >> 2;
	c <<= 5;
	c |= b >> 3;

	return c;
}

void ssd1331_setcolor(uint16_t color)
{
	/* set DC high for data */
	OLED_DC_HIGH();
	
	/* set CS low */
	OLED_CS_LOW();
	
	/* send a word as two bytes ms-byte 1st */
	spi_tx((color>>8)&0xff);
	spi_tx(color&0xff);
	
	/* set CS high */
	OLED_CS_HIGH();
}

void ssd1331_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{	
  // Boundary check
  if ((y0 >= SSD1331_HEIGHT) && (y1 >= SSD1331_HEIGHT))
	return;
  if ((x0 >= SSD1331_WIDTH) && (x1 >= SSD1331_WIDTH))
	return;	
  if (x0 >= SSD1331_WIDTH)
    x0 = SSD1331_WIDTH - 1;
  if (y0 >= SSD1331_HEIGHT)
    y0 = SSD1331_HEIGHT - 1;
  if (x1 >= SSD1331_WIDTH)
    x1 = SSD1331_WIDTH - 1;
  if (y1 >= SSD1331_HEIGHT)
    y1 = SSD1331_HEIGHT - 1;
  
  spi_command_byte(SSD1331_CMD_DRAWLINE);
  spi_command_byte(x0);
  spi_command_byte(y0);
  spi_command_byte(x1);
  spi_command_byte(y1);
  systick_delayms(SSD1331_DELAYS_HWLINE);  
  spi_command_byte((uint8_t)((color >> 11) << 1));
  spi_command_byte((uint8_t)((color >> 5) & 0x3F));
  spi_command_byte((uint8_t)((color << 1) & 0x3F));
  systick_delayms(SSD1331_DELAYS_HWLINE);  
}

void ssd1331_fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t outcolor, uint16_t fillcolor) 
{
  // x,y Bounds check
  if ((x >= SSD1331_WIDTH) || (y >= SSD1331_HEIGHT))
	return;

  // h bounds check
  if (y+h > SSD1331_HEIGHT)
  {
    h = SSD1331_HEIGHT - y;
  }

  // w bounds check
  if (x+w > SSD1331_WIDTH)
  {
    w = SSD1331_WIDTH - x;
  }
  
  // fill!
  spi_command_byte(SSD1331_CMD_FILL);
  spi_command_byte(0x01);

  spi_command_byte(SSD1331_CMD_DRAWRECT);
  spi_command_byte(x & 0xFF);							// Starting column
  spi_command_byte(y & 0xFF);							// Starting row
  spi_command_byte((x+w-1) & 0xFF);	// End column
  spi_command_byte((y+h-1) & 0xFF);	// End row
  
  // Outline color
  spi_command_byte((uint8_t)((outcolor >> 11) << 1));
  spi_command_byte((uint8_t)((outcolor >> 5) & 0x3F));
  spi_command_byte((uint8_t)((outcolor << 1) & 0x3F));
  // Fill color
  spi_command_byte((uint8_t)((fillcolor >> 11) << 1));
  spi_command_byte((uint8_t)((fillcolor >> 5) & 0x3F));
  spi_command_byte((uint8_t)((fillcolor << 1) & 0x3F));
 
  // Delay while the fill completes
  systick_delayms(SSD1331_DELAYS_HWFILL); 
}

/*
 * Convert HSV triple to RGB triple
 * use algorithm from
 * http://en.wikipedia.org/wiki/HSL_and_HSV#Converting_to_RGB
 */
void ssd1331_hsv2rgb(uint8_t rgb[], uint8_t hsv[])
{
	uint16_t C;
	int16_t Hprime, Cscl;
	uint8_t hs, X, m;
	
	/* default */
	rgb[0] = 0;
	rgb[1] = 0;
	rgb[2] = 0;
	
	/* calcs are easy if v = 0 */
	if(hsv[2] == 0)
		return;
	
	/* C is the chroma component */
	C = ((uint16_t)hsv[1] * (uint16_t)hsv[2])>>8;
	
	/* Hprime is fixed point with range 0-5.99 representing hue sector */
	Hprime = (int16_t)hsv[0] * 6;
	
	/* get intermediate value X */
	Cscl = (Hprime%512)-256;
	Cscl = Cscl < 0 ? -Cscl : Cscl;
	Cscl = 256 - Cscl;
	X = ((uint16_t)C * Cscl)>>8;
	
	/* m is value offset */
	m = hsv[2] - C;
	
	/* get the hue sector (1 of 6) */
	hs = (Hprime)>>8;
	
	/* map by sector */
	switch(hs)
	{
		case 0:
			/* Red -> Yellow sector */
			rgb[0] = C + m;
			rgb[1] = X + m;
			rgb[2] = m;
			break;
		
		case 1:
			/* Yellow -> Green sector */
			rgb[0] = X + m;
			rgb[1] = C + m;
			rgb[2] = m;
			break;
		
		case 2:
			/* Green -> Cyan sector */
			rgb[0] = m;
			rgb[1] = C + m;
			rgb[2] = X + m;
			break;
		
		case 3:
			/* Cyan -> Blue sector */
			rgb[0] = m;
			rgb[1] = X + m;
			rgb[2] = C + m;
			break;
		
		case 4:
			/* Blue -> Magenta sector */
			rgb[0] = X + m;
			rgb[1] = m;
			rgb[2] = C + m;
			break;
		
		case 5:
			/* Magenta -> Red sector */
			rgb[0] = C + m;
			rgb[1] = m;
			rgb[2] = X + m;
			break;
	}
}
