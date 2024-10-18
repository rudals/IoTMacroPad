#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "ST7735_TFT.h"
#include "ST7735_TFT_Font.h"

#define _TFT_DC     15
#define _TFT_RST    14
#define _TFT_CS     13
#define _TFT_SCLK   10
#define _TFT_SDATA  11

#define TFT_DC_INIT gpio_init(_TFT_DC)
#define TFT_RST_INIT gpio_init(_TFT_RST)
#define TFT_CS_INIT gpio_init(_TFT_CS)
#define TFT_SCLK_INIT gpio_init(_TFT_SCLK)
#define TFT_SDATA_INIT gpio_init(_TFT_SDATA)

#define TFT_DC_SetHigh gpio_put(_TFT_DC, true)
#define TFT_DC_SetLow gpio_put(_TFT_DC, false)
#define TFT_RST_SetHigh gpio_put(_TFT_RST, true)
#define TFT_RST_SetLow gpio_put(_TFT_RST, false)
#define TFT_CS_SetHigh gpio_put(_TFT_CS, true)
#define TFT_CS_SetLow gpio_put(_TFT_CS, false)
#define TFT_SCLK_SetHigh gpio_put(_TFT_SCLK, true)
#define TFT_SCLK_SetLow gpio_put(_TFT_SCLK, false)
#define TFT_SDATA_SetHigh gpio_put(_TFT_SDATA, true)
#define TFT_SDATA_SetLow gpio_put(_TFT_SDATA, false)

#define TFT_DC_SetDigitalOutput gpio_set_dir(_TFT_DC, GPIO_OUT)
#define TFT_RST_SetDigitalOutput gpio_set_dir(_TFT_RST, GPIO_OUT)
#define TFT_CS_SetDigitalOutput gpio_set_dir(_TFT_CS, GPIO_OUT)
#define TFT_SCLK_SetDigitalOutput gpio_set_dir(_TFT_SCLK, GPIO_OUT)
#define TFT_SDATA_SetDigitalOutput gpio_set_dir(_TFT_SDATA, GPIO_OUT)

#define TFT_SCLK_SPI_FUNC gpio_set_function(_TFT_SCLK, GPIO_FUNC_SPI)
#define TFT_SDATA_SPI_FUNC gpio_set_function(_TFT_SDATA, GPIO_FUNC_SPI)

// Delays
#define TFT_MILLISEC_DELAY busy_wait_ms
#define TFT_MICROSEC_DELAY busy_wait_us



spi_inst_t *_pspiInterface = spi1;
uint16_t _speedSPIKHz = 8000;

uint16_t _textcolor = 0xFFFF;	/**< ForeGround color for text*/
uint16_t _textbgcolor = 0x0000; /**< BackGround color for text*/
uint8_t _textSize = 1;			/**< Size of text , fonts 1-6 only*/

uint8_t _currentMode = 0; /**< TFT mode */
int16_t _cursorX = 0;	  /**< Current pixel column postion of Cursor*/
int16_t _cursorY = 0;	  /**< Current pixel row position of Cursor*/

uint8_t _colstart; /**< Records column offset start postion, never change after first init */
uint8_t _rowstart; /**< Records row offset start postion, never change after first init */
uint8_t _XStart;   /**< Records column offset postion */
uint8_t _YStart;   /**< Records row offset postion */

uint16_t _widthTFT;		  /**< Records width TFT postion */
uint16_t _heightTFT;	  /**< Records Height TFT postion */
uint16_t _widthStartTFT;  /**< Records width TFT postion never change after first init*/
uint16_t _heightStartTFT; /**< Records Height TFT postion never change after first init */

bool _wrap = true;

enum TFT_modes_e TFT_mode;					 /**< Enum to hold display mode */
enum TFT_PCBtype_e TFT_PCBtype;				 /**< Enum to hold TFT type  */
enum TFT_rotate_e TFT_rotate = TFT_Degrees_0; /**< Enum to hold rotation */

uint8_t _FontNumber = 1;
uint8_t _CurrentFontWidth = 5;	  /**< Store current font width */
uint8_t _CurrentFontoffset = 0;	  /**< Store current offset width */
uint8_t _CurrentFontheight = 8;	  /**< Store current offset height */
uint8_t _CurrentFontLength = 254; /**<Store current font number of characters */

