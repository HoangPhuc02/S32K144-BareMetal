/**
 * @file    lcd_i2c.h
 * @brief   LCD 16x2/20x4 I2C Driver for S32K144
 * @details This driver supports HD44780-based LCD with PCF8574 I2C adapter
 *          in 4-bit mode communication.
 * 
 * @author  PhucPH32
 * @date    November 24, 2025
 * @version 1.0
 * 
 * @note    Default I2C address: 0x27 (7-bit address)
 *          PCF8574 pin mapping: P7 P6 P5 P4 P3 P2 P1 P0
 *                               D7 D6 D5 D4 BL EN RW RS
 */

#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <stdint.h>
#include <stdbool.h>
#include "i2c_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Default LCD I2C address (7-bit) */
#define LCD_I2C_DEFAULT_ADDR    0x27

/* PCF8574 control bits */
#define LCD_BIT_RS      (1 << 0)    /* Register Select: 0=Command, 1=Data */
#define LCD_BIT_RW      (1 << 1)    /* Read/Write: 0=Write, 1=Read */
#define LCD_BIT_EN      (1 << 2)    /* Enable signal */
#define LCD_BIT_BL      (1 << 3)    /* Backlight: 0=Off, 1=On */

/* LCD commands */
#define LCD_CMD_CLEAR           0x01    /* Clear display */
#define LCD_CMD_HOME            0x02    /* Return home */
#define LCD_CMD_ENTRY_MODE      0x04    /* Entry mode set */
#define LCD_CMD_DISPLAY_CTRL    0x08    /* Display control */
#define LCD_CMD_CURSOR_SHIFT    0x10    /* Cursor/display shift */
#define LCD_CMD_FUNCTION_SET    0x20    /* Function set */
#define LCD_CMD_CGRAM_ADDR      0x40    /* Set CGRAM address */
#define LCD_CMD_DDRAM_ADDR      0x80    /* Set DDRAM address */

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief LCD handle structure
 */
typedef struct {
    LPI2C_RegType *i2cBase;     /**< I2C peripheral base address */
    uint8_t address;            /**< I2C 7-bit address of LCD */
    uint8_t backlight;          /**< Backlight state: 0=Off, 1=On */
    uint8_t rows;               /**< Number of rows (2 or 4) */
    uint8_t cols;               /**< Number of columns (16 or 20) */
} LCD_I2C_Handle_t;

/*******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * @brief Initialize LCD with default configuration (16x2, address 0x27)
 * @param lcd Pointer to LCD handle structure
 * @param i2cBase I2C peripheral base address (LPI2C0)
 */
void LCD_Init(LCD_I2C_Handle_t *lcd, LPI2C_RegType *i2cBase);

/**
 * @brief Initialize LCD with custom configuration
 * @param lcd Pointer to LCD handle structure
 * @param i2cBase I2C peripheral base address
 * @param address I2C 7-bit address
 * @param rows Number of rows (2 or 4)
 * @param cols Number of columns (16 or 20)
 */
void LCD_InitEx(LCD_I2C_Handle_t *lcd, LPI2C_RegType *i2cBase, 
                uint8_t address, uint8_t rows, uint8_t cols);

/**
 * @brief Send command to LCD
 * @param lcd Pointer to LCD handle
 * @param cmd Command byte
 */
void LCD_SendCmd(LCD_I2C_Handle_t *lcd, uint8_t cmd);

/**
 * @brief Send data (character) to LCD
 * @param lcd Pointer to LCD handle
 * @param data Data byte
 */
void LCD_SendData(LCD_I2C_Handle_t *lcd, uint8_t data);

/**
 * @brief Clear LCD display
 * @param lcd Pointer to LCD handle
 */
void LCD_Clear(LCD_I2C_Handle_t *lcd);

/**
 * @brief Move cursor to home position (0,0)
 * @param lcd Pointer to LCD handle
 */
void LCD_Home(LCD_I2C_Handle_t *lcd);

/**
 * @brief Set cursor position
 * @param lcd Pointer to LCD handle
 * @param col Column (0 to cols-1)
 * @param row Row (0 to rows-1)
 */
void LCD_SetCursor(LCD_I2C_Handle_t *lcd, uint8_t col, uint8_t row);

/**
 * @brief Print single character at current cursor position
 * @param lcd Pointer to LCD handle
 * @param ch Character to print
 */
void LCD_PutChar(LCD_I2C_Handle_t *lcd, char ch);

/**
 * @brief Print string at current cursor position
 * @param lcd Pointer to LCD handle
 * @param str Null-terminated string
 */
void LCD_Print(LCD_I2C_Handle_t *lcd, const char *str);

/**
 * @brief Print formatted string (like printf)
 * @param lcd Pointer to LCD handle
 * @param format Format string
 * @param ... Variable arguments
 */
void LCD_Printf(LCD_I2C_Handle_t *lcd, const char *format, ...);

/**
 * @brief Turn backlight on
 * @param lcd Pointer to LCD handle
 */
void LCD_BacklightOn(LCD_I2C_Handle_t *lcd);

/**
 * @brief Turn backlight off
 * @param lcd Pointer to LCD handle
 */
void LCD_BacklightOff(LCD_I2C_Handle_t *lcd);

/**
 * @brief Create custom character
 * @param lcd Pointer to LCD handle
 * @param location Character location (0-7)
 * @param charmap Character bitmap (8 bytes)
 */
void LCD_CreateChar(LCD_I2C_Handle_t *lcd, uint8_t location, uint8_t charmap[]);

#endif /* LCD_I2C_H */