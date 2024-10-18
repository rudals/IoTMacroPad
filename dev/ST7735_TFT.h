#pragma once

// ST7735 general purpose
#define ST7735_NOP 0x00		/**< Non operation */
#define ST7735_SWRESET 0x01 /**< Soft Reset */
#define ST7735_RDDID 0x04	/**< Read Display ID*/
#define ST7735_RDDST 0x09	/**< Read Display Status*/

// ST7735 Modes
#define ST7735_SLPIN 0x10			/**< Sleep ON */
#define ST7735_SLPOUT 0x11			/**< Sleep OFF */
#define ST7735_PTLON 0x12			/**< Partial mode */
#define ST7735_NORON 0x13			/**< Normal Display */
#define ST7735_INVOFF 0x20			/**< Display invert off */
#define ST7735_INVON 0x21			/**< Display Invert on */
#define ST7735_DISPOFF 0x28			/**< Display off */
#define ST7735_DISPON 0x29			/**< Display on */
#define ST7735_TFT_Idle_modeON 0x39 /**< Idle Mode ON */
#define ST7735_TFT_Idle_modeOF 0x38 /**< Idle Mode OFF */

// ST7735 Addressing
#define ST7735_CASET 0x2A	 /**< Column address set */
#define ST7735_RASET 0x2B	 /**<  Page address set */
#define ST7735_RAMWR 0x2C	 /**< Memory write */
#define ST7735_RAMRD 0x2E	 /**< Memory read */
#define ST7735_PTLAR 0x30	 /**< Partial Area */
#define ST7735_VSCRDEF 0x33	 /**< Vertical scroll def */
#define ST7735_COLMOD 0x3A	 /**< Interface Pixel Format */
#define ST7735_MADCTL 0x36	 /**< Memory Access Control */
#define ST7735_VSCRSADD 0x37 /**< Vertical Access Control */

// Frame Rate Control
#define ST7735_FRMCTR1 0xB1 /**< Normal */
#define ST7735_FRMCTR2 0xB2 /**< idle */
#define ST7735_FRMCTR3 0xB3 /**< Partial */

#define ST7735_INVCTR 0xB4	/**< Display Inversion control */
#define ST7735_DISSET5 0xB6 /**< Display Function set */

// ST7735 Power_Control
#define ST7735_PWCTR1 0xC0 /**< Power_Control VRH: Set the GVDD voltage */
#define ST7735_PWCTR2 0xC1 /**< Power_Control BT: set VGH/ VGL voltage */
#define ST7735_PWCTR3 0xC2 /**< Power_Control APA: adjust op amp & DCA: boost Voltage */
#define ST7735_PWCTR4 0xC3 /**< Power_Control APB: adjust op amp & DCB: boost Voltage */
#define ST7735_PWCTR5 0xC4 /**< Power_Control APC: adjust op amp & DCC: boost Voltage */
#define ST7735_PWCTR6 0xFC /**< Power_Control In partial mode + Idle */

#define ST7735_VMCTR1 0xC5 /**< VCOM_Control 1 */

#define ST7735_RDID1 0xDA /**< read ID1 */
#define ST7735_RDID2 0xDB /**< read ID2  */
#define ST7735_RDID3 0xDC /**< read ID3  */
#define ST7735_RDID4 0xDD /**< read ID4  */

// ST7735 color control
#define ST7735_GMCTRP1 0xE0 /**< Positive Gamma Correction Setting */
#define ST7735_GMCTRN1 0xE1 /**< Negative Gamma Correction Setting */

// Memory Access Data Control  Register
#define ST7735_MADCTL_MY 0x80  /**< Row Address Order */
#define ST7735_MADCTL_MX 0x40  /**< Column Address Order */
#define ST7735_MADCTL_MV 0x20  /**< Row/Column Order (MV) */
#define ST7735_MADCTL_ML 0x10  /**< Vertical Refresh Order */
#define ST7735_MADCTL_RGB 0x00 /**< RGB order */
#define ST7735_MADCTL_BGR 0x08 /**< BGR order */
#define ST7735_MADCTL_MH 0x04  /**< Horizontal Refresh Order */

// Color definitions 16-Bit Color Values R5G6B5
#define ST7735_BLACK 0x0000
#define ST7735_BLUE 0x001F
#define ST7735_RED 0xF800
#define ST7735_GREEN 0x07E0
#define ST7735_CYAN 0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW 0xFFE0
#define ST7735_WHITE 0xFFFF
#define ST7735_TAN 0xED01
#define ST7735_GREY 0x9CD1
#define ST7735_BROWN 0x6201
#define ST7735_DGREEN 0x01c0
#define ST7735_ORANGE 0xFC00

