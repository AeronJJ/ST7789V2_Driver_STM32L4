# ST7789V2_Driver_STM32L4

## Requirements
 - stm32l476xx.h and all sub-dependencies

## Usage
If using STM32CubeMX to generate your project, add this repo to the root directory and update ./CMakeLists.txt target_sources with:
```
    Core/Src/ST7789V2_Driver.c
    Core/Src/LCD.c
```