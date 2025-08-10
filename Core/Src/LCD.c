#include "LCD.h"
#include <stm32l4xx.h>

// #define BUFFER_LENGTH ST7789V2_HEIGHT*ST7789V2_WIDTH*2
#define BUFFER_LENGTH ST7789V2_HEIGHT*ST7789V2_WIDTH/2


static uint8_t image_buffer[BUFFER_LENGTH];

static const uint16_t colour_map[16] = {
    LCD_COLOUR_0,
    LCD_COLOUR_1,
    LCD_COLOUR_2,
    LCD_COLOUR_3,
    LCD_COLOUR_4,
    LCD_COLOUR_5,
    LCD_COLOUR_6,
    LCD_COLOUR_7,
    LCD_COLOUR_8,
    LCD_COLOUR_9,
    LCD_COLOUR_10,
    LCD_COLOUR_11,
    LCD_COLOUR_12,
    LCD_COLOUR_13,
    LCD_COLOUR_14,
    LCD_COLOUR_15
};

uint16_t LCD_Map_Pixel(uint8_t pixel) {
    return colour_map[pixel];
}

void LCD_Set_Pixel(const uint16_t x, const uint16_t y, uint8_t colour) {
    uint16_t index = (ST7789V2_WIDTH*y + x) / 2;
    if (x&1) {
        image_buffer[index] = (colour << 4) + (image_buffer[index] & 0x0F);
    }
    else {
        image_buffer[index] = colour + (image_buffer[index] & 0xF0);
    }
}

void LCD_Fill_Buffer(uint8_t colour) {
    for (int i = 0; i < BUFFER_LENGTH; i++) {
        image_buffer[i] = colour + (colour << 4);
    }
}

#define lines_per_buffer 2
// static const int lines_per_buffer = 1;
static uint16_t line_buffer0[lines_per_buffer*240]; // 240 * 2 Bytes * n rows
static uint16_t line_buffer1[lines_per_buffer*240]; // 240 * 2 Bytes * n rows

void LCD_Refresh(ST7789V2_cfg_t* cfg) {
  ST7789V2_Set_Address_Window(cfg, 0, 20, 239, 299); 
  ST7789V2_Send_Command(cfg, 0x2C);
  for (int i = 0; i < (int)((280/2)/lines_per_buffer); i++) {
    // First line buffer
    for (int j = 0; j < 120*lines_per_buffer; j++) {
      uint8_t double_pixel = image_buffer[120 * (2*i*lines_per_buffer) + j];
      line_buffer0[2*j] = colour_map[double_pixel >> 4];
      line_buffer0[2*j+1] = colour_map[double_pixel & 0x0F];
    }
    ST7789V2_Send_Data_Block(cfg, (uint8_t*) line_buffer0, (int)(480*lines_per_buffer));

    // Second line buffer
    for (int j = 0; j < (int)(120*lines_per_buffer); j++) {
      uint8_t double_pixel = image_buffer[120 * (2*i*lines_per_buffer+1) + j];
      line_buffer1[2*j] = colour_map[double_pixel >> 4];
      line_buffer1[2*j+1] = colour_map[double_pixel & 0x0F];
    }
    ST7789V2_Send_Data_Block(cfg, (uint8_t*) line_buffer1, (int)(480*lines_per_buffer));
  }
}

