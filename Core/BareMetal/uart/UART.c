
/**
*   @file    UART.c
*   @brief   LPUART Driver - Implementation
*   @details Provides initialization and basic transmit/receive functions for LPUART module.
*
*   @author  Group2
*   @date    2025-12-02
*/

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "UART.h"
#include "Pcc_Registers.h"
#include "Port_Registers.h"
/* Include device register definitions where LPUART_Type, PORT_Type,
   and bit field macros (LPUART_CTRL_*, LPUART_STAT_*, LPUART_BAUD_*, PCC_*) are defined. */
#include <string.h>
#include <stdio.h>

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* None */

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/
/* None */

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/
/* None */

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/
/* None */

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/
/* None */

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/
/* None */

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
/* None */

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/
/* None */

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
void LPUART_init(LPUART_Type *LPUARTx,
                 PORT_Type *PORTx,
                 uint8_t portIndex,
                 uint8_t TX,
                 uint8_t RX,
                 uint8_t muxValue,
                 uint8_t pcsValue,
                 uint8_t osrValue,
                 uint16_t sbrValue,
                 uart_parity_t parity)
{
    /* Enable clock for selected PORT */
    PCC->PCCn[portIndex] |= PCC_PCCn_CGC_MASK;

    /* Configure TX/RX pin multiplexing */
    PORTx->PCR[TX] = (PORTx->PCR[TX] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(muxValue);
    PORTx->PCR[RX] = (PORTx->PCR[RX] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(muxValue);

    /* Find PCC index for UART instance */
    uint32_t uart_pcc_index = 0u;

    if (LPUARTx == LPUART0)
    {
        uart_pcc_index = PCC_LPUART0_INDEX;
    }
    else if (LPUARTx == LPUART1)
    {
        uart_pcc_index = PCC_LPUART1_INDEX;
    }
    else /* LPUART2 */
    {
        uart_pcc_index = PCC_LPUART2_INDEX;
    }

    /* Reset and enable LPUART clock */
    PCC->PCCn[uart_pcc_index] &= ~PCC_PCCn_CGC_MASK;
    PCC->PCCn[uart_pcc_index]  = PCC_PCCn_PCS(pcsValue) | PCC_PCCn_CGC_MASK;

    /* Disable TX/RX before configuration */
    LPUARTx->CTRL &= ~(LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);

    /* Configure Baud Rate: OSR + SBR, 1 stop bit */
    LPUARTx->BAUD  = LPUART_BAUD_OSR(osrValue) | LPUART_BAUD_SBR(sbrValue);
    LPUARTx->BAUD &= ~LPUART_BAUD_SBNS_MASK; /* 1 stop bit */

    /* Default to 8-bit mode (M = 0). Note:
       - 7-bit mode is controlled by BAUD.M7 (not set here).
       - Parity requires PE=1; PT selects even/odd.
       - Depending on device, M and PE interaction defines data bit count.
       Keep M clear and only set PE/PT for parity per your hardware requirements. */
    LPUARTx->CTRL &= ~LPUART_CTRL_M_MASK;

    /* Enable RX interrupt (RIE) if using ISR to handle receive */
    LPUARTx->CTRL |= LPUART_CTRL_RIE_MASK;

    /* Configure Parity */
    switch (parity)
    {
        case UART_PARITY_NONE:
        {
            LPUARTx->CTRL &= ~LPUART_CTRL_PE_MASK; /* Parity disabled */
            LPUARTx->CTRL &= ~LPUART_CTRL_PT_MASK; /* Don't care */
        }
        break;

        case UART_PARITY_EVEN:
        {
            LPUARTx->CTRL &= ~LPUART_CTRL_PT_MASK; /* Even parity */
            LPUARTx->CTRL |=  LPUART_CTRL_PE_MASK; /* Parity enable  */
            LPUARTx->CTRL |=  LPUART_CTRL_M_MASK;  /* M=1 when parity is enabled */
        }
        break;

        case UART_PARITY_ODD:
        {
            LPUARTx->CTRL |=  LPUART_CTRL_PT_MASK; /* Odd parity  */
            LPUARTx->CTRL |=  LPUART_CTRL_PE_MASK; /* Parity enable */
            LPUARTx->CTRL |=  LPUART_CTRL_M_MASK;  /* M=1 when parity is enabled */
        }
        break;

        default:
        /* No action */
        break;
    }

    /* Enable Transmit and Receive */
    LPUARTx->CTRL |= (LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);
}

void LPUART_transmit_char(LPUART_Type *LPUARTx, uint8_t ch)
{
    /* Wait until transmit data register empty (TDRE = 1) */
    while ((LPUARTx->STAT & LPUART_STAT_TDRE_MASK) == 0u) { /* busy wait */ }
    /* Writing DATA clears TDRE */
    LPUARTx->DATA = ch;
}

void LPUART_transmit_buffer(LPUART_Type *LPUARTx, const uint8_t *buf, size_t len)
{
    for (size_t i = 0u; i < len; ++i)
    {
        LPUART_transmit_char(LPUARTx, buf[i]);
    }
}

void LPUART_transmit_string(LPUART_Type *LPUARTx, const char *str)
{
    /* Transmit until null terminator */
    while (*str != '\0')
    {
        LPUART_transmit_char(LPUARTx, (uint8_t)*str);
        ++str;
    }
}

uint8_t LPUART_receive_char(LPUART_Type *LPUARTx)
{
    /* Wait until receive data register full (RDRF = 1).
       Reading DATA will clear RDRF. */
    while ((LPUARTx->STAT & LPUART_STAT_RDRF_MASK) == 0u) { /* busy wait */ }
    return (uint8_t)(LPUARTx->DATA);
}

void LPUART_receive_and_echo_char(LPUART_Type *LPUARTx)
{
    uint8_t ch = LPUART_receive_char(LPUARTx);
    LPUART_transmit_char(LPUARTx, ch);
    /* Append CRLF for line break on terminals */
    LPUART_transmit_char(LPUARTx, (uint8_t)'\r');
    LPUART_transmit_char(LPUARTx, (uint8_t)'\n');
}
