/**
 * @file    uart_example.c
 * @brief   LPUART Driver Usage Examples for S32K144
 * @details This file contains practical examples demonstrating how to use
 *          the UART driver for various common applications.
 * 
 * @author  PhucPH32
 * @date    November 24, 2025
 * @version 1.0
 * 
 * @note    Hardware connections:
 *          - PTB0: LPUART0_RX
 *          - PTB1: LPUART0_TX
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "uart.h"
#include "port.h"
#include "gpio.h"
#include "pcc_reg.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define RX_BUFFER_SIZE      128
#define CMD_BUFFER_SIZE     64

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

static uint8_t rxBuffer[RX_BUFFER_SIZE];
static char cmdBuffer[CMD_BUFFER_SIZE];

/*******************************************************************************
 * Example 1: Basic UART Initialization
 ******************************************************************************/

/**
 * @brief Initialize UART0 with default configuration
 */
void Example1_BasicInit(void)
{
    /* Step 1: Enable peripheral clocks */
    UART_EnableClock(0);             /* Enable LPUART0 clock */
    PORT_EnableClock(PORT_B);        /* Enable PORTB clock */
    
    /* Step 2: Configure UART pins (PTB0=RX, PTB1=TX) */
    PORT_SetPinMux(PORTB, 0, PORT_MUX_ALT2);  /* LPUART0_RX */
    PORT_SetPinMux(PORTB, 1, PORT_MUX_ALT2);  /* LPUART0_TX */
    
    /* Step 3: Get default configuration (115200-8-N-1) */
    UART_Config_t config;
    UART_GetDefaultConfig(&config);
    
    /* Or customize configuration */
    config.baudRate = 115200;
    config.parity = UART_PARITY_DISABLED;
    config.stopBits = UART_ONE_STOP_BIT;
    config.dataBits = UART_8_DATA_BITS;
    config.enableTx = true;
    config.enableRx = true;
    
    /* Step 4: Initialize UART with 8 MHz source clock */
    uint32_t srcClock = 8000000;
    UART_Status_t status = UART_Init(LPUART0, &config, srcClock);
    
    if (status == UART_STATUS_SUCCESS) {
        printf("UART initialized successfully!\n");
    } else {
        printf("UART initialization failed!\n");
    }
}

/*******************************************************************************
 * Example 2: Send String
 ******************************************************************************/

/**
 * @brief Send a string via UART
 */
void Example2_SendString(const char *str)
{
    UART_Status_t status;
    
    status = UART_SendBlocking(LPUART0, (const uint8_t *)str, strlen(str));
    
    if (status == UART_STATUS_SUCCESS) {
        printf("String sent successfully\n");
    } else if (status == UART_STATUS_TIMEOUT) {
        printf("Send timeout\n");
    }
}

/**
 * @brief Complete send string example
 */
void Example2_Complete(void)
{
    printf("\n=== Send String Example ===\n");
    
    Example2_SendString("Hello from S32K144!\r\n");
    Example2_SendString("UART is working correctly.\r\n");
    Example2_SendString("Line 3 of text.\r\n");
}

/*******************************************************************************
 * Example 3: Printf Implementation
 ******************************************************************************/

/**
 * @brief UART printf implementation
 */
void UART_Printf(const char *format, ...)
{
    char buffer[128];
    va_list args;
    
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (len > 0) {
        UART_SendBlocking(LPUART0, (uint8_t *)buffer, len);
    }
}

/**
 * @brief Complete printf example
 */
void Example3_PrintfDemo(void)
{
    int counter = 0;
    float voltage = 3.3f;
    
    printf("\n=== Printf Example ===\n");
    
    UART_Printf("Counter: %d\r\n", counter++);
    UART_Printf("Voltage: %.2f V\r\n", voltage);
    UART_Printf("Temperature: %d C\r\n", 25);
    UART_Printf("Hex value: 0x%04X\r\n", 0xABCD);
    UART_Printf("Binary: 0b%08b\r\n", 0b10101010);
}

/*******************************************************************************
 * Example 4: Receive Data
 ******************************************************************************/

/**
 * @brief Receive a single byte with timeout
 */
bool Example4_ReceiveByteTimeout(uint8_t *data, uint32_t timeout_ms)
{
    uint32_t startTime = 0;  /* Replace with actual tick counter */
    
    while (startTime < timeout_ms) {
        if (UART_IsRxReady(LPUART0)) {
            return (UART_ReceiveByte(LPUART0, data) == UART_STATUS_SUCCESS);
        }
        
        /* Increment time counter */
        for (volatile uint32_t i = 0; i < 1000; i++);
        startTime++;
    }
    
    return false;  /* Timeout */
}

/**
 * @brief Receive a line (until \n or \r)
 */
uint32_t Example4_ReceiveLine(char *buffer, uint32_t maxLen)
{
    uint32_t count = 0;
    uint8_t ch;
    
    while (count < (maxLen - 1)) {
        if (UART_ReceiveByte(LPUART0, &ch) == UART_STATUS_SUCCESS) {
            if (ch == '\n' || ch == '\r') {
                break;
            }
            buffer[count++] = ch;
        }
    }
    
    buffer[count] = '\0';  /* Null terminate */
    return count;
}

