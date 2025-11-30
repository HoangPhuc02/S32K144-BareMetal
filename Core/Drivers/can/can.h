/**
 * @file    can.h
 * @brief   FlexCAN Driver API for S32K144
 * @details
 * CAN driver cung cấp các API để cấu hình và sử dụng FlexCAN module.
 * 
 * Tính năng:
 * - Khởi tạo và cấu hình baudrate CAN
 * - Truyền CAN message (Standard/Extended ID)
 * - Nhận CAN message với Message Buffer hoặc RX FIFO
 * - Callback cho TX/RX events
 * - Filtering và masking
 * - Error handling và status
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 * 
 * @par Example:
 * @code
 * // Khởi tạo CAN0 với 500 Kbps
 * can_config_t config = {
 *     .instance = 0,
 *     .clockSource = CAN_CLK_SRC_FIRCDIV2,
 *     .baudRate = 500000
 * };
 * 
 * CAN_Init(&config);
 * 
 * // Gửi message
 * can_message_t txMsg = {
 *     .id = 0x123,
 *     .idType = CAN_ID_STD,
 *     .dataLength = 8,
 *     .data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
 * };
 * 
 * CAN_Send(0, 0, &txMsg);
 * 
 * // Nhận message
 * can_message_t rxMsg;
 * if (CAN_Receive(0, 1, &rxMsg) == STATUS_SUCCESS) {
 *     // Process received message
 * }
 * @endcode
 */

#ifndef CAN_H
#define CAN_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "can_reg.h"
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Status return type */
typedef enum {
    STATUS_SUCCESS = 0x00U,         /**< Operation successful */
    STATUS_ERROR = 0x01U,           /**< General error */
    STATUS_BUSY = 0x02U,            /**< Resource busy */
    STATUS_TIMEOUT = 0x03U,         /**< Operation timeout */
    STATUS_INVALID_PARAM = 0x04U,   /**< Invalid parameter */
    STATUS_NOT_INITIALIZED = 0x05U  /**< Module not initialized */
} status_t;

/** @brief Maximum number of CAN instances */
#define CAN_INSTANCE_COUNT  (3U)

/** @brief Maximum data length in CAN message */
#define CAN_MAX_DATA_LENGTH (8U)

/** @brief TX/RX Message Buffer allocation */
#define CAN_TX_MB_START     (8U)    /**< TX MBs start from MB8 */
#define CAN_TX_MB_COUNT     (8U)    /**< 8 TX MBs (MB8-MB15) */
#define CAN_RX_MB_START     (16U)   /**< RX MBs start from MB16 */
#define CAN_RX_MB_COUNT     (16U)   /**< 16 RX MBs (MB16-MB31) */

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief CAN ID Type
 */
typedef enum {
    CAN_ID_STD = 0U,    /**< Standard ID (11-bit) */
    CAN_ID_EXT = 1U     /**< Extended ID (29-bit) */
} can_id_type_t;

/**
 * @brief CAN Frame Type
 */
typedef enum {
    CAN_FRAME_DATA = 0U,    /**< Data frame */
    CAN_FRAME_REMOTE = 1U   /**< Remote frame */
} can_frame_type_t;

/**
 * @brief CAN Operating Mode
 */
typedef enum {
    CAN_MODE_NORMAL = 0U,       /**< Normal operation mode */
    CAN_MODE_LOOPBACK = 1U,     /**< Loopback mode for testing */
    CAN_MODE_LISTEN_ONLY = 2U   /**< Listen-only mode (no ACK) */
} can_mode_t;

/**
 * @brief CAN Error State
 */
typedef enum {
    CAN_ERROR_ACTIVE = 0U,      /**< Error Active (normal operation) */
    CAN_ERROR_PASSIVE = 1U,     /**< Error Passive (TEC or REC > 127) */
    CAN_ERROR_BUS_OFF = 2U      /**< Bus Off (TEC > 255) */
} can_error_state_t;

/**
 * @brief CAN Message Structure
 */
typedef struct {
    uint32_t id;                        /**< CAN ID (11-bit or 29-bit) */
    can_id_type_t idType;               /**< ID type (Standard/Extended) */
    can_frame_type_t frameType;         /**< Frame type (Data/Remote) */
    uint8_t dataLength;                 /**< Data length (0-8) */
    uint8_t data[CAN_MAX_DATA_LENGTH];  /**< Data bytes */
} can_message_t;

/**
 * @brief CAN Timing Configuration Structure
 * @details Cấu hình timing để đạt baudrate mong muốn
 * 
 * Công thức: Baudrate = CAN_Clock / ((1 + PSEG1 + PSEG2 + PROPSEG) * (PRESDIV + 1))
 * 
 * Time quantum (Tq) = (PRESDIV + 1) / CAN_Clock
 * Bit time = (1 + PSEG1 + PSEG2 + PROPSEG) * Tq
 */
