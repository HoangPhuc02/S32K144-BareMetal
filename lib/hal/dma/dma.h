/**
 * @file    dma.h
 * @brief   DMA (Direct Memory Access) Driver for S32K144
 * @details
 * Driver DMA cung cấp các API để:
 * - Cấu hình và điều khiển các kênh DMA
 * - Thiết lập transfer memory-to-memory, memory-to-peripheral, peripheral-to-memory
 * - Quản lý interrupts và callbacks
 * - Hỗ trợ scatter-gather và channel linking
 * 
 * DMA giúp giảm tải CPU bằng cách tự động truyền dữ liệu giữa:
 * - Memory ↔ Memory (mem copy)
 * - Memory ↔ Peripheral (UART, SPI, ADC, etc.)
 * - Peripheral ↔ Peripheral
 * 
 * @author  PhucPH32
 * @date    27/11/2025
 * @version 1.0
 * 
 * @note    
 * - Phải enable clock cho DMA và DMAMUX trước khi sử dụng
 * - Mỗi kênh DMA có một Transfer Control Descriptor (TCD) riêng
 * - Minor loop: một lần truyền nhỏ (vd: 1 byte, 2 bytes, 4 bytes)
 * - Major loop: tập hợp nhiều minor loops
 * 
 * @warning 
 * - Địa chỉ source và destination phải aligned đúng với transfer size
 * - Không được thay đổi cấu hình khi DMA đang active
 * 
 * @par Change Log:
 * - Version 1.0 (27/11/2025): Khởi tạo driver DMA
 */

