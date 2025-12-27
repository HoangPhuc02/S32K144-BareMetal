/**
 * @file    dma.h
 * @brief   DMA (Direct Memory Access) driver for S32K144
 * @details
 * Provides a lightweight API to configure eDMA and DMAMUX so data can move between
 * memory and peripherals without CPU intervention. The driver exposes helpers for:
 * - Channel configuration (source/destination addresses, transfer sizes, loop counts)
 * - Interrupt management and callback registration
 * - Channel priority management
 * - Blocking memory copy convenience transfers
 *
 * Using DMA offloads repetitive data movement from the CPU, reducing latency and
 * allowing peripherals such as UART, SPI, CAN, ADC, etc. to operate autonomously.
 *
 * @note Enable both DMA and DMAMUX clocks before configuring channels.
 * @warning Source and destination addresses must follow the alignment requirements
 *          of the selected transfer size.
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

/** @brief Number of DMA channels available on S32K144 */
#define DMA_MAX_CHANNELS    (16U)

/**
 * @brief DMA Transfer Size
 * @details Data size for each transfer (minor loop)
 */
typedef enum {
    DMA_TRANSFER_SIZE_1B   = 0U,    /**< 1 byte (8-bit) transfer */
    DMA_TRANSFER_SIZE_2B   = 1U,    /**< 2 byte (16-bit) transfer */
    DMA_TRANSFER_SIZE_4B   = 2U,    /**< 4 byte (32-bit) transfer */
    DMA_TRANSFER_SIZE_16B  = 4U,    /**< 16 byte (128-bit) transfer (aligned source/destination) */
    DMA_TRANSFER_SIZE_32B  = 5U     /**< 32 byte (256-bit) transfer (aligned source/destination) */
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
 * @brief Initialize the DMA module.
 * @details Enables DMA/DMAMUX clocks, halts ongoing transfers, clears pending
 *          flags, and resets every TCD so channels start from a known state.
 * @retval STATUS_SUCCESS Module initialized successfully.
 * @retval STATUS_ERROR   Clock configuration failed.
 */
status_t DMA_Init(void);

/**
 * @brief Deinitialize the DMA module.
 * @details Stops all channels, disables their requests, clears DMAMUX sources,
 *          and gates the DMA clock to save power.
 * @retval STATUS_SUCCESS Module disabled successfully.
 * @retval STATUS_ERROR   Module was not initialized beforehand.
 */
status_t DMA_Deinit(void);

/**
 * @brief Configure a DMA channel.
 * @param[in] config Pointer to the channel configuration structure.
 * @retval STATUS_SUCCESS Channel configured successfully.
 * @retval STATUS_ERROR   Invalid parameters or driver not initialized.
 * @note Source and destination addresses must be aligned to the selected
 *       transfer size and the channel must be idle while being reconfigured.
 */
status_t DMA_ConfigChannel(const dma_channel_config_t *config);

/**
 * @brief Start a DMA transfer.
 * @param[in] channel DMA channel index (0-15).
 * @retval STATUS_SUCCESS Transfer request accepted.
 * @retval STATUS_ERROR   Channel is invalid or driver uninitialized.
 */
status_t DMA_StartChannel(uint8_t channel);

/**
 * @brief Stop a DMA transfer.
 * @param[in] channel DMA channel index (0-15).
 * @retval STATUS_SUCCESS Request disabled successfully.
 * @retval STATUS_ERROR   Channel is invalid.
 */
status_t DMA_StopChannel(uint8_t channel);

/**
 * @brief Check whether a DMA channel is active.
 * @param[in] channel DMA channel index (0-15).
 * @retval true  Channel is currently moving data.
 * @retval false Channel is idle or invalid.
 */
bool DMA_IsChannelActive(uint8_t channel);

/**
 * @brief Check whether a DMA channel completed its major loop.
 * @param[in] channel DMA channel index (0-15).
 * @retval true  Transfer finished and DONE flag is set.
 * @retval false Transfer not finished or channel invalid.
 */
bool DMA_IsChannelDone(uint8_t channel);

/**
 * @brief Clear the DONE flag for a channel.
 * @param[in] channel DMA channel index (0-15).
 * @retval STATUS_SUCCESS Flag cleared successfully.
 * @retval STATUS_ERROR   Channel invalid.
 */
status_t DMA_ClearDone(uint8_t channel);

/**
 * @brief Register a callback for a channel.
 * @param[in] channel  DMA channel index (0-15).
 * @param[in] callback Function invoked on major-loop complete or error.
 * @param[in] userData Pointer passed back to the callback.
 * @retval STATUS_SUCCESS Callback installed successfully.
 * @retval STATUS_ERROR   Channel invalid or callback NULL.
 */
status_t DMA_InstallCallback(uint8_t channel, dma_callback_t callback, void *userData);

/**
 * @brief Set a DMA channel priority level.
 * @param[in] channel  DMA channel index (0-15).
 * @param[in] priority Priority level (0 lowest .. 15 highest).
 * @retval STATUS_SUCCESS Priority updated successfully.
 * @retval STATUS_ERROR   Channel invalid.
 */
status_t DMA_SetChannelPriority(uint8_t channel, uint8_t priority);

/**
 * @brief Read the remaining major loop count.
 * @param[in]  channel DMA channel index (0-15).
 * @param[out] count   Pointer that receives the remaining iteration count.
 * @retval STATUS_SUCCESS Value returned successfully.
 * @retval STATUS_ERROR   Channel invalid or count is NULL.
 */
status_t DMA_GetRemainingMajorLoops(uint8_t channel, uint16_t *count);

/**
 * @brief Enable the major-loop-complete interrupt for a channel.
 * @param[in] channel DMA channel index (0-15).
 * @retval STATUS_SUCCESS Interrupt enabled.
 * @retval STATUS_ERROR   Channel invalid.
 */
status_t DMA_EnableChannelInterrupt(uint8_t channel);

/**
 * @brief Disable the major-loop-complete interrupt for a channel.
 * @param[in] channel DMA channel index (0-15).
 * @retval STATUS_SUCCESS Interrupt disabled.
 * @retval STATUS_ERROR   Channel invalid.
 */
status_t DMA_DisableChannelInterrupt(uint8_t channel);

/**
 * @brief Common DMA ISR handler helper.
 * @param[in] channel DMA channel index serviced by the ISR.
 * @note Clears the interrupt/DONE flags before invoking the callback.
 */
void DMA_IRQHandler(uint8_t channel);

/**
 * @brief Perform a blocking memory-to-memory copy using DMA.
 * @param[in]  channel DMA channel index (0-15).
 * @param[in]  src     Source buffer address.
 * @param[out] dest    Destination buffer address.
 * @param[in]  size    Number of bytes to copy (must be multiple of 4).
 * @retval STATUS_SUCCESS Transfer completed.
 * @retval STATUS_ERROR   Invalid parameters or timeout while waiting.
 * @retval STATUS_TIMEOUT Transfer did not finish before timeout.
 */
status_t DMA_MemCopy(uint8_t channel, const void *src, void *dest, uint32_t size);

/** @} */ /* End of DMA_Functions */

#endif /* DMA_H */
