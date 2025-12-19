/**
 * @file    uart.c
 * @brief   LPUART Driver Implementation for S32K144
 * @details Implementation of UART driver functions for serial communication
 * 
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "uart.h"
#include "uart_reg.h"
#include "pcc_reg.h"
#if UART_DMA_ENABLE
#include "dma.h"
#endif

/*******************************************************************************
 * Private Definitions
 ******************************************************************************/

/* Timeout counter for blocking operations */
#define UART_TIMEOUT_COUNT      (100000U)

/* Default oversampling ratio */
#define UART_DEFAULT_OSR        (15U)

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Calculate baud rate register value
 */
static void UART_CalculateBaudRate(uint32_t srcClock, uint32_t baudRate, 
                                   uint32_t *sbr, uint32_t *osr)
{
    uint32_t tempOsr, tempSbr;
    uint32_t calculatedBaud, tempDiff, minDiff = 0xFFFFFFFFU;
    
    /* Try different OSR values to find best match */
    for (tempOsr = 4U; tempOsr <= 32U; tempOsr++) {
        tempSbr = (srcClock / (baudRate * tempOsr));
        
        if ((tempSbr >= 1U) && (tempSbr <= 8191U)) {
            calculatedBaud = srcClock / (tempOsr * tempSbr);
            
            /* Calculate difference */
            if (calculatedBaud >= baudRate) {
                tempDiff = calculatedBaud - baudRate;
            } else {
                tempDiff = baudRate - calculatedBaud;
            }
            
            /* Check if this is better */
            if (tempDiff < minDiff) {
                minDiff = tempDiff;
                *sbr = tempSbr;
                *osr = tempOsr - 1U;  /* OSR field is (actual OSR - 1) */
            }
        }
    }
}

/*******************************************************************************
 * Initialization Functions
 ******************************************************************************/

/**
 * @brief Initialize UART peripheral
 */
UART_Status_t UART_Init(LPUART_RegType *base, const UART_Config_t *config, uint32_t srcClock)
{
    uint32_t sbr = 0U, osr = UART_DEFAULT_OSR;
    uint32_t baudReg = 0U;
    uint32_t ctrlReg = 0U;

    if ((base == NULL) || (config == NULL)) {
        return UART_STATUS_ERROR;
    }

    /* Disable transmitter and receiver before configuration */
    LPUART_DISABLE_TX(base);
    LPUART_DISABLE_RX(base);

    /* Software reset */
    LPUART_SW_RESET(base);
    base->GLOBAL &= ~LPUART_GLOBAL_RST_MASK;

    /* Calculate baud rate parameters */
    UART_CalculateBaudRate(srcClock, config->baudRate, &sbr, &osr);

    /* Configure BAUD register */
    baudReg = LPUART_BAUD_SBR(sbr) | LPUART_BAUD_OSR(osr);
    
    /* Configure stop bits */
    if (config->stopBits == UART_TWO_STOP_BIT) {
        baudReg |= LPUART_BAUD_SBNS_MASK;
    }
    
    base->BAUD = baudReg;

    /* Configure CTRL register */
    ctrlReg = 0U;
    
    /* Configure parity */
    if (config->parity != UART_PARITY_DISABLED) {
        ctrlReg |= LPUART_CTRL_PE_MASK;
        if (config->parity == UART_PARITY_ODD) {
            ctrlReg |= LPUART_CTRL_PT_MASK;
        }
    }
    
    /* Configure data bits */
    if (config->dataBits == UART_9_DATA_BITS) {
        ctrlReg |= LPUART_CTRL_M_MASK;
    } else if (config->dataBits == UART_10_DATA_BITS) {
        ctrlReg |= LPUART_CTRL_M_MASK;
        baudReg |= LPUART_BAUD_M10_MASK;
        base->BAUD = baudReg;
    }
    
    base->CTRL = ctrlReg;

    /* Enable transmitter and receiver if configured */
    if (config->enableTx) {
        LPUART_ENABLE_TX(base);
    }
    
    if (config->enableRx) {
        LPUART_ENABLE_RX(base);
    }

    return UART_STATUS_SUCCESS;
}

