/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    gdispSsd1289/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the Ssd1289 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "gdisp_lld.h"
#include "stm32f4xx.h"
#include <stddef.h>

//#if HAL_USE_GDISP || defined(__DOXYGEN__)

/* Include the emulation code for things we don't support */
#include "gdisp_emulation.c"


#define lld_lcdWriteIndex(x)		LCD_REG = x
#define lld_lcdWriteData(x)			LCD_RAM = x
static __inline void lld_lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue)
{
  LCD_REG = lcdReg;
  LCD_RAM = lcdRegValue;
}
#define lld_lcdReadData()	return (uint16_t)LCD_RAM
static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg)
{
  LCD_REG = lcdReg;
  (void)LCD_RAM;
  return (LCD_RAM);
}
#define lld_lcdWriteStreamStart()	LCD_REG = 0x0022
#define lld_lcdWriteStreamStop()
static __inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size)
{
  uint16_t i;

  for(i = 0; i < size; i++) LCD_RAM = buffer[i];
}
#define lld_lcdReadStreamStart()	LCD_REG = 0x0022
#define lld_lcdReadStreamStop()
static __inline void lld_lcdReadStream(uint16_t *buffer, size_t size)
{
  uint16_t i;

  (void)LCD_RAM; /* throw away first value read */
  for(i = 0; i < size; i++) buffer[i] = LCD_RAM;
}

/* ATUALIZAR AQUI*/
static __inline void lld_lcdDelay(uint16_t us)
{
  INT16U time;
  if (us<1000)
  {
	  time = 1;
  }
  else
  {
	  time = us/1000;
  }
  DelayTask(time);
}


static void lld_lcdSetCursor(uint16_t x, uint16_t y)
{
  /* Reg 0x004E is an 8 bit value
   * Reg 0x004F is 9 bit
   * Use a bit mask to make sure they are not set too high
   */
  switch(GDISP.Orientation) {
    case portraitInv:
      lld_lcdWriteReg(0x004e, (SCREEN_WIDTH-1-x) & 0x00FF);
      lld_lcdWriteReg(0x004f, (SCREEN_HEIGHT-1-y) & 0x01FF);
      break;
    case portrait:
      lld_lcdWriteReg(0x004e, x & 0x00FF);
      lld_lcdWriteReg(0x004f, y & 0x01FF);
      break;
    case landscape:
      lld_lcdWriteReg(0x004e, y & 0x00FF);
      lld_lcdWriteReg(0x004f, x & 0x01FF);
      break;
    case landscapeInv:
      lld_lcdWriteReg(0x004e, (SCREEN_WIDTH - y - 1) & 0x00FF);
      lld_lcdWriteReg(0x004f, (SCREEN_HEIGHT - x - 1) & 0x01FF);
      break;
    }
}

static void lld_lcdSetViewPort(uint16_t x, uint16_t y, uint16_t cx, uint16_t cy)
{
  lld_lcdSetCursor(x, y);

  /* Reg 0x44 - Horizontal RAM address position
   * 		Upper Byte - HEA
   * 		Lower Byte - HSA
   * 		0 <= HSA <= HEA <= 0xEF
   * Reg 0x45,0x46 - Vertical RAM address position
   * 		Lower 9 bits gives 0-511 range in each value
   * 		0 <= Reg(0x45) <= Reg(0x46) <= 0x13F
   */

  switch(GDISP.Orientation) {
    case portrait:
      lld_lcdWriteReg(0x44, (((x+cx-1) << 8) & 0xFF00 ) | (x & 0x00FF));
      lld_lcdWriteReg(0x45, y & 0x01FF);
      lld_lcdWriteReg(0x46, (y+cy-1) & 0x01FF);
      break;
    case landscape:
      lld_lcdWriteReg(0x44, (((x+cx-1) << 8) & 0xFF00) | ((y+cy) & 0x00FF));
      lld_lcdWriteReg(0x45, x & 0x01FF);
      lld_lcdWriteReg(0x46, (x+cx-1) & 0x01FF);
      break;
    case portraitInv:
      lld_lcdWriteReg(0x44, (((SCREEN_WIDTH-x-1) & 0x00FF) << 8) | ((SCREEN_WIDTH - (x+cx)) & 0x00FF));
      lld_lcdWriteReg(0x45, (SCREEN_HEIGHT-(y+cy)) & 0x01FF);
      lld_lcdWriteReg(0x46, (SCREEN_HEIGHT-y-1) & 0x01FF);
      break;
    case landscapeInv:
      lld_lcdWriteReg(0x44, (((SCREEN_WIDTH - y - 1) & 0x00FF) << 8) | ((SCREEN_WIDTH - (y+cy)) & 0x00FF));
      lld_lcdWriteReg(0x45, (SCREEN_HEIGHT - (x+cx)) & 0x01FF);
      lld_lcdWriteReg(0x46, (SCREEN_HEIGHT - x - 1) & 0x01FF);
      break;
  }
  lld_lcdSetCursor(x, y);
}