enum TFT_Font_Type_e {
    TFTFont_Default = 1,	/**< Default Font, Full extended ASCII */
    TFTFont_Thick = 2,		/**< Thick font , no lower case letters*/
    TFTFont_Seven_Seg = 3,	/**< Seven Segment Font */
    TFTFont_Wide = 4,		/**< Wide font no lowercase letters*/
    TFTFont_Tiny = 5,		/**< Tiny font */
    TFTFont_HomeSpun = 6,	/**< HomeSpun Font */
    TFTFont_Bignum = 7,		/**< Bignum numbers only */
    TFTFont_Mednum = 8,		/**< Mednum number only */
    TFTFont_ArialRound = 9, /**< Arial round font */
    TFTFont_ArialBold = 10, /**< Arial bold font */
    TFTFont_Mia = 11,		/**< Mia font */
    TFTFont_Dedica = 12		/**< dedica font */
};

/*! TFT display modes */
enum TFT_modes_e {
    TFT_Normal_mode = 0, /**< In this mode, the display is able to show maximum 262*/
    TFT_Partial_mode,	 /**< In this mode part of the display is used with maximum 262*/
    TFT_Idle_mode,		 /**< In this mode, part of the display is used but with 8 colors.*/
    TFT_Sleep_mode,		 /**<  In this mode, the DC: DC converter, internal oscillator and panel driver circuit are stopped. Only the MCU interface and
                            memory works with VDDI power supply. Contents of the memory are safe.*/
    TFT_Invert_mode,	 /**< Invert display colors */
    TFT_Display_on_mode, /**< Display On mode*/
    TFT_Display_off_mode /**< In this mode, both VDD and VDDI are removed.*/
};

/*! TFT rotate modes in degrees*/
enum TFT_rotate_e {
    TFT_Degrees_0 = 0, /**< No rotation 0 degrees*/
    TFT_Degrees_90,	   /**< Rotation 90 degrees*/
    TFT_Degrees_180,   /**< Rotation 180 degrees*/
    TFT_Degrees_270	   /**< Rotation 270 degrees*/
};

enum TFT_PCBtype_e {
    TFT_ST7735R_Red = 0, /**<  ST7735R Red Tab  */
    TFT_ST7735R_Green,	 /**<  ST7735R Green Tab */
    TFT_ST7735S_Black,	 /**<  ST7735S Black Tab */
    TFT_ST7735B,		 /**<  ST7735B controller */
};

enum TFT_Font_width_e {
    TFTFont_width_3 = 3,  /**< 3 tiny font */
    TFTFont_width_4 = 4,  /**< 4 seven segment font */
    TFTFont_width_5 = 5,  /**< 5 default font */
    TFTFont_width_6 = 6,  /**< dedica font  */
    TFTFont_width_7 = 7,  /**< 7 homespun & thick font*/
    TFTFont_width_8 = 8,  /**< 8 wide & mia font*/
    TFTFont_width_16 = 16 /**< 16 font 7-10*/
};

/*! font offset in the ASCII table*/
enum TFT_Font_offset_e {
    TFTFont_offset_none = 0x00,	 /**< extended ASCII */
    TFTFont_offset_space = 0x20, /**< Starts at Space, alphanumeric */
    TFTFont_offset_minus = 0x2D, /**< Starts at Minus, extended numeric */
    TFTFont_offset_zero = 0x30	 /**< Starts at zero, numeric */
};

/*! Height of the font in bits*/
enum TFT_Font_height_e {
    TFTFont_height_8 = 8,	/**< 8 bit font 1-6 at size 1*/
    TFTFont_height_12 = 12, /**< 12 bit font 12 */
    TFTFont_height_16 = 16, /**< 16 bit font 8, 10 11*/
    TFTFont_height_24 = 24, /**< 24 bit font 9 */
    TFTFont_height_32 = 32	/**< 32 bit font 7 */
};

/*! Number of ASCII characters in Font */
enum TFTFontLength_e {
    TFTFontLenNumeric = 14,			/**< extended Numeric 0x2D - 0x3A */
    TFTFontLenAlphaNumNoLCase = 59, /**< reduced Alphanumeric 0x20-0x5A*/
    TFTFontLenAlphaNum = 95,		/**< Full Alphanumeric 0x20-0x7E */
    TFTFontLenAll = 255				/**< Full Range  0-0xFF */
};

void TFTSetupGPIO();
void TFTInitScreenSize(uint8_t colOffset, uint8_t rowOffset, uint16_t width_TFT, uint16_t height_TFT);
void TFTInitPCBType(enum TFT_PCBtype_e pcbType);
void TFTResetPIN();
void TFTSPIInitialize(void);
void TFTRedTabInitialize();
void TFTsetRotation(enum TFT_rotate_e mode);
void TFTdrawFastVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color);
void TFTdrawFastHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color);
void TFTdrawRectWH(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
uint8_t TFTfillRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void TFTfillScreen(uint16_t color);
void TFTFontNum(enum TFT_Font_Type_e FontNumber);
void TFTdrawPixel(uint8_t x, uint8_t y, uint16_t color);
uint8_t TFTdrawChar(uint8_t x, uint8_t y, uint8_t character, uint16_t color, uint16_t bg, uint8_t size);
uint8_t TFTdrawText(uint8_t x, uint8_t y, char *pText, uint16_t color, uint16_t bg, uint8_t size);
void TFTfillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);