#ifndef DMA_H
#define DMA_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "dma_reg.h"
#include "lib/bsp/device/status.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup DMA_Definitions DMA Definitions
 * @{
 */

/** @brief Số kênh DMA tối đa */
#define DMA_MAX_CHANNELS    (16U)

/**
 * @brief DMA Transfer Size
 * @details Kích thước dữ liệu cho mỗi lần transfer (minor loop)
 */
typedef enum {
    DMA_TRANSFER_SIZE_1B   = 0U,    /**< 1 byte (8-bit) */
    DMA_TRANSFER_SIZE_2B   = 1U,    /**< 2 bytes (16-bit) */
    DMA_TRANSFER_SIZE_4B   = 2U,    /**< 4 bytes (32-bit) */
    DMA_TRANSFER_SIZE_16B  = 4U,    /**< 16 bytes (128-bit) - chỉ dùng cho aligned transfers */
    DMA_TRANSFER_SIZE_32B  = 5U     /**< 32 bytes (256-bit) - chỉ dùng cho aligned transfers */
} dma_transfer_size_t;

/**
 * @brief DMA Transfer Type
 * @details Loại transfer DMA
 */
typedef enum {
    DMA_TRANSFER_MEM_TO_MEM = 0U,   /**< Memory to Memory */
    DMA_TRANSFER_MEM_TO_PERIPH,     /**< Memory to Peripheral */
    DMA_TRANSFER_PERIPH_TO_MEM,     /**< Peripheral to Memory */
    DMA_TRANSFER_PERIPH_TO_PERIPH   /**< Peripheral to Peripheral */
} dma_transfer_type_t;

/**
 * @brief DMA Channel Priority
 * @details Mức ưu tiên của kênh DMA (0 = thấp nhất, 15 = cao nhất)
 */
typedef enum {
    DMA_PRIORITY_LOW    = 0U,       /**< Ưu tiên thấp */
    DMA_PRIORITY_NORMAL = 8U,       /**< Ưu tiên trung bình */
    DMA_PRIORITY_HIGH   = 15U       /**< Ưu tiên cao */
} dma_channel_priority_t;

/**
 * @brief DMA Interrupt Flags
 * @details Các cờ interrupt của DMA
 */
typedef enum {
    DMA_INT_MAJOR_COMPLETE = 0x02U, /**< Major loop complete interrupt */
    DMA_INT_HALF_COMPLETE  = 0x04U, /**< Half major loop complete interrupt */
    DMA_INT_ERROR          = 0x80U  /**< Error interrupt */
} dma_interrupt_t;

/**
 * @brief DMA Configuration Structure
 * @details Cấu trúc cấu hình cho một kênh DMA
 */
typedef struct {
    uint8_t channel;                    /**< Số kênh DMA (0-15) */
    dmamux_source_t source;             /**< Nguồn request từ DMAMUX */
    dma_transfer_type_t transferType;   /**< Loại transfer */
    dma_transfer_size_t transferSize;   /**< Kích thước transfer */
    dma_channel_priority_t priority;    /**< Ưu tiên kênh */
    
    uint32_t sourceAddr;                /**< Địa chỉ nguồn */
    int16_t sourceOffset;               /**< Offset nguồn sau mỗi minor loop */
    int32_t sourceLastAddrAdjust;       /**< Điều chỉnh địa chỉ nguồn sau major loop */
    
    uint32_t destAddr;                  /**< Địa chỉ đích */
    int16_t destOffset;                 /**< Offset đích sau mỗi minor loop */
    int32_t destLastAddrAdjust;         /**< Điều chỉnh địa chỉ đích sau major loop */
    
    uint32_t minorLoopBytes;            /**< Số bytes transfer trong mỗi minor loop */
    uint16_t majorLoopCount;            /**< Số lần lặp major loop */
    
    bool enableInterrupt;               /**< Enable interrupt khi hoàn thành major loop */
    bool disableRequestAfterDone;       /**< Disable DMA request sau khi hoàn thành */
    
} dma_channel_config_t;

/**
 * @brief DMA Callback Function Type
 * @details Function pointer cho callback khi DMA complete hoặc error
 * @param channel Số kênh DMA
 * @param userData Con trỏ tới user data
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
 * @brief Khởi tạo module DMA
 * @details
 * - Enable clock cho DMA và DMAMUX
 * - Reset tất cả các kênh DMA về trạng thái mặc định
 * - Xóa tất cả interrupt flags
 * 
 * @return STATUS_SUCCESS nếu thành công
 * 
 * @note Phải gọi hàm này trước khi sử dụng bất kỳ chức năng DMA nào
 * 
 * @code
 * // Khởi tạo DMA module
 * DMA_Init();
 * @endcode
 */
status_t DMA_Init(void);

/**
 * @brief Deinitialize module DMA
 * @details
 * - Disable tất cả các kênh DMA
 * - Disable clock cho DMA và DMAMUX
 * 
 * @return STATUS_SUCCESS nếu thành công
 */
status_t DMA_Deinit(void);

/**
 * @brief Cấu hình một kênh DMA
 * @details
 * Thiết lập đầy đủ các tham số cho một kênh DMA:
 * - Source/Destination addresses
 * - Transfer size và count
 * - Interrupts và callbacks
 * 
 * @param[in] config Con trỏ tới cấu trúc cấu hình DMA
 * 
 * @return STATUS_SUCCESS nếu cấu hình thành công
 * @return STATUS_ERROR nếu tham số không hợp lệ
 * 
 * @note
 * - Địa chỉ source/dest phải aligned với transfer size
 * - Không được cấu hình khi kênh đang active
 * 
 * @code
 * // Cấu hình DMA channel 0 để copy memory
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
 * @brief Bắt đầu transfer DMA cho một kênh
 * @details
 * Kích hoạt DMA request cho kênh được chỉ định.
 * Đối với software-triggered DMA (ALWAYS_ON source), sẽ start ngay lập tức.
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * 
 * @return STATUS_SUCCESS nếu start thành công
 * @return STATUS_ERROR nếu channel không hợp lệ hoặc chưa được cấu hình
 * 
 * @code
 * // Start DMA channel 0
 * DMA_StartChannel(0);
 * @endcode
 */
status_t DMA_StartChannel(uint8_t channel);

/**
 * @brief Dừng transfer DMA cho một kênh
 * @details Disable DMA request cho kênh được chỉ định
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * 
 * @return STATUS_SUCCESS nếu stop thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 */
status_t DMA_StopChannel(uint8_t channel);

/**
 * @brief Kiểm tra xem kênh DMA có đang active không
 * @details Đọc bit ACTIVE trong TCD CSR register
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * 
 * @return true nếu kênh đang active
 * @return false nếu kênh không active hoặc channel không hợp lệ
 */
bool DMA_IsChannelActive(uint8_t channel);

/**
 * @brief Kiểm tra xem kênh DMA đã hoàn thành chưa
 * @details Đọc bit DONE trong TCD CSR register
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * 
 * @return true nếu transfer đã hoàn thành
 * @return false nếu chưa hoàn thành hoặc channel không hợp lệ
 */
bool DMA_IsChannelDone(uint8_t channel);

/**
 * @brief Clear cờ DONE của kênh DMA
 * @details Xóa bit DONE trong TCD CSR register
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * 
 * @return STATUS_SUCCESS nếu clear thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 */
status_t DMA_ClearDone(uint8_t channel);

/**
 * @brief Đăng ký callback function cho kênh DMA
 * @details
 * Callback sẽ được gọi khi:
 * - DMA transfer hoàn thành (major loop done)
 * - Có lỗi xảy ra trong quá trình transfer
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * @param[in] callback Function pointer cho callback
 * @param[in] userData Con trỏ tới user data (có thể NULL)
 * 
 * @return STATUS_SUCCESS nếu đăng ký thành công
 * @return STATUS_ERROR nếu channel không hợp lệ hoặc callback NULL
 * 
 * @code
 * void myDmaCallback(uint8_t channel, void *userData) {
 *     // Xử lý khi DMA complete
 *     printf("DMA channel %d completed!\n", channel);
 * }
 * 
 * DMA_InstallCallback(0, myDmaCallback, NULL);
 * @endcode
 */
status_t DMA_InstallCallback(uint8_t channel, dma_callback_t callback, void *userData);

/**
 * @brief Thiết lập ưu tiên cho kênh DMA
 * @details
 * Khi nhiều kênh DMA request đồng thời, kênh có priority cao hơn sẽ được phục vụ trước
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * @param[in] priority Mức ưu tiên (0-15, với 15 là cao nhất)
 * 
 * @return STATUS_SUCCESS nếu thiết lập thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 */
status_t DMA_SetChannelPriority(uint8_t channel, uint8_t priority);

/**
 * @brief Lấy số lượng major loops còn lại
 * @details Đọc giá trị CITER (Current Iteration Count) từ TCD
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * @param[out] count Con trỏ để lưu số lượng iterations còn lại
 * 
 * @return STATUS_SUCCESS nếu đọc thành công
 * @return STATUS_ERROR nếu channel không hợp lệ hoặc count NULL
 */
status_t DMA_GetRemainingMajorLoops(uint8_t channel, uint16_t *count);

/**
 * @brief Enable interrupt cho kênh DMA
 * @details Enable interrupt khi major loop complete
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * 
 * @return STATUS_SUCCESS nếu enable thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 */
status_t DMA_EnableChannelInterrupt(uint8_t channel);

/**
 * @brief Disable interrupt cho kênh DMA
 * @details Disable interrupt khi major loop complete
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * 
 * @return STATUS_SUCCESS nếu disable thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 */
status_t DMA_DisableChannelInterrupt(uint8_t channel);

/**
 * @brief Xử lý DMA interrupt (gọi từ ISR)
 * @details
 * Hàm này xử lý interrupt và gọi callback đã đăng ký.
 * Phải được gọi trong DMA ISR.
 * 
 * @param[in] channel Số kênh DMA (0-15)
 * 
 * @note Hàm này sẽ clear các interrupt flags tự động
 */
void DMA_IRQHandler(uint8_t channel);

/**
 * @brief Copy dữ liệu từ source sang destination bằng DMA
 * @details
 * Hàm tiện ích để thực hiện memory-to-memory transfer.
 * Hàm này sẽ:
 * - Tự động cấu hình kênh DMA
 * - Start transfer
 * - Chờ cho đến khi transfer hoàn thành (blocking)
 * 
 * @param[in] channel Số kênh DMA sử dụng (0-15)
 * @param[in] src Địa chỉ nguồn
 * @param[out] dest Địa chỉ đích
 * @param[in] size Số bytes cần copy
 * 
 * @return STATUS_SUCCESS nếu copy thành công
 * @return STATUS_ERROR nếu có lỗi xảy ra
 * 
 * @note
 * - Hàm này là blocking (chờ cho đến khi transfer xong)
 * - Không nên dùng trong interrupt context
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
