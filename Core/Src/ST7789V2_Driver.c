#include "ST7789V2_Driver.h"
#include <usart.h>
#include <string.h>

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
    HAL_SPI_Transmit(cfg->spi, (uint8_t*)&command, 1, HAL_MAX_DELAY);

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
    HAL_SPI_Transmit(cfg->spi, (uint8_t*)&data, 1, HAL_MAX_DELAY);

    // Deassert CS
    HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_SET);
  }
}

void Print_SPI_State(SPI_HandleTypeDef *hspi, UART_HandleTypeDef *huart) {
    HAL_SPI_StateTypeDef state = HAL_SPI_GetState(hspi);
    char *state_str;

    switch (state) {
        case HAL_SPI_STATE_RESET:      state_str = "RESET"; break;
        case HAL_SPI_STATE_READY:      state_str = "READY"; break;
        case HAL_SPI_STATE_BUSY:       state_str = "BUSY"; break;
        case HAL_SPI_STATE_BUSY_TX:    state_str = "BUSY_TX"; break;
        case HAL_SPI_STATE_BUSY_RX:    state_str = "BUSY_RX"; break;
        case HAL_SPI_STATE_BUSY_TX_RX: state_str = "BUSY_TX_RX"; break;
        case HAL_SPI_STATE_ERROR:      state_str = "ERROR"; break;
        default:                       state_str = "UNKNOWN"; break;
    }

    char msg[64];
    snprintf(msg, sizeof(msg), "SPI State: %s\r\n", state_str);
    HAL_UART_Transmit(huart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void ST7789V2_Send_Data_Block(ST7789V2_cfg_t* cfg, uint8_t* data, uint32_t length) {
  if (cfg->setup_done) {
    // Assert CS
    HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_RESET);

    // Set DC 1
    HAL_GPIO_WritePin(cfg->port_DC, cfg->pin_DC, GPIO_PIN_SET);

    // Send command
    // HAL_SPI_Transmit(&cfg->spi, data, length, HAL_MAX_DELAY);
    HAL_SPI_Transmit_DMA(cfg->spi, data, length);
    char msg[] = "Waiting for DMA to finish...\r\n";
    char msg1[] = "DMA finished.\r\n";
    //while(HAL_DMA_GetState(cfg->spi.hdmatx) != HAL_DMA_STATE_READY) {
    // HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    while(HAL_SPI_GetState(cfg->spi) != HAL_SPI_STATE_READY) {
      //HAL_UART_Transmit(&huart2, HAL_DMA_GetState(cfg->spi.hdmatx), sizeof(HAL_DMA_STATE_READY), HAL_MAX_DELAY);
      //Print_SPI_State(cfg->spi, &huart2);
    }
    // HAL_UART_Transmit(&huart2, (uint8_t *)msg1, strlen(msg), HAL_MAX_DELAY);
    // Deassert CS
    //HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_SET);
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