void spiWrite(uint8_t spiData)
{
    spi_write_blocking(_pspiInterface, &spiData, 1);
}

void writeCommand(uint8_t command)
{
	TFT_DC_SetLow;
	TFT_CS_SetLow;
	spiWrite(command);
	TFT_CS_SetHigh;
}

void writeData(uint8_t dataByte)
{
	TFT_DC_SetHigh;
	TFT_CS_SetLow;
	spiWrite(dataByte);
	TFT_CS_SetHigh;
}

/*!
	@brief  Write a buffer to SPI, both Software and hardware SPI supported
	@param spiData to send
	@param len length of buffer
*/
void spiWriteDataBuffer(uint8_t *spiData, uint32_t len)
{
	TFT_DC_SetHigh;
	TFT_CS_SetLow;
    spi_write_blocking(_pspiInterface, spiData, len);
	TFT_CS_SetHigh;
}

void Rcmd1() {
	uint8_t seq1[] = { 0x01, 0x2C, 0x2D };
	uint8_t seq2[] = { 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D };
	uint8_t seq3[] = { 0xA2, 0x02, 0x84 }; 
	writeCommand(ST7735_SWRESET);
	TFT_MILLISEC_DELAY(150);
	writeCommand(ST7735_SLPOUT);
	TFT_MILLISEC_DELAY(500);
	writeCommand(ST7735_FRMCTR1);
	spiWriteDataBuffer(seq1, sizeof(seq1));
	writeCommand(ST7735_FRMCTR2);
	spiWriteDataBuffer(seq1, sizeof(seq1));
	writeCommand(ST7735_FRMCTR3);
	spiWriteDataBuffer(seq2, sizeof(seq2));
	writeCommand(ST7735_INVCTR);
	writeData(0x07);
	writeCommand(ST7735_PWCTR1);
	spiWriteDataBuffer(seq3, sizeof(seq3));
	writeCommand(ST7735_PWCTR2);
	writeData(0xC5);
	writeCommand(ST7735_PWCTR3);
	writeData(0x0A);
	writeData(0x00);
	writeCommand(ST7735_PWCTR4);
	writeData(0x8A);
	writeData(0x2A);
	writeCommand(ST7735_PWCTR5);
	writeData(0x8A);
	writeData(0xEE);
	writeCommand(ST7735_VMCTR1);
	writeData(0x0E);
	writeCommand(ST7735_INVOFF);
	writeCommand(ST7735_MADCTL);
	writeData(0xC8);
	writeCommand(ST7735_COLMOD);
	writeData(0x05);
}

void Rcmd3() {
	uint8_t seq4[] = {0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10}; 
	uint8_t seq5[] = {0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10}; 
	writeCommand(ST7735_GMCTRP1);
	spiWriteDataBuffer(seq4, sizeof(seq4));
	writeCommand(ST7735_GMCTRN1);
	spiWriteDataBuffer(seq5, sizeof(seq5));
	writeCommand(ST7735_NORON);
	TFT_MILLISEC_DELAY(10);
	writeCommand(ST7735_DISPON);
	TFT_MILLISEC_DELAY(100);
}

void Rcmd2red() {
	uint8_t seq1[] = { 0x00, 0x00, 0x00, 0x7F};
	uint8_t seq2[] = { 0x00, 0x00, 0x00, 0x9F};
	writeCommand(ST7735_CASET);
	spiWriteDataBuffer(seq1, sizeof(seq1));
	writeCommand(ST7735_RASET);
	spiWriteDataBuffer(seq2, sizeof(seq2));
}

void Rcmd2green() {
	writeCommand(ST7735_CASET);
	writeData(0x00);
	writeData(0x02);
	writeData(0x00);
	writeData(0x7F + 0x02);
	writeCommand(ST7735_RASET);
	writeData(0x00);
	writeData(0x01);
	writeData(0x00);
	writeData(0x9F + 0x01);
}

void TFTSetupGPIO()
{
	TFT_SDATA_INIT; 
	TFT_SCLK_INIT; 
	TFT_RST_INIT;
	TFT_DC_INIT; 
	TFT_CS_INIT; 
}

void TFTInitScreenSize(uint8_t colOffset, uint8_t rowOffset, uint16_t width_TFT, uint16_t height_TFT)
{
	_colstart = colOffset; 
	_rowstart = rowOffset;
	_XStart = colOffset; 
	_YStart = rowOffset;
	
	_widthTFT = width_TFT;
	_heightTFT = height_TFT;
	_widthStartTFT = width_TFT;
	_heightStartTFT = height_TFT;
}