/**
 * @brief Complete receive example
 */
void Example4_ReceiveDemo(void)
{
    printf("\n=== Receive Example ===\n");
    UART_Printf("Type something and press Enter:\r\n");
    
    char inputBuffer[64];
    uint32_t len = Example4_ReceiveLine(inputBuffer, sizeof(inputBuffer));
    
    UART_Printf("You typed: %s (length=%d)\r\n", inputBuffer, len);
}

/*******************************************************************************
 * Example 5: Echo Application
 ******************************************************************************/

/**
 * @brief Simple echo - receive and send back
 */
void Example5_SimpleEcho(void)
{
    printf("\n=== Echo Example ===\n");
    UART_Printf("Echo mode - type characters (ESC to exit)\r\n");
    
    uint8_t ch;
    bool running = true;
    
    while (running) {
        if (UART_IsRxReady(LPUART0)) {
            if (UART_ReceiveByte(LPUART0, &ch) == UART_STATUS_SUCCESS) {
                /* Check for ESC key */
                if (ch == 0x1B) {
                    running = false;
                    UART_Printf("\r\nEcho mode exited\r\n");
                } else {
                    /* Echo back */
                    UART_SendByte(LPUART0, ch);
                }
            }
        }
    }
}

/*******************************************************************************
 * Example 6: Command Line Interface
 ******************************************************************************/

/**
 * @brief Process received command
 */
void Example6_ProcessCommand(const char *cmd)
{
    if (strcmp(cmd, "help") == 0) {
        UART_Printf("Available commands:\r\n");
        UART_Printf("  help     - Show this help\r\n");
        UART_Printf("  status   - Show system status\r\n");
        UART_Printf("  reset    - Reset the system\r\n");
        UART_Printf("  led on   - Turn on LED\r\n");
        UART_Printf("  led off  - Turn off LED\r\n");
        UART_Printf("  version  - Show firmware version\r\n");
        
    } else if (strcmp(cmd, "status") == 0) {
        UART_Printf("System Status:\r\n");
        UART_Printf("  CPU: S32K144 @ 80MHz\r\n");
        UART_Printf("  UART: 115200 baud\r\n");
        UART_Printf("  Status: OK\r\n");
        
    } else if (strcmp(cmd, "reset") == 0) {
        UART_Printf("Resetting system...\r\n");
        /* Perform system reset */
        
    } else if (strcmp(cmd, "led on") == 0) {
        UART_Printf("LED turned ON\r\n");
        /* GPIO_WritePin(PORTD, 15, 1); */
        
    } else if (strcmp(cmd, "led off") == 0) {
        UART_Printf("LED turned OFF\r\n");
        /* GPIO_WritePin(PORTD, 15, 0); */
        
    } else if (strcmp(cmd, "version") == 0) {
        UART_Printf("Firmware Version: 1.0.0\r\n");
        UART_Printf("Build Date: %s %s\r\n", __DATE__, __TIME__);
        
    } else {
        UART_Printf("Unknown command: %s\r\n", cmd);
        UART_Printf("Type 'help' for available commands\r\n");
    }
}

/**
 * @brief Command line interface task
 */
void Example6_CLI_Task(void)
{
    static uint32_t cmdIndex = 0;
    uint8_t ch;
    
    if (UART_IsRxReady(LPUART0)) {
        UART_ReceiveByte(LPUART0, &ch);
        
        if (ch == '\r' || ch == '\n') {
            /* Process command */
            if (cmdIndex > 0) {
                cmdBuffer[cmdIndex] = '\0';
                UART_Printf("\r\n");
                
                Example6_ProcessCommand(cmdBuffer);
                
                cmdIndex = 0;
            }
            UART_Printf("\r\n> ");  /* Prompt */
            
        } else if (ch == 0x08 || ch == 0x7F) {  /* Backspace */
            if (cmdIndex > 0) {
                cmdIndex--;
                UART_Printf("\b \b");  /* Erase character */
            }
            
        } else if (cmdIndex < (CMD_BUFFER_SIZE - 1)) {
            cmdBuffer[cmdIndex++] = ch;
            UART_SendByte(LPUART0, ch);  /* Echo */
        }
    }
}

/**
 * @brief Complete CLI example
 */
void Example6_CLI_Demo(void)
{
    printf("\n=== Command Line Interface ===\n");
    UART_Printf("\r\nWelcome to S32K144 CLI!\r\n");
    UART_Printf("Type 'help' for available commands\r\n");
    UART_Printf("> ");
    
    /* Run for a limited time (or indefinitely in real application) */
    for (int i = 0; i < 1000; i++) {
        Example6_CLI_Task();
        
        /* Small delay */
        for (volatile uint32_t d = 0; d < 10000; d++);
    }
}

/*******************************************************************************
 * Example 7: Data Logging
 ******************************************************************************/

/**
 * @brief Log sensor data to UART
 */
