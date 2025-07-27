#ifndef LCD_h
#define LCD_h

#include "ST7789V2_Driver.h"

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
            uint16_t colour);

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
*   @param  type - 0 white,1 black,2 dotted*/
void LCD_drawLine(unsigned int const x0, unsigned int const y0, unsigned int const x1, unsigned int const y1, unsigned int const type);

/* Draw Rectangle
*   This function draws a rectangle.
*   @param  x0 - x-coordinate of origin (top-left)
*   @param  y0 - y-coordinate of origin (top-left)
*   @param  width - width of rectangle
*   @param  height - height of rectangle
*   @param  fill   - fill-type for the shape*/
// void LCD_drawRect(unsigned int const x0, unsigned int const y0, unsigned int const width, unsigned int const height, FillType const fill);

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

#endif