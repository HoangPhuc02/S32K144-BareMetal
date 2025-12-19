/**
 * @file    dma.c
 * @brief   DMA (Direct Memory Access) Driver Implementation for S32K144
 * @details
 * DMA driver implementation allows:
 * - Automatic data transfer between memory and peripherals
 * - Reduce CPU load by performing data transfer without CPU intervention
 * - Support interrupts and callbacks
 * 
 * DMA Concepts:
 * - Minor Loop: One small transfer (e.g., 4 bytes)
 * - Major Loop: Collection of multiple minor loops (e.g., 256 times x 4 bytes = 1024 bytes)
 * - TCD (Transfer Control Descriptor): Control structure for each channel
 * 
 * @author  PhucPH32
 * @date    27/11/2025
 * @version 1.0
 * 
 * @note    Ensure clock is enabled for DMA and DMAMUX before use
 * @warning Do not change configuration while DMA is active
 * 
 * @par Change Log:
 * - Version 1.0 (27/11/2025): Initialize DMA driver
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "dma.h"
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief PCC base address */
#define PCC_BASE            (0x40065000UL)

/** @brief PCC register offsets */
#define PCC_DMA_OFFSET      (0x20U)
#define PCC_DMAMUX_OFFSET   (0x21U)

/** @brief PCC register structure */
typedef struct {
    __IO uint32_t CGC : 1;      /**< Bit 30: Clock Gate Control */
    __IO uint32_t PR  : 1;      /**< Bit 31: Present */
} PCC_CGC_Type;

/** @brief Macro to enable clock for module */
#define PCC_CGC_ENABLE(offset)  (*((volatile uint32_t *)(PCC_BASE + ((offset) << 2))) |= (1UL << 30))

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Callback function pointers for each DMA channel */
static dma_callback_t s_dmaCallbacks[DMA_MAX_CHANNELS] = {NULL};

/** @brief User data pointers for each DMA channel */
static void *s_dmaUserData[DMA_MAX_CHANNELS] = {NULL};

/** @brief Flag indicating DMA has been initialized */
static bool s_dmaInitialized = false;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Enable clock for DMA module
 * @return STATUS_SUCCESS if successful
 */
static status_t DMA_EnableClock(void);

/**
 * @brief Validate channel number
 * @param channel DMA channel number
 * @return true if channel is valid (0-15)
 */
static inline bool DMA_IsValidChannel(uint8_t channel);

/**
 * @brief Reset TCD to default state
 * @param channel DMA channel number
 */
static void DMA_ResetTCD(uint8_t channel);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Enable clock for DMA and DMAMUX
 */
