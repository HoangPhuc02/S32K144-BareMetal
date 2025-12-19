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
#include "pcc_reg.h"
#include <stddef.h>
#include <string.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MSG_BUF_SIZE  4  /* Message Buffer Size: 4 words (CS, ID, DATA0, DATA1) */
/*******************************************************************************
 * Private Definitions
 ******************************************************************************/

/** @brief Timeout for entering/exiting freeze mode */
#define CAN_FREEZE_TIMEOUT      (10000U)

/** 
 * @brief Default CAN clock frequency
 * @note  CAN clock source can be:
 *        - FIRC DIV2: 48 MHz / 2 = 24 MHz
 *        - Bus clock: Typically 40 MHz
 *        - SOSCDIV2: 8 MHz / 2 = 4 MHz
 *        Update this value based on your clock configuration
 */
#define CAN_DEFAULT_CLK_FREQ    (40000000U)  /* 40 MHz from bus clock */
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
static void CAN_EnableClock(uint8_t instance, can_clk_src_t clockSource);
static void CAN_InitMessageBuffers(CAN_Type *base);
static uint32_t CAN_GetClockFrequency(can_clk_src_t clockSource);
static status_t CAN_ConfigTxMailbox(uint8_t instance, uint8_t mbIndex);
static status_t CAN_ConfigRxMailbox(uint8_t instance, uint8_t mbIndex, uint32_t id, can_id_type_t idType, uint32_t mask);

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
    CAN_EnableClock(config->instance, config->clockSource);
    
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
                  ((uint32_t)timing.propSeg << CAN_CTRL1_PROPSEG_SHIFT)|
                  ((uint32_t)3<<CAN_CTRL1_SMP_SHIFT);
    
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
//    base->RXMGMASK = 0x00000000UL;

    base->RXMGMASK = 0x1FFFFFFFUL;
    
    /* Clear error counters */
    base->ECR = 0U;
    
