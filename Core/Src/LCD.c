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


static const int lines_per_buffer = 1;
static uint16_t line_buffer0[1*240]; // 240 * 2 Bytes * n rows
static uint16_t line_buffer1[1*240]; // 240 * 2 Bytes * n rows

void LCD_Refresh(ST7789V2_cfg_t* cfg) {
    ST7789V2_Send_Command(cfg, 0x2C);
    for (int i = 0; i < (int)((280/2)/lines_per_buffer); i++) {
        for (int j = 0; j < 120*lines_per_buffer; j++) {
            uint8_t double_pixel = image_buffer[120 * (2*i*lines_per_buffer) + j];
            line_buffer0[2*j] = colour_map[double_pixel >> 4];
            line_buffer0[2*j+1] = colour_map[double_pixel & 0x0F];
        }
        // uart_println("Waiting...");
        // while(HAL_SPI_GetState(cfg->spi) != HAL_SPI_STATE_READY) {
        //     ;
        // }
        // uart_println("Start\n");
        ST7789V2_Send_Data_Block(cfg, (uint8_t*) line_buffer0, (int)(480*lines_per_buffer));
        // uart_println("Done\n");
        for (int j = 0; j < (int)(120*lines_per_buffer); j++) {
            uint8_t double_pixel = image_buffer[120 * (2*i*lines_per_buffer+1) + j];
            // uint16_t pixel = LCD_Map_Pixel(double_pixel >> 4);
            // line_buffer1[4*j] = pixel >> 8;
            // line_buffer1[4*j + 1] = pixel & 0xFF;
            // line_buffer1[2*j] = pixel;
            // pixel = LCD_Map_Pixel(double_pixel & 0x0F);
            // line_buffer1[4*j + 2] = pixel >> 8;
            // line_buffer1[4*j + 3] = pixel & 0xFF;
            // line_buffer1[2*j+1] = pixel;

            // line_buffer1[2*j] = LCD_Map_Pixel(double_pixel >> 4);
            // line_buffer1[2*j+1] = LCD_Map_Pixel(double_pixel & 0x0F);


            line_buffer1[2*j] = colour_map[double_pixel >> 4];
            line_buffer1[2*j+1] = colour_map[double_pixel & 0x0F];
        }
        // uart_println("Waiting...");
        // while(HAL_SPI_GetState(cfg->spi) != HAL_SPI_STATE_READY) {
        //     ;
        // }
        // uart_println("First Line buffer valu: %d", line_buffer0[0]);
        // uart_println("Second Line buffer valu: %d", line_buffer1[0]);
        ST7789V2_Send_Data_Block(cfg, (uint8_t*) line_buffer1, (int)(480*lines_per_buffer));
        // uart_println("Done\n");
    }

    // while(HAL_SPI_GetState(cfg->spi) != HAL_SPI_STATE_READY) {
    //     ;
    // }
    // ST7789V2_Send_Command(cfg, 0x2C);
    // for (int i = 0; i < BUFFER_LENGTH; i++) {
    //     uint8_t double_pixel = image_buffer[i];
    //     uint16_t pixel = LCD_Map_Pixel(double_pixel >> 4);
    //     while(HAL_SPI_GetState(cfg->spi) != HAL_SPI_STATE_READY) ;
    //     ST7789V2_Send_Data_Block(cfg, &pixel, 2);
    //     // while(HAL_SPI_GetState(cfg->spi) != HAL_SPI_STATE_READY) ;
    //     // ST7789V2_Send_Data_Block(cfg, pixel & 0xFF, 1);
    //     pixel = LCD_Map_Pixel(double_pixel & 0x0F);
    //     while(HAL_SPI_GetState(cfg->spi) != HAL_SPI_STATE_READY) ;
    //     ST7789V2_Send_Data_Block(cfg, &pixel, 2);
    // }
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

// uint16_t colour_ = 0x001F;

// void LCD_Fill(ST7789V2_cfg_t* cfg, const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, const uint16_t colour) {
//     // Set address window
//     ST7789V2_Set_Address_Window(cfg, x0, y0, x1, y1);

//     //colour_ = colour;
    
//     ST7789V2_Send_Command(cfg, 0x2C);

//     // Change to 16 bits
//     // HAL_SPI_DeInit(cfg->spi);
//     // cfg->spi->Init.DataSize = SPI_DATASIZE_16BIT;
//     // HAL_SPI_Init(cfg->spi);

//     // // Don't increment DMA:
//     // cfg->spi->hdmatx->Init.MemInc = DMA_MINC_DISABLE;
//     // if (HAL_DMA_Init(cfg->spi->hdmatx) != HAL_OK) {
//     //     char err_msg[] = "DMA init failed!\r\n";
//     //     HAL_UART_Transmit(&huart2, (uint8_t*)err_msg, sizeof(err_msg) - 1, HAL_MAX_DELAY);
//     // }

//     __HAL_SPI_DISABLE(cfg->spi);              // disable SPI
//     __HAL_DMA_DISABLE(cfg->spi->hdmatx);        // disable DMA TX

//     CLEAR_BIT(cfg->spi->Instance->CR2, SPI_CR2_DS);
//     SET_BIT(cfg->spi->Instance->CR2, SPI_DATASIZE_16BIT);  // SPI_DATASIZE_16BIT is 0x0F << 8
//     CLEAR_BIT(cfg->spi->Instance->CR2, SPI_CR2_FRXTH);

//     // For TX DMA (assuming DMA1_Channel4 is used for SPI2_TX)
//     // MODIFY_REG(cfg->spi->hdmatx->Instance->CCR,
//     //        DMA_CCR_MSIZE | DMA_CCR_PSIZE,
//     //        DMA_PDATAALIGN_HALFWORD | DMA_MDATAALIGN_HALFWORD);  // 16-bit alignment

//     cfg->spi->hdmatx->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//     cfg->spi->hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//     cfg->spi->hdmatx->Init.MemInc = DMA_MINC_DISABLE;
//     HAL_DMA_Init(cfg->spi->hdmatx);  // re-init the DMA

//     __HAL_DMA_ENABLE(cfg->spi->hdmatx);  // if manually disabling
//     __HAL_SPI_ENABLE(cfg->spi);
    
//     // Write DMA
//     ST7789V2_Send_Data_Block(cfg, (uint8_t*)&colour_, (x1-x0)*(y1-y0)/2);

//     // // Reset DMA increment value
//     // cfg->spi->hdmatx->Init.MemInc = DMA_MINC_ENABLE;
//     // if (HAL_DMA_Init(cfg->spi->hdmatx) != HAL_OK) {
//     //     char err_msg[] = "DMA init failed!\r\n";
//     //     HAL_UART_Transmit(&huart2, (uint8_t*)err_msg, sizeof(err_msg) - 1, HAL_MAX_DELAY);
//     // }

//     // // HAL_SPI_DeInit(cfg->spi);
//     // cfg->spi->Init.DataSize = SPI_DATASIZE_8BIT;
//     // HAL_SPI_Init(cfg->spi);

//     __HAL_SPI_DISABLE(cfg->spi);              // disable SPI
//     __HAL_DMA_DISABLE(cfg->spi->hdmatx);        // disable DMA TX

//     CLEAR_BIT(cfg->spi->Instance->CR2, SPI_CR2_DS);
//     SET_BIT(cfg->spi->Instance->CR2, SPI_DATASIZE_8BIT);  // SPI_DATASIZE_16BIT is 0x0F << 8
//     SET_BIT(cfg->spi->Instance->CR2, SPI_CR2_FRXTH);

//     // MODIFY_REG(hspi2.hdmatx->Instance->CCR,
//     //        DMA_CCR_MSIZE | DMA_CCR_PSIZE,
//     //        DMA_PDATAALIGN_BYTE | DMA_MDATAALIGN_BYTE);

//     cfg->spi->hdmatx->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//     cfg->spi->hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//     cfg->spi->hdmatx->Init.MemInc = DMA_MINC_ENABLE;
//     HAL_DMA_Init(cfg->spi->hdmatx);  // re-init the DMA

//     __HAL_DMA_ENABLE(cfg->spi->hdmatx);  // if manually disabling
//     __HAL_SPI_ENABLE(cfg->spi);

//     LCD_Refresh(cfg);

//     ST7789V2_Set_Address_Window(cfg, 0, 20, 239, 299); 

//     HAL_Delay(500);

//     LCD_Refresh(cfg);
// }

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

// // extern SPI_HandleTypeDef hspi2;
// extern DMA_HandleTypeDef hdma_spi2_tx;

// uint16_t line_buffer[240];

// static void ST7789_SelectCommandMode(ST7789V2_cfg_t* cfg) {
//     HAL_GPIO_WritePin(cfg->port_DC, cfg->pin_DC, GPIO_PIN_RESET);
// }

// static void ST7789_SelectDataMode(ST7789V2_cfg_t* cfg) {
//     HAL_GPIO_WritePin(cfg->port_DC, cfg->pin_DC, GPIO_PIN_SET);
// }

// static void ST7789_Reset(ST7789V2_cfg_t* cfg) {
//     HAL_GPIO_WritePin(cfg->port_RST, cfg->pin_RST, GPIO_PIN_RESET);
//     HAL_Delay(50);
//     HAL_GPIO_WritePin(cfg->port_RST, cfg->pin_RST, GPIO_PIN_SET);
//     HAL_Delay(50);
// }

// void ST7789_SendCommand(ST7789V2_cfg_t* cfg, uint8_t cmd) {
//     ST7789_SelectCommandMode(cfg);
//     HAL_SPI_Transmit(cfg->spi, &cmd, 1, HAL_MAX_DELAY);
// }

// void ST7789_SendData(ST7789V2_cfg_t* cfg, uint8_t *data, size_t size) {
//     ST7789_SelectDataMode(cfg);
//     HAL_SPI_Transmit(cfg->spi, data, size, HAL_MAX_DELAY);
// }

// void ST7789_SetAddressWindow(ST7789V2_cfg_t* cfg, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
//     uint8_t data[4];

//     ST7789_SendCommand(cfg, 0x2A); // Column addr set
//     data[0] = x0 >> 8; data[1] = x0 & 0xFF;
//     data[2] = x1 >> 8; data[3] = x1 & 0xFF;
//     ST7789_SendData(cfg, data, 4);

//     ST7789_SendCommand(cfg, 0x2B); // Row addr set
//     data[0] = y0 >> 8; data[1] = y0 & 0xFF;
//     data[2] = y1 >> 8; data[3] = y1 & 0xFF;
//     ST7789_SendData(cfg, data, 4);

//     ST7789_SendCommand(cfg, 0x2C); // Write to RAM
// }

// void ST7789_Init(ST7789V2_cfg_t* cfg) {
//     ST7789_Reset(cfg);

//     ST7789_SendCommand(cfg, 0x01); // Software reset
//     HAL_Delay(150);

//     ST7789_SendCommand(cfg, 0x36); // MADCTL
//     uint8_t madctl = 0x00;
//     ST7789_SendData(cfg, &madctl, 1);

//     ST7789_SendCommand(cfg, 0x3A); // COLMOD
//     uint8_t colmod = 0x55;    // 16-bit/pixel
//     ST7789_SendData(cfg, &colmod, 1);

//     ST7789_SendCommand(cfg, 0x21); // Inversion ON
//     ST7789_SendCommand(cfg, 0x11); // Sleep OUT
//     HAL_Delay(120);
//     ST7789_SendCommand(cfg, 0x29); // Display ON
// }

// void ST7789_SendLine_DMA(ST7789V2_cfg_t* cfg, uint16_t x, uint16_t y, uint16_t *data, uint16_t len) {
//     // Set address window
//     ST7789_SetAddressWindow(cfg, x, y, x + len - 1, y);

//     // Set data mode
//     ST7789_SelectDataMode(cfg);

//     // Switch to 16-bit SPI mode
//     MODIFY_REG(cfg->spi->CR1, SPI_CR1_SPE, 0); // Disable SPI
//     CLEAR_BIT(cfg->spi->CR1, SPI_CR1_SPE);
//     MODIFY_REG(cfg->spi->CR2, SPI_CR2_DS, SPI_DATASIZE_16BIT);
//     SET_BIT(cfg->spi->CR1, SPI_CR1_SPE); // Re-enable SPI

//     // Start DMA transfer
//     HAL_SPI_Transmit_DMA(cfg->spi, (uint8_t *)data, len * 2);
// }

// void Print_SPI_CR2(SPI_HandleTypeDef *hspi) {
//     char msg[64];
//     uint32_t cr2_val = hspi->Instance->CR2;

//     // Format CR2 value as hexadecimal
//     int len = snprintf(msg, sizeof(msg), "SPI2->CR2 = 0x%08lX\r\n", cr2_val);

//     // Transmit over UART
//     HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);
// }

// void Print_SPI2_DMA_Registers(void) {
//     char msg[128];
//     int len;

//     // SPI2 Registers
//     len = snprintf(msg, sizeof(msg), "\r\n--- SPI2 Registers ---\r\n");
//     HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     len = snprintf(msg, sizeof(msg), "CR1  = 0x%08lX\r\n", SPI2->CR1);
//     HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     len = snprintf(msg, sizeof(msg), "CR2  = 0x%08lX\r\n", SPI2->CR2);
//     HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     len = snprintf(msg, sizeof(msg), "SR   = 0x%08lX\r\n", SPI2->SR);
//     HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     // len = snprintf(msg, sizeof(msg), "DR   = 0x%08lX\r\n", SPI2->DR);
//     // HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     // DMA Registers for SPI2 TX
//     // DMA_Channel_TypeDef *dma = hdma_spi2_tx.Instance;

//     // len = snprintf(msg, sizeof(msg), "\r\n--- DMA (SPI2 TX) Registers ---\r\n");
//     // HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     // len = snprintf(msg, sizeof(msg), "CCR   = 0x%08lX\r\n", dma->CCR);
//     // HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     // len = snprintf(msg, sizeof(msg), "CNDTR = 0x%08lX\r\n", dma->CNDTR);  // Number of data to transfer
//     // HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     // len = snprintf(msg, sizeof(msg), "CPAR  = 0x%08lX\r\n", dma->CPAR);   // Peripheral address
//     // HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     // len = snprintf(msg, sizeof(msg), "CMAR  = 0x%08lX\r\n", dma->CMAR);   // Memory address
//     // HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     // // Optional: DMA Interrupt Status (from DMA1 or DMA2)
//     // len = snprintf(msg, sizeof(msg), "\r\n--- DMA Interrupt Flags (DMA1->ISR) ---\r\n");
//     // HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);

//     // len = snprintf(msg, sizeof(msg), "ISR   = 0x%08lX\r\n", DMA1->ISR);
//     // HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, HAL_MAX_DELAY);
// }

// void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
//     if (hspi->Instance == SPI2) {
//         // Print_SPI_CR2(hspi);
//         // Print_SPI2_DMA_Registers();
//         // Switch SPI back to 8-bit mode
//         CLEAR_BIT(hspi2.Instance->CR1, SPI_CR1_SPE);  // Disable SPI
//         hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
//         MODIFY_REG(hspi2.Instance->CR2, SPI_CR2_DS, SPI_DATASIZE_8BIT);
//         SET_BIT(hspi2.Instance->CR1, SPI_CR1_SPE);    // Enable SPI

//         // Re-enable memory increment for future transfers
//         SET_BIT(hdma_spi2_tx.Instance->CCR, DMA_CCR_MINC);
//         // Print_SPI_CR2(hspi);
//         //Print_SPI2_DMA_Registers();
//         uart_println("SPI TX Callback");
//     }
// }

// void ST7789_SendSolidLine_DMA(ST7789V2_cfg_t* cfg, uint16_t x, uint16_t y, uint16_t pixel, uint16_t len) {
//     // Set address window
//     ST7789_SetAddressWindow(cfg, x, y, x + len - 1, y);

//     // Set data mode
//     ST7789_SelectDataMode(cfg);

//     // Switch to 16-bit SPI mode
//     CLEAR_BIT(cfg->spi->CR1, SPI_CR1_SPE);  // Disable SPI
//     MODIFY_REG(cfg->spi->CR2, SPI_CR2_DS, SPI_DATASIZE_16BIT);
//     SET_BIT(cfg->spi->CR1, SPI_CR1_SPE);    // Re-enable SPI

//     // Disable source increment
//     CLEAR_BIT(hdma_spi2_tx.Instance->CCR, DMA_CCR_MINC); // Disable memory increment

//     // Start DMA transfer using address of the same pixel value
//     HAL_SPI_Transmit_DMA(cfg->spi, (uint8_t *)&pixel, len * 2);
// }