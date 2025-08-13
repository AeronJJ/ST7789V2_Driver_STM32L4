#ifndef LCD_h
#define LCD_h

#include "ST7789V2_Driver.h"
#include <stdlib.h>

// https://github.com/newdigate/rgb565_colors for more colours

#define RGB565_BLACK      0x0000
#define RGB565_WHITE      0xFFFF
#define RGB565_RED        0xF800
#define RGB565_GREEN      0x07E0
#define RGB565_BLUE       0x001F
#define RGB565_YELLOW     0xFFE0
#define RGB565_CYAN       0x07FF
#define RGB565_MAGENTA    0xF81F
#define RGB565_GRAY       0x8410
#define RGB565_LIGHT_GRAY 0xC618
#define RGB565_DARK_GRAY  0x4208
#define RGB565_ORANGE     0xFD20
#define RGB565_BROWN      0xA145
#define RGB565_PINK       0xFC18
#define RGB565_PURPLE     0x780F
#define RGB565_TEAL       0x0438
#define RGB565_NAVY       0x000F
#define RGB565_MAROON     0x8000
#define RGB565_OLIVE      0x8400
#define RGB565_SKY_BLUE   0x867D
#define RGB565_GOLD       0xFEA0
#define RGB565_VIOLET     0x915C

#define LCD_COLOUR_0  RGB565_BLACK
#define LCD_COLOUR_1  RGB565_WHITE
#define LCD_COLOUR_2  RGB565_RED
#define LCD_COLOUR_3  RGB565_GREEN
#define LCD_COLOUR_4  RGB565_BLUE
#define LCD_COLOUR_5  RGB565_ORANGE
#define LCD_COLOUR_6  RGB565_YELLOW
#define LCD_COLOUR_7  RGB565_PINK
#define LCD_COLOUR_8  RGB565_PURPLE
#define LCD_COLOUR_9  RGB565_NAVY
#define LCD_COLOUR_10 RGB565_GOLD
#define LCD_COLOUR_11 RGB565_VIOLET
#define LCD_COLOUR_12 RGB565_BROWN
#define LCD_COLOUR_13 RGB565_GRAY
#define LCD_COLOUR_14 RGB565_CYAN
#define LCD_COLOUR_15 RGB565_MAGENTA

/* Initialise display
*   Powers up the display and turns on backlight.
*   Sets the display up in horizontal addressing mode and with normal video mode.*/
void LCD_init(ST7789V2_cfg_t* cfg);

/* Turn off
*   Powers down the display and turns off the backlight.*/
void LCD_turnOff(ST7789V2_cfg_t* cfg);

/* Turn on
*   Powers up the display and turns on the backlight.*/
void LCD_turnOn(ST7789V2_cfg_t* cfg);

/* Clear
*   Clears the screen buffer.*/
void LCD_clear();

/* Normal mode
*   Turn on normal video mode (default).*/
void LCD_normalMode(ST7789V2_cfg_t* cfg);

/* Inverse mode
*   Turn on inverse video mode */
void LCD_inverseMode(ST7789V2_cfg_t* cfg);

/* Print String
*   Prints a string of characters to the screen buffer. String is cut-off after the 83rd pixel.
*   @param x - the column number (0 to 83)
*   @param y - the row number (0 to 5) - the display is split into 6 banks - each bank can be considered a row*/
void LCD_printString(char const *str, const uint16_t x, const uint16_t y, uint8_t colour, uint8_t font_size);

/* Print Character
*   Sends a character to the screen buffer.  Printed at the specified location. Character is cut-off after the 83rd pixel.
*   @param  c - the character to print. Can print ASCII as so printChar('C').
*   @param x - the column number (0 to 83)
*   @param y - the row number (0 to 5) - the display is split into 6 banks - each bank can be considered a row*/
void LCD_printChar(char const c, const uint16_t x, const uint16_t y, uint8_t colour);

/* Set a Pixel
* @param x      The x co-ordinate of the pixel (0 to 239)
* @param y      The y co-ordinate of the pixel (0 to 279)
* @param colour The colour of the pixel
* @details This function sets the colour of a pixel in the screen buffer.*/
void LCD_Set_Pixel(const uint16_t x, const uint16_t y, uint8_t colour);

/* Get a Pixel
*   This function gets the status of a pixel in the screen buffer.
*   @param  x - the x co-ordinate of the pixel (0 to 83)
*   @param  y - the y co-ordinate of the pixel (0 to 47)
*   @returns
*       0           - pixel is clear
*       1    - pixel is set*/
int getPixel(unsigned int const x, unsigned int const y);

/* Refresh display
*   This functions sends the screen buffer to the display.*/
void LCD_refresh();

/* Randomise buffer
*   This function fills the buffer with random data.  Can be used to test the display.
*   A call to refresh() must be made to update the display to reflect the change in pixels.
*   The seed is not set and so the generated pattern will probably be the same each time.
*   TODO: Randomise the seed - maybe using the noise on the AnalogIn pins.*/
void LCD_randomiseBuffer();

