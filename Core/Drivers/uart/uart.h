/**
 * @file    uart.h
 * @brief   LPUART Driver API Header for S32K144
 * @details High-level UART driver interface providing functions for serial communication
 * 
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 24, 2025): Initial version
 */

#ifndef UART_H
#define UART_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "uart_reg.h"

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief UART parity mode
 */
typedef enum {
    UART_PARITY_DISABLED = 0U,      /**< No parity */
    UART_PARITY_EVEN     = 2U,      /**< Even parity */
    UART_PARITY_ODD      = 3U       /**< Odd parity */
} UART_Parity_t;

/**
 * @brief UART stop bits
 */
typedef enum {
    UART_ONE_STOP_BIT = 0U,         /**< One stop bit */
    UART_TWO_STOP_BIT = 1U          /**< Two stop bits */
} UART_StopBits_t;

/**
 * @brief UART data bits
 */
typedef enum {
    UART_8_DATA_BITS  = 0U,         /**< 8 data bits */
    UART_9_DATA_BITS  = 1U,         /**< 9 data bits */
    UART_10_DATA_BITS = 2U          /**< 10 data bits */
} UART_DataBits_t;

/**
 * @brief UART configuration structure
 */
typedef struct {
    uint32_t          baudRate;     /**< Baud rate (e.g., 9600, 115200) */
    UART_Parity_t     parity;       /**< Parity mode */
    UART_StopBits_t   stopBits;     /**< Number of stop bits */
    UART_DataBits_t   dataBits;     /**< Number of data bits */
    bool              enableTx;     /**< Enable transmitter */
    bool              enableRx;     /**< Enable receiver */
} UART_Config_t;

/**
 * @brief UART transfer structure
 */
typedef struct {
    const uint8_t *txData;          /**< Pointer to transmit data */
    uint8_t       *rxData;          /**< Pointer to receive data */
    uint32_t       dataSize;        /**< Size of data transfer */
} UART_Transfer_t;

/**
 * @brief UART status codes
 */
typedef enum {
    UART_STATUS_SUCCESS = 0U,       /**< Operation successful */
    UART_STATUS_BUSY,               /**< UART is busy */
    UART_STATUS_TIMEOUT,            /**< Operation timeout */
    UART_STATUS_PARITY_ERROR,       /**< Parity error detected */
    UART_STATUS_FRAME_ERROR,        /**< Framing error detected */
    UART_STATUS_NOISE_ERROR,        /**< Noise error detected */
    UART_STATUS_OVERRUN_ERROR,      /**< Overrun error detected */
    UART_STATUS_ERROR               /**< General error */
} UART_Status_t;

/*******************************************************************************
 * Function Prototypes - Initialization
 ******************************************************************************/

/**
 * @brief Initialize UART peripheral
 * 
 * @param[in] base      Pointer to UART peripheral base address
 * @param[in] config    Pointer to configuration structure
 * @param[in] srcClock  Source clock frequency in Hz
 * 
 * @return UART_STATUS_SUCCESS if successful
 */
UART_Status_t UART_Init(LPUART_RegType *base, const UART_Config_t *config, uint32_t srcClock);

/**
 * @brief Deinitialize UART peripheral
 * 
 * @param[in] base  Pointer to UART peripheral base address
 */
void UART_Deinit(LPUART_RegType *base);

/**
 * @brief Get default configuration
 * 
 * @param[out] config  Pointer to configuration structure to fill
 */
void UART_GetDefaultConfig(UART_Config_t *config);

/*******************************************************************************
 * Function Prototypes - Blocking Transfer
 ******************************************************************************/

/**
 * @brief Send data in blocking mode
 * 
 * @param[in] base      Pointer to UART peripheral base address
 * @param[in] txBuff    Pointer to transmit buffer
 * @param[in] txSize    Number of bytes to send
 * 
 * @return UART_STATUS_SUCCESS if successful
 */
UART_Status_t UART_SendBlocking(LPUART_RegType *base, const uint8_t *txBuff, uint32_t txSize);

/**
 * @brief Receive data in blocking mode
 * 
 * @param[in]  base    Pointer to UART peripheral base address
 * @param[out] rxBuff  Pointer to receive buffer
 * @param[in]  rxSize  Number of bytes to receive
 * 
 * @return UART_STATUS_SUCCESS if successful
 */
UART_Status_t UART_ReceiveBlocking(LPUART_RegType *base, uint8_t *rxBuff, uint32_t rxSize);