void TFTResetPIN() {
	TFT_RST_SetDigitalOutput;
	TFT_RST_SetHigh;
	TFT_MILLISEC_DELAY(10);
	TFT_RST_SetLow;
	TFT_MILLISEC_DELAY(10);
	TFT_RST_SetHigh;
	TFT_MILLISEC_DELAY(10);
}

void TFTSPIInitialize(void)
{
	spi_init(_pspiInterface, _speedSPIKHz * 1000); // Initialize SPI port 
	// Initialize SPI pins : clock and data
	TFT_SDATA_SPI_FUNC;
	TFT_SCLK_SPI_FUNC;

    // Set SPI format
    spi_set_format( _pspiInterface,   // SPI instance
                    8,      // Number of bits per transfer
                    SPI_CPOL_0,      // Polarity (CPOL)
                    SPI_CPHA_0,      // Phase (CPHA)
                    SPI_MSB_FIRST);

}

void TFTRedTabInitialize() {
	TFTResetPIN();
	TFT_DC_SetLow;
	TFT_DC_SetDigitalOutput;
	TFT_CS_SetHigh;
	TFT_CS_SetDigitalOutput;

	TFTSPIInitialize();

	Rcmd1();
	Rcmd2red();
	Rcmd3();
	TFT_PCBtype = TFT_ST7735R_Red;
}

void TFTGreenTabInitialize() {
	TFTResetPIN();
	TFT_DC_SetLow;
	TFT_DC_SetDigitalOutput;
	TFT_CS_SetHigh;
	TFT_CS_SetDigitalOutput;
    
	TFTSPIInitialize();

	Rcmd1();
	Rcmd2green();
	Rcmd3();
	TFT_PCBtype = TFT_ST7735R_Green;
}

void TFTBlackTabInitialize() {
	TFTResetPIN();
	TFT_DC_SetLow;
	TFT_DC_SetDigitalOutput;
	TFT_CS_SetHigh;
	TFT_CS_SetDigitalOutput;

	TFTSPIInitialize();

	Rcmd1();
	Rcmd2red();
	Rcmd3();
	writeCommand(ST7735_MADCTL);
	writeData(0xC0);
	TFT_PCBtype = TFT_ST7735S_Black ;
}

void TFTInitPCBType(enum TFT_PCBtype_e pcbType)
{
	uint8_t choice = pcbType;
	switch(choice)
	{
		case TFT_ST7735R_Red : TFTRedTabInitialize(); break;
		case TFT_ST7735R_Green: TFTGreenTabInitialize(); break;
		case TFT_ST7735S_Black: TFTBlackTabInitialize(); break;
		// case TFT_ST7735B : TFTST7735BInitialize(); break;
	}
}

void TFTFontNum(enum TFT_Font_Type_e FontNumber)
{
	_FontNumber = (uint8_t)FontNumber;

    _CurrentFontWidth = TFTFont_width_5;
    _CurrentFontoffset = TFTFont_offset_none;
    _CurrentFontheight = TFTFont_height_8;
    _CurrentFontLength = TFTFontLenAll;
}

void TFTsetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	writeCommand(ST7735_CASET);
	writeData(0);
	writeData(x0 + _XStart);
	writeData(0);
	writeData(x1 + _XStart);
	writeCommand(ST7735_RASET);
	writeData(0);
	writeData(y0 + _YStart);
	writeData(0);
	writeData(y1 + _YStart);
	writeCommand(ST7735_RAMWR); // Write to RAM
}

uint8_t TFTfillRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color)
{
	uint8_t hi, lo;

	// Check bounds
	if ((x >= _widthTFT) || (y >= _heightTFT))
	{
		printf("Error TFTfillRectangle 2: Out of screen bounds\r\n");
		return 2;
	}
	if ((x + w - 1) >= _widthTFT)
		w = _widthTFT - x;
	if ((y + h - 1) >= _heightTFT)
		h = _heightTFT - y;

	// Colour to bytes
	hi = color >> 8;
	lo = color;

	// Create bitmap buffer
	uint8_t *buffer = (uint8_t *)malloc(w * h * sizeof(uint16_t));
	if (buffer == NULL) // check malloc
	{
		printf("Error TFTfillRectangle 3: MALLOC could not assign memory\r\n");
		return 3;
	}
	for (uint32_t i = 0; i < w * h * sizeof(uint16_t);)
	{
		buffer[i++] = hi;
		buffer[i++] = lo;
	}

	// Set window and write buffer
	TFTsetAddrWindow(x, y, x + w - 1, y + h - 1);
	spiWriteDataBuffer(buffer, h * w * sizeof(uint16_t));

	free(buffer);
	return 0;
}