static status_t DMA_EnableClock(void)
{
    /* Enable clock for DMA module */
    PCC_CGC_ENABLE(PCC_DMA_OFFSET);
    
    /* Enable clock for DMAMUX module */
    PCC_CGC_ENABLE(PCC_DMAMUX_OFFSET);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Check if channel is valid
 */
static inline bool DMA_IsValidChannel(uint8_t channel)
{
    return (channel < DMA_MAX_CHANNELS);
}

/**
 * @brief Reset TCD (Transfer Control Descriptor) to default value
 */
static void DMA_ResetTCD(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return;
    }
    
    /* Reset all TCD registers to 0 */
    DMA->TCD[channel].SADDR = 0U;
    DMA->TCD[channel].SOFF = 0U;
    DMA->TCD[channel].ATTR = 0U;
    DMA->TCD[channel].NBYTES_MLNO = 0U;
    DMA->TCD[channel].SLAST = 0;
    DMA->TCD[channel].DADDR = 0U;
    DMA->TCD[channel].DOFF = 0U;
    DMA->TCD[channel].CITER_ELINKNO = 0U;
    DMA->TCD[channel].DLAST_SGA = 0;
    DMA->TCD[channel].CSR = 0U;
    DMA->TCD[channel].BITER_ELINKNO = 0U;
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize DMA module
 */
status_t DMA_Init(void)
{
    uint8_t i;
    
    /* Enable clock for DMA and DMAMUX */
    DMA_EnableClock();
    
    /* Cancel all transfers in progress */
    REG_WRITE32(DMA_BASE + 0x0000, DMA_CR_CX_MASK);
    
    /* Halt DMA operations for safe configuration */
    REG_BIT_SET32(DMA_BASE + 0x0000, DMA_CR_HALT_MASK);
    
    /* Enable Debug mode - DMA operates when CPU is in debug mode */
    REG_BIT_SET32(DMA_BASE + 0x0000, DMA_CR_EDBG_MASK);
    
    /* Enable Round Robin Channel Arbitration - Round robin priority */
    REG_BIT_SET32(DMA_BASE + 0x0000, DMA_CR_ERCA_MASK);
    
    /* Clear Halt bit so DMA can operate */
    REG_BIT_CLEAR32(DMA_BASE + 0x0000, DMA_CR_HALT_MASK);
    
    /* Clear all error flags */
    REG_WRITE32(DMA_BASE + 0x002C, 0xFFFFFFFFU);
    
    /* Clear all interrupt flags */
    REG_WRITE32(DMA_BASE + 0x0024, 0xFFFFFFFFU);
    
    /* Reset all DMA channels */
    for (i = 0U; i < DMA_MAX_CHANNELS; i++) {
        /* Reset TCD */
        DMA_ResetTCD(i);
        
        /* Disable channel trong DMAMUX */
        DMAMUX->CHCFG[i] = 0U;
        
        /* Clear callback */
        s_dmaCallbacks[i] = NULL;
        s_dmaUserData[i] = NULL;
        
        /* Disable DMA request cho kênh này */
        REG_WRITE32(DMA_BASE + 0x001A, i); /* Clear Enable Request */
    }
    
    s_dmaInitialized = true;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Deinitialize DMA module
 */
status_t DMA_Deinit(void)
{
    uint8_t i;
    
    if (!s_dmaInitialized) {
        return STATUS_ERROR;
    }
    
    /* Halt all DMA operations */
    REG_BIT_SET32(DMA_BASE + 0x0000, DMA_CR_HALT_MASK);
    
    /* Disable all channels */
    for (i = 0U; i < DMA_MAX_CHANNELS; i++) {
        DMA_StopChannel(i);
        DMA_ResetTCD(i);
        DMAMUX->CHCFG[i] = 0U;
    }
    
    s_dmaInitialized = false;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Configure one DMA channel
 */
status_t DMA_ConfigChannel(const dma_channel_config_t *config)
{
    uint16_t attr;
    uint16_t csr;
    uint8_t channel;
    
    /* Validate parameters */
    if (config == NULL) {
        return STATUS_ERROR;
    }
    
    channel = config->channel;
    
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (!s_dmaInitialized) {
        return STATUS_ERROR;
    }
    
    /* Disable channel before configuring */
    DMA_StopChannel(channel);
    
    /* Reset TCD */
    DMA_ResetTCD(channel);
    
    /* Configure Source Address */
    DMA->TCD[channel].SADDR = config->sourceAddr;
    
    /* Configure Source Offset */
    DMA->TCD[channel].SOFF = config->sourceOffset;
    
    /* Configure Source Last Address Adjustment */
    DMA->TCD[channel].SLAST = config->sourceLastAddrAdjust;
    
    /* Configure Destination Address */
    DMA->TCD[channel].DADDR = config->destAddr;
    
    /* Configure Destination Offset */
    DMA->TCD[channel].DOFF = config->destOffset;
    
    /* Configure Destination Last Address Adjustment */
    DMA->TCD[channel].DLAST_SGA = config->destLastAddrAdjust;
    
    /* Configure Transfer Attributes (ATTR) */
    attr = 0U;
    /* Source size */
    attr |= ((uint16_t)config->transferSize << DMA_TCD_ATTR_SSIZE_SHIFT) & DMA_TCD_ATTR_SSIZE_MASK;
    /* Destination size */
    attr |= ((uint16_t)config->transferSize << DMA_TCD_ATTR_DSIZE_SHIFT) & DMA_TCD_ATTR_DSIZE_MASK;
    DMA->TCD[channel].ATTR = attr;
    
    /* Configure Minor Loop Byte Count */
    DMA->TCD[channel].NBYTES_MLNO = config->minorLoopBytes;
    
    /* Configure Major Loop Count (CITER and BITER must be the same) */
    DMA->TCD[channel].CITER_ELINKNO = config->majorLoopCount;
    DMA->TCD[channel].BITER_ELINKNO = config->majorLoopCount;
    
    /* Configure Control and Status Register (CSR) */
    csr = 0U;
    
    if (config->enableInterrupt) {
        csr |= DMA_TCD_CSR_INTMAJOR_MASK;  /* Enable interrupt when major loop complete */
    }
    
    if (config->disableRequestAfterDone) {
        csr |= DMA_TCD_CSR_DREQ_MASK;  /* Disable request after completion */
    }
    
    DMA->TCD[channel].CSR = csr;
    
    /* Configure Channel Priority */
    DMA_SetChannelPriority(channel, (uint8_t)config->priority);
    
    /* Configure DMAMUX */
    /* Disable channel before */
    DMAMUX->CHCFG[channel] = 0U;
    
    /* Enable channel with source */
    DMAMUX->CHCFG[channel] = (uint8_t)(
        DMAMUX_CHCFG_ENBL_MASK |  /* Enable channel */
        ((uint8_t)config->source & DMAMUX_CHCFG_SOURCE_MASK)  /* Set source */
    );
    
    return STATUS_SUCCESS;
}

/**
 * @brief Start DMA transfer for one channel
 */
status_t DMA_StartChannel(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (!s_dmaInitialized) {
        return STATUS_ERROR;
    }
    
    /* Clear DONE flag if any */
    DMA_ClearDone(channel);
    
    /* Enable DMA request for this channel */
    REG_WRITE32(DMA_BASE + 0x001B, channel); /* Set Enable Request */
    
    /* If software trigger (ALWAYS_ON source), start by setting START bit */
    if ((DMAMUX->CHCFG[channel] & DMAMUX_CHCFG_SOURCE_MASK) >= 60U) {
        DMA->TCD[channel].CSR |= DMA_TCD_CSR_START_MASK;
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Stop DMA transfer for one channel
 */
status_t DMA_StopChannel(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Disable DMA request for this channel */
    REG_WRITE32(DMA_BASE + 0x001A, channel); /* Clear Enable Request */
    
    /* Disable DMAMUX channel */
    DMAMUX->CHCFG[channel] &= ~DMAMUX_CHCFG_ENBL_MASK;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Check if DMA channel is active
 */
bool DMA_IsChannelActive(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return false;
    }
    
    return ((DMA->TCD[channel].CSR & DMA_TCD_CSR_ACTIVE_MASK) != 0U);
}

/**
 * @brief Check if DMA channel is done
 */
bool DMA_IsChannelDone(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return false;
    }
    
    return ((DMA->TCD[channel].CSR & DMA_TCD_CSR_DONE_MASK) != 0U);
}

/**
 * @brief Clear DONE flag of DMA channel
 */
status_t DMA_ClearDone(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Write to CDNE register to clear DONE bit */
    REG_WRITE32(DMA_BASE + 0x001C, channel);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Register callback function for DMA channel
 */
status_t DMA_InstallCallback(uint8_t channel, dma_callback_t callback, void *userData)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (callback == NULL) {
        return STATUS_ERROR;
    }
    
    s_dmaCallbacks[channel] = callback;
    s_dmaUserData[channel] = userData;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Set priority for DMA channel
 */
status_t DMA_SetChannelPriority(uint8_t channel, uint8_t priority)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* DCHPRI register: bits 3-0 are priority (0=low, 15=high) */
    DMA->DCHPRI[channel] = (priority & 0x0FU);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get remaining number of major loops
 */
status_t DMA_GetRemainingMajorLoops(uint8_t channel, uint16_t *count)
{
    if (!DMA_IsValidChannel(channel) || (count == NULL)) {
        return STATUS_ERROR;
    }
    
    /* Read CITER (Current Iteration Count) */
    *count = DMA->TCD[channel].CITER_ELINKNO;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Enable interrupt for DMA channel
 */
status_t DMA_EnableChannelInterrupt(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Set INTMAJOR bit in CSR */
    DMA->TCD[channel].CSR |= DMA_TCD_CSR_INTMAJOR_MASK;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Disable interrupt for DMA channel
 */
status_t DMA_DisableChannelInterrupt(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Clear INTMAJOR bit in CSR */
    DMA->TCD[channel].CSR &= ~DMA_TCD_CSR_INTMAJOR_MASK;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Handle DMA interrupt (called from ISR)
 */
void DMA_IRQHandler(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return;
    }
    
    /* Clear interrupt flag */
    REG_WRITE32(DMA_BASE + 0x001F, channel); /* Clear Interrupt Request */
    
    /* Clear DONE flag */
    DMA_ClearDone(channel);
    
    /* Call callback if available */
    if (s_dmaCallbacks[channel] != NULL) {
        s_dmaCallbacks[channel](channel, s_dmaUserData[channel]);
    }
}

/**
 * @brief Copy data using DMA (blocking)
 */
status_t DMA_MemCopy(uint8_t channel, const void *src, void *dest, uint32_t size)
{
    dma_channel_config_t config;
    uint32_t timeout = 1000000U;  /* Timeout counter */
    
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if ((src == NULL) || (dest == NULL) || (size == 0U)) {
        return STATUS_ERROR;
    }
    
    /* Configure DMA for memory-to-memory transfer */
    config.channel = channel;
    config.source = DMAMUX_SRC_ALWAYS_ON_60;  /* Always-on source for software trigger */
    config.transferType = DMA_TRANSFER_MEM_TO_MEM;
    config.transferSize = DMA_TRANSFER_SIZE_4B;  /* 4 bytes each time */
    config.priority = DMA_PRIORITY_NORMAL;
    
    config.sourceAddr = (uint32_t)src;
    config.sourceOffset = 4;  /* Increment 4 bytes after each transfer */
    config.sourceLastAddrAdjust = 0;
    
    config.destAddr = (uint32_t)dest;
    config.destOffset = 4;  /* Increment 4 bytes after each transfer */
    config.destLastAddrAdjust = 0;
    
    config.minorLoopBytes = 4;  /* Transfer 4 bytes each minor loop */
    config.majorLoopCount = (uint16_t)(size / 4U);  /* Number of iterations = size / 4 */
    
    config.enableInterrupt = false;  /* Do not use interrupt (polling mode) */
    config.disableRequestAfterDone = true;
    
    /* Configure DMA channel */
    if (DMA_ConfigChannel(&config) != STATUS_SUCCESS) {
        return STATUS_ERROR;
    }
    
    /* Start transfer */
    if (DMA_StartChannel(channel) != STATUS_SUCCESS) {
        return STATUS_ERROR;
    }
    
    /* Wait until transfer completes (blocking) */
    while (!DMA_IsChannelDone(channel) && (timeout > 0U)) {
        timeout--;
    }
    
    if (timeout == 0U) {
        /* Timeout - transfer not complete */
        DMA_StopChannel(channel);
        return STATUS_TIMEOUT;
    }
    
    /* Clear DONE flag */
    DMA_ClearDone(channel);
    
    return STATUS_SUCCESS;
}
