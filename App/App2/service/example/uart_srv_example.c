/**
 * @file    uart_srv_example.c
 * @brief   UART Service Example
 * @details Demonstrate cách sử dụng UART service
 * 
 * Hardware Setup:
 * - UART1 TX: PTC7
 * - UART1 RX: PTC6
 * - Baudrate: 115200
 * - Connect to PC via USB-UART converter
 * 
 * Expected Behavior:
 * - Send "Hello World" message
 * - Send byte-by-byte
 * - Send formatted strings
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 */

#include "../inc/uart_srv.h"
#include "../inc/clock_srv_v2.h"
#include <stdio.h>

void UART_BasicExample(void)
{
    /* 1. Initialize clock system */
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    
    /* 2. Enable LPUART1 clock */
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_SOSCDIV2);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    
    /* 3. Initialize UART (PORT config handled internally) */
    uart_srv_status_t status = UART_SRV_Init(115200);
    
    if (status != UART_SRV_SUCCESS) {
        /* Error - stuck here */
        while(1);
    }
    
    /* 4. Send welcome message */
    UART_SRV_SendString("\r\n");
    UART_SRV_SendString("==============================\r\n");
    UART_SRV_SendString("  UART Service Example V1.0  \r\n");
    UART_SRV_SendString("==============================\r\n");
    UART_SRV_SendString("\r\n");
    
    /* 5. Send byte by byte */
    UART_SRV_SendString("Sending bytes: ");
    UART_SRV_SendByte('H');
    UART_SRV_SendByte('e');
    UART_SRV_SendByte('l');
    UART_SRV_SendByte('l');
    UART_SRV_SendByte('o');
    UART_SRV_SendString("\r\n");
    
    /* 6. Counter example */
    uint32_t counter = 0;
    char buffer[64];
    
    UART_SRV_SendString("\r\nStarting counter...\r\n\r\n");
    
    while(1) {
        /* Format and send counter value */
        sprintf(buffer, "Counter: %lu\r\n", counter);
        UART_SRV_SendString(buffer);
        
        counter++;
        
        /* Delay ~1 second */
        for(volatile uint32_t i = 0; i < 2000000; i++);
    }
}

/* Example: Send data at different baudrates */
void UART_BaudrateExample(void)
{
    uint32_t baudrates[] = {9600, 19200, 38400, 57600, 115200};
    
    /* Initialize clock */
    CLOCK_SRV_V2_InitPreset("RUN_48MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_FIRCDIV2);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    
    for(uint8_t i = 0; i < 5; i++) {
        /* Re-initialize UART with new baudrate */
        UART_SRV_Init(baudrates[i]);
        
        /* Send test message */
        UART_SRV_SendString("Testing baudrate: ");
        
        char buf[16];
        sprintf(buf, "%lu\r\n", baudrates[i]);
        UART_SRV_SendString(buf);
        
        /* Delay 3 seconds */
        for(volatile uint32_t j = 0; j < 3000000; j++);
    }
}

/* Example: Menu system via UART */
void UART_MenuExample(void)
{
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_SOSCDIV2);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    
    UART_SRV_Init(115200);
    
    /* Display menu */
    while(1) {
        UART_SRV_SendString("\r\n");
        UART_SRV_SendString("=== Main Menu ===\r\n");
        UART_SRV_SendString("1. Read ADC\r\n");
        UART_SRV_SendString("2. Toggle LED\r\n");
        UART_SRV_SendString("3. System Info\r\n");
        UART_SRV_SendString("4. Reset\r\n");
        UART_SRV_SendString("\r\nSelect option: ");
        
        /* Note: This example shows output only */
        /* For input, you'd need to implement UART receive */
        
        /* Delay */
        for(volatile uint32_t i = 0; i < 5000000; i++);
    }
}

/* Example: Debug logging */
void UART_DebugExample(void)
{
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_SOSCDIV2);
    UART_SRV_Init(115200);
    
    /* Log levels */
    #define LOG_INFO(msg)  UART_SRV_SendString("[INFO]  " msg "\r\n")
    #define LOG_WARN(msg)  UART_SRV_SendString("[WARN]  " msg "\r\n")
    #define LOG_ERROR(msg) UART_SRV_SendString("[ERROR] " msg "\r\n")
    
    LOG_INFO("System starting...");
    
    for(volatile uint32_t i = 0; i < 1000000; i++);
    
    LOG_INFO("Initializing peripherals...");
    LOG_INFO("ADC initialized");
    LOG_INFO("GPIO initialized");
    
    for(volatile uint32_t i = 0; i < 1000000; i++);
    
    LOG_WARN("Low battery detected");
    
    for(volatile uint32_t i = 0; i < 1000000; i++);
    
    LOG_ERROR("Sensor communication failed!");
    
    LOG_INFO("Entering main loop...");
    
    while(1) {
        /* Main application */
        for(volatile uint32_t i = 0; i < 5000000; i++);
    }
}