/**
 * @brief Deinitialize UART peripheral
 */
void UART_Deinit(LPUART_RegType *base)
{
    if (base != NULL) {
        /* Disable transmitter and receiver */
        LPUART_DISABLE_TX(base);
        LPUART_DISABLE_RX(base);
        
        /* Software reset */
        LPUART_SW_RESET(base);
    }
}

/**
 * @brief Get default configuration
 */
void UART_GetDefaultConfig(UART_Config_t *config)
{
    if (config != NULL) {
        config->baudRate  = 115200U;
        config->parity    = UART_PARITY_DISABLED;
        config->stopBits  = UART_ONE_STOP_BIT;
        config->dataBits  = UART_8_DATA_BITS;
        config->enableTx  = true;
        config->enableRx  = true;
    }
}

/*******************************************************************************
 * Blocking Transfer Functions
 ******************************************************************************/

/**
 * @brief Send data in blocking mode
 */
UART_Status_t UART_SendBlocking(LPUART_RegType *base, const uint8_t *txBuff, uint32_t txSize)
{
    uint32_t i;
    uint32_t timeout;

    if ((base == NULL) || (txBuff == NULL) || (txSize == 0U)) {
        return UART_STATUS_ERROR;
    }

    for (i = 0U; i < txSize; i++) {
        timeout = UART_TIMEOUT_COUNT;

        /* Wait for transmit data register empty */
        while (!LPUART_IS_TX_READY(base) && (timeout > 0U)) {
            timeout--;
        }

        if (timeout == 0U) {
            return UART_STATUS_TIMEOUT;
        }

        /* Write data */
        LPUART_WRITE_DATA(base, txBuff[i]);
    }

    /* Wait for transmission complete */
    timeout = UART_TIMEOUT_COUNT;
    while (!LPUART_IS_TX_COMPLETE(base) && (timeout > 0U)) {
        timeout--;
    }

    return (timeout > 0U) ? UART_STATUS_SUCCESS : UART_STATUS_TIMEOUT;
}

/**
 * @brief Receive data in blocking mode
 */
UART_Status_t UART_ReceiveBlocking(LPUART_RegType *base, uint8_t *rxBuff, uint32_t rxSize)
{
    uint32_t i;
    uint32_t timeout;
    UART_Status_t errorStatus;

    if ((base == NULL) || (rxBuff == NULL) || (rxSize == 0U)) {
        return UART_STATUS_ERROR;
    }

    for (i = 0U; i < rxSize; i++) {
        timeout = UART_TIMEOUT_COUNT;

        /* Wait for receive data ready */
        while (!LPUART_IS_RX_READY(base) && (timeout > 0U)) {
            timeout--;
        }

        if (timeout == 0U) {
            return UART_STATUS_TIMEOUT;
        }

        /* Check for errors */
        errorStatus = UART_GetError(base);
        if (errorStatus != UART_STATUS_SUCCESS) {
            return errorStatus;
        }

        /* Read data */
        rxBuff[i] = (uint8_t)LPUART_READ_DATA(base);
    }

    return UART_STATUS_SUCCESS;
}

/**
 * @brief Send a single byte
 */
UART_Status_t UART_SendByte(LPUART_RegType *base, uint8_t data)
{
    uint32_t timeout = UART_TIMEOUT_COUNT;

    if (base == NULL) {
        return UART_STATUS_ERROR;
    }

    /* Wait for transmit data register empty */
    while (!LPUART_IS_TX_READY(base) && (timeout > 0U)) {
        timeout--;
    }

    if (timeout == 0U) {
        return UART_STATUS_TIMEOUT;
    }

    /* Write data */
    LPUART_WRITE_DATA(base, data);

    return UART_STATUS_SUCCESS;
}

/**
 * @brief Receive a single byte
 */
