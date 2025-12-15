/**
 * @file    i2c_example.c
 * @brief   LPI2C Driver Usage Examples for S32K144
 * @details This file contains practical examples demonstrating how to use
 *          the I2C driver with LCD 16x2 display via PCF8574 I2C adapter.
 * 
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 * 
 * @note    Hardware connections:
 *          - PTA2: I2C0_SCL (with 4.7k pull-up to VDD)
 *          - PTA3: I2C0_SDA (with 4.7k pull-up to VDD)
 *          - LCD 16x2 with PCF8574 I2C adapter (address 0x27)
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "i2c.h"
#include "port.h"
#include "pcc_reg.h"
#include "lcd_i2c.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* LCD I2C address */
#define LCD_I2C_ADDR    0x27

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

static LCD_I2C_Handle_t g_lcd;

/*******************************************************************************
 * Example 1: Basic I2C and LCD Initialization
 ******************************************************************************/

/**
 * @brief Initialize I2C0 and LCD with standard configuration
 */
void Example1_BasicInit(void)
{
    /* Step 1: Enable peripheral clocks */
    I2C_EnableClock(0);              /* Enable LPI2C0 clock */
    PORT_EnableClock(PORT_A);        /* Enable PORT_A clock */
    
    /* Step 2: Configure I2C pins (PTA2=SCL, PTA3=SDA) */
    PORT_SetPinMux(PORT_A, 2, PORT_MUX_ALT3);  /* I2C0_SCL */
    PORT_SetPinMux(PORT_A, 3, PORT_MUX_ALT3);  /* I2C0_SDA */
    
    
    /* Enable internal pull-ups (if no external pull-ups) */
    PORT_SetPullConfig(PORT_A, 2, PORT_PULL_UP);
    PORT_SetPullConfig(PORT_A, 3, PORT_PULL_UP);
    
    /* Step 3: Configure I2C Master */
    I2C_MasterConfig_t config = {
        .baudRate = 100000,          /* 100 kHz - Standard mode */
        .prescaler = 0,              /* No prescaler */
        .enableMaster = true,
        .enableDebug = false
    };
    
    /* Step 4: Initialize I2C with 8 MHz source clock */
    uint32_t srcClock = 8000000;
    I2C_Status_t status = I2C_MasterInit(LPI2C0, &config, srcClock);
    
    if (status == I2C_STATUS_SUCCESS) {
        printf("I2C initialized successfully!\n");
    } else {
        printf("I2C initialization failed!\n");
        return;
    }
    
    /* Step 5: Initialize LCD */
    LCD_InitEx(&g_lcd, LPI2C0, LCD_I2C_ADDR, 2, 16);
    printf("LCD initialized!\n");
}

/*******************************************************************************
 * Example 2: Display Welcome Message
 ******************************************************************************/

/**
 * @brief Display welcome message on LCD
 */
void Example2_WelcomeMessage(void)
{
    printf("\n=== Welcome Message ===\n");
    
    /* Clear LCD */
    LCD_Clear(&g_lcd);
    
    /* Display on line 1 */
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "  S32K144 LCD  ");
    
    /* Display on line 2 */
    LCD_SetCursor(&g_lcd, 0, 1);
    LCD_Print(&g_lcd, "   I2C Demo!   ");
    
    printf("Welcome message displayed\n");
}

/*******************************************************************************
 * Example 3: Counter Display
 ******************************************************************************/

/**
 * @brief Display incrementing counter on LCD
 */
void Example3_CounterDisplay(void)
{
    printf("\n=== Counter Display ===\n");
    
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "Counter Demo:");
    
    for (int i = 0; i < 20; i++) {
        /* Update counter on line 2 */
        LCD_SetCursor(&g_lcd, 0, 1);
        LCD_Printf(&g_lcd, "Count: %03d      ", i);
        
        printf("Counter: %d\n", i);
        
        /* Delay */
        for (volatile uint32_t d = 0; d < 500000; d++);
    }
}

/*******************************************************************************
 * Example 4: Sensor Data Display
 ******************************************************************************/

/**
 * @brief Simulate and display sensor readings
 */
void Example4_SensorDisplay(void)
{
    printf("\n=== Sensor Data Display ===\n");
    
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "Sensor Readings");
    
    for (int i = 0; i < 10; i++) {
        /* Simulate temperature (20-30°C) */
        float temp = 20.0f + (i % 10) * 1.1f;
        
        /* Display temperature */
        LCD_SetCursor(&g_lcd, 0, 1);
        LCD_Printf(&g_lcd, "Temp: %.1fC    ", temp);
        
        printf("Temperature: %.1f°C\n", temp);
        
        /* Delay */
        for (volatile uint32_t d = 0; d < 800000; d++);
    }
}

/*******************************************************************************
 * Example 5: Scrolling Text
 ******************************************************************************/

/**
 * @brief Display scrolling text on LCD
 */
