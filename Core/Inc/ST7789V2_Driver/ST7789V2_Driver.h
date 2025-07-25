#include "spi.h"
#include "gpio.h"
#include "stm32l4xx_hal.h"
#include <stm32l476xx.h>
#include <stdint.h>

/* Control Registers and constant codes */
#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36
/**
 * Memory Data Access Control Register (0x36H)
 * MAP:     D7  D6  D5  D4  D3  D2  D1  D0
 * param:   MY  MX  MV  ML  RGB MH  -   -
 *
 */

/* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789_MADCTL_MY  0x80
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789_MADCTL_MX  0x40
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789_MADCTL_MV  0x20
/* Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789_MADCTL_ML  0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define ST7789_MADCTL_RGB 0x00

#define ST7789_RDID1   0xDA
#define ST7789_RDID2   0xDB
#define ST7789_RDID3   0xDC
#define ST7789_RDID4   0xDD

/* Advanced options */
#define ST7789_COLOR_MODE_16bit 0x55    //  RGB565 (16bit)
#define ST7789_COLOR_MODE_18bit 0x66    //  RGB666 (18bit)

#define ST7789_ROTATION 2	

void ST7789V2_Init(GPIO_TypeDef* port_RST, uint16_t pin_RST, GPIO_TypeDef* port_BL, uint16_t pin_BL, GPIO_TypeDef* port_DC, uint16_t pin_DC, GPIO_TypeDef* port_CS, uint16_t pin_CS, GPIO_TypeDef* port_MOSI, uint16_t pin_MOSI, GPIO_TypeDef* port_SCLK, uint16_t pin_SCLK);

void ST7789V2_Reset();

void ST7789V2_Send_Command(uint8_t command);

void ST7789V2_Send_Data(uint8_t data);

void ST7789V2_Send_Data_Block(uint8_t* data, uint32_t length);

void ST7789V2_Set_Address_Window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

void ST7789V2_Clear_RAM();

void ST7789V2_BL_On();

void ST7789V2_BL_Off();