UART_Status_t UART_ReceiveByte(LPUART_RegType *base, uint8_t *data)
{
    uint32_t timeout = UART_TIMEOUT_COUNT;
    UART_Status_t errorStatus;

    if ((base == NULL) || (data == NULL)) {
        return UART_STATUS_ERROR;
    }

    /* Wait for receive data ready */
    while (!LPUART_IS_RX_READY(base) && (timeout > 0U)) {
        timeout--;
    }

    if (timeout == 0U) {
        return UART_STATUS_TIMEOUT;
    }

    /* Check for errors */
    errorStatus = UART_GetError(base);
    if (errorStatus != UART_STATUS_SUCCESS) {
        return errorStatus;
    }

    /* Read data */
    *data = (uint8_t)LPUART_READ_DATA(base);

    return UART_STATUS_SUCCESS;
}

/*******************************************************************************
 * Status and Control Functions
 ******************************************************************************/

/**
 * @brief Get UART status flags
 */
uint32_t UART_GetStatusFlags(LPUART_RegType *base)
{
    return (base != NULL) ? base->STAT : 0U;
}

/**
 * @brief Clear UART status flags
 */
void UART_ClearStatusFlags(LPUART_RegType *base, uint32_t flags)
{
    if (base != NULL) {
        LPUART_CLEAR_STATUS_FLAG(base, flags);
    }
}

/**
 * @brief Check if transmitter is ready
 */
bool UART_IsTxReady(LPUART_RegType *base)
{
    return (base != NULL) ? LPUART_IS_TX_READY(base) : false;
}

/**
 * @brief Check if receiver has data
 */
bool UART_IsRxReady(LPUART_RegType *base)
{
    return (base != NULL) ? LPUART_IS_RX_READY(base) : false;
}

/**
 * @brief Check for errors
 */
UART_Status_t UART_GetError(LPUART_RegType *base)
{
    uint32_t statusFlags;

    if (base == NULL) {
        return UART_STATUS_ERROR;
    }

    statusFlags = base->STAT;

    /* Check parity error */
    if ((statusFlags & LPUART_STAT_PF_MASK) != 0U) {
        LPUART_CLEAR_STATUS_FLAG(base, LPUART_STAT_PF_MASK);
        return UART_STATUS_PARITY_ERROR;
    }

    /* Check framing error */
    if ((statusFlags & LPUART_STAT_FE_MASK) != 0U) {
        LPUART_CLEAR_STATUS_FLAG(base, LPUART_STAT_FE_MASK);
        return UART_STATUS_FRAME_ERROR;
    }

    /* Check noise error */
    if ((statusFlags & LPUART_STAT_NF_MASK) != 0U) {
        LPUART_CLEAR_STATUS_FLAG(base, LPUART_STAT_NF_MASK);
        return UART_STATUS_NOISE_ERROR;
    }

    /* Check overrun error */
    if ((statusFlags & LPUART_STAT_OR_MASK) != 0U) {
        LPUART_CLEAR_STATUS_FLAG(base, LPUART_STAT_OR_MASK);
        return UART_STATUS_OVERRUN_ERROR;
    }

    return UART_STATUS_SUCCESS;
}

/*******************************************************************************
 * Interrupt Control Functions
 ******************************************************************************/

/**
 * @brief Enable UART interrupts
 */
void UART_EnableInterrupts(LPUART_RegType *base, uint32_t intMask)
{
    if (base != NULL) {
        base->CTRL |= intMask;
    }
}

/**
 * @brief Disable UART interrupts
 */
void UART_DisableInterrupts(LPUART_RegType *base, uint32_t intMask)
{
    if (base != NULL) {
        base->CTRL &= ~intMask;
    }
}

/**
 * @brief Get enabled interrupts
 */
