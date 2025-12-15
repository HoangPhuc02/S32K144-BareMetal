/**
 * @file    dma.c
 * @brief   DMA (Direct Memory Access) Driver Implementation for S32K144
 * @details
 * Implementation của DMA driver cho phép:
 * - Transfer dữ liệu tự động giữa memory và peripherals
 * - Giảm tải CPU bằng cách thực hiện data transfer không cần CPU can thiệp
 * - Hỗ trợ interrupts và callbacks
 * 
 * DMA Concepts:
 * - Minor Loop: Một lần transfer nhỏ (vd: 4 bytes)
 * - Major Loop: Tập hợp nhiều minor loops (vd: 256 lần x 4 bytes = 1024 bytes)
 * - TCD (Transfer Control Descriptor): Cấu trúc điều khiển cho mỗi kênh
 * 
 * @author  PhucPH32
 * @date    27/11/2025
 * @version 1.0
 * 
 * @note    Đảm bảo enable clock cho DMA và DMAMUX trước khi sử dụng
 * @warning Không thay đổi cấu hình khi DMA đang active
 * 
 * @par Change Log:
 * - Version 1.0 (27/11/2025): Khởi tạo driver DMA
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

/** @brief Macro để enable clock cho module */
#define PCC_CGC_ENABLE(offset)  (*((volatile uint32_t *)(PCC_BASE + ((offset) << 2))) |= (1UL << 30))

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Callback function pointers cho mỗi kênh DMA */
static dma_callback_t s_dmaCallbacks[DMA_MAX_CHANNELS] = {NULL};

/** @brief User data pointers cho mỗi kênh DMA */
static void *s_dmaUserData[DMA_MAX_CHANNELS] = {NULL};

/** @brief Cờ đánh dấu DMA đã được khởi tạo */
static bool s_dmaInitialized = false;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Enable clock cho DMA module
 * @return STATUS_SUCCESS nếu thành công
 */
static status_t DMA_EnableClock(void);

/**
 * @brief Validate channel number
 * @param channel Số kênh DMA
 * @return true nếu channel hợp lệ (0-15)
 */
static inline bool DMA_IsValidChannel(uint8_t channel);

/**
 * @brief Reset TCD về trạng thái mặc định
 * @param channel Số kênh DMA
 */
static void DMA_ResetTCD(uint8_t channel);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Enable clock cho DMA và DMAMUX
 */
