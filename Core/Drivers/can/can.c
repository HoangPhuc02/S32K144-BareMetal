/**
 * @file    can.c
 * @brief   FlexCAN Driver Implementation for S32K144
 * @details Implementation of CAN driver functions
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "can.h"
#include "can_mb_access.h"
#include "../device_compat.h"
#include "pcc_reg.h"
#include <stddef.h>
#include <string.h>
/*******************************************************************************
 * Private Definitions
 ******************************************************************************/

/** @brief Timeout for entering/exiting freeze mode */
#define CAN_FREEZE_TIMEOUT      (10000U)

/** @brief Default CAN clock frequency (48 MHz from FIRC DIV2) */
#define CAN_DEFAULT_CLK_FREQ    (24000000U)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief CAN base address array */
static CAN_Type * const s_canBases[CAN_INSTANCE_COUNT] = {CAN0, CAN1, CAN2};

/** @brief CAN initialized flag */
static bool s_canInitialized[CAN_INSTANCE_COUNT] = {false, false, false};

/** @brief CAN clock frequency for each instance */
static uint32_t s_canClockFreq[CAN_INSTANCE_COUNT] = {0U, 0U, 0U};

/** @brief TX callback array */
static can_callback_t s_txCallbacks[CAN_INSTANCE_COUNT][CAN_MB_COUNT];

/** @brief RX callback array */
static can_callback_t s_rxCallbacks[CAN_INSTANCE_COUNT][CAN_MB_COUNT];

/** @brief TX user data array */
static void *s_txUserData[CAN_INSTANCE_COUNT][CAN_MB_COUNT];

/** @brief RX user data array */
static void *s_rxUserData[CAN_INSTANCE_COUNT][CAN_MB_COUNT];

/** @brief Error callback array */
static can_error_callback_t s_errorCallbacks[CAN_INSTANCE_COUNT];

/** @brief Error callback user data */
static void *s_errorUserData[CAN_INSTANCE_COUNT];

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static status_t CAN_EnterFreezeMode(CAN_Type *base);
static status_t CAN_ExitFreezeMode(CAN_Type *base);
static status_t CAN_SoftReset(CAN_Type *base);
static void CAN_EnableClock(uint8_t instance);
static void CAN_InitMessageBuffers(CAN_Type *base);
static uint32_t CAN_GetClockFrequency(can_clk_src_t clockSource);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize CAN module
 */
