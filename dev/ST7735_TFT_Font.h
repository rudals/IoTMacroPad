/*!
	@file     ST7735_TFT_Font.hpp
	@author   Gavin Lyons
	@brief    Library header file for ST7735_TFT_PICO font data
			  This file contains the font definitions and data.
			  10 fonts see main readme file for details.
			  font data is in cpp file.
	@note 		Fonts 9 -12  optional , comment in define _TFT_OPTIONAL_FONT_X to enable
*/

#pragma once

#include "pico/stdlib.h"

#define _TFT_OPTIONAL_FONT_9
#define _TFT_OPTIONAL_FONT_10
#define _TFT_OPTIONAL_FONT_11
#define _TFT_OPTIONAL_FONT_12

extern const unsigned char *pFontDefaultptr;
extern const unsigned char *pFontThickptr;
extern const unsigned char *pFontSevenSegptr;
extern const unsigned char *pFontWideptr;
extern const unsigned char *pFontTinyptr;
extern const unsigned char *pFontHomeSpunptr;
extern const uint8_t (*pFontBigNum16x32ptr)[64];
extern const uint8_t (*pFontMedNum16x16ptr)[32];
#ifdef _TFT_OPTIONAL_FONT_9
extern const uint8_t (*pFontArial16x24ptr)[48];
#endif
#ifdef _TFT_OPTIONAL_FONT_10
extern const uint8_t (*pFontArial16x16ptr)[32];
#endif
#ifdef _TFT_OPTIONAL_FONT_11
extern const uint8_t (*pFontMia8x16ptr)[16];
#endif
#ifdef _TFT_OPTIONAL_FONT_12
extern const uint8_t (*pFontDedica6x12ptr)[12];
#endif