void TFTfillScreen(uint16_t color)
{
	TFTfillRectangle(0, 0, _widthTFT, _heightTFT, color);
}

void TFTsetRotation(enum TFT_rotate_e mode) 
{
	uint8_t madctl = 0;
	uint8_t rotation;
	rotation = mode % 4;
	switch (rotation) {
		case TFT_Degrees_0 :
			if (TFT_PCBtype == TFT_ST7735S_Black ){
				madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_RGB;
			}else{
				madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR;
			}
			_widthTFT =_widthStartTFT;
			_heightTFT = _heightStartTFT;
			_XStart = _colstart;
			_YStart = _rowstart;
			break;
		case TFT_Degrees_90:
			if (TFT_PCBtype == TFT_ST7735S_Black )
			{
				madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_RGB;
			}else{
				madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_BGR;
			}
			_YStart = _colstart;
			_XStart = _rowstart;
			_widthTFT  =_heightStartTFT;
			_heightTFT = _widthStartTFT;
			break;
		case TFT_Degrees_180:
			if (TFT_PCBtype == TFT_ST7735S_Black)
			{
				madctl = ST7735_MADCTL_RGB;
			}else{
				madctl = ST7735_MADCTL_BGR;
			}
			_XStart = _colstart;
			_YStart = _rowstart;
			_widthTFT =_widthStartTFT;
			_heightTFT = _heightStartTFT;
			break;
		case TFT_Degrees_270:
			if (TFT_PCBtype == TFT_ST7735S_Black){
				madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_RGB;
			}else{
				madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR;
			}
			_YStart = _colstart;
			_XStart = _rowstart;
			_widthTFT =_heightStartTFT;
			_heightTFT = _widthStartTFT;
			break;
	}
	writeCommand(ST7735_MADCTL);
	writeData(madctl);
}

void TFTdrawPixel(uint8_t x, uint8_t y, uint16_t color)
{
	if ((x >= _widthTFT) || (y >= _heightTFT))
		return;
	TFTsetAddrWindow(x, y, x + 1, y + 1);
	writeData(color >> 8);
	writeData(color & 0xFF);
}

uint8_t TFTdrawChar(uint8_t x, uint8_t y, uint8_t character, uint16_t color, uint16_t bg, uint8_t size)
{

	int8_t i, j;
	uint8_t line;
	// 0. Check size
	if (size == 0 || size >= 15)
		return 2;
	// 1. Check for screen out of bounds
	if ((x >= _widthTFT) ||								  // Clip right
		(y >= _heightTFT) ||							  // Clip bottom
		((x + (_CurrentFontWidth + 1) * size - 1) < 0) || // Clip left
		((y + _CurrentFontheight * size - 1) < 0))		  // Clip top
	{
		printf("Error TFTdrawChar 3: Co-ordinates out of bounds\r\n");
		return 3;
	}

	// 2. Check for character out of font range bounds
	if (character < _CurrentFontoffset || character >= (_CurrentFontLength + _CurrentFontoffset))
	{
		printf("Error TFTdrawChar 4: Character = %u , Out of Font bounds %u <-> %u\r\n", character, _CurrentFontoffset, _CurrentFontLength + _CurrentFontoffset);
		return 4;
	}

	for (i = 0; i < (_CurrentFontWidth + 1); i++)
	{

		if (i == _CurrentFontWidth)
		{
			line = 0x00;
		}
		else
		{
			switch (_FontNumber)
			{
			case TFTFont_Default:
				line = pFontDefaultptr[(character - _CurrentFontoffset) * _CurrentFontWidth + i];
				break;
			case TFTFont_Thick:
				line = pFontThickptr[(character - _CurrentFontoffset) * _CurrentFontWidth + i];
				break;
			case TFTFont_Seven_Seg:
				line = pFontSevenSegptr[(character - _CurrentFontoffset) * _CurrentFontWidth + i];
				break;
			case TFTFont_Wide:
				line = pFontWideptr[(character - _CurrentFontoffset) * _CurrentFontWidth + i];
				break;
			case TFTFont_Tiny:
				line = pFontTinyptr[(character - _CurrentFontoffset) * _CurrentFontWidth + i];
				break;
			case TFTFont_HomeSpun:
				line = pFontHomeSpunptr[(character - _CurrentFontoffset) * _CurrentFontWidth + i];
				break;
			default:
				printf("Error TFTdrawChar 5: Wrong font number set must be 1-6 : %u \r\n", _FontNumber);
				return 5;
				break;
			}
		}
		for (j = 0; j < _CurrentFontheight; j++, line >>= 1)
		{
			if (line & 0x01)
			{
				if (size == 1)
					TFTdrawPixel(x + i, y + j, color);
				else
					TFTfillRect(x + (i * size), y + (j * size), size, size, color);
			}
			else if (bg != color)
			{
				if (size == 1)
					TFTdrawPixel(x + i, y + j, bg);
				else
					TFTfillRect(x + i * size, y + j * size, size, size, bg);
			}
		}
	}
	return 0;
}