static __inline void lld_lcdResetViewPort(void)							{}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/* ---- Required Routines ---- */
/*
	The following 2 routines are required.
	All other routines are optional.
*/

/**
 * @brief   Low level GDISP driver initialization.
 *
 * @notapi
 */
bool_t GDISP_LLD(init)(void) {
	#ifdef LCD_USE_FSMC
		#if defined(STM32F1XX) || defined(STM32F3XX)
			/* FSMC setup for F1/F3 */
			//rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

		#elif defined(STM32F4XX) || defined(STM32F2XX)
			/* STM32F2-F4 FSMC init */
			//rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);
			RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
		#else
		#error "FSMC not implemented for this device"
		#endif


	  /* FSMC pin configuration */
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);

	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);		// D2
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);		// D3
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);		// NOE -> RD
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);		// NWE -> WR
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);		// NE1 -> CS
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);		// D13
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);		// D14
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);		// D15
	  //GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);		// A16 -> RS
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);		// D0
	  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);		// D1

	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_FSMC);		// A20 -> RS
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);		// D4
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);		// D5
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);		// D6
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);		// D7
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);		// D8
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);		// D9
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);		// D10
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);		// D11
	  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);		// D12

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
									  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
									  GPIO_Pin_14 | GPIO_Pin_15;

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
									  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
									  GPIO_Pin_15;

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	  GPIO_Init(GPIOE, &GPIO_InitStructure);


//		/* set pins to FSMC mode (using A20) */
//		IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
//								(1 << 9) | (1 << 10) | (1 << 14) | (1 << 15), 0};
//
//		IOBus busE = {GPIOE, (1 << 4) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) |
//								(1 << 12) | (1 << 13) | (1 << 14) | (1 << 15), 0};
//
//		palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
//		palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));

		const unsigned char FSMC_Bank = 0;
		/* FSMC timing */

		FSMC_Bank1->BTCR[FSMC_Bank+1] = (FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_3) \
				| (FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_3) \
				| (FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_3) ;

		/* Bank1 NOR/SRAM control register configuration
		 * This is actually not needed as already set by default after reset */
		FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
	#elif defined(LCD_USE_GPIO)
		//IOBus busCMD = {LCD_CMD_PORT, (1 << LCD_CS) | (1 << LCD_RS) | (1 << LCD_WR) | (1 << LCD_RD), 0};
		//IOBus busDATA = {LCD_CMD_PORT, 0xFFFFF, 0};
		//palSetBusMode(&busCMD, PAL_MODE_OUTPUT_PUSHPULL);
		//palSetBusMode(&busDATA, PAL_MODE_OUTPUT_PUSHPULL);

	#else
		#error "Please define LCD_USE_FSMC or LCD_USE_GPIO"
	#endif

	lld_lcdWriteReg(0x0000,0x0001);		lld_lcdDelay(5);
    lld_lcdWriteReg(0x0003,0xA8A4);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000C,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000D,0x080C);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000E,0x2B00);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x001E,0x00B0);    	lld_lcdDelay(5);
	lld_lcdWriteReg(0x0001,0x2B3F);		lld_lcdDelay(5);
    lld_lcdWriteReg(0x0002,0x0600);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0010,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0011,0x6070);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0005,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0006,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0016,0xEF1C);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0017,0x0003);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0007,0x0133);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000B,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000F,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0041,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0042,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0048,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0049,0x013F);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x004A,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x004B,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0044,0xEF00);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0045,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0046,0x013F);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0030,0x0707);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0031,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0032,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0033,0x0502);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0034,0x0507);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0035,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0036,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0037,0x0502);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x003A,0x0302);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x003B,0x0302);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0023,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0024,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0025,0x8000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x004f,0x0000);		lld_lcdDelay(5);
    lld_lcdWriteReg(0x004e,0x0000);		lld_lcdDelay(5);


	#if defined(LCD_USE_FSMC)
		/* FSMC delay reduced as the controller now runs at full speed */
		FSMC_Bank1->BTCR[FSMC_Bank+1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0 ;
		FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
	#endif

    /* Initialise the GDISP structure */
	GDISP.Width = SCREEN_WIDTH;
	GDISP.Height = SCREEN_HEIGHT;
	GDISP.Orientation = portrait;
	GDISP.Powermode = powerOn;
	GDISP.Backlight = 100;
	GDISP.Contrast = 50;
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		GDISP.clipx0 = 0;
		GDISP.clipy0 = 0;
		GDISP.clipx1 = GDISP.Width;
		GDISP.clipy1 = GDISP.Height;
	#endif
	return TRUE;
}

