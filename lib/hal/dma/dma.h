/**
 * @file    dma.h
 * @brief   DMA (Direct Memory Access) Driver for S32K144
 * @details
 * DMA driver provides APIs to:
 * - Configure and control DMA channels
 * - Set up memory-to-memory, memory-to-peripheral, peripheral-to-memory transfers
 * - Manage interrupts and callbacks
 * - Support scatter-gather and channel linking
 * 
 * DMA helps reduce CPU load by automatically transferring data between:
 * - Memory to Memory (mem copy)
 * - Memory to Peripheral (UART, SPI, ADC, etc.)
 * - Peripheral to Peripheral
 * 
 * @author  PhucPH32
 * @date    27/11/2025
 * @version 1.0
 * 
 * @note    
 * - Must enable clock for DMA and DMAMUX before use
 * - Each DMA channel has its own Transfer Control Descriptor (TCD)
 * - Minor loop: one small transfer (e.g., 1 byte, 2 bytes, 4 bytes)
 * - Major loop: collection of multiple minor loops
 * 
 * @warning 
 * - Source and destination addresses must be aligned according to transfer size
 * - Cannot change configuration while DMA is active
 * 
 * @par Change Log:
 * - Version 1.0 (27/11/2025): Initialize DMA driver
 */

#ifndef DMA_H
#define DMA_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "dma_reg.h"
#include "status.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup DMA_Definitions DMA Definitions
 * @{
 */

/** @brief Maximum DMA channels */
#define DMA_MAX_CHANNELS    (16U)

/**
 * @brief DMA Transfer Size
 * @details Data size for each transfer (minor loop)
 */
typedef enum {
    DMA_TRANSFER_SIZE_1B   = 0U,    /**< 1 byte (8-bit) */
    DMA_TRANSFER_SIZE_2B   = 1U,    /**< 2 bytes (16-bit) */
    DMA_TRANSFER_SIZE_4B   = 2U,    /**< 4 bytes (32-bit) */
    DMA_TRANSFER_SIZE_16B  = 4U,    /**< 16 bytes (128-bit) - chá»‰ dÃ¹ng cho aligned transfers */
    DMA_TRANSFER_SIZE_32B  = 5U     /**< 32 bytes (256-bit) - chá»‰ dÃ¹ng cho aligned transfers */
} dma_transfer_size_t;

/**
 * @brief DMA Transfer Type
 * @details DMA transfer type
 */
typedef enum {
    DMA_TRANSFER_MEM_TO_MEM = 0U,   /**< Memory to Memory */
    DMA_TRANSFER_MEM_TO_PERIPH,     /**< Memory to Peripheral */
    DMA_TRANSFER_PERIPH_TO_MEM,     /**< Peripheral to Memory */
    DMA_TRANSFER_PERIPH_TO_PERIPH   /**< Peripheral to Peripheral */
} dma_transfer_type_t;

/**
 * @brief DMA Channel Priority
 * @details DMA channel priority level (0 = lowest, 15 = highest)
 */
typedef enum {
    DMA_PRIORITY_LOW    = 0U,       /**< Low priority */
    DMA_PRIORITY_NORMAL = 8U,       /**< Normal priority */
    DMA_PRIORITY_HIGH   = 15U       /**< High priority */
} dma_channel_priority_t;

/**
 * @brief DMA Interrupt Flags
 * @details DMA interrupt flags
 */
typedef enum {
    DMA_INT_MAJOR_COMPLETE = 0x02U, /**< Major loop complete interrupt */
    DMA_INT_HALF_COMPLETE  = 0x04U, /**< Half major loop complete interrupt */
    DMA_INT_ERROR          = 0x80U  /**< Error interrupt */
} dma_interrupt_t;

/**
 * @brief DMA Configuration Structure
 * @details Configuration structure for one DMA channel
 */
typedef struct {
    uint8_t channel;                    /**< DMA channel number (0-15) */
    dmamux_source_t source;             /**< Request source from DMAMUX */
    dma_transfer_type_t transferType;   /**< Transfer type */
    dma_transfer_size_t transferSize;   /**< Transfer size */
    dma_channel_priority_t priority;    /**< Channel priority */
    
    uint32_t sourceAddr;                /**< Source address */
    int16_t sourceOffset;               /**< Source offset after each minor loop */
    int32_t sourceLastAddrAdjust;       /**< Source address adjustment after major loop */
    
    uint32_t destAddr;                  /**< Destination address */
    int16_t destOffset;                 /**< Destination offset after each minor loop */
    int32_t destLastAddrAdjust;         /**< Destination address adjustment after major loop */
    
    uint32_t minorLoopBytes;            /**< Number of bytes to transfer in each minor loop */
    uint16_t majorLoopCount;            /**< Number of major loop iterations */
    
    bool enableInterrupt;               /**< Enable interrupt when major loop completes */
    bool disableRequestAfterDone;       /**< Disable DMA request after completion */
    
} dma_channel_config_t;

/**
 * @brief DMA Callback Function Type
 * @details Function pointer for callback when DMA completes or encounters error
 * @param channel DMA channel number
 * @param userData Pointer to user data
 */
typedef void (*dma_callback_t)(uint8_t channel, void *userData);

/** @} */ /* End of DMA_Definitions */

/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @defgroup DMA_Functions DMA Functions
 * @{
 */

/**
 * @brief Khá»Ÿi táº¡o module DMA
 * @details
 * - Enable clock cho DMA vÃ  DMAMUX
 * - Reset táº¥t cáº£ cÃ¡c kÃªnh DMA vá»� tráº¡ng thÃ¡i máº·c Ä‘á»‹nh
 * - XÃ³a táº¥t cáº£ interrupt flags
 * 
 * @return STATUS_SUCCESS náº¿u thÃ nh cÃ´ng
 * 
 * @note Pháº£i gá»�i hÃ m nÃ y trÆ°á»›c khi sá»­ dá»¥ng báº¥t ká»³ chá»©c nÄƒng DMA nÃ o
 * 
 * @code
 * // Khá»Ÿi táº¡o DMA module
 * DMA_Init();
 * @endcode
 */
status_t DMA_Init(void);

/**
 * @brief Deinitialize module DMA
 * @details
 * - Disable táº¥t cáº£ cÃ¡c kÃªnh DMA
 * - Disable clock cho DMA vÃ  DMAMUX
 * 
 * @return STATUS_SUCCESS náº¿u thÃ nh cÃ´ng
 */
status_t DMA_Deinit(void);

/**
 * @brief Cáº¥u hÃ¬nh má»™t kÃªnh DMA
 * @details
 * Thiáº¿t láº­p Ä‘áº§y Ä‘á»§ cÃ¡c tham sá»‘ cho má»™t kÃªnh DMA:
 * - Source/Destination addresses
 * - Transfer size vÃ  count
 * - Interrupts vÃ  callbacks
 * 
 * @param[in] config Con trá»� tá»›i cáº¥u trÃºc cáº¥u hÃ¬nh DMA
 * 
 * @return STATUS_SUCCESS náº¿u cáº¥u hÃ¬nh thÃ nh cÃ´ng
 * @return STATUS_ERROR náº¿u tham sá»‘ khÃ´ng há»£p lá»‡
 * 
 * @note
 * - Ä�á»‹a chá»‰ source/dest pháº£i aligned vá»›i transfer size
 * - KhÃ´ng Ä‘Æ°á»£c cáº¥u hÃ¬nh khi kÃªnh Ä‘ang active
 * 
 * @code
 * // Cáº¥u hÃ¬nh DMA channel 0 Ä‘á»ƒ copy memory
 * dma_channel_config_t config = {
 *     .channel = 0,
 *     .source = DMAMUX_SRC_ALWAYS_ON_60,
 *     .transferType = DMA_TRANSFER_MEM_TO_MEM,
 *     .transferSize = DMA_TRANSFER_SIZE_4B,
 *     .priority = DMA_PRIORITY_NORMAL,
 *     .sourceAddr = (uint32_t)srcBuffer,
 *     .sourceOffset = 4,
 *     .sourceLastAddrAdjust = 0,
 *     .destAddr = (uint32_t)destBuffer,
 *     .destOffset = 4,
 *     .destLastAddrAdjust = 0,
 *     .minorLoopBytes = 4,
 *     .majorLoopCount = 256,
 *     .enableInterrupt = true,
 *     .disableRequestAfterDone = true
 * };
 * DMA_ConfigChannel(&config);
 * @endcode
 */
status_t DMA_ConfigChannel(const dma_channel_config_t *config);

/**
 * @brief Báº¯t Ä‘áº§u transfer DMA cho má»™t kÃªnh
 * @details
 * KÃ­ch hoáº¡t DMA request cho kÃªnh Ä‘Æ°á»£c chá»‰ Ä‘á»‹nh.
 * Ä�á»‘i vá»›i software-triggered DMA (ALWAYS_ON source), sáº½ start ngay láº­p tá»©c.
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * 
 * @return STATUS_SUCCESS náº¿u start thÃ nh cÃ´ng
 * @return STATUS_ERROR náº¿u channel khÃ´ng há»£p lá»‡ hoáº·c chÆ°a Ä‘Æ°á»£c cáº¥u hÃ¬nh
 * 
 * @code
 * // Start DMA channel 0
 * DMA_StartChannel(0);
 * @endcode
 */
status_t DMA_StartChannel(uint8_t channel);

/**
 * @brief Dá»«ng transfer DMA cho má»™t kÃªnh
 * @details Disable DMA request cho kÃªnh Ä‘Æ°á»£c chá»‰ Ä‘á»‹nh
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * 
 * @return STATUS_SUCCESS náº¿u stop thÃ nh cÃ´ng
 * @return STATUS_ERROR náº¿u channel khÃ´ng há»£p lá»‡
 */
status_t DMA_StopChannel(uint8_t channel);

/**
 * @brief Kiá»ƒm tra xem kÃªnh DMA cÃ³ Ä‘ang active khÃ´ng
 * @details Ä�á»�c bit ACTIVE trong TCD CSR register
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * 
 * @return true náº¿u kÃªnh Ä‘ang active
 * @return false náº¿u kÃªnh khÃ´ng active hoáº·c channel khÃ´ng há»£p lá»‡
 */
bool DMA_IsChannelActive(uint8_t channel);

/**
 * @brief Kiá»ƒm tra xem kÃªnh DMA Ä‘Ã£ hoÃ n thÃ nh chÆ°a
 * @details Ä�á»�c bit DONE trong TCD CSR register
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * 
 * @return true náº¿u transfer Ä‘Ã£ hoÃ n thÃ nh
 * @return false náº¿u chÆ°a hoÃ n thÃ nh hoáº·c channel khÃ´ng há»£p lá»‡
 */
bool DMA_IsChannelDone(uint8_t channel);

/**
 * @brief Clear cá»� DONE cá»§a kÃªnh DMA
 * @details XÃ³a bit DONE trong TCD CSR register
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * 
 * @return STATUS_SUCCESS náº¿u clear thÃ nh cÃ´ng
 * @return STATUS_ERROR náº¿u channel khÃ´ng há»£p lá»‡
 */
status_t DMA_ClearDone(uint8_t channel);

/**
 * @brief Ä�Äƒng kÃ½ callback function cho kÃªnh DMA
 * @details
 * Callback sáº½ Ä‘Æ°á»£c gá»�i khi:
 * - DMA transfer hoÃ n thÃ nh (major loop done)
 * - CÃ³ lá»—i xáº£y ra trong quÃ¡ trÃ¬nh transfer
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * @param[in] callback Function pointer cho callback
 * @param[in] userData Con trá»� tá»›i user data (cÃ³ thá»ƒ NULL)
 * 
 * @return STATUS_SUCCESS náº¿u Ä‘Äƒng kÃ½ thÃ nh cÃ´ng
 * @return STATUS_ERROR náº¿u channel khÃ´ng há»£p lá»‡ hoáº·c callback NULL
 * 
 * @code
 * void myDmaCallback(uint8_t channel, void *userData) {
 *     // Xá»­ lÃ½ khi DMA complete
 *     printf("DMA channel %d completed!\n", channel);
 * }
 * 
 * DMA_InstallCallback(0, myDmaCallback, NULL);
 * @endcode
 */
status_t DMA_InstallCallback(uint8_t channel, dma_callback_t callback, void *userData);

/**
 * @brief Thiáº¿t láº­p Æ°u tiÃªn cho kÃªnh DMA
 * @details
 * Khi nhiá»�u kÃªnh DMA request Ä‘á»“ng thá»�i, kÃªnh cÃ³ priority cao hÆ¡n sáº½ Ä‘Æ°á»£c phá»¥c vá»¥ trÆ°á»›c
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * @param[in] priority Má»©c Æ°u tiÃªn (0-15, vá»›i 15 lÃ  cao nháº¥t)
 * 
 * @return STATUS_SUCCESS náº¿u thiáº¿t láº­p thÃ nh cÃ´ng
 * @return STATUS_ERROR náº¿u channel khÃ´ng há»£p lá»‡
 */
status_t DMA_SetChannelPriority(uint8_t channel, uint8_t priority);

/**
 * @brief Láº¥y sá»‘ lÆ°á»£ng major loops cÃ²n láº¡i
 * @details Ä�á»�c giÃ¡ trá»‹ CITER (Current Iteration Count) tá»« TCD
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * @param[out] count Con trá»� Ä‘á»ƒ lÆ°u sá»‘ lÆ°á»£ng iterations cÃ²n láº¡i
 * 
 * @return STATUS_SUCCESS náº¿u Ä‘á»�c thÃ nh cÃ´ng
 * @return STATUS_ERROR náº¿u channel khÃ´ng há»£p lá»‡ hoáº·c count NULL
 */
status_t DMA_GetRemainingMajorLoops(uint8_t channel, uint16_t *count);

/**
 * @brief Enable interrupt cho kÃªnh DMA
 * @details Enable interrupt khi major loop complete
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * 
 * @return STATUS_SUCCESS náº¿u enable thÃ nh cÃ´ng
 * @return STATUS_ERROR náº¿u channel khÃ´ng há»£p lá»‡
 */
status_t DMA_EnableChannelInterrupt(uint8_t channel);

/**
 * @brief Disable interrupt cho kÃªnh DMA
 * @details Disable interrupt khi major loop complete
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * 
 * @return STATUS_SUCCESS náº¿u disable thÃ nh cÃ´ng
 * @return STATUS_ERROR náº¿u channel khÃ´ng há»£p lá»‡
 */
status_t DMA_DisableChannelInterrupt(uint8_t channel);

/**
 * @brief Xá»­ lÃ½ DMA interrupt (gá»�i tá»« ISR)
 * @details
 * HÃ m nÃ y xá»­ lÃ½ interrupt vÃ  gá»�i callback Ä‘Ã£ Ä‘Äƒng kÃ½.
 * Pháº£i Ä‘Æ°á»£c gá»�i trong DMA ISR.
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA (0-15)
 * 
 * @note HÃ m nÃ y sáº½ clear cÃ¡c interrupt flags tá»± Ä‘á»™ng
 */
void DMA_IRQHandler(uint8_t channel);

/**
 * @brief Copy dá»¯ liá»‡u tá»« source sang destination báº±ng DMA
 * @details
 * HÃ m tiá»‡n Ã­ch Ä‘á»ƒ thá»±c hiá»‡n memory-to-memory transfer.
 * HÃ m nÃ y sáº½:
 * - Tá»± Ä‘á»™ng cáº¥u hÃ¬nh kÃªnh DMA
 * - Start transfer
 * - Chá»� cho Ä‘áº¿n khi transfer hoÃ n thÃ nh (blocking)
 * 
 * @param[in] channel Sá»‘ kÃªnh DMA sá»­ dá»¥ng (0-15)
 * @param[in] src Ä�á»‹a chá»‰ nguá»“n
 * @param[out] dest Ä�á»‹a chá»‰ Ä‘Ã­ch
 * @param[in] size Sá»‘ bytes cáº§n copy
 * 
 * @return STATUS_SUCCESS náº¿u copy thÃ nh cÃ´ng
 * @return STATUS_ERROR náº¿u cÃ³ lá»—i xáº£y ra
 * 
 * @note
 * - HÃ m nÃ y lÃ  blocking (chá»� cho Ä‘áº¿n khi transfer xong)
 * - KhÃ´ng nÃªn dÃ¹ng trong interrupt context
 * 
 * @code
 * uint32_t src[256], dest[256];
 * // Copy 1024 bytes (256 x 4 bytes)
 * DMA_MemCopy(0, src, dest, 1024);
 * @endcode
 */
status_t DMA_MemCopy(uint8_t channel, const void *src, void *dest, uint32_t size);

/** @} */ /* End of DMA_Functions */

#endif /* DMA_H */
