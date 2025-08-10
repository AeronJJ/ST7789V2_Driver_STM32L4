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
*   Powers up the display and turns on backlight (50% brightness default).
*   Sets the display up in horizontal addressing mode and with normal video mode.*/
void LCD_init();

/* Turn off
*   Powers down the display and turns of the backlight.
*   Needs to be reinitialised before being re-used.*/
void LCD_turnOff();

/* Clear
*   Clears the screen buffer.*/
void LCD_clear();

/* Set screen constrast
@param constrast - float in range 0.0 to 1.0 (0.40 to 0.60 is usually a good value)*/
void LCD_setContrast(float contrast);

// Turn on normal video mode (default) Black on white
void LCD_normalMode();

// Turn on inverse video mode (default) White on black
void LCD_inverseMode();

/* Set Brightness
*   Sets brightness of LED backlight.
*   @param brightness - float in range 0.0 to 1.0*/
void LCD_setBrightness(float const brightness);

/* Print String
*   Prints a string of characters to the screen buffer. String is cut-off after the 83rd pixel.
*   @param x - the column number (0 to 83)
*   @param y - the row number (0 to 5) - the display is split into 6 banks - each bank can be considered a row*/
void LCD_printString(char const *str, unsigned int const  x, unsigned int const  y);

/* Print Character
*   Sends a character to the screen buffer.  Printed at the specified location. Character is cut-off after the 83rd pixel.
*   @param  c - the character to print. Can print ASCII as so printChar('C').
*   @param x - the column number (0 to 83)
*   @param y - the row number (0 to 5) - the display is split into 6 banks - each bank can be considered a row*/
void LCD_printChar(char const c, unsigned int const x, unsigned int const y);

/* Set a Pixel
* @param x     The x co-ordinate of the pixel (0 to 83)
* @param y     The y co-ordinate of the pixel (0 to 47)
* @param state The state of the pixel [true=black (default), false=white]
* @details This function sets the state of a pixel in the screen buffer.
*          The third parameter can be omitted*/
void LCD_Set_Pixel(const uint16_t x,
            const uint16_t  y,
            uint8_t colour);

/* Clear a Pixel
*   @param  x - the x co-ordinate of the pixel (0 to 83)
*   @param  y - the y co-ordinate of the pixel (0 to 47)
*   @details This function clears pixel in the screen buffer
*   @deprecated Use setPixel(x, y, false) instead*/
void LCD_clearPixel(unsigned int const x,
            unsigned int const y)
__attribute__((deprecated("Use setPixel(x,y,false) instead")));

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

#endif