
/**
*   @file    UART.h
*   @brief   LPUART Driver - API
*   @details Declarations for LPUART initialization and basic TX/RX operations.
*
*   @author  Group
*   @date    2025-12-02
*/

#ifndef UART_H_
#define UART_H_

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include <stdint.h>
#include <stddef.h>   /* for size_t */
#include "UART_Registers.h"
#include "Port_Registers.h"

/*==================================================================================================
*                              TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/**
 * @brief  UART parity configuration.
 */

typedef enum
{
    UART_PARITY_NONE = 0u,
    UART_PARITY_EVEN,
    UART_PARITY_ODD
} uart_parity_t;

typedef struct {
		 LPUART_Type *LPUARTx;
		 PORT_Type *portbase;
		 uint8_t portIndex;
		 uint8_t TX; uint8_t RX;
		 uint8_t muxValue;
		 uint8_t pcsValue;
		 uint8_t osrValue;
		 uint16_t sbrValue;
		 uart_parity_t parity;
} Uart_ConfigType;


/*==================================================================================================
*                                     GLOBAL CONSTANTS
==================================================================================================*/
/* None */

/*==================================================================================================
*                                     GLOBAL VARIABLES
==================================================================================================*/
/* None */

/*==================================================================================================
*                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
/**
* @brief        Initialize LPUART module with pins, clock, baud and parity.
* @details      Configures PORT mux for TX/RX, enables PCC clock for LPUART,
*               sets OSR/SBR for baud rate, stop bits = 1, parity per parameter,
*               and enables TX/RX. Also enables RX interrupt (RIE).
*
* @param[in]    LPUARTx    Pointer to LPUART instance (LPUART0/LPUART1/LPUART2).
* @param[in]    PORTx      Pointer to PORT instance used for TX/RX pins.
* @param[in]    portIndex  PCC index for PORT clock gate.
* @param[in]    TX         TX pin index in PORTx.
* @param[in]    RX         RX pin index in PORTx.
* @param[in]    muxValue   PORT mux value for the pins (e.g., ALT2/ALT3).
* @param[in]    pcsValue   PCC_PCS value (clock source selection).
* @param[in]    osrValue   Oversampling ratio value for BAUD.OSR (typically 8–32).
* @param[in]    sbrValue   Baud rate divisor for BAUD.SBR.
* @param[in]    parity     Parity configuration (NONE/EVEN/ODD).
*
* @return       void
*
* @pre          System clocks configured; pins valid for the selected instance.
* @post         LPUART ready; RX interrupt enabled.
*/
void LPUART_init(LPUART_Type *LPUARTx,
                 PORT_Type *PORTx,
                 uint8_t portIndex,
                 uint8_t TX,
                 uint8_t RX,
                 uint8_t muxValue,
                 uint8_t pcsValue,
                 uint8_t osrValue,
                 uint16_t sbrValue,
                 uart_parity_t parity);

/**
* @brief   Transmit a single byte (blocking).
* @param   LPUARTx   LPUART instance pointer.
* @param   ch        Byte to send.
*/
void LPUART_transmit_char(LPUART_Type *LPUARTx, uint8_t ch);

/**
* @brief   Transmit a buffer of bytes (blocking).
* @param   LPUARTx   LPUART instance pointer.
* @param   buf       Pointer to data buffer (binary-safe).
* @param   len       Number of bytes to transmit.
*/
void LPUART_transmit_buffer(LPUART_Type *LPUARTx, const uint8_t *buf, size_t len);

/**
* @brief   Transmit a C-string (null-terminated) (blocking).
* @param   LPUARTx   LPUART instance pointer.
* @param   str       Pointer to null-terminated ASCII string.
*
* @note    This API is for text. For binary, use LPUART_transmit_buffer().
*/
void LPUART_transmit_string(LPUART_Type *LPUARTx, const char *str);

/**
* @brief   Receive a single byte (blocking).
* @param   LPUARTx   LPUART instance pointer.
* @return  Received byte.
*/
uint8_t LPUART_receive_char(LPUART_Type *LPUARTx);

/**
* @brief   Receive one byte and echo it back with CRLF.
* @param   LPUARTx   LPUART instance pointer.
*/
void LPUART_receive_and_echo_char(LPUART_Type *LPUARTx);

#endif /* LPUART_H_ */