status_t CAN_Init(const can_config_t *config)
{
    CAN_Type *base;
    status_t status;
    can_timing_config_t timing;
    uint32_t canClockHz;
    
    /* Validate parameters */
    if (config == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    if (config->instance >= CAN_INSTANCE_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[config->instance];
    
    /* Enable CAN clock */
    CAN_EnableClock(config->instance);
    
    /* Get CAN clock frequency */
    canClockHz = CAN_GetClockFrequency(config->clockSource);
    s_canClockFreq[config->instance] = canClockHz;
    
    /* Enter freeze mode for configuration */
    status = CAN_EnterFreezeMode(base);
    if (status != STATUS_SUCCESS) {
        return STATUS_ERROR;
    }
    
    /* Soft reset */
    status = CAN_SoftReset(base);
    if (status != STATUS_SUCCESS) {
        return STATUS_ERROR;
    }
    
    /* Calculate timing parameters */
    status = CAN_CalculateTiming(canClockHz, config->baudRate, &timing);
    if (status != STATUS_SUCCESS) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Configure CTRL1 register */
    base->CTRL1 = ((uint32_t)timing.preDiv << CAN_CTRL1_PRESDIV_SHIFT) |
                  ((uint32_t)timing.rJumpWidth << CAN_CTRL1_RJW_SHIFT) |
                  ((uint32_t)timing.phaseSeg1 << CAN_CTRL1_PSEG1_SHIFT) |
                  ((uint32_t)timing.phaseSeg2 << CAN_CTRL1_PSEG2_SHIFT) |
                  ((uint32_t)timing.propSeg << CAN_CTRL1_PROPSEG_SHIFT);
    
    /* Configure operating mode */
    if (config->mode == CAN_MODE_LOOPBACK) {
        base->CTRL1 |= CAN_CTRL1_LPB_MASK;
    } else if (config->mode == CAN_MODE_LISTEN_ONLY) {
        base->CTRL1 |= CAN_CTRL1_LOM_MASK;
    }
    
    /* Configure self-reception */
    if (!config->enableSelfReception) {
        base->MCR |= CAN_MCR_SRXDIS_MASK;
    }
    
    /* Configure RX FIFO or individual MBs */
    if (config->useRxFifo) {
        base->MCR |= CAN_MCR_RFEN_MASK;
    } else {
        base->MCR &= ~CAN_MCR_RFEN_MASK;
    }
    
    /* Set maximum number of MBs */
    base->MCR = (base->MCR & ~CAN_MCR_MAXMB_MASK) | 
                ((CAN_MB_COUNT - 1U) << CAN_MCR_MAXMB_SHIFT);
    
    /* Initialize all Message Buffers */
    CAN_InitMessageBuffers(base);
    
    /* Initialize global mask to accept all messages */
    base->RXMGMASK = 0x00000000UL;
    
    /* Clear error counters */
    base->ECR = 0U;
    
    /* Exit freeze mode */
    status = CAN_ExitFreezeMode(base);
    if (status != STATUS_SUCCESS) {
        return STATUS_ERROR;
    }
    
    /* Mark as initialized */
    s_canInitialized[config->instance] = true;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Deinitialize CAN module
 */
status_t CAN_Deinit(uint8_t instance)
{
    CAN_Type *base;
    
    /* Validate parameter */
    if (instance >= CAN_INSTANCE_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[instance];
    
    /* Disable module */
    base->MCR |= CAN_MCR_MDIS_MASK;
    
    /* Disable clock */
    if (instance == 0U) {
    	PCC->PCCn[PCC_FLEXCAN0_INDEX] &=~ PCC_PCCn_CGC_MASK;
    } else if (instance == 1U) {
    	PCC->PCCn[PCC_FLEXCAN1_INDEX] &=~ PCC_PCCn_CGC_MASK;
    } else {
    	PCC->PCCn[PCC_FLEXCAN2_INDEX] &=~ PCC_PCCn_CGC_MASK;
    }
    



    /* Clear initialized flag */
    s_canInitialized[instance] = false;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Send CAN message
 */
status_t CAN_Send(uint8_t instance, uint8_t mbIndex, const can_message_t *message)
{
    CAN_Type *base;
    uint32_t cs;
    
    /* Validate parameters */
    if (instance >= CAN_INSTANCE_COUNT || message == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    if (!s_canInitialized[instance]) {
        return STATUS_NOT_INITIALIZED;
    }
    
    if (mbIndex < CAN_TX_MB_START ||
        mbIndex >= (CAN_TX_MB_START + CAN_TX_MB_COUNT)) {
        return STATUS_INVALID_PARAM;
    }
    
    if (message->dataLength > CAN_MAX_DATA_LENGTH) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[instance];
    
    /* Check if MB is busy */
    cs = CAN_ReadMbCs(base, mbIndex);
    if (((cs & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) != CAN_CS_CODE_TX_INACTIVE) {
        return STATUS_BUSY;
    }
    
    /* Configure ID */
    if (message->idType == CAN_ID_STD) {
        CAN_WriteMbId(base, mbIndex, (message->id << CAN_ID_STD_SHIFT) & CAN_ID_STD_MASK);
    } else {
        CAN_WriteMbId(base, mbIndex, (message->id << CAN_ID_EXT_SHIFT) & CAN_ID_EXT_MASK);
    }
    
    /* Copy data */
    CAN_CopyDataToMb(base, mbIndex, message->data, message->dataLength);
    
    /* Configure CS and trigger transmission */
    cs = (CAN_CS_CODE_TX_ONCE << CAN_CS_CODE_SHIFT) |
         ((uint32_t)message->dataLength << CAN_CS_DLC_SHIFT);
    
    if (message->idType == CAN_ID_EXT) {
        cs |= CAN_CS_IDE_MASK;
        cs |= CAN_CS_SRR_MASK;
    }
    
    if (message->frameType == CAN_FRAME_REMOTE) {
        cs |= CAN_CS_RTR_MASK;
    }
    
    CAN_WriteMbCs(base, mbIndex, cs);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Send CAN message with timeout
 */
status_t CAN_SendBlocking(uint8_t instance, uint8_t mbIndex, 
                          const can_message_t *message, uint32_t timeoutMs)
{
    CAN_Type *base;
    status_t status;
    uint32_t timeoutCount = 0;
    uint32_t mbMask;
    
    /* Send message */
    status = CAN_Send(instance, mbIndex, message);
    if (status != STATUS_SUCCESS) {
        return status;
    }
    
    base = s_canBases[instance];
    mbMask = (1UL << mbIndex);
    
    /* Wait for transmission complete */
    while (timeoutCount < (timeoutMs * 1000U)) {
        if (base->IFLAG1 & mbMask) {
            /* Clear flag */
            base->IFLAG1 = mbMask;
            return STATUS_SUCCESS;
        }
        timeoutCount++;
    }
    
    return STATUS_TIMEOUT;
}

/**
 * @brief Receive CAN message
 */
status_t CAN_Receive(uint8_t instance, uint8_t mbIndex, can_message_t *message)
{
    CAN_Type *base;
    uint32_t cs;
    uint32_t id;
    uint32_t mbMask;
    
    /* Validate parameters */
    if (instance >= CAN_INSTANCE_COUNT || message == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    if (!s_canInitialized[instance]) {
        return STATUS_NOT_INITIALIZED;
    }
    
    if (mbIndex < CAN_RX_MB_START || mbIndex >= CAN_MB_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[instance];
    mbMask = (1UL << mbIndex);
    
    /* Check if message available */
    if ((base->IFLAG1 & mbMask) == 0U) {
        return STATUS_ERROR;
    }
    
    /* Read CS to lock MB */
    cs = CAN_ReadMbCs(base, mbIndex);
    
    /* Read ID */
    id = CAN_ReadMbId(base, mbIndex);
    
    /* Extract message information */
    if (cs & CAN_CS_IDE_MASK) {
        message->idType = CAN_ID_EXT;
        message->id = (id & CAN_ID_EXT_MASK) >> CAN_ID_EXT_SHIFT;
    } else {
        message->idType = CAN_ID_STD;
        message->id = (id & CAN_ID_STD_MASK) >> CAN_ID_STD_SHIFT;
    }
    
    message->frameType = (cs & CAN_CS_RTR_MASK) ? CAN_FRAME_REMOTE : CAN_FRAME_DATA;
    message->dataLength = (cs & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT;
    
    /* Copy data */
    CAN_CopyDataFromMb(base, mbIndex, message->data, message->dataLength);
    
    /* Read free running timer to unlock MB */
    (void)base->TIMER;
    
    /* Clear interrupt flag */
    base->IFLAG1 = mbMask;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Receive CAN message with timeout
 */
status_t CAN_ReceiveBlocking(uint8_t instance, uint8_t mbIndex, 
                             can_message_t *message, uint32_t timeoutMs)
{
    CAN_Type *base;
    uint32_t timeoutCount = 0;
    uint32_t mbMask;
    
    /* Validate parameters */
    if (instance >= CAN_INSTANCE_COUNT || message == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    if (!s_canInitialized[instance]) {
        return STATUS_NOT_INITIALIZED;
    }
    
    base = s_canBases[instance];
    mbMask = (1UL << mbIndex);
    
    /* Wait for message */
    while (timeoutCount < (timeoutMs * 1000U)) {
        if (base->IFLAG1 & mbMask) {
            return CAN_Receive(instance, mbIndex, message);
        }
        timeoutCount++;
    }
    
    return STATUS_TIMEOUT;
}

/**
 * @brief Configure RX filter
 */
status_t CAN_ConfigRxFilter(uint8_t instance, uint8_t mbIndex, 
                             const can_rx_filter_t *filter)
{
    CAN_Type *base;
    uint32_t cs;
    
    /* Validate parameters */
    if (instance >= CAN_INSTANCE_COUNT || filter == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    if (!s_canInitialized[instance]) {
        return STATUS_NOT_INITIALIZED;
    }
    
    if (mbIndex < CAN_RX_MB_START || mbIndex >= CAN_MB_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[instance];
    
    /* Configure MB as RX */
    cs = (CAN_CS_CODE_RX_EMPTY << CAN_CS_CODE_SHIFT);
    
    if (filter->idType == CAN_ID_EXT) {
        cs |= CAN_CS_IDE_MASK;
        CAN_WriteMbId(base, mbIndex, (filter->id << CAN_ID_EXT_SHIFT) & CAN_ID_EXT_MASK);
    } else {
        CAN_WriteMbId(base, mbIndex, (filter->id << CAN_ID_STD_SHIFT) & CAN_ID_STD_MASK);
    }
    
    CAN_WriteMbCs(base, mbIndex, cs);
    
    /* Configure individual mask */
    base->RXIMR[mbIndex] = filter->mask;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Install TX callback
 */
status_t CAN_InstallTxCallback(uint8_t instance, uint8_t mbIndex,
                                can_callback_t callback, void *userData)
{
    if (instance >= CAN_INSTANCE_COUNT || mbIndex >= CAN_MB_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    s_txCallbacks[instance][mbIndex] = callback;
    s_txUserData[instance][mbIndex] = userData;
    
    /* Enable interrupt for this MB */
    if (callback != NULL) {
        CAN_Type *base = s_canBases[instance];
        base->IMASK1 |= (1UL << mbIndex);
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Install RX callback
 */
status_t CAN_InstallRxCallback(uint8_t instance, uint8_t mbIndex,
                                can_callback_t callback, void *userData)
{
    if (instance >= CAN_INSTANCE_COUNT || mbIndex >= CAN_MB_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    s_rxCallbacks[instance][mbIndex] = callback;
    s_rxUserData[instance][mbIndex] = userData;
    
    /* Enable interrupt for this MB */
    if (callback != NULL) {
        CAN_Type *base = s_canBases[instance];
        base->IMASK1 |= (1UL << mbIndex);
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Install error callback
 */
status_t CAN_InstallErrorCallback(uint8_t instance, 
                                   can_error_callback_t callback, void *userData)
{
    if (instance >= CAN_INSTANCE_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    s_errorCallbacks[instance] = callback;
    s_errorUserData[instance] = userData;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get error state
 */
status_t CAN_GetErrorState(uint8_t instance, can_error_state_t *errorState)
{
    CAN_Type *base;
    uint32_t fltConf;
    
    if (instance >= CAN_INSTANCE_COUNT || errorState == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[instance];
    
    fltConf = (base->ESR1 & CAN_ESR1_FLTCONF_MASK) >> CAN_ESR1_FLTCONF_SHIFT;
    
    if (fltConf == 0U) {
        *errorState = CAN_ERROR_ACTIVE;
    } else if (fltConf == 1U) {
        *errorState = CAN_ERROR_PASSIVE;
    } else {
        *errorState = CAN_ERROR_BUS_OFF;
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get error counters
 */
status_t CAN_GetErrorCounters(uint8_t instance, 
                               uint8_t *txErrorCount, uint8_t *rxErrorCount)
{
    CAN_Type *base;
    
    if (instance >= CAN_INSTANCE_COUNT || 
        txErrorCount == NULL || rxErrorCount == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[instance];
    
    *txErrorCount = (base->ECR >> 8U) & 0xFFU;
    *rxErrorCount = base->ECR & 0xFFU;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Abort transmission
 */
status_t CAN_AbortTransmission(uint8_t instance, uint8_t mbIndex)
{
    CAN_Type *base;
    
    if (instance >= CAN_INSTANCE_COUNT || mbIndex >= CAN_MB_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[instance];
    
    /* Set abort code */
    CAN_WriteMbCs(base, mbIndex, (CAN_CS_CODE_TX_ABORT << CAN_CS_CODE_SHIFT));
    
    return STATUS_SUCCESS;
}

/**
 * @brief Check if MB is busy
 */
status_t CAN_IsMbBusy(uint8_t instance, uint8_t mbIndex, bool *isBusy)
{
    CAN_Type *base;
    uint32_t cs;
    uint32_t code;
    
    if (instance >= CAN_INSTANCE_COUNT || 
        mbIndex >= CAN_MB_COUNT || isBusy == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[instance];
    cs = CAN_ReadMbCs(base, mbIndex);
    code = (cs & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT;
    
    *isBusy = (code != CAN_CS_CODE_TX_INACTIVE &&
               code != CAN_CS_CODE_RX_INACTIVE);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Calculate timing parameters
 */
status_t CAN_CalculateTiming(uint32_t canClockHz, uint32_t baudRate, 
                             can_timing_config_t *timing)
{
    uint32_t numTq;
    uint32_t preDiv;
    
    if (timing == NULL || baudRate == 0U || canClockHz == 0U) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Target: 16 time quanta per bit for good timing */
    numTq = 16U;
    
    /* Calculate prescaler */
    preDiv = (canClockHz / (baudRate * numTq)) - 1U;
    
    if (preDiv > 255U) {
        /* Try with 8 time quanta */
        numTq = 8U;
        preDiv = (canClockHz / (baudRate * numTq)) - 1U;
        
        if (preDiv > 255U) {
            return STATUS_INVALID_PARAM;
        }
    }
    
    /* Configure timing with good margin */
    timing->preDiv = (uint8_t)preDiv;
    timing->propSeg = 7U;       /* Propagation segment */
    timing->phaseSeg1 = 4U;     /* Phase segment 1 */
    timing->phaseSeg2 = 3U;     /* Phase segment 2 */
    timing->rJumpWidth = 1U;    /* Resynchronization jump width */
    
    return STATUS_SUCCESS;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Enter freeze mode
 */
static status_t CAN_EnterFreezeMode(CAN_Type *base)
{
    uint32_t timeout = CAN_FREEZE_TIMEOUT;
    
    /* Request freeze mode */
    base->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
    
    /* Wait for freeze mode acknowledge */
    while ((timeout > 0U) && ((base->MCR & CAN_MCR_FRZACK_MASK) == 0U)) {
        timeout--;
    }
    
    if (timeout == 0U) {
        return STATUS_TIMEOUT;
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Exit freeze mode
 */
static status_t CAN_ExitFreezeMode(CAN_Type *base)
{
    uint32_t timeout = CAN_FREEZE_TIMEOUT;
    
    /* Clear freeze and halt bits */
    base->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
    
    /* Wait for module ready */
    while ((timeout > 0U) && ((base->MCR & CAN_MCR_FRZACK_MASK) != 0U)) {
        timeout--;
    }
    
    if (timeout == 0U) {
        return STATUS_TIMEOUT;
    }
    
    /* Wait for module not ready flag to clear */
    timeout = CAN_FREEZE_TIMEOUT;
    while ((timeout > 0U) && ((base->MCR & CAN_MCR_NOTRDY_MASK) != 0U)) {
        timeout--;
    }
    
    if (timeout == 0U) {
        return STATUS_TIMEOUT;
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Soft reset CAN module
 */
static status_t CAN_SoftReset(CAN_Type *base)
{
    uint32_t timeout = CAN_FREEZE_TIMEOUT;
    
    /* Request soft reset */
    base->MCR |= CAN_MCR_SOFTRST_MASK;
    
    /* Wait for reset to complete */
    while ((timeout > 0U) && ((base->MCR & CAN_MCR_SOFTRST_MASK) != 0U)) {
        timeout--;
    }
    
    if (timeout == 0U) {
        return STATUS_TIMEOUT;
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Enable CAN peripheral clock
 */
static void CAN_EnableClock(uint8_t instance)
{
//	DEV_ASSERT(instance > 2U);

    /* Get PCC register address */
    if (instance == 0U) {
        PCC->PCCn[PCC_FLEXCAN0_INDEX] |= PCC_PCCn_CGC_MASK;

        CAN0->MCR |= CAN_MCR_MDIS_MASK; /* MDIS=1: Disable module before selecting clock */
        CAN0->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK; /* CLKSRC=0: Clock Source = oscillator (8 MHz) */
    } else if (instance == 1U) {
    	PCC->PCCn[PCC_FLEXCAN1_INDEX] |= PCC_PCCn_CGC_MASK;
    	CAN1->MCR |= CAN_MCR_MDIS_MASK; /* MDIS=1: Disable module before selecting clock */
    	CAN1->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK; /* CLKSRC=0: Clock Source = oscillator (8 MHz) */
    } else {
    	PCC->PCCn[PCC_FLEXCAN2_INDEX] |= PCC_PCCn_CGC_MASK;
    	CAN2->MCR |= CAN_MCR_MDIS_MASK; /* MDIS=1: Disable module before selecting clock */
    	CAN2->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK; /* CLKSRC=0: Clock Source = oscillator (8 MHz) */
    }


}

/**
 * @brief Initialize all Message Buffers to inactive
 */
static void CAN_InitMessageBuffers(CAN_Type *base)
{
    /* Set all MBs to inactive */
    for (uint8_t i = 0; i < CAN_MB_COUNT; i++) {
        CAN_ClearMb(base, i);
        
        /* Clear individual mask */
        base->RXIMR[i] = 0xFFFFFFFFUL;
    }
    
    /* Clear all interrupt flags */
    base->IFLAG1 = 0xFFFFFFFFUL;
}

/**
 * @brief Get clock frequency from clock source
 */
static uint32_t CAN_GetClockFrequency(can_clk_src_t clockSource)
{
    uint32_t freq;
    
    switch (clockSource) {
        case CAN_CLK_SRC_SOSCDIV2:
            freq = 8000000U;    /* 8 MHz / 2 = 4 MHz typical */
            break;
        case CAN_CLK_SRC_BUSCLOCK:
            freq = 40000000U;   /* 80 MHz / 2 = 40 MHz typical */
            break;
        default:
            freq = CAN_DEFAULT_CLK_FREQ;
            break;
    }
    
    return freq;
}