void Example7_LogData(void)
{
    static uint32_t logCounter = 0;
    
    /* Simulate sensor readings */
    float temperature = 25.5f + (logCounter % 10) * 0.1f;
    float humidity = 60.0f + (logCounter % 20) * 0.5f;
    uint16_t pressure = 1013 + (logCounter % 5);
    
    /* Create CSV format log */
    UART_Printf("%lu,%.2f,%.2f,%u\r\n", 
                logCounter, temperature, humidity, pressure);
    
    logCounter++;
}

/**
 * @brief Complete data logging example
 */
void Example7_DataLogging(void)
{
    printf("\n=== Data Logging Example ===\n");
    
    /* Send CSV header */
    UART_Printf("Counter,Temperature(C),Humidity(%%),Pressure(hPa)\r\n");
    
    /* Log 10 samples */
    for (int i = 0; i < 10; i++) {
        Example7_LogData();
        
        /* Delay 1 second */
        for (volatile uint32_t d = 0; d < 1000000; d++);
    }
    
    UART_Printf("Logging complete\r\n");
}

/*******************************************************************************
 * Example 8: Error Handling
 ******************************************************************************/

/**
 * @brief Demonstrate error detection and handling
 */
void Example8_ErrorHandling(void)
{
    printf("\n=== Error Handling Example ===\n");
    
    /* Check for various errors */
    UART_Status_t error = UART_GetError(LPUART0);
    
    switch (error) {
        case UART_STATUS_SUCCESS:
            UART_Printf("No errors detected\r\n");
            break;
            
        case UART_STATUS_PARITY_ERROR:
            UART_Printf("Parity error detected!\r\n");
            break;
            
        case UART_STATUS_FRAME_ERROR:
            UART_Printf("Frame error - check baud rate!\r\n");
            break;
            
        case UART_STATUS_NOISE_ERROR:
            UART_Printf("Noise detected on line\r\n");
            break;
            
        case UART_STATUS_OVERRUN_ERROR:
            UART_Printf("Data overrun - processing too slow!\r\n");
            break;
            
        default:
            UART_Printf("Unknown error\r\n");
            break;
    }
    
    /* Get status flags */
    uint32_t status = UART_GetStatusFlags(LPUART0);
    UART_Printf("Status register: 0x%08X\r\n", status);
    
    /* Clear error flags */
    uint32_t errorFlags = LPUART_STAT_PF_MASK | 
                          LPUART_STAT_FE_MASK |
                          LPUART_STAT_NF_MASK |
                          LPUART_STAT_OR_MASK;
    UART_ClearStatusFlags(LPUART0, errorFlags);
}

/*******************************************************************************
 * Example 9: Loopback Test
 ******************************************************************************/

/**
 * @brief Test UART with loopback (connect TX to RX)
 */
void Example9_LoopbackTest(void)
{
    printf("\n=== Loopback Test ===\n");
    UART_Printf("Connect TX to RX for this test\r\n");
    
    uint8_t testData[] = {0x55, 0xAA, 0x12, 0x34, 0x56, 0x78};
    uint8_t rxData[6] = {0};
    bool testPassed = true;
    
    /* Send test data */
    for (int i = 0; i < 6; i++) {
        UART_SendByte(LPUART0, testData[i]);
        
        /* Small delay */
        for (volatile uint32_t d = 0; d < 1000; d++);
        
        /* Receive */
        if (UART_ReceiveByte(LPUART0, &rxData[i]) != UART_STATUS_SUCCESS) {
            testPassed = false;
            break;
        }
    }
    
    /* Verify */
    if (testPassed) {
        for (int i = 0; i < 6; i++) {
            if (testData[i] != rxData[i]) {
                testPassed = false;
                break;
            }
        }
    }
    
    if (testPassed) {
        UART_Printf("Loopback test PASSED\r\n");
    } else {
        UART_Printf("Loopback test FAILED\r\n");
    }
    
    /* Print results */
    UART_Printf("Sent:     ");
    for (int i = 0; i < 6; i++) {
        UART_Printf("0x%02X ", testData[i]);
    }
    UART_Printf("\r\nReceived: ");
    for (int i = 0; i < 6; i++) {
        UART_Printf("0x%02X ", rxData[i]);
    }
    UART_Printf("\r\n");
}

/*******************************************************************************
 * Main Function
 ******************************************************************************/

/**
 * @brief Main function - runs all examples
 */
void UART_ExamplesMain(void)
{
    /* Example 1: Initialize UART */
    Example1_BasicInit();
    
    /* Example 2: Send string */
    Example2_Complete();
    
    /* Example 3: Printf */
    Example3_PrintfDemo();
    
    /* Example 4: Receive data */
    Example4_ReceiveDemo();
    
    /* Example 5: Echo */
    Example5_SimpleEcho();
    
    /* Example 6: CLI */
    Example6_CLI_Demo();
    
    /* Example 7: Data logging */
    Example7_DataLogging();
    
    /* Example 8: Error handling */
    Example8_ErrorHandling();
    
    /* Example 9: Loopback test */
    Example9_LoopbackTest();
    
    UART_Printf("\n=== All UART examples completed! ===\n");
}