uint32_t UART_GetEnabledInterrupts(LPUART_RegType *base)
{
    if (base != NULL) {
        return (base->CTRL & (LPUART_CTRL_TIE_MASK | LPUART_CTRL_RIE_MASK | 
                              LPUART_CTRL_TCIE_MASK | LPUART_CTRL_ILIE_MASK |
                              LPUART_CTRL_PEIE_MASK | LPUART_CTRL_FEIE_MASK |
                              LPUART_CTRL_NEIE_MASK | LPUART_CTRL_ORIE_MASK));
    }
    return 0U;
}

/*******************************************************************************
 * Clock Control Functions
 ******************************************************************************/

/**
 * @brief Enable UART clock
 */
void UART_EnableClock(uint8_t instance)
{
    if (instance == 0U) {
        /* Enable LPUART0 clock via PCC */
        PCC->PCCn[PCC_LPUART0_INDEX] |= 1 << PCC_PCCn_PCS_SHIFT
        						     |PCC_PCCn_CGC_MASK;
    } else if (instance == 1U) {
        /* Enable LPUART1 clock via PCC */
        PCC->PCCn[PCC_LPUART1_INDEX] |= 1 << PCC_PCCn_PCS_SHIFT
			     	 	 	 	 	 |PCC_PCCn_CGC_MASK;
    } else if (instance == 2U) {
        /* Enable LPUART2 clock via PCC */
        PCC->PCCn[PCC_LPUART2_INDEX] |= 1 << PCC_PCCn_PCS_SHIFT
			     	 	 	 	 	 |PCC_PCCn_CGC_MASK;
    }
}

/**
 * @brief Disable UART clock
 */
void UART_DisableClock(uint8_t instance)
{
    if (instance == 0U) {
        /* Disable LPUART0 clock via PCC */
        PCC->PCCn[PCC_LPUART0_INDEX] &= ~PCC_PCCn_CGC_MASK;
    } else if (instance == 1U) {
        /* Disable LPUART1 clock via PCC */
        PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;
    } else if (instance == 2U) {
        /* Disable LPUART2 clock via PCC */
        PCC->PCCn[PCC_LPUART2_INDEX] &= ~PCC_PCCn_CGC_MASK;
    }
}

/*******************************************************************************
 * DMA Transfer Functions
 ******************************************************************************/
#if UART_DMA_ENABLE
/**
 * @brief Get DMAMUX source for UART instance TX
 */
static dmamux_source_t UART_GetTxDmaMuxSource(const LPUART_RegType *base)
{
    if (base == LPUART0) {
        return DMAMUX_SRC_LPUART0_TX;
    } else if (base == LPUART1) {
        return DMAMUX_SRC_LPUART1_TX;
    } else if (base == LPUART2) {
        return DMAMUX_SRC_LPUART2_TX;
    }
    return DMAMUX_SRC_DISABLED;
}

/**
 * @brief Get DMAMUX source for UART instance RX
 */
static dmamux_source_t UART_GetRxDmaMuxSource(const LPUART_RegType *base)
{
    if (base == LPUART0) {
        return DMAMUX_SRC_LPUART0_RX;
    } else if (base == LPUART1) {
        return DMAMUX_SRC_LPUART1_RX;
    } else if (base == LPUART2) {
        return DMAMUX_SRC_LPUART2_RX;
    }
    return DMAMUX_SRC_DISABLED;
}

/**
 * @brief Configure UART for DMA transmission
 */
UART_Status_t UART_ConfigTxDMA(LPUART_RegType *base, uint8_t dmaChannel)
{
    dmamux_source_t dmaMuxSource;
    
    if (base == NULL) {
        return UART_STATUS_ERROR;
    }
    
    /* Get DMAMUX source for this UART instance */
    dmaMuxSource = UART_GetTxDmaMuxSource(base);
    if (dmaMuxSource == DMAMUX_SRC_DISABLED) {
        return UART_STATUS_ERROR;
    }
    
    /* Enable UART TX DMA request */
    base->BAUD |= LPUART_BAUD_TDMAE_MASK;
    
    return UART_STATUS_SUCCESS;
}

/**
 * @brief Configure UART for DMA reception
 */