void LCD_Draw_Line(const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, uint8_t colour) {
  // Note that the ranges can be negative so we have to turn the input values into signed integers first
  const int16_t y_range = (int)y1 - (int)y0;
  const int16_t x_range = (int)x1 - (int)x0;;

  // if dotted line, set step to 2, else step is 1
  // const uint16_t step = (type==2) ? 2:1;
  uint16_t step = 1;

  // make sure we loop over the largest range to get the most pixels on the display
  // for instance, if drawing a vertical line (x_range = 0), we need to loop down the y pixels
  // or else we'll only end up with 1 pixel in the x column
  if ( abs(x_range) > abs(y_range) ) {
    // ensure we loop from smallest to largest or else for-loop won't run as expected
    const uint16_t start = x_range > 0 ? x0:x1;
    const uint16_t stop =  x_range > 0 ? x1:x0;

    // loop between x pixels
    for (unsigned int x = start; x<= stop ; x+=step) {  // do linear interpolation
        const int16_t dx = (int)x - (int)x0;
        const uint16_t y = y0 + y_range * dx / x_range;

        // If the line type is '0', this will clear the pixel
        // If it is '1' or '2', the pixel will be set
        LCD_Set_Pixel(x, y, colour);
    }
  } else {
    // ensure we loop from smallest to largest or else for-loop won't run as expected
    const uint16_t start = y_range > 0 ? y0:y1;
    const uint16_t stop =  y_range > 0 ? y1:y0;

    for (unsigned int y = start; y<= stop ; y+=step) {  // do linear interpolation
        const int16_t dy = (int)y - (int)y0;;
        const uint16_t x = x0 + x_range * dy / y_range;

        // If the line type is '0', this will clear the pixel
        // If it is '1' or '2', the pixel will be set
        LCD_Set_Pixel(x, y, colour);
    }
  }
}

void LCD_Draw_Rect(const uint16_t x0, const uint16_t y0, const uint16_t width, const uint16_t height, uint8_t colour, uint8_t fill) {
    if (fill) {
        for (int y = y0; y<y0+height; y++) {
            LCD_Draw_Line(x0, y, x0+(width-1), y, colour);
        }
    }
    else {
        LCD_Draw_Line(x0, y0, x0+(width-1), y0, colour);
        LCD_Draw_Line(x0, y0+(height-1), x0+(width-1), y0+(height-1), colour);
        LCD_Draw_Line(x0, y0, x0, y0+(height-1), colour);
        LCD_Draw_Line(x0+(width-1), y0, x0+(width-1), y0+(height-1), colour);
    }
}

uint16_t colour_ = 0x001F;

void LCD_Fill(ST7789V2_cfg_t* cfg, const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, const uint16_t colour) {
  // Wait for not busy
  while (cfg->spi->SR & SPI_SR_BSY);

  // Set address window
  ST7789V2_Set_Address_Window(cfg, x0, y0, x1, y1);
  colour_ = colour;

  uint32_t len = (x1-x0 + 1) * (y1-y0 + 1);
  ST7789V2_Fill(cfg, &colour_, len);
}

// // void LCD_Set_Pixel(const uint16_t x, const uint16_t y, uint16_t colour) {
// //     image_buffer[2*(ST7789V2_HEIGHT*y + x)] = colour >> 8;
// //     image_buffer[2*(ST7789V2_HEIGHT*y + x) + 1] = colour & 0xFF;
// // }

// // void LCD_Fill_Buffer(ST7789V2_cfg_t* cfg, uint16_t colour) {
// //     uint8_t line_buffer[480];

// //     for (int i = 0; i < 240; i++) {
// //         line_buffer[2*i] = colour >> 8;
// //         line_buffer[2*i + 1] = colour & 0xFF;
// //     }
// //     ST7789V2_Send_Command(cfg, 0x2C);
// //     for (int i = 0; i < 280; i++) {
// //         ST7789V2_Send_Data_Block(cfg, line_buffer, 480);
// //     }
// //     // for (int i = 0; i < ST7789V2_HEIGHT*ST7789V2_WIDTH; i++) {
// //     //     image_buffer[2*i] = colour >> 8;
// //     //     image_buffer[2*i + 1] = colour & 0xFF;
// //     // }
// // }

// // void LCD_Refresh(ST7789V2_cfg_t* cfg) {
// //     ST7789V2_Send_Command(cfg, 0x2C);
// //     ST7789V2_Send_Data_Block(cfg, image_buffer, BUFFER_LENGTH);
// // }