/* Plot Array
*   This function plots a one-dimensional array in the buffer.
*   @param array[] - y values of the plot. Values should be normalised in the range 0.0 to 1.0. First 84 plotted.*/
void LCD_plotArray(float const array[]);

/* Draw Circle
*   This function draws a circle at the specified origin with specified radius in the screen buffer
*   Uses the midpoint circle algorithm.
*   @see http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
*   @param  x0     - x-coordinate of centre
*   @param  y0     - y-coordinate of centre
*   @param  radius - radius of circle in pixels
*   @param  fill   - fill-type for the shape*/
// void LCD_drawCircle(unsigned int const x0, unsigned int const y0, unsigned int const radius, FillType const fill);

/* Draw Line
*   This function draws a line between the specified points using linear interpolation.
*   @param  x0 - x-coordinate of first point
*   @param  y0 - y-coordinate of first point
*   @param  x1 - x-coordinate of last point
*   @param  y1 - y-coordinate of last point
*   @param  colour - 4-bit colour*/
void LCD_Draw_Line(const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, uint8_t colour);

/* Draw Rectangle
*   This function draws a rectangle.
*   @param  x0 - x-coordinate of origin (top-left)
*   @param  y0 - y-coordinate of origin (top-left)
*   @param  width - width of rectangle
*   @param  height - height of rectangle
*   @param  fill   - fill-type for the shape*/
void LCD_Draw_Rect(const uint16_t x0, const uint16_t y0, const uint16_t width, const uint16_t height, uint8_t colour, uint8_t fill);

/* Draw Sprite
*   This function draws a sprite as defined in a 2D array
*   @param  x0 - x-coordinate of origin (top-left)
*   @param  y0 - y-coordinate of origin (top-left)
*   @param  nrows - number of rows in sprite
*   @param  ncols - number of columns in sprite
*   @param  sprite - 2D array representing the sprite*/
void LCD_drawSprite(int x0, int y0, int nrows, int ncols, int *sprite);

void LCD_setXYAddress(unsigned int const x,
                unsigned int const y);
                
void LCD_initSPI();
void LCD_turnOn();
void LCD_reset();
void LCD_clearRAM();
void LCD_sendCommand(unsigned char command);
void LCD_sendData(unsigned char data);

void LCD_Refresh(ST7789V2_cfg_t* cfg);
// void LCD_Fill_Buffer(ST7789V2_cfg_t* cfg, uint16_t colour);
void LCD_Fill_Buffer(uint8_t colour);

void LCD_Fill(ST7789V2_cfg_t* cfg, const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, const uint16_t colour);

void ST7789_Init(ST7789V2_cfg_t* cfg);
void ST7789_SendLine_DMA(ST7789V2_cfg_t* cfg, uint16_t x, uint16_t y, uint16_t *data, uint16_t len);
void ST7789_SendSolidLine_DMA(ST7789V2_cfg_t* cfg, uint16_t x, uint16_t y, uint16_t pixel, uint16_t len);

void Print_SPI2_DMA_Registers(void);