/**
 * @brief   Draws a pixel on the display.
 *
 * @param[in] x        X location of the pixel
 * @param[in] y        Y location of the pixel
 * @param[in] color    The color of the pixel
 *
 * @notapi
 */
void GDISP_LLD(drawpixel)(coord_t x, coord_t y, color_t color) {
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
	#endif
	lld_lcdSetCursor(x, y);
	lld_lcdWriteReg(0x0022, color);
}

/* ---- Optional Routines ---- */
/*
	All the below routines are optional.
	Defining them will increase speed but everything
	will work if they are not defined.
	If you are not using a routine - turn it off using
	the appropriate GDISP_HARDWARE_XXXX macro.
	Don't bother coding for obvious similar routines if
	there is no performance penalty as the emulation software
	makes a good job of using similar routines.
		eg. If gfillarea() is defined there is little
			point in defining clear() unless the
			performance bonus is significant.
	For good performance it is suggested to implement
		fillarea() and blitarea().
*/

#if GDISP_HARDWARE_CLEARS || defined(__DOXYGEN__)
	/**
	 * @brief   Clear the display.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] color    The color of the pixel
	 *
	 * @notapi
	 */
	void GDISP_LLD(clear)(color_t color) {
	    unsigned i;

	    lld_lcdSetCursor(0, 0);
	    lld_lcdWriteStreamStart();

	    for(i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	    	lld_lcdWriteData(color);

	    lld_lcdWriteStreamStop();
	}
#endif

#if GDISP_HARDWARE_FILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a color.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] color    The color of the fill
	 *
	 * @notapi
	 */
	void GDISP_LLD(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		unsigned i, area;

		area = cx*cy;
		lld_lcdSetViewPort(x, y, cx, cy);
		lld_lcdWriteStreamStart();
		for(i = 0; i < area; i++)
			lld_lcdWriteData(color);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a bitmap.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] srcx, srcy   The bitmap position to start the fill from
	 * @param[in] srccx    The width of a line in the bitmap.
	 * @param[in] buffer   The pixels to use to fill the area.
	 *
	 * @notapi
	 */
	void GDISP_LLD(blitareaex)(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
		coord_t endx, endy;
		unsigned lg;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; srcx += GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; srcy += GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (srcx+cx > srccx)		cx = srccx - srcx;
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		lld_lcdSetViewPort(x, y, cx, cy);
		lld_lcdWriteStreamStart();

		endx = srcx + cx;
		endy = y + cy;
		lg = srccx - cx;
		buffer += srcx + srcy * srccx;
		for(; y < endy; y++, buffer += lg)
			for(x=srcx; x < endx; x++)
				lld_lcdWriteData(*buffer++);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
	}
#endif

#if (GDISP_NEED_PIXELREAD && GDISP_HARDWARE_PIXELREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Get the color of a particular pixel.
	 * @note    Optional.
	 * @note    If x,y is off the screen, the result is undefined.
	 *
	 * @param[in] x, y     The start of the text
	 *
	 * @notapi
	 */
	color_t GDISP_LLD(getpixelcolor)(coord_t x, coord_t y) {
		color_t color;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < 0 || x >= GDISP.Width || y < 0 || y >= GDISP.Height) return 0;
		#endif

		lld_lcdSetCursor(x, y);
		lld_lcdWriteStreamStart();

		color = lld_lcdReadData();
		color = lld_lcdReadData();

		lld_lcdWriteStreamStop();

		return color;
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL) || defined(__DOXYGEN__)
	/**
	 * @brief   Scroll vertically a section of the screen.
	 * @note    Optional.
	 * @note    If x,y + cx,cy is off the screen, the result is undefined.
	 * @note    If lines is >= cy, it is equivelent to a area fill with bgcolor.
	 *
	 * @param[in] x, y     The start of the area to be scrolled
	 * @param[in] cx, cy   The size of the area to be scrolled
	 * @param[in] lines    The number of lines to scroll (Can be positive or negative)
	 * @param[in] bgcolor  The color to fill the newly exposed area.
	 *
	 * @notapi
	 */
	void GDISP_LLD(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		static color_t buf[((SCREEN_HEIGHT > SCREEN_WIDTH ) ? SCREEN_HEIGHT : SCREEN_WIDTH)];
		coord_t row0, row1;
		unsigned i, gap, abslines;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (!lines || cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		abslines = lines < 0 ? -lines : lines;

		if (abslines >= cy) {
			abslines = cy;
			gap = 0;
		} else {
			gap = cy - abslines;
			for(i = 0; i < gap; i++) {
				if(lines > 0) {
					row0 = y + i + lines;
					row1 = y + i;
				} else {
					row0 = (y - i - 1) + lines;
					row1 = (y - i - 1);
				}

				/* read row0 into the buffer and then write at row1*/
				lld_lcdSetViewPort(x, row0, cx, 1);
				lld_lcdReadStreamStart();
				lld_lcdReadStream(buf, cx);
				lld_lcdReadStreamStop();

				lld_lcdSetViewPort(x, row1, cx, 1);
				lld_lcdWriteStreamStart();
				lld_lcdWriteStream(buf, cx);
				lld_lcdWriteStreamStop();
			}
		}

		/* fill the remaining gap */
		lld_lcdSetViewPort(x, lines > 0 ? (y+gap) : y, cx, abslines);
		lld_lcdWriteStreamStart();
		gap = cx*abslines;
		for(i = 0; i < gap; i++) lld_lcdWriteData(bgcolor);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
	}
#endif

#if (GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL) || defined(__DOXYGEN__)
	/**
	 * @brief   Driver Control
	 * @detail	Unsupported control codes are ignored.
	 * @note	The value parameter should always be typecast to (void *).
	 * @note	There are some predefined and some specific to the low level driver.
	 * @note	GDISP_CONTROL_POWER			- Takes a gdisp_powermode_t
	 * 			GDISP_CONTROL_ORIENTATION	- Takes a gdisp_orientation_t
	 * 			GDISP_CONTROL_BACKLIGHT -	 Takes an int from 0 to 100. For a driver
	 * 											that only supports off/on anything other
	 * 											than zero is on.
	 * 			GDISP_CONTROL_CONTRAST		- Takes an int from 0 to 100.
	 * 			GDISP_CONTROL_LLD			- Low level driver control constants start at
	 * 											this value.
	 *
	 * @param[in] what		What to do.
	 * @param[in] value		The value to use (always cast to a void *).
	 *
	 * @notapi
	 */
	void GDISP_LLD(control)(unsigned what, void *value) {
		switch(what) {
		case GDISP_CONTROL_POWER:
			if (GDISP.Powermode == (gdisp_powermode_t)value)
				return;
			switch((gdisp_powermode_t)value) {
			case powerOff:
				lld_lcdWriteReg(0x0010, 0x0000);	// leave sleep mode
				lld_lcdWriteReg(0x0007, 0x0000);	// halt operation
				lld_lcdWriteReg(0x0000, 0x0000);	// turn off oszillator
				lld_lcdWriteReg(0x0010, 0x0001);	// enter sleepmode
				break;
			case powerOn:
				lld_lcdWriteReg(0x0010, 0x0000);	// leave sleep mode
				if (GDISP.Powermode != powerSleep)
					GDISP_LLD(init)();
				break;
			case powerSleep:
				lld_lcdWriteReg(0x0010, 0x0001);	// enter sleep mode
				break;
			default:
				return;
			}
			GDISP.Powermode = (gdisp_powermode_t)value;
			return;
		case GDISP_CONTROL_ORIENTATION:
			if (GDISP.Orientation == (gdisp_orientation_t)value)
				return;
			switch((gdisp_orientation_t)value) {
			case portrait:
				lld_lcdWriteReg(0x0001, 0x2B3F);
				/* ID = 11 AM = 0 */
				lld_lcdWriteReg(0x0011, 0x6070);
				GDISP.Height = SCREEN_HEIGHT;
				GDISP.Width = SCREEN_WIDTH;
				break;
			case landscape:
				lld_lcdWriteReg(0x0001, 0x293F);
				/* ID = 11 AM = 1 */
				lld_lcdWriteReg(0x0011, 0x6078);
				GDISP.Height = SCREEN_WIDTH;
				GDISP.Width = SCREEN_HEIGHT;
				break;
			case portraitInv:
				lld_lcdWriteReg(0x0001, 0x2B3F);
				/* ID = 01 AM = 0 */
				lld_lcdWriteReg(0x0011, 0x6040);
				GDISP.Height = SCREEN_HEIGHT;
				GDISP.Width = SCREEN_WIDTH;
				break;
			case landscapeInv:
				lld_lcdWriteReg(0x0001, 0x293F);
				/* ID = 01 AM = 1 */
				lld_lcdWriteReg(0x0011, 0x6048);
				GDISP.Height = SCREEN_WIDTH;
				GDISP.Width = SCREEN_HEIGHT;
				break;
			default:
				return;
			}
			#if GDISP_NEED_CLIP || GDISP_NEED_VALIDATION
				GDISP.clipx0 = 0;
				GDISP.clipy0 = 0;
				GDISP.clipx1 = GDISP.Width;
				GDISP.clipy1 = GDISP.Height;
			#endif
			GDISP.Orientation = (gdisp_orientation_t)value;
			return;
/*
		case GDISP_CONTROL_BACKLIGHT:
		case GDISP_CONTROL_CONTRAST:
*/
		}
	}
#endif

//#endif /* HAL_USE_GDISP */
/** @} */