void Example5_ScrollingText(void)
{
    const char *message = "   Hello from S32K144! This is a scrolling demo.   ";
    int msgLen = strlen(message);
    
    printf("\n=== Scrolling Text ===\n");
    
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, " Scroll Demo ");
    
    /* Scroll for 3 cycles */
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int pos = 0; pos < msgLen - 16; pos++) {
            LCD_SetCursor(&g_lcd, 0, 1);
            
            /* Display 16 characters starting from pos */
            for (int i = 0; i < 16; i++) {
                LCD_PutChar(&g_lcd, message[pos + i]);
            }
            
            /* Delay for smooth scrolling */
            for (volatile uint32_t d = 0; d < 200000; d++);
        }
    }
    
    printf("Scrolling complete\n");
}

/*******************************************************************************
 * Example 6: Custom Characters
 ******************************************************************************/

/**
 * @brief Display custom characters (heart, smile, battery)
 */
void Example6_CustomChars(void)
{
    printf("\n=== Custom Characters ===\n");
    
    /* Define custom characters */
    uint8_t heart[8] = {
        0b00000,
        0b01010,
        0b11111,
        0b11111,
        0b01110,
        0b00100,
        0b00000,
        0b00000
    };
    
    uint8_t smile[8] = {
        0b00000,
        0b01010,
        0b01010,
        0b00000,
        0b10001,
        0b01110,
        0b00000,
        0b00000
    };
    
    uint8_t battery[8] = {
        0b01110,
        0b11011,
        0b10001,
        0b10001,
        0b10001,
        0b11111,
        0b11111,
        0b00000
    };
    
    /* Create custom characters */
    LCD_CreateChar(&g_lcd, 0, heart);
    LCD_CreateChar(&g_lcd, 1, smile);
    LCD_CreateChar(&g_lcd, 2, battery);
    
    /* Display custom characters */
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "Custom Chars:");
    
    LCD_SetCursor(&g_lcd, 0, 1);
    LCD_PutChar(&g_lcd, 0);  /* Heart */
    LCD_Print(&g_lcd, " ");
    LCD_PutChar(&g_lcd, 1);  /* Smile */
    LCD_Print(&g_lcd, " ");
    LCD_PutChar(&g_lcd, 2);  /* Battery */
    LCD_Print(&g_lcd, " Icons!");
    
    printf("Custom characters displayed\n");
    
    /* Delay to show */
    for (volatile uint32_t d = 0; d < 2000000; d++);
}

/*******************************************************************************
 * Example 7: Backlight Control
 ******************************************************************************/

/**
 * @brief Demonstrate backlight on/off
 */
void Example7_BacklightDemo(void)
{
    printf("\n=== Backlight Demo ===\n");
    
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "Backlight Test");
    
    for (int i = 0; i < 5; i++) {
        LCD_SetCursor(&g_lcd, 0, 1);
        LCD_Printf(&g_lcd, "Blink: %d/5    ", i + 1);
        
        /* Backlight off */
        LCD_BacklightOff(&g_lcd);
        printf("Backlight OFF\n");
        for (volatile uint32_t d = 0; d < 500000; d++);
        
        /* Backlight on */
        LCD_BacklightOn(&g_lcd);
        printf("Backlight ON\n");
        for (volatile uint32_t d = 0; d < 500000; d++);
    }
}

/*******************************************************************************
 * Example 8: System Information Display
 ******************************************************************************/

/**
 * @brief Display system information
 */
void Example8_SystemInfo(void)
{
    printf("\n=== System Information ===\n");
    
    /* Screen 1: MCU Info */
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "MCU: S32K144");
    LCD_SetCursor(&g_lcd, 0, 1);
    LCD_Print(&g_lcd, "Core: ARM M4F");
    
    for (volatile uint32_t d = 0; d < 1500000; d++);
    
    /* Screen 2: Clock Info */
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "Clock: 80 MHz");
    LCD_SetCursor(&g_lcd, 0, 1);
    LCD_Print(&g_lcd, "I2C: 100 kHz");
    
    for (volatile uint32_t d = 0; d < 1500000; d++);
    
    /* Screen 3: Status */
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "Status: OK");
    LCD_SetCursor(&g_lcd, 0, 1);
    LCD_Print(&g_lcd, "Ready!");
    
    printf("System info displayed\n");
}

/*******************************************************************************
 * Example 9: Menu System
 ******************************************************************************/

/**
 * @brief Simple menu navigation demo
 */
void Example9_MenuDemo(void)
{
    const char *menuItems[] = {
        "1. Settings",
        "2. Sensors",
        "3. Network",
        "4. About",
        "5. Exit"
    };
    int numItems = 5;
    
    printf("\n=== Menu Demo ===\n");
    
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "   Main Menu   ");
    
    for (int i = 0; i < numItems; i++) {
        LCD_SetCursor(&g_lcd, 0, 1);
        LCD_Print(&g_lcd, menuItems[i]);
        LCD_Print(&g_lcd, "    ");  /* Clear remaining */
        
        printf("Menu: %s\n", menuItems[i]);
        
        /* Delay */
        for (volatile uint32_t d = 0; d < 1000000; d++);
    }
}

/*******************************************************************************
 * Example 10: Progress Bar
 ******************************************************************************/

/**
 * @brief Display progress bar animation
 */