UART_Status_t UART_ConfigRxDMA(LPUART_RegType *base, uint8_t dmaChannel)
{
    dmamux_source_t dmaMuxSource;
    
    if (base == NULL) {
        return UART_STATUS_ERROR;
    }
    
    /* Get DMAMUX source for this UART instance */
    dmaMuxSource = UART_GetRxDmaMuxSource(base);
    if (dmaMuxSource == DMAMUX_SRC_DISABLED) {
        return UART_STATUS_ERROR;
    }
    
    /* Enable UART RX DMA request */
    base->BAUD |= LPUART_BAUD_RDMAE_MASK;
    
    return UART_STATUS_SUCCESS;
}

/**
 * @brief Send data using DMA
 */
UART_Status_t UART_SendDMA(LPUART_RegType *base, uint8_t dmaChannel, 
                           const uint8_t *txBuff, uint32_t txSize)
{
    status_t dmaStatus;
    dmamux_source_t dmaMuxSource;
    dma_channel_config_t dmaConfig;
    
    if ((base == NULL) || (txBuff == NULL) || (txSize == 0U)) {
        return UART_STATUS_ERROR;
    }
    
    /* Get DMAMUX source */
    dmaMuxSource = UART_GetTxDmaMuxSource(base);
    if (dmaMuxSource == DMAMUX_SRC_DISABLED) {
        return UART_STATUS_ERROR;
    }
    
    /* Configure DMA channel for UART TX */
    dmaConfig.channel = dmaChannel;
    dmaConfig.source = dmaMuxSource;
    dmaConfig.transferType = DMA_TRANSFER_MEM_TO_PERIPH;
    dmaConfig.transferSize = DMA_TRANSFER_SIZE_1B;          /* 1 byte per transfer */
    dmaConfig.priority = DMA_PRIORITY_NORMAL;
    
    /* Source: TX buffer */
    dmaConfig.sourceAddr = (uint32_t)txBuff;
    dmaConfig.sourceOffset = 1;                             /* Move forward 1 byte */
    dmaConfig.sourceLastAddrAdjust = 0;
    
    /* Destination: UART DATA register */
    dmaConfig.destAddr = (uint32_t)&(base->DATA);
    dmaConfig.destOffset = 0;                               /* Keep same address */
    dmaConfig.destLastAddrAdjust = 0;
    
    /* Transfer configuration */
    dmaConfig.minorLoopBytes = 1U;                          /* 1 byte per minor loop */
    dmaConfig.majorLoopCount = (uint16_t)txSize;           /* Total bytes to send */
    dmaConfig.enableInterrupt = false;
    dmaConfig.disableRequestAfterDone = true;
    
    /* Configure and start DMA */
    dmaStatus = DMA_ConfigChannel(&dmaConfig);
    if (dmaStatus != STATUS_SUCCESS) {
        return UART_STATUS_ERROR;
    }
    
    dmaStatus = DMA_StartChannel(dmaChannel);
    if (dmaStatus != STATUS_SUCCESS) {
        return UART_STATUS_ERROR;
    }
    
    return UART_STATUS_SUCCESS;
}

/**
 * @brief Receive data using DMA
 */
