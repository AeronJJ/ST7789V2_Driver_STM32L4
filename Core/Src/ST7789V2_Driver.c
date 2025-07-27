#include "ST7789V2_Driver.h"

// Needs RST Pin, BL Pin, DC Pin, CS Pin, MOSI Pin, SCLK Pin, 
void ST7789V2_Init(ST7789V2_cfg_t* cfg) {
  cfg->setup_done = 1;

  ST7789V2_Reset(cfg);

  ST7789V2_BL_On(cfg);

  ST7789V2_Send_Command(cfg, ST7789_SLPOUT);
  HAL_Delay(50);

  ST7789V2_Send_Command(cfg, ST7789_COLMOD);
  ST7789V2_Send_Data(cfg, 0x55);
  HAL_Delay(10);

  ST7789V2_Send_Command(cfg, ST7789_MADCTL);
  ST7789V2_Send_Data(cfg, 0x00);

  ST7789V2_Send_Command(cfg, ST7789_INVON);
  HAL_Delay(10);

  ST7789V2_Send_Command(cfg, ST7789_NORON);
  HAL_Delay(10);

  ST7789V2_Set_Address_Window(cfg, 0, 20, 239, 299); 

  ST7789V2_Send_Command(cfg, 0x29);
  HAL_Delay(10);

}

void ST7789V2_Reset(ST7789V2_cfg_t* cfg) {
  if (cfg->setup_done) {
    MX_SPI2_Init();

    // Deassert chip select
    HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_SET);
    
    // Set reset low
    HAL_GPIO_WritePin(cfg->port_RST, cfg->pin_RST, GPIO_PIN_RESET);
    HAL_Delay(50);
    // Set reset high
    HAL_GPIO_WritePin(cfg->port_RST, cfg->pin_RST, GPIO_PIN_SET);

    // // Assert chip select
    // HAL_GPIO_WritePin(port_CS_, pin_CS_, GPIO_PIN_RESET);

    // Software reset
    ST7789V2_Send_Command(cfg, ST7789_SWRESET);
    // Wait 120ms after resetting before sleep out
    HAL_Delay(150);
  }
}

void ST7789V2_Send_Command(ST7789V2_cfg_t* cfg, uint8_t command) {
  if (cfg->setup_done) {
    // Assert CS
    HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_RESET);

    // Set DC 0
    HAL_GPIO_WritePin(cfg->port_DC, cfg->pin_DC, GPIO_PIN_RESET);

    // Send command
    HAL_SPI_Transmit(&cfg->spi, (uint8_t*)&command, 1, HAL_MAX_DELAY);

    // Deassert CS
    HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_SET);
  }
}

void ST7789V2_Send_Data(ST7789V2_cfg_t* cfg, uint8_t data) {
  if (cfg->setup_done) {
    // Assert CS
    HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_RESET);

    // Set DC 1
    HAL_GPIO_WritePin(cfg->port_DC, cfg->pin_DC, GPIO_PIN_SET);

    // Send command
    HAL_SPI_Transmit(&cfg->spi, (uint8_t*)&data, 1, HAL_MAX_DELAY);

    // Deassert CS
    HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_SET);
  }
}

void ST7789V2_Send_Data_Block(ST7789V2_cfg_t* cfg, uint8_t* data, uint32_t length) {
  if (cfg->setup_done) {
    // Assert CS
    HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_RESET);

    // Set DC 1
    HAL_GPIO_WritePin(cfg->port_DC, cfg->pin_DC, GPIO_PIN_SET);

    // Send command
    HAL_SPI_Transmit(&cfg->spi, data, length, HAL_MAX_DELAY);

    // Deassert CS
    HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_SET);
  }
}

void ST7789V2_Set_Address_Window(ST7789V2_cfg_t* cfg, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    ST7789V2_Send_Command(cfg, ST7789_CASET);
    ST7789V2_Send_Data(cfg, x0 >> 8);
    ST7789V2_Send_Data(cfg, x0 & 0xFF);
    ST7789V2_Send_Data(cfg, x1 >> 8);
    ST7789V2_Send_Data(cfg, x1 & 0xFF);

    ST7789V2_Send_Command(cfg, ST7789_RASET);
    ST7789V2_Send_Data(cfg, y0 >> 8);
    ST7789V2_Send_Data(cfg, y0 & 0xFF);
    ST7789V2_Send_Data(cfg, y1 >> 8);
    ST7789V2_Send_Data(cfg, y1 & 0xFF);
}

void ST7789V2_Clear_RAM(ST7789V2_cfg_t* cfg);

void ST7789V2_BL_On(ST7789V2_cfg_t* cfg) {
  HAL_GPIO_WritePin(cfg->port_BL, cfg->pin_BL, GPIO_PIN_SET);
}

void ST7789V2_BL_Off(ST7789V2_cfg_t* cfg) {
  HAL_GPIO_WritePin(cfg->port_BL, cfg->pin_BL, GPIO_PIN_RESET);
}