void Example10_ProgressBar(void)
{
    printf("\n=== Progress Bar ===\n");
    
    /* Custom block character for progress bar */
    uint8_t block[8] = {
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111
    };
    
    LCD_CreateChar(&g_lcd, 0, block);
    
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "Loading...");
    
    /* Draw progress bar */
    for (int i = 0; i <= 16; i++) {
        LCD_SetCursor(&g_lcd, 0, 1);
        
        /* Draw filled portion */
        for (int j = 0; j < i; j++) {
            LCD_PutChar(&g_lcd, 0);  /* Custom block */
        }
        
        /* Calculate percentage */
        int percent = (i * 100) / 16;
        
        /* Show percentage after progress bar */
        LCD_SetCursor(&g_lcd, 13, 1);
        LCD_Printf(&g_lcd, "%3d%%", percent);
        
        printf("Progress: %d%%\n", percent);
        
        /* Delay */
        for (volatile uint32_t d = 0; d < 300000; d++);
    }
    
    /* Complete */
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "   Complete!   ");
    LCD_SetCursor(&g_lcd, 0, 1);
    LCD_Print(&g_lcd, "   Success!    ");
    
    printf("Progress complete\n");
}

/*******************************************************************************
 * Example 11: I2C Bus Scan
 ******************************************************************************/

/**
 * @brief Scan I2C bus and display results on LCD
 */
void Example11_BusScan(void)
{
    uint8_t devicesFound = 0;
    uint8_t foundAddrs[10];
    
    printf("\n=== I2C Bus Scan ===\n");
    
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "Scanning I2C...");
    
    /* Scan addresses 0x08 to 0x77 */
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        /* Try to start communication with this address */
        I2C_Status_t status = I2C_MasterStart(LPI2C0, addr, I2C_WRITE);
        
        if (status == I2C_STATUS_SUCCESS) {
            /* Device found - ACK received */
            if (devicesFound < 10) {
                foundAddrs[devicesFound] = addr;
            }
            devicesFound++;
            
            /* Send STOP condition */
            I2C_MasterStop(LPI2C0);
            
            printf("Device found at 0x%02X\n", addr);
        }
        /* If NACK, the function already sent STOP internally */
        
        /* Small delay between scans */
        for (volatile uint32_t i = 0; i < 1000; i++);
    }
    
    /* Display results */
    LCD_SetCursor(&g_lcd, 0, 1);
    LCD_Printf(&g_lcd, "Found: %d dev   ", devicesFound);
    
    for (volatile uint32_t d = 0; d < 1500000; d++);
    
    /* Show first few addresses */
    if (devicesFound > 0) {
        LCD_Clear(&g_lcd);
        LCD_SetCursor(&g_lcd, 0, 0);
        LCD_Print(&g_lcd, "Addresses:");
        
        LCD_SetCursor(&g_lcd, 0, 1);
        for (int i = 0; i < devicesFound && i < 3; i++) {
            LCD_Printf(&g_lcd, "%02X ", foundAddrs[i]);
        }
    }
    
    printf("Scan complete: %d device(s) found\n", devicesFound);
}



/*******************************************************************************
 * Main Function
 ******************************************************************************/

/**
 * @brief Main function - runs all LCD examples
 */
int main(void)
{
    /* Example 1: Initialize I2C and LCD */
    Example1_BasicInit();
    
    /* Delay after init */
    for (volatile uint32_t d = 0; d < 1000000; d++);
    
    /* Example 2: Welcome message */
    Example2_WelcomeMessage();
    for (volatile uint32_t d = 0; d < 2000000; d++);
    
    /* Example 3: Counter display */
    Example3_CounterDisplay();
    for (volatile uint32_t d = 0; d < 1000000; d++);
    
    /* Example 4: Sensor data */
    Example4_SensorDisplay();
    for (volatile uint32_t d = 0; d < 1000000; d++);
    
    /* Example 5: Scrolling text */
    Example5_ScrollingText();
    for (volatile uint32_t d = 0; d < 1000000; d++);
    
    /* Example 6: Custom characters */
    Example6_CustomChars();
    for (volatile uint32_t d = 0; d < 2000000; d++);
    
    /* Example 7: Backlight control */
    Example7_BacklightDemo();
    for (volatile uint32_t d = 0; d < 1000000; d++);
    
    /* Example 8: System info */
    Example8_SystemInfo();
    for (volatile uint32_t d = 0; d < 2000000; d++);
    
    /* Example 9: Menu demo */
    Example9_MenuDemo();
    for (volatile uint32_t d = 0; d < 1000000; d++);
    
    /* Example 10: Progress bar */
    Example10_ProgressBar();
    for (volatile uint32_t d = 0; d < 2000000; d++);
    
    /* Example 11: I2C bus scan */
    Example11_BusScan();
    for (volatile uint32_t d = 0; d < 2000000; d++);
    
    /* Final message */
    LCD_Clear(&g_lcd);
    LCD_SetCursor(&g_lcd, 0, 0);
    LCD_Print(&g_lcd, "All Examples");
    LCD_SetCursor(&g_lcd, 0, 1);
    LCD_Print(&g_lcd, "  Completed!  ");
    
    printf("\n=== All LCD I2C examples completed! ===\n");
}
