/**
 * @file    lcd_i2c.c
 * @brief   LCD 16x2/20x4 I2C Driver Implementation for S32K144
 * @details HD44780-based LCD with PCF8574 I2C adapter in 4-bit mode
 * 
 * @author  PhucPH32
 * @date    November 24, 2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "lcd_i2c.h"
#include "i2c.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Delay in milliseconds (simple busy-wait)
 */
static void LCD_DelayMs(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 8000; i++);
}

/**
 * @brief Delay in microseconds
 */
static void LCD_DelayUs(uint32_t us)
{
    for (volatile uint32_t i = 0; i < us * 8; i++);
}

/**
 * @brief Send 4-bit data to LCD via I2C
 */
static void LCD_SendNibble(LCD_I2C_Handle_t *lcd, uint8_t nibble, uint8_t rs)
{
    uint8_t data;
    
    /* Prepare data byte: D7-D4 = nibble, D3 = BL, D2 = EN, D1 = RW, D0 = RS */
    data = (nibble & 0xF0) | (lcd->backlight ? LCD_BIT_BL : 0) | (rs ? LCD_BIT_RS : 0);
    
    /* Send with EN=1 */
    uint8_t dataH = data | LCD_BIT_EN;
    I2C_MasterStart(lcd->i2cBase, lcd->address, I2C_WRITE);
    I2C_MasterSend(lcd->i2cBase, &dataH, 1, false);
    LCD_DelayUs(1);
    
    /* Send with EN=0 */
    uint8_t dataL = data;
    I2C_MasterSend(lcd->i2cBase, &dataL, 1, true);
    LCD_DelayUs(50);
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Send command to LCD
 */
void LCD_SendCmd(LCD_I2C_Handle_t *lcd, uint8_t cmd)
{
    /* Send upper nibble */
    LCD_SendNibble(lcd, cmd & 0xF0, 0);
    
    /* Send lower nibble */
    LCD_SendNibble(lcd, (cmd << 4) & 0xF0, 0);
    
    /* Commands need time to execute */
    if (cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME) {
        LCD_DelayMs(2);
    } else {
        LCD_DelayUs(50);
    }
}

/**
 * @brief Send data to LCD
 */
void LCD_SendData(LCD_I2C_Handle_t *lcd, uint8_t data)
{
    /* Send upper nibble with RS=1 */
    LCD_SendNibble(lcd, data & 0xF0, 1);
    
    /* Send lower nibble with RS=1 */
    LCD_SendNibble(lcd, (data << 4) & 0xF0, 1);
    
    LCD_DelayUs(50);
}

/**
 * @brief Initialize LCD with default configuration
 */
void LCD_Init(LCD_I2C_Handle_t *lcd, LPI2C_RegType *i2cBase)
{
    LCD_InitEx(lcd, i2cBase, LCD_I2C_DEFAULT_ADDR, 2, 16);
}

/**
 * @brief Initialize LCD with custom configuration
 */
void LCD_InitEx(LCD_I2C_Handle_t *lcd, LPI2C_RegType *i2cBase,
                uint8_t address, uint8_t rows, uint8_t cols)
{
    /* Save configuration */
    lcd->i2cBase = i2cBase;
    lcd->address = address;
    lcd->backlight = 1;
    lcd->rows = rows;
    lcd->cols = cols;
    
    /* Wait for LCD power-up */
    LCD_DelayMs(50);
    
    /* Initialization sequence for 4-bit mode */
    LCD_SendNibble(lcd, 0x30, 0);  /* Function set: 8-bit */
    LCD_DelayMs(5);
    
    LCD_SendNibble(lcd, 0x30, 0);  /* Function set: 8-bit */
    LCD_DelayUs(150);
    
    LCD_SendNibble(lcd, 0x30, 0);  /* Function set: 8-bit */
    LCD_DelayUs(150);
    
    LCD_SendNibble(lcd, 0x20, 0);  /* Function set: 4-bit */
    LCD_DelayUs(150);
    
    /* Configure LCD */
    LCD_SendCmd(lcd, 0x28);  /* 4-bit, 2 lines, 5x8 font */
    LCD_SendCmd(lcd, 0x08);  /* Display off */
    LCD_SendCmd(lcd, 0x01);  /* Clear display */
    LCD_SendCmd(lcd, 0x06);  /* Entry mode: increment cursor */
    LCD_SendCmd(lcd, 0x0C);  /* Display on, cursor off, blink off */
}

/**
 * @brief Clear LCD display
 */
void LCD_Clear(LCD_I2C_Handle_t *lcd)
{
    LCD_SendCmd(lcd, LCD_CMD_CLEAR);
}

/**
 * @brief Move cursor to home position
 */
void LCD_Home(LCD_I2C_Handle_t *lcd)
{
    LCD_SendCmd(lcd, LCD_CMD_HOME);
}

/**
 * @brief Set cursor position
 */
void LCD_SetCursor(LCD_I2C_Handle_t *lcd, uint8_t col, uint8_t row)
{
    uint8_t address;
    
    /* Calculate DDRAM address based on row */
    switch (row) {
        case 0: address = 0x00 + col; break;  /* Row 1 */
        case 1: address = 0x40 + col; break;  /* Row 2 */
        case 2: address = 0x14 + col; break;  /* Row 3 (for 20x4) */
        case 3: address = 0x54 + col; break;  /* Row 4 (for 20x4) */
        default: return;
    }
    
    LCD_SendCmd(lcd, LCD_CMD_DDRAM_ADDR | address);
}

/**
 * @brief Print single character
 */
void LCD_PutChar(LCD_I2C_Handle_t *lcd, char ch)
{
    LCD_SendData(lcd, (uint8_t)ch);
}

/**
 * @brief Print string
 */
void LCD_Print(LCD_I2C_Handle_t *lcd, const char *str)
{
    while (*str) {
        LCD_SendData(lcd, *str++);
    }
}

/**
 * @brief Print formatted string
 */
void LCD_Printf(LCD_I2C_Handle_t *lcd, const char *format, ...)
{
    char buffer[64];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    LCD_Print(lcd, buffer);
}

/**
 * @brief Turn backlight on
 */
void LCD_BacklightOn(LCD_I2C_Handle_t *lcd)
{
    lcd->backlight = 1;
    uint8_t data = LCD_BIT_BL;
    
    I2C_MasterStart(lcd->i2cBase, lcd->address, I2C_WRITE);
    I2C_MasterSend(lcd->i2cBase, &data, 1, true);
}

/**
 * @brief Turn backlight off
 */
void LCD_BacklightOff(LCD_I2C_Handle_t *lcd)
{
    lcd->backlight = 0;
    uint8_t data = 0x00;
    
    I2C_MasterStart(lcd->i2cBase, lcd->address, I2C_WRITE);
    I2C_MasterSend(lcd->i2cBase, &data, 1, true);
}

/**
 * @brief Create custom character
 */
void LCD_CreateChar(LCD_I2C_Handle_t *lcd, uint8_t location, uint8_t charmap[])
{
    location &= 0x07;  /* Only 8 locations (0-7) */
    
    LCD_SendCmd(lcd, LCD_CMD_CGRAM_ADDR | (location << 3));
    
    for (uint8_t i = 0; i < 8; i++) {
        LCD_SendData(lcd, charmap[i]);
    }
    
    /* Return to DDRAM */
    LCD_SendCmd(lcd, LCD_CMD_DDRAM_ADDR);
}