static status_t DMA_EnableClock(void)
{
    /* Enable clock cho DMA module */
    PCC_CGC_ENABLE(PCC_DMA_OFFSET);
    
    /* Enable clock cho DMAMUX module */
    PCC_CGC_ENABLE(PCC_DMAMUX_OFFSET);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Kiểm tra channel có hợp lệ không
 */
static inline bool DMA_IsValidChannel(uint8_t channel)
{
    return (channel < DMA_MAX_CHANNELS);
}

/**
 * @brief Reset TCD (Transfer Control Descriptor) về giá trị mặc định
 */
static void DMA_ResetTCD(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return;
    }
    
    /* Reset tất cả các thanh ghi TCD về 0 */
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
 * @brief Khởi tạo DMA module
 */
status_t DMA_Init(void)
{
    uint8_t i;
    
    /* Enable clock cho DMA và DMAMUX */
    DMA_EnableClock();
    
    /* Cancel tất cả các transfer đang thực hiện */
    REG_WRITE32(DMA_BASE + 0x0000, DMA_CR_CX_MASK);
    
    /* Halt DMA operations để cấu hình an toàn */
    REG_BIT_SET32(DMA_BASE + 0x0000, DMA_CR_HALT_MASK);
    
    /* Enable Debug mode - DMA hoạt động khi CPU ở debug mode */
    REG_BIT_SET32(DMA_BASE + 0x0000, DMA_CR_EDBG_MASK);
    
    /* Enable Round Robin Channel Arbitration - Ưu tiên vòng tròn */
    REG_BIT_SET32(DMA_BASE + 0x0000, DMA_CR_ERCA_MASK);
    
    /* Clear Halt bit để DMA có thể hoạt động */
    REG_BIT_CLEAR32(DMA_BASE + 0x0000, DMA_CR_HALT_MASK);
    
    /* Clear tất cả error flags */
    REG_WRITE32(DMA_BASE + 0x002C, 0xFFFFFFFFU);
    
    /* Clear tất cả interrupt flags */
    REG_WRITE32(DMA_BASE + 0x0024, 0xFFFFFFFFU);
    
    /* Reset tất cả các kênh DMA */
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
    
    /* Halt tất cả DMA operations */
    REG_BIT_SET32(DMA_BASE + 0x0000, DMA_CR_HALT_MASK);
    
    /* Disable tất cả các kênh */
    for (i = 0U; i < DMA_MAX_CHANNELS; i++) {
        DMA_StopChannel(i);
        DMA_ResetTCD(i);
        DMAMUX->CHCFG[i] = 0U;
    }
    
    s_dmaInitialized = false;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Cấu hình một kênh DMA
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
    
    /* Disable channel trước khi cấu hình */
    DMA_StopChannel(channel);
    
    /* Reset TCD */
    DMA_ResetTCD(channel);
    
    /* Cấu hình Source Address */
    DMA->TCD[channel].SADDR = config->sourceAddr;
    
    /* Cấu hình Source Offset */
    DMA->TCD[channel].SOFF = config->sourceOffset;
    
    /* Cấu hình Source Last Address Adjustment */
    DMA->TCD[channel].SLAST = config->sourceLastAddrAdjust;
    
    /* Cấu hình Destination Address */
    DMA->TCD[channel].DADDR = config->destAddr;
    
    /* Cấu hình Destination Offset */
    DMA->TCD[channel].DOFF = config->destOffset;
    
    /* Cấu hình Destination Last Address Adjustment */
    DMA->TCD[channel].DLAST_SGA = config->destLastAddrAdjust;
    
    /* Cấu hình Transfer Attributes (ATTR) */
    attr = 0U;
    /* Source size */
    attr |= ((uint16_t)config->transferSize << DMA_TCD_ATTR_SSIZE_SHIFT) & DMA_TCD_ATTR_SSIZE_MASK;
    /* Destination size */
    attr |= ((uint16_t)config->transferSize << DMA_TCD_ATTR_DSIZE_SHIFT) & DMA_TCD_ATTR_DSIZE_MASK;
    DMA->TCD[channel].ATTR = attr;
    
    /* Cấu hình Minor Loop Byte Count */
    DMA->TCD[channel].NBYTES_MLNO = config->minorLoopBytes;
    
    /* Cấu hình Major Loop Count (CITER và BITER phải giống nhau) */
    DMA->TCD[channel].CITER_ELINKNO = config->majorLoopCount;
    DMA->TCD[channel].BITER_ELINKNO = config->majorLoopCount;
    
    /* Cấu hình Control and Status Register (CSR) */
    csr = 0U;
    
    if (config->enableInterrupt) {
        csr |= DMA_TCD_CSR_INTMAJOR_MASK;  /* Enable interrupt khi major loop complete */
    }
    
    if (config->disableRequestAfterDone) {
        csr |= DMA_TCD_CSR_DREQ_MASK;  /* Disable request sau khi hoàn thành */
    }
    
    DMA->TCD[channel].CSR = csr;
    
    /* Cấu hình Channel Priority */
    DMA_SetChannelPriority(channel, (uint8_t)config->priority);
    
    /* Cấu hình DMAMUX */
    /* Disable channel trước */
    DMAMUX->CHCFG[channel] = 0U;
    
    /* Enable channel với source */
    DMAMUX->CHCFG[channel] = (uint8_t)(
        DMAMUX_CHCFG_ENBL_MASK |  /* Enable channel */
        ((uint8_t)config->source & DMAMUX_CHCFG_SOURCE_MASK)  /* Set source */
    );
    
    return STATUS_SUCCESS;
}

/**
 * @brief Start DMA transfer cho một kênh
 */
status_t DMA_StartChannel(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (!s_dmaInitialized) {
        return STATUS_ERROR;
    }
    
    /* Clear DONE flag nếu có */
    DMA_ClearDone(channel);
    
    /* Enable DMA request cho kênh này */
    REG_WRITE32(DMA_BASE + 0x001B, channel); /* Set Enable Request */
    
    /* Nếu là software trigger (ALWAYS_ON source), start bằng cách set START bit */
    if ((DMAMUX->CHCFG[channel] & DMAMUX_CHCFG_SOURCE_MASK) >= 60U) {
        DMA->TCD[channel].CSR |= DMA_TCD_CSR_START_MASK;
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Stop DMA transfer cho một kênh
 */
status_t DMA_StopChannel(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Disable DMA request cho kênh này */
    REG_WRITE32(DMA_BASE + 0x001A, channel); /* Clear Enable Request */
    
    /* Disable DMAMUX channel */
    DMAMUX->CHCFG[channel] &= ~DMAMUX_CHCFG_ENBL_MASK;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Kiểm tra kênh DMA có đang active không
 */
bool DMA_IsChannelActive(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return false;
    }
    
    return ((DMA->TCD[channel].CSR & DMA_TCD_CSR_ACTIVE_MASK) != 0U);
}

/**
 * @brief Kiểm tra kênh DMA đã hoàn thành chưa
 */
bool DMA_IsChannelDone(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return false;
    }
    
    return ((DMA->TCD[channel].CSR & DMA_TCD_CSR_DONE_MASK) != 0U);
}

/**
 * @brief Clear cờ DONE của kênh DMA
 */
status_t DMA_ClearDone(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Write to CDNE register để clear DONE bit */
    REG_WRITE32(DMA_BASE + 0x001C, channel);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Đăng ký callback function cho kênh DMA
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
 * @brief Thiết lập ưu tiên cho kênh DMA
 */
status_t DMA_SetChannelPriority(uint8_t channel, uint8_t priority)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* DCHPRI register: bits 3-0 là priority (0=low, 15=high) */
    DMA->DCHPRI[channel] = (priority & 0x0FU);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Lấy số lượng major loops còn lại
 */
status_t DMA_GetRemainingMajorLoops(uint8_t channel, uint16_t *count)
{
    if (!DMA_IsValidChannel(channel) || (count == NULL)) {
        return STATUS_ERROR;
    }
    
    /* Đọc CITER (Current Iteration Count) */
    *count = DMA->TCD[channel].CITER_ELINKNO;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Enable interrupt cho kênh DMA
 */
status_t DMA_EnableChannelInterrupt(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Set INTMAJOR bit trong CSR */
    DMA->TCD[channel].CSR |= DMA_TCD_CSR_INTMAJOR_MASK;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Disable interrupt cho kênh DMA
 */
status_t DMA_DisableChannelInterrupt(uint8_t channel)
{
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Clear INTMAJOR bit trong CSR */
    DMA->TCD[channel].CSR &= ~DMA_TCD_CSR_INTMAJOR_MASK;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Xử lý DMA interrupt (gọi từ ISR)
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
    
    /* Gọi callback nếu có */
    if (s_dmaCallbacks[channel] != NULL) {
        s_dmaCallbacks[channel](channel, s_dmaUserData[channel]);
    }
}

/**
 * @brief Copy dữ liệu bằng DMA (blocking)
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
    
    /* Cấu hình DMA cho memory-to-memory transfer */
    config.channel = channel;
    config.source = DMAMUX_SRC_ALWAYS_ON_60;  /* Always-on source cho software trigger */
    config.transferType = DMA_TRANSFER_MEM_TO_MEM;
    config.transferSize = DMA_TRANSFER_SIZE_4B;  /* 4 bytes mỗi lần */
    config.priority = DMA_PRIORITY_NORMAL;
    
    config.sourceAddr = (uint32_t)src;
    config.sourceOffset = 4;  /* Tăng 4 bytes sau mỗi transfer */
    config.sourceLastAddrAdjust = 0;
    
    config.destAddr = (uint32_t)dest;
    config.destOffset = 4;  /* Tăng 4 bytes sau mỗi transfer */
    config.destLastAddrAdjust = 0;
    
    config.minorLoopBytes = 4;  /* Transfer 4 bytes mỗi minor loop */
    config.majorLoopCount = (uint16_t)(size / 4U);  /* Số lần lặp = size / 4 */
    
    config.enableInterrupt = false;  /* Không dùng interrupt (polling mode) */
    config.disableRequestAfterDone = true;
    
    /* Cấu hình kênh DMA */
    if (DMA_ConfigChannel(&config) != STATUS_SUCCESS) {
        return STATUS_ERROR;
    }
    
    /* Start transfer */
    if (DMA_StartChannel(channel) != STATUS_SUCCESS) {
        return STATUS_ERROR;
    }
    
    /* Chờ cho đến khi transfer hoàn thành (blocking) */
    while (!DMA_IsChannelDone(channel) && (timeout > 0U)) {
        timeout--;
    }
    
    if (timeout == 0U) {
        /* Timeout - transfer không hoàn thành */
        DMA_StopChannel(channel);
        return STATUS_TIMEOUT;
    }
    
    /* Clear DONE flag */
    DMA_ClearDone(channel);
    
    return STATUS_SUCCESS;
}
