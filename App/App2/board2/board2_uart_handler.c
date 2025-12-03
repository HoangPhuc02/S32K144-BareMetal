/**
 * @file board2_uart_handler.c
 * @brief Board 2 UART Communication Handler (Implementation)
 * @details UART transmission to PC
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#include "board2_uart_handler.h"
#include "app_config.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* Driver API includes */
// #include "uart.h"
// #include "pcc.h"
// #include "port.h"

/*******************************************************************************
 * PRIVATE VARIABLES
 ******************************************************************************/

/** UART handler state */
static struct {
    bool initialized;                       /**< Initialization flag */
    char tx_buffer[UART_TX_BUFFER_SIZE];    /**< TX buffer */
    uint16_t tx_buffer_length;              /**< TX buffer length */
    bool tx_busy;                           /**< TX busy flag */
} s_uartHandler = {
    .initialized = false,
    .tx_buffer_length = 0,
    .tx_busy = false
};

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/

/**
 * @brief Initialize UART peripheral
 * @return APP_STATUS_SUCCESS if successful
 */
static app_status_t Board2_UartHandler_InitPeripheral(void);

/**
 * @brief Send data via UART (blocking)
 * @param[in] data Pointer to data
 * @param[in] length Data length
 * @return APP_STATUS_SUCCESS if sent
 */
static app_status_t Board2_UartHandler_SendData(const char *data, uint16_t length);

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize UART handler
 */
app_status_t Board2_UartHandler_Init(void)
{
    app_status_t status;
    
    /* Check if already initialized */
    if (s_uartHandler.initialized) {
        return APP_STATUS_ALREADY_INITIALIZED;
    }
    
    /* Initialize UART peripheral */
    status = Board2_UartHandler_InitPeripheral();
    if (status != APP_STATUS_SUCCESS) {
        return status;
    }
    
    /* Initialize state */
    s_uartHandler.tx_buffer_length = 0;
    s_uartHandler.tx_busy = false;
    
    /* Mark as initialized */
    s_uartHandler.initialized = true;
    
    /* Send welcome message */
    Board2_UartHandler_SendString("\r\n=== Board 2 Ready ===\r\n");
    Board2_UartHandler_SendString("Press Button 1 to start ADC\r\n");
    Board2_UartHandler_SendString("Press Button 2 to stop ADC\r\n\r\n");
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Send ADC value via UART
 */
app_status_t Board2_UartHandler_SendAdcValue(uint16_t adc_value)
{
    char buffer[32];
    
    /* Check initialization */
    if (!s_uartHandler.initialized) {
        return APP_STATUS_NOT_INITIALIZED;
    }
    
    /* Format string */
    snprintf(buffer, sizeof(buffer), "ADC: %u\r\n", adc_value);
    
    /* Send via UART */
    return Board2_UartHandler_SendString(buffer);
}

/**
 * @brief Send string via UART
 */
app_status_t Board2_UartHandler_SendString(const char *str)
{
    /* Validate input */
    if (str == NULL) {
        return APP_STATUS_INVALID_PARAM;
    }
    
    /* Check initialization */
    if (!s_uartHandler.initialized) {
        return APP_STATUS_NOT_INITIALIZED;
    }
    
    /* Get length */
    uint16_t length = strlen(str);
    
    /* Send data */
    return Board2_UartHandler_SendData(str, length);
}

/**
 * @brief Send formatted string via UART
 */
app_status_t Board2_UartHandler_Printf(const char *format, ...)
{
    char buffer[UART_TX_BUFFER_SIZE];
    va_list args;
    int length;
    
    /* Validate input */
    if (format == NULL) {
        return APP_STATUS_INVALID_PARAM;
    }
    
    /* Check initialization */
    if (!s_uartHandler.initialized) {
        return APP_STATUS_NOT_INITIALIZED;
    }
    
    /* Format string */
    va_start(args, format);
    length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    /* Check length */
    if (length < 0 || length >= (int)sizeof(buffer)) {
        return APP_STATUS_ERROR;
    }
    
    /* Send data */
    return Board2_UartHandler_SendData(buffer, (uint16_t)length);
}

/**
 * @brief Process UART transmission
 */
void Board2_UartHandler_Process(void)
{
    /* Currently using blocking mode, so nothing to process */
    /* In interrupt-driven mode, this would handle TX buffer */
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize UART peripheral
 */
static app_status_t Board2_UartHandler_InitPeripheral(void)
{
    /* Enable PCC clock for UART */
    // PCC_EnableClock(PCC_LPUART1_INDEX);
    // PCC_SetClockSource(PCC_LPUART1_INDEX, PCC_CLK_SRC_SOSCDIV2);  /* 8MHz / 2 = 4MHz */
    
    /* Configure UART pins */
    // PCC_EnableClock(PCC_PORTC_INDEX);
    // PORT_SetPinMux(UART_TX_PORT, UART_TX_PIN, PORT_MUX_ALT2);  /* LPUART1_TX */
    // PORT_SetPinMux(UART_RX_PORT, UART_RX_PIN, PORT_MUX_ALT2);  /* LPUART1_RX */
    
    /* Initialize UART peripheral */
    // uart_config_t config;
    // config.baudrate = UART_BAUDRATE;
    // config.data_bits = UART_DATA_BITS_8;
    // config.parity = UART_PARITY_NONE;
    // config.stop_bits = UART_STOP_BITS_1;
    // UART_Init(UART_INSTANCE, &config);
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Send data via UART (blocking)
 */
static app_status_t Board2_UartHandler_SendData(const char *data, uint16_t length)
{
    /* Validate inputs */
    if (data == NULL || length == 0) {
        return APP_STATUS_INVALID_PARAM;
    }
    
    /* Check buffer size */
    if (length > UART_TX_BUFFER_SIZE) {
        length = UART_TX_BUFFER_SIZE;
    }
    
    /* Send via UART driver (blocking mode) */
    // for (uint16_t i = 0; i < length; i++) {
    //     /* Wait for TX ready */
    //     while (!UART_IsTxReady(UART_INSTANCE));
    //     
    //     /* Send byte */
    //     UART_SendByte(UART_INSTANCE, data[i]);
    // }
    
    /* Alternative: DMA or interrupt-driven mode */
    // if (UART_SendData(UART_INSTANCE, (const uint8_t *)data, length) != UART_STATUS_SUCCESS) {
    //     return APP_STATUS_ERROR;
    // }
    
    return APP_STATUS_SUCCESS;
}
