#include "LCD.h"

// #define BUFFER_LENGTH ST7789V2_HEIGHT*ST7789V2_WIDTH*2
#define BUFFER_LENGTH ST7789V2_HEIGHT*ST7789V2_WIDTH/2


static uint8_t image_buffer[BUFFER_LENGTH];

// void LCD_Set_Pixel(const uint16_t x, const uint16_t y, uint8_t colour) {
//     uint16_t index = ST7789V2_HEIGHT*y + x;
//     if (index&1) {
//         image_buffer[index] = colour << 4 + image_buffer[index] & 0x0F;
//     }
//     else {
//         image_buffer[index] = colour + image_buffer[index] & 0xF0;
//     }
// }

void LCD_Fill_Buffer(uint8_t colour) {
    for (int i = 0; i < BUFFER_LENGTH; i++) {
        image_buffer[i] = colour + (colour << 4);
    }
}

uint16_t LCD_Map_Pixel(uint8_t pixel) {
    switch (pixel) {
        case 0:
            return 0x0000;
            break;
        case 1:
            return 0xF800;
            break;
        case 2:
            return 0x07E0;
            break;
        case 3:
            return 0x001F;
            break;
        default:
            return 0xFFFF;
            break;
    }
}

void LCD_Refresh(ST7789V2_cfg_t* cfg) {
    uint8_t line_buffer[480]; // 240 * 2 Bytes
    ST7789V2_Send_Command(cfg, 0x2C);
    for (int i = 0; i < 280; i++) {
        for (int j = 0; j < 120; j++) {
            uint8_t double_pixel = image_buffer[120 * i + j];
            uint16_t pixel = LCD_Map_Pixel(double_pixel >> 4);
            line_buffer[4*j] = pixel >> 8;
            line_buffer[4*j + 1] = pixel & 0xFF;
            pixel = LCD_Map_Pixel(double_pixel & 0x0F);
            line_buffer[4*j + 2] = pixel >> 8;
            line_buffer[4*j + 3] = pixel & 0xFF;
        }
        ST7789V2_Send_Data_Block(cfg, line_buffer, 480);
    }
}

void LCD_Set_Pixel(const uint16_t x, const uint16_t y, uint16_t colour) {
    image_buffer[2*(ST7789V2_HEIGHT*y + x)] = colour >> 8;
    image_buffer[2*(ST7789V2_HEIGHT*y + x) + 1] = colour & 0xFF;
}

// void LCD_Fill_Buffer(ST7789V2_cfg_t* cfg, uint16_t colour) {
//     uint8_t line_buffer[480];

//     for (int i = 0; i < 240; i++) {
//         line_buffer[2*i] = colour >> 8;
//         line_buffer[2*i + 1] = colour & 0xFF;
//     }
//     ST7789V2_Send_Command(cfg, 0x2C);
//     for (int i = 0; i < 280; i++) {
//         ST7789V2_Send_Data_Block(cfg, line_buffer, 480);
//     }
//     // for (int i = 0; i < ST7789V2_HEIGHT*ST7789V2_WIDTH; i++) {
//     //     image_buffer[2*i] = colour >> 8;
//     //     image_buffer[2*i + 1] = colour & 0xFF;
//     // }
// }

// void LCD_Refresh(ST7789V2_cfg_t* cfg) {
//     ST7789V2_Send_Command(cfg, 0x2C);
//     ST7789V2_Send_Data_Block(cfg, image_buffer, BUFFER_LENGTH);
// }