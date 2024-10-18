#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "ST7735_TFT_graphics.h"
#include "ST7735_TFT.h"
#include "ST7735_TFT_Font.h"

extern uint16_t _widthTFT;
extern uint16_t _heightTFT;
extern void TFTsetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
extern void spiWriteDataBuffer(uint8_t *spiData, uint32_t len);

uint8_t TFTdrawIcon(uint8_t x, uint8_t y, uint8_t w, uint16_t color, uint16_t backcolor, const unsigned char character[])
{	
	if ((x >= _widthTFT) || (y >= _heightTFT)) {
		printf("Error TFTdrawIcon 2: Out of screen bounds\r\n");
		return 2;
	}
	
	if (character == NULL) {
		printf("Error TFTdrawIcon 3: Character array is not valid pointer object\r\n");
		return 3;
	}
	
	if (w >= _widthTFT) {
		printf("Error TFTdrawIcon 4: Icon is greater than Screen width\r\n");
		return 4;
	}

	uint8_t value;
	for (uint8_t byte = 0; byte < w; byte++) {
		for (uint8_t mybit = 0; mybit < 8; mybit++) {
			value = !!(character[byte] & (1 << mybit));
			if (value) {
				TFTdrawPixel(x + byte, y + mybit, backcolor);
			} else {
				TFTdrawPixel(x + byte, y + mybit, color);
			}
			value = 0;
		}
	}
	return 0;
}

uint8_t TFTdrawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, uint16_t bgcolor, uint8_t *pBmp, uint16_t sizeOfBitmap)
{
	int16_t byteWidth = (w + 7) / 8;
	uint8_t byte = 0;
	uint16_t mycolor = 0;
	uint32_t ptr;

	if (sizeOfBitmap != ((w / 8) * h)) {
		printf("Error TFTdrawBitmap 4 : Horizontal Bitmap size is incorrect:  Check Size =  (w/8 * h): %u  %i  %i \n", sizeOfBitmap, w, h);
		printf("Check size = ((w/8)*h) or Is bitmap width divisible evenly by eight or is all bitmap data there or too much \n");
		return 4;
	}

	if (pBmp == NULL) {
		printf("Error TFTdrawBitmap 1: Bitmap array is NULL\r\n");
		return 1;
	}

	if ((x >= _widthTFT) || (y >= _heightTFT)) {
		printf("Error TFTdrawBitmap 2: Out of screen bounds, check x & y\r\n");
		return 2;
	}

	if ((x + w - 1) >= _widthTFT)
		w = _widthTFT - x;
	if ((y + h - 1) >= _heightTFT)
		h = _heightTFT - y;

	uint8_t *buffer = (uint8_t *)malloc(w * h * 2);
	if (buffer == NULL) {
		printf("Error TFTdrawBitmap 3: MALLOC could not assign memory \r\n");
		return 3;
	}

	ptr = 0;
	for (int16_t j = 0; j < h; j++) {
		for (int16_t i = 0; i < w; i++) {
			if (i & 7)
				byte <<= 1;
			else
				byte = (pBmp[j * byteWidth + i / 8]);
			mycolor = (byte & 0x80) ? color : bgcolor;
			buffer[ptr++] = mycolor >> 8;
			buffer[ptr++] = mycolor;
		}
	}
	
	TFTsetAddrWindow(x, y, x + w - 1, y + h - 1);
	spiWriteDataBuffer(buffer, h * w * sizeof(uint16_t));

	free(buffer);
	return 0;
}

uint8_t TFTdrawBitmap24Data(uint8_t x, uint8_t y, uint8_t *pBmp, uint8_t w, uint8_t h)
{
	uint8_t i, j;
	uint32_t ptr;
	uint16_t color, red, green, blue = 0;
	
	if (pBmp == NULL) {
		printf("Error TFTdrawBitmap24 1: Bitmap array is NULL\r\n");
		return 1;
	}
	
	if ((x >= _widthTFT) || (y >= _heightTFT)) {
		printf("Error TFTdrawBitmap24 2: Out of screen bounds\r\n");
		return 2;
	}
	
	if ((x + w - 1) >= _widthTFT)
		w = _widthTFT - x;
	if ((y + h - 1) >= _heightTFT)
		h = _heightTFT - y;

	uint8_t *buffer = (uint8_t *)malloc(w * h * 2);
	if (buffer == NULL) {
		printf("Error TFTdrawBitmap24 3: MALLOC could not assign memory \r\n");
		return 3;
	}

	ptr = 0;
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			red = *pBmp++;
			green = *pBmp++;
			blue = *pBmp++;
			color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
			buffer[ptr++] = color >> 8;
			buffer[ptr++] = color & 0x00FF;
		}
	}

	TFTsetAddrWindow(x, y, x + w - 1, y + h - 1);
	spiWriteDataBuffer(buffer, h * w * sizeof(uint16_t));

	free(buffer);
	return 0;
}

uint8_t TFTdrawBitmap16Data(uint8_t x, uint8_t y, uint8_t *pBmp, uint8_t w, uint8_t h)
{
	uint8_t i, j;
	uint32_t ptr;

	if (pBmp == NULL) {
		printf("Error TFTdrawBitmap24 1: Bitmap array is NULL\r\n");
		return 1;
	}
	
	if ((x >= _widthTFT) || (y >= _heightTFT)) {
		printf("Error TFTdrawBitmap16 2: Out of screen bounds\r\n");
		return 2;
	}
	if ((x + w - 1) >= _widthTFT)
		w = _widthTFT - x;
	if ((y + h - 1) >= _heightTFT)
		h = _heightTFT - y;


	uint8_t *buffer = (uint8_t *)malloc(w * h * 2);
	if (buffer == NULL)	{
		printf("Error TFTdrawBitmap16 3 :MALLOC could not assign memory\r\n");
		return 3;
	}
	ptr = 0;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			buffer[ptr++] = (*pBmp++);
			buffer[ptr++] = (*pBmp++);
		}
	}
	TFTsetAddrWindow(x, y, x + w - 1, y + h - 1);
	spiWriteDataBuffer(buffer, h * w * sizeof(uint16_t));

	free(buffer);
	return 0;
}