//     base->RAMn[4*4 + 0] = (0x4 << 24) | (8 << 16);
//     base->RAMn[4*4 + 1] = 0x01EC0000; //filter
//     base->RAMn[4*4 + 0] = (0x4 << 24);
//     base->IMASK1|= (1 << 4);

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
    	PCC->PCCn[PCC_FlexCAN0_INDEX] &=~ PCC_PCCn_CGC_MASK;
    } else if (instance == 1U) {
    	PCC->PCCn[PCC_FlexCAN1_INDEX] &=~ PCC_PCCn_CGC_MASK;
    } else {
    	PCC->PCCn[PCC_FlexCAN2_INDEX] &=~ PCC_PCCn_CGC_MASK;
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
    uint32_t mbOffset;
    uint32_t cs, id;
    
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
    mbOffset = mbIndex * MSG_BUF_SIZE;
    
    /* Clear interrupt flag */
    base->IFLAG1 = (1UL << mbIndex);
    
    /* Write data words */
    base->RAMn[mbOffset + 2] = ((uint32_t)message->data[0] << 24) |
                                ((uint32_t)message->data[1] << 16) |
                                ((uint32_t)message->data[2] << 8) |
                                ((uint32_t)message->data[3]);
    
    base->RAMn[mbOffset + 3] = ((uint32_t)message->data[4] << 24) |
                                ((uint32_t)message->data[5] << 16) |
                                ((uint32_t)message->data[6] << 8) |
                                ((uint32_t)message->data[7]);
    
    /* Configure ID word */
    if (message->idType == CAN_ID_STD) {
        id = (message->id << CAN_ID_STD_SHIFT) & CAN_ID_STD_MASK;
    } else {
        id = (message->id << CAN_ID_EXT_SHIFT) & CAN_ID_EXT_MASK;
    }
    base->RAMn[mbOffset + 1] = id;
    
    /* Configure CS word and activate transmission */
    cs = (CAN_CS_CODE_TX_DATA << CAN_CS_CODE_SHIFT) |
         ((uint32_t)message->dataLength << CAN_WMBn_CS_DLC_SHIFT);
    
    if (message->idType == CAN_ID_EXT) {
        cs |= CAN_WMBn_CS_IDE_MASK;
    }
    
    if (message->idType == CAN_ID_STD) {
        cs |= CAN_WMBn_CS_SRR_MASK;  /* SRR=1 for standard ID transmission */
    }
    
    if (message->frameType == CAN_FRAME_REMOTE) {
        cs |= CAN_WMBn_CS_RTR_MASK;
    }
    
    base->RAMn[mbOffset + 0] = cs;  /* Write CS to activate MB */
    
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
    uint32_t mbOffset;
    uint32_t cs, id;
    uint32_t mbMask;
    uint32_t data0, data1;
    uint32_t dummy;
    
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
    mbOffset = mbIndex * MSG_BUF_SIZE;
    
    /* Check if message available */
    if ((base->IFLAG1 & mbMask) == 0U) {
        return STATUS_ERROR;
    }
    
    /* Read CS word (locks the MB) */
    cs = base->RAMn[mbOffset + 0];
    
    /* Read ID word */
    id = base->RAMn[mbOffset + 1];
    
    /* Extract message information */
    if (cs & CAN_WMBn_CS_IDE_MASK) {
        message->idType = CAN_ID_EXT;
        message->id = (id & CAN_ID_EXT_MASK) >> CAN_ID_EXT_SHIFT;
    } else {
        message->idType = CAN_ID_STD;
        message->id = (id & CAN_ID_STD_MASK) >> CAN_ID_STD_SHIFT;
    }
    
    message->frameType = (cs & CAN_WMBn_CS_RTR_MASK) ? CAN_FRAME_REMOTE : CAN_FRAME_DATA;
    message->dataLength = (cs & CAN_WMBn_CS_DLC_MASK) >> CAN_WMBn_CS_DLC_SHIFT;
    
    /* Read data words */
    data0 = base->RAMn[mbOffset + 2];
    data1 = base->RAMn[mbOffset + 3];
    
    /* Extract bytes from data words (big-endian) */
    message->data[0] = (uint8_t)((data0 >> 24) & 0xFFU);
    message->data[1] = (uint8_t)((data0 >> 16) & 0xFFU);
    message->data[2] = (uint8_t)((data0 >> 8) & 0xFFU);
    message->data[3] = (uint8_t)(data0 & 0xFFU);
    message->data[4] = (uint8_t)((data1 >> 24) & 0xFFU);
    message->data[5] = (uint8_t)((data1 >> 16) & 0xFFU);
    message->data[6] = (uint8_t)((data1 >> 8) & 0xFFU);
    message->data[7] = (uint8_t)(data1 & 0xFFU);
    
    /* Read TIMER to unlock message buffers */
    dummy = base->TIMER;
    (void)dummy;
    
    /* Clear interrupt flag */
    base->IFLAG1 = mbMask;
    
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
    uint32_t mbOffset;
    uint32_t cs, id;
    
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
    mbOffset = mbIndex * MSG_BUF_SIZE;
    
    /* Configure ID word */
    if (filter->idType == CAN_ID_EXT) {
        id = (filter->id << CAN_ID_EXT_SHIFT) & CAN_ID_EXT_MASK;
    } else {
        id = (filter->id << CAN_ID_STD_SHIFT) & CAN_ID_STD_MASK;
    }
    base->RAMn[mbOffset + 1] = id;
    
    /* Configure CS word as RX empty */
    cs = (CAN_CS_CODE_RX_EMPTY << CAN_CS_CODE_SHIFT);
    
    if (filter->idType == CAN_ID_EXT) {
        cs |= CAN_WMBn_CS_IDE_MASK;
    }
    
    base->RAMn[mbOffset + 0] = cs;
    
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
  
    CAN_Type *base = s_canBases[instance];
    base->IMASK1 |= (1UL << mbIndex);
    
    
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
    uint32_t mbOffset;
    
    if (instance >= CAN_INSTANCE_COUNT || mbIndex >= CAN_MB_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[instance];
    mbOffset = mbIndex * MSG_BUF_SIZE;
    
    /* Set abort code */
    base->RAMn[mbOffset + 0] = (CAN_CS_CODE_TX_ABORT << CAN_CS_CODE_SHIFT);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Check if MB is busy
 */
status_t CAN_IsMbBusy(uint8_t instance, uint8_t mbIndex, bool *isBusy)
{
    CAN_Type *base;
    uint32_t mbOffset;
    uint32_t cs;
    uint32_t code;
    
    if (instance >= CAN_INSTANCE_COUNT || 
        mbIndex >= CAN_MB_COUNT || isBusy == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    base = s_canBases[instance];
    mbOffset = mbIndex * MSG_BUF_SIZE;
    
    cs = base->RAMn[mbOffset + 0];
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
    timing->preDiv = 0U;
    timing->propSeg = 6U;       /* Propagation segment */
    timing->phaseSeg1 = 3U;     /* Phase segment 1 */
    timing->phaseSeg2 = 3U;     /* Phase segment 2 */
    timing->rJumpWidth = 3U;    /* Resynchronization jump width */
    
    return STATUS_SUCCESS;
}

/**
 * @brief Setup TX Mailbox (Public API)
 */
status_t CAN_SetupTxMailbox(uint8_t instance, uint8_t mbIndex)
{
    return CAN_ConfigTxMailbox(instance, mbIndex);
}

/**
 * @brief Setup RX Mailbox with filter (Public API)
 */
status_t CAN_SetupRxMailbox(uint8_t instance, uint8_t mbIndex, 
                            uint32_t id, can_id_type_t idType, uint32_t mask)
{
    return CAN_ConfigRxMailbox(instance, mbIndex, id, idType, mask);
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
static void CAN_EnableClock(uint8_t instance, can_clk_src_t clockSource)
{
//	DEV_ASSERT(instance > 2U);
    CAN_Type *base;
    
    base = s_canBases[instance];
    /* Get PCC register address */
    if (instance == 0U) {
        PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;
    } else if (instance == 1U) {
    	PCC->PCCn[PCC_FlexCAN1_INDEX] |= PCC_PCCn_CGC_MASK;
    } else {
    	PCC->PCCn[PCC_FlexCAN2_INDEX] |= PCC_PCCn_CGC_MASK;
    }

    /* MDIS=1: Disable module before selecting clock */
    base->MCR 	|= CAN_MCR_MDIS_MASK; 

    /* Select clock source in CTRL1*/ 
    if(clockSource == CAN_CLK_SRC_SOSCDIV2){
    	base->CTRL1 &=~ CAN_CTRL1_CLKSRC_MASK; /* CLKSRC=0: Clock Source = oscillator (8 MHz) */
    }else{
    	base->CTRL1 |= CAN_CTRL1_CLKSRC_MASK; /* CLKSRC=1: Clock Source = oscillator (8 MHz) */
    }

    /* MDIS=0; Enable module config. (Sets FRZ, HALT)*/
    base->MCR 	&= ~CAN_MCR_MDIS_MASK; 


}

/**
 * @brief Initialize all Message Buffers to inactive
 */
static void CAN_InitMessageBuffers(CAN_Type *base)
{
    uint32_t i;
    
    /* Clear all 32 msg bufs x 4 words/msg buf = 128 words */
    for (i = 0; i < 128; i++) {
        base->RAMn[i] = 0;
    }
    
    /* In FRZ mode, init CAN 16 msg buf filters */
    for (i = 0; i < 16; i++) {
        /* Check all ID bits for incoming messages */
        base->RXIMR[i] = 0xFFFFFFFFUL;
    }
    
    /* Clear all interrupt flags 
        This reg is w1c*/
    base->IFLAG1 = 0xFFFFFFFFUL;
}

/**
 * @brief Configure TX Mailbox
 * @param instance CAN instance number (0-2)
 * @param mbIndex Message Buffer index to configure for TX
 * @return STATUS_SUCCESS if successful, error code otherwise
 */
static status_t CAN_ConfigTxMailbox(uint8_t instance, uint8_t mbIndex)
{
    CAN_Type *base;
    uint32_t mbOffset;
    
    /* Validate parameters */
    if (instance >= CAN_INSTANCE_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    if (mbIndex >= CAN_MB_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    if (!s_canInitialized[instance]) {
        return STATUS_NOT_INITIALIZED;
    }
    
    base = s_canBases[instance];
    mbOffset = mbIndex * MSG_BUF_SIZE;
    
    /* Enter freeze mode to configure */
    if (CAN_EnterFreezeMode(base) != STATUS_SUCCESS) {
        return STATUS_TIMEOUT;
    }
    
    /* Clear Message Buffer */
    base->RAMn[mbOffset + 0] = 0U;  /* CS - set to INACTIVE */
    base->RAMn[mbOffset + 1] = 0U;  /* ID */
    base->RAMn[mbOffset + 2] = 0U;  /* DATA0 */
    base->RAMn[mbOffset + 3] = 0U;  /* DATA1 */
    
    /* Set MB code to TX_INACTIVE */
    base->RAMn[mbOffset + 0] = (CAN_CS_CODE_TX_INACTIVE << CAN_CS_CODE_SHIFT);
    
    /* Clear interrupt flag */
    base->IFLAG1 = (1UL << mbIndex);
    
    /* Exit freeze mode */
    if (CAN_ExitFreezeMode(base) != STATUS_SUCCESS) {
        return STATUS_TIMEOUT;
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Configure RX Mailbox
 * @param instance CAN instance number (0-2)
 * @param mbIndex Message Buffer index to configure for RX
 * @param id CAN ID to filter
 * @param idType Standard or Extended ID
 * @param mask Filter mask (1=care, 0=don't care)
 * @return STATUS_SUCCESS if successful, error code otherwise
 */
static status_t CAN_ConfigRxMailbox(uint8_t instance, uint8_t mbIndex, 
                                    uint32_t id, can_id_type_t idType, uint32_t mask)
{
    CAN_Type *base;
    uint32_t mbOffset;
    uint32_t cs, idWord;
    
    /* Validate parameters */
    if (instance >= CAN_INSTANCE_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    if (mbIndex >= CAN_MB_COUNT) {
        return STATUS_INVALID_PARAM;
    }
    
    if (!s_canInitialized[instance]) {
        return STATUS_NOT_INITIALIZED;
    }
    
    base = s_canBases[instance];
    mbOffset = mbIndex * MSG_BUF_SIZE;
    
    /* Enter freeze mode to configure */
    if (CAN_EnterFreezeMode(base) != STATUS_SUCCESS) {
        return STATUS_TIMEOUT;
    }
    
    /* Clear Message Buffer */
    base->RAMn[mbOffset + 0] = 0U;  /* CS */
    base->RAMn[mbOffset + 1] = 0U;  /* ID */
    base->RAMn[mbOffset + 2] = 0U;  /* DATA0 */
    base->RAMn[mbOffset + 3] = 0U;  /* DATA1 */
    
    /* Configure ID word */
    if (idType == CAN_ID_EXT) {
        idWord = (id << CAN_ID_EXT_SHIFT) & CAN_ID_EXT_MASK;
    } else {
        idWord = (id << CAN_ID_STD_SHIFT) & CAN_ID_STD_MASK;
    }
    base->RAMn[mbOffset + 1] = idWord;
    
    /* Configure individual mask */
    base->RXIMR[mbIndex] = mask;
    
    /* Configure CS word as RX EMPTY */
    cs = (CAN_CS_CODE_RX_EMPTY << CAN_CS_CODE_SHIFT);
    if (idType == CAN_ID_EXT) {
        cs |= CAN_WMBn_CS_IDE_MASK;
    }
    base->RAMn[mbOffset + 0] = cs;
    
    /* Clear interrupt flag */
    base->IFLAG1 = (1UL << mbIndex);
    
    /* Exit freeze mode */
    if (CAN_ExitFreezeMode(base) != STATUS_SUCCESS) {
        return STATUS_TIMEOUT;
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get clock frequency from clock source
 */
static uint32_t CAN_GetClockFrequency(can_clk_src_t clockSource)
{
    uint32_t freq;
    
    switch (clockSource) {
        case CAN_CLK_SRC_SOSCDIV2:
            freq = 4000000U;    /* 8 MHz / 2 = 4 MHz typical */
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