typedef struct {
    uint8_t propSeg;    /**< Propagation Segment (0-7) */
    uint8_t phaseSeg1;  /**< Phase Segment 1 (0-7) */
    uint8_t phaseSeg2;  /**< Phase Segment 2 (0-7) */
    uint8_t rJumpWidth; /**< Resynchronization Jump Width (0-3) */
    uint8_t preDiv;     /**< Prescaler Division Factor (0-255) */
} can_timing_config_t;

/**
 * @brief CAN Configuration Structure
 */
typedef struct {
    uint8_t instance;               /**< CAN instance (0, 1, or 2) */
    can_clk_src_t clockSource;      /**< Clock source selection */
    uint32_t baudRate;              /**< Baudrate in bps (e.g., 500000 for 500 Kbps) */
    can_mode_t mode;                /**< Operating mode */
    bool enableSelfReception;       /**< Enable self-reception */
    bool useRxFifo;                 /**< Use RX FIFO instead of individual MBs */
} can_config_t;

/**
 * @brief CAN RX Filter Configuration
 */
typedef struct {
    uint32_t id;            /**< Filter ID */
    uint32_t mask;          /**< Filter mask (1 = must match, 0 = don't care) */
    can_id_type_t idType;   /**< ID type */
} can_rx_filter_t;

/**
 * @brief CAN Callback Function Type
 * @param instance CAN instance number
 * @param mbIndex Message Buffer index
 * @param userData User data pointer
 */
typedef void (*can_callback_t)(uint8_t instance, uint8_t mbIndex, void *userData);

/**
 * @brief CAN Error Callback Function Type
 * @param instance CAN instance number
 * @param errorFlags Error flags from ESR1 register
 * @param userData User data pointer
 */
typedef void (*can_error_callback_t)(uint8_t instance, uint32_t errorFlags, void *userData);

/*******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * @brief Initialize CAN module
 * @details Cấu hình CAN module với các thông số cơ bản
 * 
 * @param[in] config Pointer to configuration structure
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Initialization successful
 *         - STATUS_INVALID_PARAM: Invalid parameter
 *         - STATUS_ERROR: Initialization failed
 * 
 * @note Hàm này sẽ:
 *       - Enable clock cho CAN module
 *       - Soft reset CAN module
 *       - Cấu hình baudrate
 *       - Cấu hình operating mode
 *       - Initialize Message Buffers
 * 
 * @warning Phải gọi hàm này trước khi sử dụng các API khác
 */
status_t CAN_Init(const can_config_t *config);

/**
 * @brief Deinitialize CAN module
 * @details Disable CAN module và clock
 * 
 * @param[in] instance CAN instance (0-2)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Deinitialization successful
 *         - STATUS_INVALID_PARAM: Invalid instance
 */
status_t CAN_Deinit(uint8_t instance);

/**
 * @brief Send CAN message
 * @details Gửi message qua Message Buffer được chỉ định
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[in] mbIndex Message Buffer index (8-15 for TX)
 * @param[in] message Pointer to message structure
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Message queued for transmission
 *         - STATUS_INVALID_PARAM: Invalid parameter
 *         - STATUS_BUSY: Message Buffer busy
 *         - STATUS_NOT_INITIALIZED: CAN not initialized
 * 
 * @note Hàm này chỉ queue message, không đợi transmission complete
 */
status_t CAN_Send(uint8_t instance, uint8_t mbIndex, const can_message_t *message);

/**
 * @brief Send CAN message và chờ transmission complete
 * @details Blocking function - chờ đến khi message được gửi xong
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[in] mbIndex Message Buffer index (8-15 for TX)
 * @param[in] message Pointer to message structure
 * @param[in] timeoutMs Timeout in milliseconds
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Message transmitted successfully
 *         - STATUS_TIMEOUT: Transmission timeout
 *         - STATUS_INVALID_PARAM: Invalid parameter
 *         - STATUS_NOT_INITIALIZED: CAN not initialized
 */
status_t CAN_SendBlocking(uint8_t instance, uint8_t mbIndex, 
                          const can_message_t *message, uint32_t timeoutMs);

/**
 * @brief Receive CAN message
 * @details Đọc message từ Message Buffer
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[in] mbIndex Message Buffer index (16-31 for RX)
 * @param[out] message Pointer to store received message
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Message received
 *         - STATUS_ERROR: No message available
 *         - STATUS_INVALID_PARAM: Invalid parameter
 *         - STATUS_NOT_INITIALIZED: CAN not initialized
 * 
 * @note Hàm này non-blocking, return ngay lập tức
 */
status_t CAN_Receive(uint8_t instance, uint8_t mbIndex, can_message_t *message);

