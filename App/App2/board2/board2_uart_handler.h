/**
 * @file board2_uart_handler.h
 * @brief Board 2 UART Communication Handler (Header)
 * @details Handles UART transmission to PC
 * 
 * Responsibilities:
 * - Initialize UART peripheral (9600bps)
 * - Format and send ADC data to PC
 * - Send status messages
 * 
 * UART Format:
 * - ADC value: "ADC: 1234\r\n"
 * - Status: "ADC sampling started\r\n"
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#ifndef BOARD2_UART_HANDLER_H
#define BOARD2_UART_HANDLER_H

#include "app_types.h"

/*******************************************************************************
 * API FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize UART handler
 * 
 * Initializes:
 * - UART peripheral (9600bps, 8N1)
 * - TX buffer
 * - Pin configuration
 * 
 * @return APP_STATUS_SUCCESS if successful
 * @return APP_STATUS_ERROR if initialization failed
 */
app_status_t Board2_UartHandler_Init(void);

/**
 * @brief Send ADC value via UART
 * 
 * Formats and sends: "ADC: 1234\r\n"
 * 
 * @param[in] adc_value ADC value (0-4095)
 * 
 * @return APP_STATUS_SUCCESS if sent
 * @return APP_STATUS_ERROR if transmission failed
 */
app_status_t Board2_UartHandler_SendAdcValue(uint16_t adc_value);

/**
 * @brief Send string via UART
 * 
 * @param[in] str Null-terminated string
 * 
 * @return APP_STATUS_SUCCESS if sent
 * @return APP_STATUS_INVALID_PARAM if str is NULL
 * @return APP_STATUS_ERROR if transmission failed
 */
app_status_t Board2_UartHandler_SendString(const char *str);

/**
 * @brief Send formatted string via UART
 * 
 * @param[in] format Printf-style format string
 * @param[in] ... Variable arguments
 * 
 * @return APP_STATUS_SUCCESS if sent
 * @return APP_STATUS_ERROR if transmission failed
 * 
 * @note Maximum length: UART_TX_BUFFER_SIZE
 */
app_status_t Board2_UartHandler_Printf(const char *format, ...);

/**
 * @brief Process UART transmission
 * 
 * Should be called from main loop
 * 
 * Actions:
 * - Check TX buffer
 * - Send pending data
 */
void Board2_UartHandler_Process(void);

#endif /* BOARD2_UART_HANDLER_H */