UART_Status_t UART_ReceiveDMA(LPUART_RegType *base, uint8_t dmaChannel,
                              uint8_t *rxBuff, uint32_t rxSize)
{
    status_t dmaStatus;
    dmamux_source_t dmaMuxSource;
    dma_channel_config_t dmaConfig;
    
    if ((base == NULL) || (rxBuff == NULL) || (rxSize == 0U)) {
        return UART_STATUS_ERROR;
    }
    
    /* Get DMAMUX source */
    dmaMuxSource = UART_GetRxDmaMuxSource(base);
    if (dmaMuxSource == DMAMUX_SRC_DISABLED) {
        return UART_STATUS_ERROR;
    }
    
    /* Configure DMA channel for UART RX */
    dmaConfig.channel = dmaChannel;
    dmaConfig.source = dmaMuxSource;
    dmaConfig.transferType = DMA_TRANSFER_PERIPH_TO_MEM;
    dmaConfig.transferSize = DMA_TRANSFER_SIZE_1B;          /* 1 byte per transfer */
    dmaConfig.priority = DMA_PRIORITY_NORMAL;
    
    /* Source: UART DATA register */
    dmaConfig.sourceAddr = (uint32_t)&(base->DATA);
    dmaConfig.sourceOffset = 0;                             /* Keep same address */
    dmaConfig.sourceLastAddrAdjust = 0;
    
    /* Destination: RX buffer */
    dmaConfig.destAddr = (uint32_t)rxBuff;
    dmaConfig.destOffset = 1;                               /* Move forward 1 byte */
    dmaConfig.destLastAddrAdjust = 0;
    
    /* Transfer configuration */
    dmaConfig.minorLoopBytes = 1U;                          /* 1 byte per minor loop */
    dmaConfig.majorLoopCount = (uint16_t)rxSize;           /* Total bytes to receive */
    dmaConfig.enableInterrupt = false;
    dmaConfig.disableRequestAfterDone = true;
    
    /* Configure and start DMA */
    dmaStatus = DMA_ConfigChannel(&dmaConfig);
    if (dmaStatus != STATUS_SUCCESS) {
        return UART_STATUS_ERROR;
    }
    
    dmaStatus = DMA_StartChannel(dmaChannel);
    if (dmaStatus != STATUS_SUCCESS) {
        return UART_STATUS_ERROR;
    }
    
    return UART_STATUS_SUCCESS;
}

/**
 * @brief Send data using DMA with blocking wait
 */
UART_Status_t UART_SendDMABlocking(LPUART_RegType *base, uint8_t dmaChannel,
                                   const uint8_t *txBuff, uint32_t txSize)
{
    UART_Status_t status;
    uint32_t timeout = UART_TIMEOUT_COUNT * txSize;
    
    /* Start DMA transfer */
    status = UART_SendDMA(base, dmaChannel, txBuff, txSize);
    if (status != UART_STATUS_SUCCESS) {
        return status;
    }
    
    /* Wait for completion */
    while (!DMA_IsChannelDone(dmaChannel) && (timeout > 0U)) {
        timeout--;
    }
    
    if (timeout == 0U) {
        DMA_StopChannel(dmaChannel);
        return UART_STATUS_TIMEOUT;
    }
    
    /* Clear done flag */
    DMA_ClearDone(dmaChannel);
    
    return UART_STATUS_SUCCESS;
}

/**
 * @brief Receive data using DMA with blocking wait
 */
UART_Status_t UART_ReceiveDMABlocking(LPUART_RegType *base, uint8_t dmaChannel,
                                      uint8_t *rxBuff, uint32_t rxSize)
{
    UART_Status_t status;
    uint32_t timeout = UART_TIMEOUT_COUNT * rxSize;
    
    /* Start DMA transfer */
    status = UART_ReceiveDMA(base, dmaChannel, rxBuff, rxSize);
    if (status != UART_STATUS_SUCCESS) {
        return status;
    }
    
    /* Wait for completion */
    while (!DMA_IsChannelDone(dmaChannel) && (timeout > 0U)) {
        timeout--;
    }
    
    if (timeout == 0U) {
        DMA_StopChannel(dmaChannel);
        return UART_STATUS_TIMEOUT;
    }
    
    /* Clear done flag */
    DMA_ClearDone(dmaChannel);
    
    return UART_STATUS_SUCCESS;
}

/**
 * @brief Disable UART TX DMA
 */
void UART_DisableTxDMA(LPUART_RegType *base)
{
    if (base != NULL) {
        base->BAUD &= ~LPUART_BAUD_TDMAE_MASK;
    }
}

/**
 * @brief Disable UART RX DMA
 */
void UART_DisableRxDMA(LPUART_RegType *base)
{
    if (base != NULL) {
        base->BAUD &= ~LPUART_BAUD_RDMAE_MASK;
    }
}
#endif
