#include "ST7789V2_Driver.h"

SPI_HandleTypeDef spi_;

GPIO_TypeDef *port_RST_, *port_BL_, *port_DC_, *port_CS_, *port_MOSI_, *port_SCLK_;
uint16_t pin_RST_, pin_BL_, pin_DC_, pin_CS_, pin_MOSI_, pin_SCLK_;

uint8_t setup_done_ = 0;


// Needs RST Pin, BL Pin, DC Pin, CS Pin, MOSI Pin, SCLK Pin, 
void ST7789V2_Init(SPI_HandleTypeDef spi, GPIO_TypeDef* port_RST, uint16_t pin_RST, GPIO_TypeDef* port_BL, uint16_t pin_BL, GPIO_TypeDef* port_DC, uint16_t pin_DC, GPIO_TypeDef* port_CS, uint16_t pin_CS, GPIO_TypeDef* port_MOSI, uint16_t pin_MOSI, GPIO_TypeDef* port_SCLK, uint16_t pin_SCLK) {
  spi_ = spi;

  port_RST_  = port_RST;
  port_BL_   = port_BL;
  port_DC_   = port_DC;
  port_CS_   = port_CS;
  port_MOSI_ = port_MOSI;
  port_SCLK_ = port_SCLK;

  pin_RST_   = pin_RST;
  pin_BL_    = pin_BL;
  pin_DC_    = pin_DC;
  pin_CS_    = pin_CS;
  pin_MOSI_  = pin_MOSI;
  pin_SCLK_  = pin_SCLK;

  setup_done_ = 1;


}

void ST7789V2_Reset() {
  if (setup_done_) {
    MX_SPI2_Init();

    // Deassert chip select
    HAL_GPIO_WritePin(port_CS_, pin_CS_, GPIO_PIN_SET);
    
    // Set reset low
    HAL_GPIO_WritePin(port_RST_, pin_RST_, GPIO_PIN_RESET);
    HAL_Delay(50);
    // Set reset high
    HAL_GPIO_WritePin(port_RST_, pin_RST_, GPIO_PIN_SET);

    // // Assert chip select
    // HAL_GPIO_WritePin(port_CS_, pin_CS_, GPIO_PIN_RESET);

    // Software reset
    ST7789V2_Send_Command(ST7789_SWRESET);
    // Wait 120ms after resetting before sleep out
    HAL_Delay(150);

    ST7789V2_BL_On();

    ST7789V2_Send_Command(ST7789_SLPOUT);
    HAL_Delay(50);

    ST7789V2_Send_Command(ST7789_COLMOD);
    ST7789V2_Send_Data(0x55);
    HAL_Delay(10);

    ST7789V2_Send_Command(ST7789_MADCTL);
    ST7789V2_Send_Data(0x00);

    ST7789V2_Send_Command(ST7789_INVON);
    HAL_Delay(10);

    ST7789V2_Send_Command(ST7789_NORON);
    HAL_Delay(10);

    ST7789V2_Set_Address_Window(0, 20, 239, 299); 

    ST7789V2_Send_Command(0x29);
    HAL_Delay(10);
  }
}

void ST7789V2_Send_Command(uint8_t command) {
  if (setup_done_) {
    // Assert CS
    HAL_GPIO_WritePin(port_CS_, pin_CS_, GPIO_PIN_RESET);

    // Set DC 0
    HAL_GPIO_WritePin(port_DC_, pin_DC_, GPIO_PIN_RESET);

    // Send command
    HAL_SPI_Transmit(&spi_, (uint8_t*)&command, 1, HAL_MAX_DELAY);

    // Deassert CS
    HAL_GPIO_WritePin(port_CS_, pin_CS_, GPIO_PIN_SET);
  }
}

void ST7789V2_Send_Data(uint8_t data) {
  if (setup_done_) {
    // Assert CS
    HAL_GPIO_WritePin(port_CS_, pin_CS_, GPIO_PIN_RESET);

    // Set DC 1
    HAL_GPIO_WritePin(port_DC_, pin_DC_, GPIO_PIN_SET);

    // Send command
    HAL_SPI_Transmit(&spi_, (uint8_t*)&data, 1, HAL_MAX_DELAY);

    // Deassert CS
    HAL_GPIO_WritePin(port_CS_, pin_CS_, GPIO_PIN_SET);
  }
}

void ST7789V2_Send_Data_Block(uint8_t* data, uint32_t length) {
  if (setup_done_) {
    // Assert CS
    HAL_GPIO_WritePin(port_CS_, pin_CS_, GPIO_PIN_RESET);

    // Set DC 0
    HAL_GPIO_WritePin(port_DC_, pin_DC_, GPIO_PIN_RESET);

    // Send command
    HAL_SPI_Transmit(&spi_, (uint8_t*)&data, length, HAL_MAX_DELAY);

    // Deassert CS
    HAL_GPIO_WritePin(port_CS_, pin_CS_, GPIO_PIN_SET);
  }
}

void ST7789V2_Set_Address_Window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    ST7789V2_Send_Command(ST7789_CASET);
    ST7789V2_Send_Data(x0 >> 8);
    ST7789V2_Send_Data(x0 & 0xFF);
    ST7789V2_Send_Data(x1 >> 8);
    ST7789V2_Send_Data(x1 & 0xFF);

    ST7789V2_Send_Command(ST7789_RASET); // RASET
    ST7789V2_Send_Data(y0 >> 8);
    ST7789V2_Send_Data(y0 & 0xFF);
    ST7789V2_Send_Data(y1 >> 8);
    ST7789V2_Send_Data(y1 & 0xFF);
}

void ST7789V2_Clear_RAM();

void ST7789V2_BL_On() {
  HAL_GPIO_WritePin(port_BL_, pin_BL_, GPIO_PIN_SET);
}

void ST7789V2_BL_Off() {
  HAL_GPIO_WritePin(port_BL_, pin_BL_, GPIO_PIN_RESET);
}