/**
 * @brief Receive CAN message và chờ
 * @details Blocking function - chờ đến khi có message
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[in] mbIndex Message Buffer index (16-31 for RX)
 * @param[out] message Pointer to store received message
 * @param[in] timeoutMs Timeout in milliseconds
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Message received
 *         - STATUS_TIMEOUT: Receive timeout
 *         - STATUS_INVALID_PARAM: Invalid parameter
 *         - STATUS_NOT_INITIALIZED: CAN not initialized
 */
status_t CAN_ReceiveBlocking(uint8_t instance, uint8_t mbIndex, 
                             can_message_t *message, uint32_t timeoutMs);

/**
 * @brief Configure RX filter for Message Buffer
 * @details Cấu hình filter để chỉ nhận message với ID phù hợp
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[in] mbIndex Message Buffer index
 * @param[in] filter Pointer to filter configuration
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Filter configured
 *         - STATUS_INVALID_PARAM: Invalid parameter
 * 
 * @note Mask bit = 1 nghĩa là bit đó trong ID phải match
 *       Mask bit = 0 nghĩa là don't care
 */
status_t CAN_ConfigRxFilter(uint8_t instance, uint8_t mbIndex, 
                             const can_rx_filter_t *filter);

/**
 * @brief Install TX callback
 * @details Đăng ký callback function cho TX complete event
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[in] mbIndex Message Buffer index
 * @param[in] callback Callback function pointer
 * @param[in] userData User data pointer
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Callback installed
 *         - STATUS_INVALID_PARAM: Invalid parameter
 */
status_t CAN_InstallTxCallback(uint8_t instance, uint8_t mbIndex,
                                can_callback_t callback, void *userData);

/**
 * @brief Install RX callback
 * @details Đăng ký callback function cho RX event
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[in] mbIndex Message Buffer index
 * @param[in] callback Callback function pointer
 * @param[in] userData User data pointer
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Callback installed
 *         - STATUS_INVALID_PARAM: Invalid parameter
 */
status_t CAN_InstallRxCallback(uint8_t instance, uint8_t mbIndex,
                                can_callback_t callback, void *userData);

/**
 * @brief Install error callback
 * @details Đăng ký callback function cho error events
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[in] callback Callback function pointer
 * @param[in] userData User data pointer
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Callback installed
 *         - STATUS_INVALID_PARAM: Invalid parameter
 */
status_t CAN_InstallErrorCallback(uint8_t instance, 
                                   can_error_callback_t callback, void *userData);

/**
 * @brief Get CAN error state
 * @details Lấy trạng thái lỗi hiện tại (Error Active/Passive/Bus Off)
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[out] errorState Pointer to store error state
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Error state retrieved
 *         - STATUS_INVALID_PARAM: Invalid parameter
 */
status_t CAN_GetErrorState(uint8_t instance, can_error_state_t *errorState);

/**
 * @brief Get TX/RX error counters
 * @details Lấy giá trị của TX Error Counter và RX Error Counter
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[out] txErrorCount Pointer to store TX error count
 * @param[out] rxErrorCount Pointer to store RX error count
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Error counters retrieved
 *         - STATUS_INVALID_PARAM: Invalid parameter
 */
status_t CAN_GetErrorCounters(uint8_t instance, 
                               uint8_t *txErrorCount, uint8_t *rxErrorCount);

/**
 * @brief Abort transmission
 * @details Hủy bỏ transmission đang pending
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[in] mbIndex Message Buffer index
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Transmission aborted
 *         - STATUS_INVALID_PARAM: Invalid parameter
 */
status_t CAN_AbortTransmission(uint8_t instance, uint8_t mbIndex);

/**
 * @brief Check if Message Buffer is busy
 * @details Kiểm tra xem MB có đang được sử dụng không
 * 
 * @param[in] instance CAN instance (0-2)
 * @param[in] mbIndex Message Buffer index
 * @param[out] isBusy Pointer to store busy status
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Status checked
 *         - STATUS_INVALID_PARAM: Invalid parameter
 */
status_t CAN_IsMbBusy(uint8_t instance, uint8_t mbIndex, bool *isBusy);

/**
 * @brief Calculate timing parameters for baudrate
 * @details Tính toán các tham số timing để đạt baudrate mong muốn
 * 
 * @param[in] canClockHz CAN peripheral clock frequency in Hz
 * @param[in] baudRate Desired baudrate in bps
 * @param[out] timing Pointer to store calculated timing parameters
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Timing calculated
 *         - STATUS_INVALID_PARAM: Invalid parameter or impossible baudrate
 * 
 * @note Hàm này tự động tính toán timing parameters phù hợp
 */
status_t CAN_CalculateTiming(uint32_t canClockHz, uint32_t baudRate, 
                             can_timing_config_t *timing);

#endif /* CAN_H */
