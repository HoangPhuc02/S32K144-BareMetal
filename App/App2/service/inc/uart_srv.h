/**
 * @file    uart_srv.h
 * @brief   UART Service Layer - Abstraction API
 * @details
 * Service layer cung cấp interface abstraction cho UART communication.
 * Che giấu chi tiết implementation và cung cấp API dễ sử dụng cho application.
 * 
 * Features:
 * - UART initialization với baudrate
 * - Send byte operation
 * - Send string operation
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

#ifndef UART_SRV_H
#define UART_SRV_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief UART service status codes
 */
typedef enum {
    UART_SRV_SUCCESS = 0,
    UART_SRV_ERROR,
    UART_SRV_NOT_INITIALIZED
} uart_srv_status_t;

/*******************************************************************************
 * API Function Declarations
 ******************************************************************************/

/**
 * @brief Initialize UART service
 * @param baudrate Baudrate in bps (9600, 115200, etc.)
 * @return uart_srv_status_t Status of initialization
 */
uart_srv_status_t UART_SRV_Init(uint32_t baudrate);

/**
 * @brief Send single byte
 * @param data Byte to send
 * @return uart_srv_status_t Status of operation
 */
uart_srv_status_t UART_SRV_SendByte(uint8_t data);

/**
 * @brief Send string (null-terminated)
 * @param str String to send
 * @return uart_srv_status_t Status of operation
 */
uart_srv_status_t UART_SRV_SendString(const char *str);

#endif /* UART_SRV_H */