extern const unsigned char font5x7_[480];// = {
//     0x00, 0x00, 0x00, 0x00, 0x00,// (space)
//     0x00, 0x00, 0x5F, 0x00, 0x00,// !
//     0x00, 0x07, 0x00, 0x07, 0x00,// "
//     0x14, 0x7F, 0x14, 0x7F, 0x14,// #
//     0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
//     0x23, 0x13, 0x08, 0x64, 0x62,// %
//     0x36, 0x49, 0x55, 0x22, 0x50,// &
//     0x00, 0x05, 0x03, 0x00, 0x00,// '
//     0x00, 0x1C, 0x22, 0x41, 0x00,// (
//     0x00, 0x41, 0x22, 0x1C, 0x00,// )
//     0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
//     0x08, 0x08, 0x3E, 0x08, 0x08,// +
//     0x00, 0x50, 0x30, 0x00, 0x00,// ,
//     0x08, 0x08, 0x08, 0x08, 0x08,// -
//     0x00, 0x60, 0x60, 0x00, 0x00,// .
//     0x20, 0x10, 0x08, 0x04, 0x02,// /
//     0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
//     0x00, 0x42, 0x7F, 0x40, 0x00,// 1
//     0x42, 0x61, 0x51, 0x49, 0x46,// 2
//     0x21, 0x41, 0x45, 0x4B, 0x31,// 3
//     0x18, 0x14, 0x12, 0x7F, 0x10,// 4
//     0x27, 0x45, 0x45, 0x45, 0x39,// 5
//     0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
//     0x01, 0x71, 0x09, 0x05, 0x03,// 7
//     0x36, 0x49, 0x49, 0x49, 0x36,// 8
//     0x06, 0x49, 0x49, 0x29, 0x1E,// 9
//     0x00, 0x36, 0x36, 0x00, 0x00,// :
//     0x00, 0x56, 0x36, 0x00, 0x00,// ;
//     0x00, 0x08, 0x14, 0x22, 0x41,// <
//     0x14, 0x14, 0x14, 0x14, 0x14,// =
//     0x41, 0x22, 0x14, 0x08, 0x00,// >
//     0x02, 0x01, 0x51, 0x09, 0x06,// ?
//     0x32, 0x49, 0x79, 0x41, 0x3E,// @
//     0x7E, 0x11, 0x11, 0x11, 0x7E,// A
//     0x7F, 0x49, 0x49, 0x49, 0x36,// B
//     0x3E, 0x41, 0x41, 0x41, 0x22,// C
//     0x7F, 0x41, 0x41, 0x22, 0x1C,// D
//     0x7F, 0x49, 0x49, 0x49, 0x41,// E
//     0x7F, 0x09, 0x09, 0x01, 0x01,// F
//     0x3E, 0x41, 0x41, 0x51, 0x32,// G
//     0x7F, 0x08, 0x08, 0x08, 0x7F,// H
//     0x00, 0x41, 0x7F, 0x41, 0x00,// I
//     0x20, 0x40, 0x41, 0x3F, 0x01,// J
//     0x7F, 0x08, 0x14, 0x22, 0x41,// K
//     0x7F, 0x40, 0x40, 0x40, 0x40,// L
//     0x7F, 0x02, 0x04, 0x02, 0x7F,// M
//     0x7F, 0x04, 0x08, 0x10, 0x7F,// N
//     0x3E, 0x41, 0x41, 0x41, 0x3E,// O
//     0x7F, 0x09, 0x09, 0x09, 0x06,// P
//     0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
//     0x7F, 0x09, 0x19, 0x29, 0x46,// R
//     0x46, 0x49, 0x49, 0x49, 0x31,// S
//     0x01, 0x01, 0x7F, 0x01, 0x01,// T
//     0x3F, 0x40, 0x40, 0x40, 0x3F,// U
//     0x1F, 0x20, 0x40, 0x20, 0x1F,// V
//     0x7F, 0x20, 0x18, 0x20, 0x7F,// W
//     0x63, 0x14, 0x08, 0x14, 0x63,// X
//     0x03, 0x04, 0x78, 0x04, 0x03,// Y
//     0x61, 0x51, 0x49, 0x45, 0x43,// Z
//     0x00, 0x00, 0x7F, 0x41, 0x41,// [
//     0x02, 0x04, 0x08, 0x10, 0x20,// "\"
//     0x41, 0x41, 0x7F, 0x00, 0x00,// ]
//     0x04, 0x02, 0x01, 0x02, 0x04,// ^
//     0x40, 0x40, 0x40, 0x40, 0x40,// _
//     0x00, 0x01, 0x02, 0x04, 0x00,// `
//     0x20, 0x54, 0x54, 0x54, 0x78,// a
//     0x7F, 0x48, 0x44, 0x44, 0x38,// b
//     0x38, 0x44, 0x44, 0x44, 0x20,// c
//     0x38, 0x44, 0x44, 0x48, 0x7F,// d
//     0x38, 0x54, 0x54, 0x54, 0x18,// e
//     0x08, 0x7E, 0x09, 0x01, 0x02,// f
//     0x08, 0x14, 0x54, 0x54, 0x3C,// g
//     0x7F, 0x08, 0x04, 0x04, 0x78,// h
//     0x00, 0x44, 0x7D, 0x40, 0x00,// i
//     0x20, 0x40, 0x44, 0x3D, 0x00,// j
//     0x00, 0x7F, 0x10, 0x28, 0x44,// k
//     0x00, 0x41, 0x7F, 0x40, 0x00,// l
//     0x7C, 0x04, 0x18, 0x04, 0x78,// m
//     0x7C, 0x08, 0x04, 0x04, 0x78,// n
//     0x38, 0x44, 0x44, 0x44, 0x38,// o
//     0x7C, 0x14, 0x14, 0x14, 0x08,// p
//     0x08, 0x14, 0x14, 0x18, 0x7C,// q
//     0x7C, 0x08, 0x04, 0x04, 0x08,// r
//     0x48, 0x54, 0x54, 0x54, 0x20,// s
//     0x04, 0x3F, 0x44, 0x40, 0x20,// t
//     0x3C, 0x40, 0x40, 0x20, 0x7C,// u
//     0x1C, 0x20, 0x40, 0x20, 0x1C,// v
//     0x3C, 0x40, 0x30, 0x40, 0x3C,// w
//     0x44, 0x28, 0x10, 0x28, 0x44,// x
//     0x0C, 0x50, 0x50, 0x50, 0x3C,// y
//     0x44, 0x64, 0x54, 0x4C, 0x44,// z
//     0x00, 0x08, 0x36, 0x41, 0x00,// {
//     0x00, 0x00, 0x7F, 0x00, 0x00,// |
//     0x00, 0x41, 0x36, 0x08, 0x00,// }
//     0x08, 0x08, 0x2A, 0x1C, 0x08,// ->
//     0x08, 0x1C, 0x2A, 0x08, 0x08 // <-
// };

#endif