/**
 * @brief Send a single byte
 * 
 * @param[in] base  Pointer to UART peripheral base address
 * @param[in] data  Data byte to send
 * 
 * @return UART_STATUS_SUCCESS if successful
 */
UART_Status_t UART_SendByte(LPUART_RegType *base, uint8_t data);

/**
 * @brief Receive a single byte
 * 
 * @param[in]  base  Pointer to UART peripheral base address
 * @param[out] data  Pointer to store received byte
 * 
 * @return UART_STATUS_SUCCESS if successful
 */
UART_Status_t UART_ReceiveByte(LPUART_RegType *base, uint8_t *data);

/*******************************************************************************
 * Function Prototypes - Status and Control
 ******************************************************************************/

/**
 * @brief Get UART status flags
 * 
 * @param[in] base  Pointer to UART peripheral base address
 * 
 * @return Status register value
 */
uint32_t UART_GetStatusFlags(LPUART_RegType *base);

/**
 * @brief Clear UART status flags
 * 
 * @param[in] base   Pointer to UART peripheral base address
 * @param[in] flags  Flags to clear
 */
void UART_ClearStatusFlags(LPUART_RegType *base, uint32_t flags);

/**
 * @brief Check if transmitter is ready
 * 
 * @param[in] base  Pointer to UART peripheral base address
 * 
 * @return true if ready to transmit, false otherwise
 */
bool UART_IsTxReady(LPUART_RegType *base);

/**
 * @brief Check if receiver has data
 * 
 * @param[in] base  Pointer to UART peripheral base address
 * 
 * @return true if data available, false otherwise
 */
bool UART_IsRxReady(LPUART_RegType *base);

/**
 * @brief Check for errors
 * 
 * @param[in] base  Pointer to UART peripheral base address
 * 
 * @return Error status
 */
UART_Status_t UART_GetError(LPUART_RegType *base);

/*******************************************************************************
 * Function Prototypes - Interrupt Control
 ******************************************************************************/

/**
 * @brief Enable UART interrupts
 * 
 * @param[in] base      Pointer to UART peripheral base address
 * @param[in] intMask   Interrupt mask (can be ORed together)
 */
void UART_EnableInterrupts(LPUART_RegType *base, uint32_t intMask);

/**
 * @brief Disable UART interrupts
 * 
 * @param[in] base      Pointer to UART peripheral base address
 * @param[in] intMask   Interrupt mask (can be ORed together)
 */
void UART_DisableInterrupts(LPUART_RegType *base, uint32_t intMask);

/**
 * @brief Get enabled interrupts
 * 
 * @param[in] base  Pointer to UART peripheral base address
 * 
 * @return Enabled interrupt mask
 */
uint32_t UART_GetEnabledInterrupts(LPUART_RegType *base);

/*******************************************************************************
 * Function Prototypes - Clock Control
 ******************************************************************************/

/**
 * @brief Enable UART clock
 * 
 * @param[in] instance  UART instance number (0, 1, 2)
 */
void UART_EnableClock(uint8_t instance);

/**
 * @brief Disable UART clock
 * 
 * @param[in] instance  UART instance number (0, 1, 2)
 */
void UART_DisableClock(uint8_t instance);

/*******************************************************************************
 * Function Prototypes - DMA Transfer
 ******************************************************************************/

/**
 * @brief Configure UART for DMA transmission
 * 
 * @param[in] base          Pointer to UART peripheral base address
 * @param[in] dmaChannel    DMA channel number for TX (0-15)
 * 
 * @return UART_STATUS_SUCCESS if successful
 * 
 * @note This function enables UART TX DMA request but does not start transfer.
 *       Use UART_SendDMA() to initiate the actual transfer.
 */
UART_Status_t UART_ConfigTxDMA(LPUART_RegType *base, uint8_t dmaChannel);

/**
 * @brief Configure UART for DMA reception
 * 
 * @param[in] base          Pointer to UART peripheral base address
 * @param[in] dmaChannel    DMA channel number for RX (0-15)
 * 
 * @return UART_STATUS_SUCCESS if successful
 * 
 * @note This function enables UART RX DMA request but does not start transfer.
 *       Use UART_ReceiveDMA() to initiate the actual transfer.
 */
UART_Status_t UART_ConfigRxDMA(LPUART_RegType *base, uint8_t dmaChannel);

