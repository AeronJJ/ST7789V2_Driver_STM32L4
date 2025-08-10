#include "ST7789V2_Driver.h"
#include <usart.h>
#include <string.h>

void Print_SPI2_DMA_Registers_int(void) {
    char msg[128];
    int len;

    // SPI2 Registers
    len = snprintf(msg, sizeof(msg), "\r\n--- SPI2 Registers ---\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    len = snprintf(msg, sizeof(msg), "CR1  = 0x%08lX\r\n", SPI2->CR1);
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    len = snprintf(msg, sizeof(msg), "CR2  = 0x%08lX\r\n", SPI2->CR2);
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    len = snprintf(msg, sizeof(msg), "SR   = 0x%08lX\r\n", SPI2->SR);
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    // len = snprintf(msg, sizeof(msg), "DR   = 0x%08lX\r\n", SPI2->DR);
    // HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    // DMA Registers for SPI2 TX
    DMA_Channel_TypeDef *dma = DMA1_Channel5;//hdma_spi2_tx.Instance;

    len = snprintf(msg, sizeof(msg), "\r\n--- DMA (SPI2 TX) Registers ---\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    len = snprintf(msg, sizeof(msg), "CCR   = 0x%08lX\r\n", dma->CCR);
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    len = snprintf(msg, sizeof(msg), "CNDTR = 0x%08lX\r\n", dma->CNDTR);  // Number of data to transfer
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    len = snprintf(msg, sizeof(msg), "CPAR  = 0x%08lX\r\n", dma->CPAR);   // Peripheral address
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    len = snprintf(msg, sizeof(msg), "CMAR  = 0x%08lX\r\n", dma->CMAR);   // Memory address
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    // Optional: DMA Interrupt Status (from DMA1 or DMA2)
    len = snprintf(msg, sizeof(msg), "\r\n--- DMA Interrupt Flags (DMA1->ISR) ---\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

    len = snprintf(msg, sizeof(msg), "ISR   = 0x%08lX\r\n", DMA1->ISR);
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);
}

// Needs RST Pin, BL Pin, DC Pin, CS Pin, MOSI Pin, SCLK Pin, 
void ST7789V2_Init(ST7789V2_cfg_t* cfg) {
  // uart_println("Starting Inits");
  gpio_init(cfg);
  // uart_println("GPIO Done");
  spi_init(cfg);
  // uart_println("SPI Done");
  dma_init(cfg);
  // uart_println("DMA Done");

  GPIOB->BSRR = 0x00000002;

  cfg->setup_done = 1;
  // Print_SPI2_DMA_Registers_int();
  ST7789V2_Reset(cfg);
  // Print_SPI2_DMA_Registers_int();

  // uart_println("Reset done");

  ST7789V2_BL_On(cfg);

  // uart_println("bl on");

  ST7789V2_Send_Command(cfg, ST7789_SLPOUT);

  
  HAL_Delay(50);
  // uart_println("SLP out");
  // Print_SPI2_DMA_Registers_int();

  ST7789V2_Send_Command(cfg, ST7789_COLMOD);
  // uart_println("COLMOD command done");
  ST7789V2_Send_Data(cfg, 0x55);
  // uart_println("COLMOD done");
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
    // MX_SPI2_Init();

    //spi_init(cfg);

    // Assert chip select
    GPIOB->BSRR = 0x10000000;
    // HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_SET);
    
    // Set reset low
    GPIOB->BSRR = 0x00040000;
    // HAL_GPIO_WritePin(cfg->port_RST, cfg->pin_RST, GPIO_PIN_RESET);
    HAL_Delay(50);
    // Set reset high
    GPIOB->BSRR = 0x00000004;
    // HAL_GPIO_WritePin(cfg->port_RST, cfg->pin_RST, GPIO_PIN_SET);

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
    GPIOB->BSRR = 0x10000000;
    // HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_RESET);

    // Set DC 0
    GPIOB->BSRR = 0x08000000;
    // HAL_GPIO_WritePin(cfg->port_DC, cfg->pin_DC, GPIO_PIN_RESET);

    // Send command
    // HAL_SPI_Transmit(cfg->spi, (uint8_t*)&command, 1, HAL_MAX_DELAY);
    spi_transmit_byte(cfg, command);

    // Deassert CS
    GPIOB->BSRR = 0x00001000;
    // HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_SET);
  }
}

void ST7789V2_Send_Data(ST7789V2_cfg_t* cfg, uint8_t data) {
  if (cfg->setup_done) {
    // Assert CS
    GPIOB->BSRR = 0x10000000;
    // HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_RESET);

    // Set DC 1
    GPIOB->BSRR = 0x00000800;
    // HAL_GPIO_WritePin(cfg->port_DC, cfg->pin_DC, GPIO_PIN_SET);

    // Send command
    // HAL_SPI_Transmit(cfg->spi, (uint8_t*)&data, 1, HAL_MAX_DELAY);
    spi_transmit_byte(cfg, data);

    // Deassert CS
    GPIOB->BSRR = 0x00001000;
    // HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_SET);
  }
}

// void Print_SPI_State(SPI_HandleTypeDef *hspi, UART_HandleTypeDef *huart) {
//     HAL_SPI_StateTypeDef state = HAL_SPI_GetState(hspi);
//     char *state_str;

//     switch (state) {
//         case HAL_SPI_STATE_RESET:      state_str = "RESET"; break;
//         case HAL_SPI_STATE_READY:      state_str = "READY"; break;
//         case HAL_SPI_STATE_BUSY:       state_str = "BUSY"; break;
//         case HAL_SPI_STATE_BUSY_TX:    state_str = "BUSY_TX"; break;
//         case HAL_SPI_STATE_BUSY_RX:    state_str = "BUSY_RX"; break;
//         case HAL_SPI_STATE_BUSY_TX_RX: state_str = "BUSY_TX_RX"; break;
//         case HAL_SPI_STATE_ERROR:      state_str = "ERROR"; break;
//         default:                       state_str = "UNKNOWN"; break;
//     }

//     char msg[64];
//     snprintf(msg, sizeof(msg), "SPI State: %s\r\n", state_str);
//     HAL_UART_Transmit(huart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
// }

void ST7789V2_Send_Data_Block(ST7789V2_cfg_t* cfg, uint8_t* data, uint32_t length) {
  if (cfg->setup_done) {
    // Print_SPI2_DMA_Registers_int();
    // uart_println("Sending data block");
    // Assert CS
    // HAL_GPIO_WritePin(cfg->port_CS, cfg->pin_CS, GPIO_PIN_RESET);

    // Set DC 1
    GPIOB->BSRR = 0x00000800;
    // HAL_GPIO_WritePin(cfg->port_DC, cfg->pin_DC, GPIO_PIN_SET);

    // Send command
    // HAL_SPI_Transmit(&cfg->spi, data, length, HAL_MAX_DELAY);

    // Wait for any previous DMA transactions to finish
    // while(HAL_SPI_GetState(cfg->spi) != HAL_SPI_STATE_READY) {
    //   ;
    // }
    while(cfg->spi->SR & SPI_SR_BSY) {
      ;
    }


    // uart_println("DMA ISR REG: %x", DMA1->ISR);
    // while(!(DMA1->ISR & DMA_ISR_TCIF5)) {
    //   ;
    // }


    // HAL_SPI_Transmit_DMA(cfg->spi, data, length);
    // uart_println("Sending data block");
    spi_transmit_dma_8bit(cfg, data, length);
    // Print_SPI2_DMA_Registers_int();
    // uart_println("done");
    
    // char msg[] = "Waiting for DMA to finish...\r\n";
    // char msg1[] = "DMA finished.\r\n";
    //while(HAL_DMA_GetState(cfg->spi.hdmatx) != HAL_DMA_STATE_READY) {
    // HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    // while(HAL_SPI_GetState(cfg->spi) != HAL_SPI_STATE_READY) {
    //   //HAL_UART_Transmit(&huart2, HAL_DMA_GetState(cfg->spi.hdmatx), sizeof(HAL_DMA_STATE_READY), HAL_MAX_DELAY);
    //   //Print_SPI_State(cfg->spi, &huart2);
    // }
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




// #include "handwritten.h"

void gpio_init(ST7789V2_cfg_t* cfg) {
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIOB->MODER = 0xB97FFFD7;
    GPIOB->OTYPER = 0x00000000;
    GPIOB->OSPEEDR = 0xCF000000;
    GPIOB->PUPDR = 0x00000010;
    GPIOB->AFR[1] = 0x50500000;
}

void spi_init(ST7789V2_cfg_t* cfg) {
    __HAL_RCC_SPI2_CLK_ENABLE();

    // RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;     // GPIOB for SPI2 pins
    // RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;    // Enable SPI2
    // RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;      // Enable DMA1

    // SPI_TypeDef* spi_inst = cfg->spi->Instance;

    // spi_inst->CR1 &= ~SPI_CR1_SPE;
    // spi_inst->CR2 = SPI_CR2_TXDMAEN | (0b0111 << SPI_CR2_DS_Pos);
    // spi_inst->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_SSM | SPI_CR1_SPE;
    // cfg->spi->Instance = SPI2;
    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPI2->CR1 = 0xC304;
    SPI2->CR2 = 0x1708;

    SPI2->CR1 |= SPI_CR1_SPE;
}

void dma_init(ST7789V2_cfg_t* cfg) {
    __HAL_RCC_DMA1_CLK_ENABLE();
    DMA1_Channel5->CCR = 0x00003090;
    DMA1_CSELR->CSELR = 0x00010000;
}

void spi_set_8bit_mode(ST7789V2_cfg_t* cfg) {
    SPI_TypeDef* spi_inst = cfg->spi;

    spi_inst->CR1 &= ~SPI_CR1_SPE;

    SPI2->CR2 &= ~SPI_CR2_DS_Msk;
    SPI2->CR2 |= (0b0111 << SPI_CR2_DS_Pos); // 8-bit data 

    DMA1_Channel5->CCR &= ~(DMA_CCR_EN | DMA_CCR_PSIZE);
    DMA1_Channel5->CCR |= 0x00 << DMA_CCR_PSIZE_Pos;

    spi_inst->CR1 |= SPI_CR1_SPE;
}

void spi_set_16bit_mode(ST7789V2_cfg_t* cfg) {
    SPI_TypeDef* spi_inst = cfg->spi;

    spi_inst->CR1 &= ~SPI_CR1_SPE;

    SPI2->CR2 &= ~SPI_CR2_DS_Msk;
    SPI2->CR2 |= (0b1111 << SPI_CR2_DS_Pos); // 16-bit data 

    // DMA1_Channel5->CCR &= ~(DMA_CCR_EN | DMA_CCR_PSIZE);
    // DMA1_Channel5->CCR |= 0x00 << DMA_CCR_PSIZE_Pos;

    spi_inst->CR1 |= SPI_CR1_SPE;
}


void spi_transmit_byte(ST7789V2_cfg_t* cfg, uint8_t data) {
    SPI_TypeDef* spi_inst = cfg->spi;

    // uart_println("Transmitting Byte");
    // Print_SPI2_DMA_Registers_int();

    while (!(spi_inst->SR & SPI_SR_TXE));      // Wait for TXE (transmit buffer empty)
    // uart_println("TXE set");
    *((__IO uint8_t*)&spi_inst->DR) = data;    // Write data
    while (!(spi_inst->SR & SPI_SR_TXE));      // Wait again for transmission
    while (spi_inst->SR & SPI_SR_BSY);         // Wait for not busy
}

void spi_transmit_dma_8bit(ST7789V2_cfg_t* cfg, uint8_t* data, uint16_t len) {
    // spi_set_8bit_mode(cfg);
    // __HAL_RCC_DMA1_CLK_ENABLE();
    // __HAL_RCC_DMA2_CLK_ENABLE();

    // Deassert CS
    GPIOB->BSRR = 0x00001000;

    uint32_t isr = DMA1->ISR;
    DMA1->IFCR = isr;

    SPI_TypeDef* spi_inst = cfg->spi;
    spi_inst->CR1 &= ~SPI_CR1_SPE;
    spi_inst->CR2 |= SPI_CR2_TXDMAEN;

    // DMA1_Channel5->CCR &= ~DMA_CCR_EN; // Disable
    // while (DMA1_Channel5->CCR & DMA_CCR_EN); // Wait until disabled

    DMA1_Channel5->CCR = 0; // Disable & reset
    while (DMA1_Channel5->CCR & DMA_CCR_EN); // Wait until disabled

    // uart_println("addr: %x, %x", (uint32_t)&SPI2->DR, (uint32_t)data);

    DMA1_Channel5->CPAR = (uint32_t)&SPI2->DR;        // Peripheral = SPI2 data register
    DMA1_Channel5->CMAR = (uint32_t)data;           // Memory = pixel buffer
    DMA1_Channel5->CNDTR = len;                // Number of 16-bit words

    // Configure CCR:
    // DIR = 1 (mem->periph), MINC = 1, PINC = 0
    // MSIZE = 16-bit, PSIZE = 16-bit
    // DMA1_Channel5->CCR =
    //     (1 << 4)  | // DIR: memory to peripheral
    //     (1 << 7)  | // MINC: increment memory
    //     (0 << 8)  | // PSIZE: 8-bit
    //     (0 << 10) | // MSIZE: 8-bit
    //     (0 << 1);   // Enable transfer complete interrupt (optional)
    DMA1_Channel5->CCR = 0x00003090;

    // Print_SPI2_DMA_Registers_int();

    spi_inst->CR1 |= SPI_CR1_SPE;

    // Assert CS
    GPIOB->BSRR = 0x10000000;

    // Enable DMA channel (starts transfer)
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}

void spi_transmit_dma_16bit(ST7789V2_cfg_t* cfg, uint16_t* data, uint16_t len) {
    spi_set_16bit_mode(cfg);

    DMA1_Channel5->CCR = 0; // Disable & reset

    DMA1_Channel5->CPAR = (uint32_t)&SPI2->DR;        // Peripheral = SPI2 data register
    DMA1_Channel5->CMAR = (uint32_t)data;           // Memory = pixel buffer
    DMA1_Channel5->CNDTR = len;                // Number of 16-bit words

    // Configure CCR:
    // DIR = 1 (mem->periph), MINC = 1, PINC = 0
    // MSIZE = 16-bit, PSIZE = 16-bit
    DMA1_Channel5->CCR =
        (1 << 4)  | // DIR: memory to peripheral
        (1 << 7)  | // MINC: increment memory
        (1 << 8)  | // PSIZE: 16-bit
        (1 << 10) | // MSIZE: 16-bit
        (0 << 1);   // Enable transfer complete interrupt (optional)

    // Enable DMA channel (starts transfer)
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}

void spi_transmit_dma_16_bit_noinc(ST7789V2_cfg_t* cfg, uint16_t* data, uint16_t len) {
    spi_set_16bit_mode(cfg);

    DMA1_Channel5->CCR = 0; // Disable & reset

    DMA1_Channel5->CPAR = (uint32_t)&SPI2->DR;        // Peripheral = SPI2 data register
    DMA1_Channel5->CMAR = (uint32_t)data;           // Memory = pixel buffer
    DMA1_Channel5->CNDTR = len;                // Number of 16-bit words

    // Configure CCR:
    // DIR = 1 (mem->periph), MINC = 0, PINC = 0
    // MSIZE = 16-bit, PSIZE = 16-bit
    DMA1_Channel5->CCR =
        (1 << 4)  | // DIR: memory to peripheral
        (0 << 7)  | // MINC: increment memory
        (1 << 8)  | // PSIZE: 16-bit
        (1 << 10) | // MSIZE: 16-bit
        (0 << 1);   // Enable transfer complete interrupt (optional)

    // Enable DMA channel (starts transfer)
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}
void uart_println(const char *fmt, ...) {
    char line[128]; // Adjust size as needed
    va_list args;

    va_start(args, fmt);
    vsnprintf(line, sizeof(line) - 3, fmt, args); // Leave room for \r\n\0
    va_end(args);

    strcat(line, "\r\n"); // Add newline (CRLF)

    HAL_UART_Transmit(&huart2, (uint8_t *)line, strlen(line), HAL_MAX_DELAY);
}