uint8_t TFTdrawText(uint8_t x, uint8_t y, char *pText, uint16_t color, uint16_t bg, uint8_t size)
{

	// Check if correct font
	if (_FontNumber >= TFTFont_Bignum)
	{
		printf("Error TFTdrawText 2: Wrong font number selected, must be 1-6\r\n");
		return 2;
	}
	// Check for null pointer
	if (pText == NULL)
	{
		printf("Error TFTdrawText 3: String array is not valid pointer object\r\n");
		return 3;
	}
	// Out of screen bounds
	if ((x >= _widthTFT) || (y >= _heightTFT))
	{
		printf("Error TFTdrawText 4: Out of screen bounds\r\n");
		return 4;
	}
	uint8_t cursorX = x;
	uint8_t cursorY = y;
	while (*pText != '\0')
	{
		if (_wrap && ((cursorX + size * _CurrentFontWidth) > _widthTFT))
		{
			cursorX = 0;
			cursorY = cursorY + size * 7 + 3;
			if (cursorY > _heightTFT)
				cursorY = _heightTFT;
		}
		if (TFTdrawChar(cursorX, cursorY, *pText, color, bg, size) != 0)
		{
			printf("Error TFTdrawText 5: Method TFTdrawChar failed\r\n");
			return 5;
		}
		cursorX = cursorX + size * (_CurrentFontWidth + 1);

		if (cursorX > _widthTFT)
			cursorX = _widthTFT;
		pText++;
	}
	return 0;
}

void TFTdrawFastVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color)
{
	uint8_t hi, lo;
	if ((x >= _widthTFT) || (y >= _heightTFT))
		return;
	if ((y + h - 1) >= _heightTFT)
		h = _heightTFT - y;
	hi = color >> 8;
	lo = color;
	TFTsetAddrWindow(x, y, x, y + h - 1);
	TFT_DC_SetHigh;

	TFT_CS_SetLow;
	while (h--)
	{
		spiWrite(hi);
		spiWrite(lo);
	}
	TFT_CS_SetHigh;
}

void TFTdrawFastHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color)
{
	uint8_t hi, lo;
	if ((x >= _widthTFT) || (y >= _heightTFT))
		return;
	if ((x + w - 1) >= _widthTFT)
		w = _widthTFT - x;
	hi = color >> 8;
	lo = color;
	TFTsetAddrWindow(x, y, x + w - 1, y);
	TFT_DC_SetHigh;
	TFT_CS_SetLow;
	while (w--)
	{
		spiWrite(hi);
		spiWrite(lo);
	}
	TFT_CS_SetHigh;
}

void TFTdrawRectWH(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color)
{
	TFTdrawFastHLine(x, y, w, color);
	TFTdrawFastHLine(x, y + h - 1, w, color);
	TFTdrawFastVLine(x, y, h, color);
	TFTdrawFastVLine(x + w - 1, y, h, color);
}

void TFTfillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color)
{
	int16_t i;
	for (i = x; i < x + w; i++)
	{
		TFTdrawFastVLine(i, y, h, color);
	}
}