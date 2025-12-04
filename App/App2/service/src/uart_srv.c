/**
 * @file    uart_srv.c
 * @brief   UART Service Layer Implementation
 * @details Implementation của UART service layer, wrapper cho UART driver
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../inc/uart_srv.h"
#include "../../../../Core/BareMetal/uart/UART.h"
#include <string.h>

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static bool s_uart_initialized = false;
static LPUART_Type *s_uart_instance = NULL;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

uart_srv_status_t UART_SRV_Init(uint32_t baudrate)
{
    /* Default configuration cho LPUART1 on PORTC pins 6,7 */
    /* OSR = 16, calculate SBR based on baudrate */
    /* Assuming 8MHz clock, SBR = 8000000 / (baudrate * 16) */
    uint16_t sbr = (uint16_t)(8000000U / (baudrate * 16U));
    
    LPUART_init(
        LPUART1,              /* LPUART instance */
        PORTC,                /* PORT for TX/RX */
        9,                    /* PCC_PORTC_INDEX */
        6,                    /* TX pin */
        7,                    /* RX pin */
        2,                    /* ALT2 mux */
        6,                    /* PCS = SPLLDIV2 */
        15,                   /* OSR = 16-1 */
        sbr,                  /* SBR value */
        UART_PARITY_NONE      /* No parity */
    );
    
    s_uart_instance = LPUART1;
    s_uart_initialized = true;
    
    return UART_SRV_SUCCESS;
}

uart_srv_status_t UART_SRV_SendByte(uint8_t data)
{
    if (!s_uart_initialized) {
        return UART_SRV_NOT_INITIALIZED;
    }
    
    LPUART_transmit_char(s_uart_instance, data);
    
    return UART_SRV_SUCCESS;
}

uart_srv_status_t UART_SRV_SendString(const char *str)
{
    if (!s_uart_initialized) {
        return UART_SRV_NOT_INITIALIZED;
    }
    
    if (str == NULL) {
        return UART_SRV_ERROR;
    }
    
    while (*str != '\0') {
        LPUART_transmit_char(s_uart_instance, (uint8_t)*str);
        str++;
    }
    
    return UART_SRV_SUCCESS;
}