/**
 * @brief Send data using DMA
 * 
 * @param[in] base          Pointer to UART peripheral base address
 * @param[in] dmaChannel    DMA channel number for TX (0-15)
 * @param[in] txBuff        Pointer to transmit buffer
 * @param[in] txSize        Number of bytes to send
 * 
 * @return UART_STATUS_SUCCESS if transfer started successfully
 * 
 * @note This is a non-blocking function. The transfer will complete in the background.
 *       Use DMA_IsChannelDone() or register a DMA callback to check completion.
 * 
 * @code
 * // Example usage
 * uint8_t txData[] = "Hello DMA!";
 * UART_ConfigTxDMA(LPUART1, 0);
 * UART_SendDMA(LPUART1, 0, txData, sizeof(txData)-1);
 * // Wait for completion
 * while (!DMA_IsChannelDone(0));
 * @endcode
 */
UART_Status_t UART_SendDMA(LPUART_RegType *base, uint8_t dmaChannel, 
                           const uint8_t *txBuff, uint32_t txSize);

/**
 * @brief Receive data using DMA
 * 
 * @param[in]  base         Pointer to UART peripheral base address
 * @param[in]  dmaChannel   DMA channel number for RX (0-15)
 * @param[out] rxBuff       Pointer to receive buffer
 * @param[in]  rxSize       Number of bytes to receive
 * 
 * @return UART_STATUS_SUCCESS if transfer started successfully
 * 
 * @note This is a non-blocking function. The transfer will complete in the background.
 *       Use DMA_IsChannelDone() or register a DMA callback to check completion.
 * 
 * @code
 * // Example usage
 * uint8_t rxData[100];
 * UART_ConfigRxDMA(LPUART1, 1);
 * UART_ReceiveDMA(LPUART1, 1, rxData, sizeof(rxData));
 * // Wait for completion
 * while (!DMA_IsChannelDone(1));
 * @endcode
 */
UART_Status_t UART_ReceiveDMA(LPUART_RegType *base, uint8_t dmaChannel,
                              uint8_t *rxBuff, uint32_t rxSize);

/**
 * @brief Send data using DMA with blocking wait
 * 
 * @param[in] base          Pointer to UART peripheral base address
 * @param[in] dmaChannel    DMA channel number for TX (0-15)
 * @param[in] txBuff        Pointer to transmit buffer
 * @param[in] txSize        Number of bytes to send
 * 
 * @return UART_STATUS_SUCCESS if transfer completed successfully
 * 
 * @note This function blocks until the transfer is complete or timeout occurs.
 */
UART_Status_t UART_SendDMABlocking(LPUART_RegType *base, uint8_t dmaChannel,
                                   const uint8_t *txBuff, uint32_t txSize);

/**
 * @brief Receive data using DMA with blocking wait
 * 
 * @param[in]  base         Pointer to UART peripheral base address
 * @param[in]  dmaChannel   DMA channel number for RX (0-15)
 * @param[out] rxBuff       Pointer to receive buffer
 * @param[in]  rxSize       Number of bytes to receive
 * 
 * @return UART_STATUS_SUCCESS if transfer completed successfully
 * 
 * @note This function blocks until the transfer is complete or timeout occurs.
 */
UART_Status_t UART_ReceiveDMABlocking(LPUART_RegType *base, uint8_t dmaChannel,
                                      uint8_t *rxBuff, uint32_t rxSize);

/**
 * @brief Disable UART TX DMA
 * 
 * @param[in] base  Pointer to UART peripheral base address
 */
void UART_DisableTxDMA(LPUART_RegType *base);

/**
 * @brief Disable UART RX DMA
 * 
 * @param[in] base  Pointer to UART peripheral base address
 */
void UART_DisableRxDMA(LPUART_RegType *base);

/*******************************************************************************
 * Interrupt Masks
 ******************************************************************************/

/** Transmit interrupt */
#define UART_INT_TX_READY           LPUART_CTRL_TIE_MASK

/** Receive interrupt */
#define UART_INT_RX_READY           LPUART_CTRL_RIE_MASK

/** Transmission complete interrupt */
#define UART_INT_TX_COMPLETE        LPUART_CTRL_TCIE_MASK

/** Idle line interrupt */
#define UART_INT_IDLE               LPUART_CTRL_ILIE_MASK

/** Parity error interrupt */
#define UART_INT_PARITY_ERROR       LPUART_CTRL_PEIE_MASK

/** Framing error interrupt */
#define UART_INT_FRAME_ERROR        LPUART_CTRL_FEIE_MASK

/** Noise error interrupt */
#define UART_INT_NOISE_ERROR        LPUART_CTRL_NEIE_MASK

/** Overrun error interrupt */
#define UART_INT_OVERRUN_ERROR      LPUART_CTRL_ORIE_MASK

#endif /* UART_H */
