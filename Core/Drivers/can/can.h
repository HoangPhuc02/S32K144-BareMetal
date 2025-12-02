/**
 * @file    can.h
 * @brief   FlexCAN Driver API for S32K144
 * @details This driver provides a comprehensive API for configuring and using the FlexCAN module.
 *          The driver supports both blocking and non-blocking operations with callback support.
 * 
 * Features:
 * - FlexCAN initialization and configuration with automatic baudrate calculation
 * - Transmit CAN messages (Standard 11-bit or Extended 29-bit ID)
 * - Receive CAN messages using Message Buffers or RX FIFO
 * - Callback support for TX/RX events and error handling
 * - Message filtering and masking capabilities
 * - Error state monitoring and fault management
 * - Operating modes: Normal, Loopback, Listen-Only
 * - Remote frame support
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 * 
 * @par Example Usage:
 * @code
 * // Initialize CAN0 at 500 Kbps
 * can_config_t config = {
 *     .instance = 0,
 *     .clockSource = CAN_CLK_SRC_FIRCDIV2,
 *     .baudRate = 500000,
 *     .mode = CAN_MODE_NORMAL,
 *     .enableSelfReception = false,
 *     .useRxFifo = false
 * };
 * 
 * CAN_Init(&config);
 * 
 * // Send a message
 * can_message_t txMsg = {
 *     .id = 0x123,
 *     .idType = CAN_ID_STD,
 *     .frameType = CAN_FRAME_DATA,
 *     .dataLength = 8,
 *     .data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
 * };
 * 
 * CAN_Send(0, 8, &txMsg);  // Send using MB8
 * 
 * // Receive a message
 * can_message_t rxMsg;
 * if (CAN_Receive(0, 16, &rxMsg) == STATUS_SUCCESS) {
 *     // Process received message
 *     ProcessMessage(&rxMsg);
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
 * Message Buffer Field Definitions
 ******************************************************************************/

/* CS field masks and shifts */
#define CAN_CS_CODE_SHIFT                (24U)
#define CAN_CS_CODE_MASK                 (0x0F000000U)
#define CAN_CS_IDE_MASK                  (0x00200000U)
#define CAN_CS_SRR_MASK                  (0x00400000U)
#define CAN_CS_RTR_MASK                  (0x00100000U)
#define CAN_CS_DLC_SHIFT                 (16U)
#define CAN_CS_DLC_MASK                  (0x000F0000U)

/* MB CODE values */
#define CAN_CS_CODE_TX_INACTIVE          (0x08U)
#define CAN_CS_CODE_TX_ABORT             (0x09U)
#define CAN_CS_CODE_TX_ONCE              (0x0CU)
#define CAN_CS_CODE_RX_INACTIVE          (0x00U)
#define CAN_CS_CODE_RX_EMPTY             (0x04U)
#define CAN_CS_CODE_RX_FULL              (0x02U)

/* ID field masks and shifts */
#define CAN_ID_STD_SHIFT                 (18U)
#define CAN_ID_STD_MASK                  (0x1FFC0000U)
#define CAN_ID_EXT_SHIFT                 (0U)
#define CAN_ID_EXT_MASK                  (0x1FFFFFFFU)

/** @brief Number of Message Buffers per CAN instance */
// TODO : thiss iss based on CAN peri there are only 16 buffer in CAN1 and CAN2       
#define CAN_MB_COUNT        (32U)

/**
 * @brief CAN Message Buffer Structure
 * @details Each Message Buffer occupies 16 bytes consisting of:
 *          - CS (4 bytes): Control and Status word containing CODE, IDE, RTR, DLC, TIME_STAMP
 *          - ID (4 bytes): Message Identifier (Standard 11-bit or Extended 29-bit)
 *          - DATA (8 bytes): Payload data bytes 0-7
 */
typedef struct {
    __IO uint32_t CS;       /**< Control and Status register */
    __IO uint32_t ID;       /**< Message Buffer Identifier register */
    __IO uint8_t  DATA[8];  /**< Data bytes 0-7 */
} CAN_MB_Type;

/**
 * @brief CAN RX FIFO Structure
 * @details Structure for RX FIFO output message format.
 *          When RX FIFO is enabled, MBs 0-5 are occupied by FIFO
 */
typedef struct {
    __IO uint32_t CS;       /**< Control and Status register */
    __IO uint32_t ID;       /**< Message Identifier register */
    __IO uint8_t  DATA[8];  /**< Data bytes 0-7 */
} CAN_RX_FIFO_Type;

/*******************************************************************************
 * Inline Helper Functions
 ******************************************************************************/

/**
 * @brief Get Message Buffer offset in RAMn array
 * @param mbIdx Message Buffer index (0-31)
 * @return Offset in 32-bit words
 */
static inline uint32_t CAN_GetMbOffset(uint8_t mbIdx)
{
    return (uint32_t)mbIdx * 4U;
}

/**
 * @brief Read Message Buffer CS register
 */
static inline uint32_t CAN_ReadMbCs(CAN_Type *base, uint8_t mbIdx)
{
    return base->RAMn[CAN_GetMbOffset(mbIdx) + 0U];
}

/**
 * @brief Write Message Buffer CS register
 */
static inline void CAN_WriteMbCs(CAN_Type *base, uint8_t mbIdx, uint32_t cs)
{
    base->RAMn[CAN_GetMbOffset(mbIdx) + 0U] = cs;
}

/**
 * @brief Read Message Buffer ID register
 */
static inline uint32_t CAN_ReadMbId(CAN_Type *base, uint8_t mbIdx)
{
    return base->RAMn[CAN_GetMbOffset(mbIdx) + 1U];
}

/**
 * @brief Write Message Buffer ID register
 */
static inline void CAN_WriteMbId(CAN_Type *base, uint8_t mbIdx, uint32_t id)
{
    base->RAMn[CAN_GetMbOffset(mbIdx) + 1U] = id;
}

/**
 * @brief Read Message Buffer DATA word (2 words for 8 bytes)
 */
static inline uint32_t CAN_ReadMbData(CAN_Type *base, uint8_t mbIdx, uint8_t wordIdx)
{
    return base->RAMn[CAN_GetMbOffset(mbIdx) + 2U + wordIdx];
}

/**
 * @brief Write Message Buffer DATA word
 */
static inline void CAN_WriteMbData(CAN_Type *base, uint8_t mbIdx, uint8_t wordIdx, uint32_t data)
{
    base->RAMn[CAN_GetMbOffset(mbIdx) + 2U + wordIdx] = data;
}

/**
 * @brief Copy data to Message Buffer
 * @param base CAN peripheral base
 * @param mbIdx Message Buffer index
 * @param data Source data array
 * @param length Data length in bytes (max 8)
 */
static inline void CAN_CopyDataToMb(CAN_Type *base, uint8_t mbIdx, const uint8_t *data, uint8_t length)
{
    uint32_t dataWord0 = 0U;
    uint32_t dataWord1 = 0U;

    /* Pack bytes into 32-bit words (big-endian format) */
    for (uint8_t i = 0; i < length && i < 4U; i++) {
        dataWord0 |= ((uint32_t)data[i] << (24U - i * 8U));
    }

    for (uint8_t i = 4; i < length && i < 8U; i++) {
        dataWord1 |= ((uint32_t)data[i] << (56U - i * 8U));
    }

    CAN_WriteMbData(base, mbIdx, 0U, dataWord0);
    CAN_WriteMbData(base, mbIdx, 1U, dataWord1);
}

/**
 * @brief Copy data from Message Buffer
 * @param base CAN peripheral base
 * @param mbIdx Message Buffer index
 * @param data Destination data array
 * @param length Data length in bytes (max 8)
 */
static inline void CAN_CopyDataFromMb(CAN_Type *base, uint8_t mbIdx, uint8_t *data, uint8_t length)
{
    uint32_t dataWord0 = CAN_ReadMbData(base, mbIdx, 0U);
    uint32_t dataWord1 = CAN_ReadMbData(base, mbIdx, 1U);

    /* Unpack 32-bit words into bytes (big-endian format) */
    for (uint8_t i = 0; i < length && i < 4U; i++) {
        data[i] = (uint8_t)(dataWord0 >> (24U - i * 8U));
    }

    for (uint8_t i = 4; i < length && i < 8U; i++) {
        data[i] = (uint8_t)(dataWord1 >> (56U - i * 8U));
    }
}

/**
 * @brief Clear Message Buffer (set CS, ID, DATA to 0)
 */
static inline void CAN_ClearMb(CAN_Type *base, uint8_t mbIdx)
{
    uint32_t offset = CAN_GetMbOffset(mbIdx);
    base->RAMn[offset + 0U] = 0U;  /* CS */
    base->RAMn[offset + 1U] = 0U;  /* ID */
    base->RAMn[offset + 2U] = 0U;  /* DATA word 0 */
    base->RAMn[offset + 3U] = 0U;  /* DATA word 1 */
}


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief Status return type for all driver functions
 * @details Provides standardized return codes for error handling
 */
typedef enum {
    STATUS_SUCCESS = 0x00U,         /**< Operation completed successfully */
    STATUS_ERROR = 0x01U,           /**< General error occurred */
    STATUS_BUSY = 0x02U,            /**< Resource is currently busy */
    STATUS_TIMEOUT = 0x03U,         /**< Operation timed out */
    STATUS_INVALID_PARAM = 0x04U,   /**< Invalid parameter provided */
    STATUS_NOT_INITIALIZED = 0x05U  /**< Module not initialized */
} status_t;

/** @brief Maximum number of CAN instances available on S32K144 */
#define CAN_INSTANCE_COUNT  (3U)

/** @brief Maximum data length in bytes for a CAN message */
#define CAN_MAX_DATA_LENGTH (8U)

/**
 * @name Message Buffer Allocation
 * @brief Default allocation scheme for TX and RX Message Buffers
 * @details - MB0-MB7: Reserved for system use or RX FIFO (if enabled)
 *          - MB8-MB15: Allocated for transmission (8 TX buffers)
 *          - MB16-MB31: Allocated for reception (16 RX buffers)
 * @{
 */
#define CAN_TX_MB_START     (8U)    /**< First TX Message Buffer index */
#define CAN_TX_MB_COUNT     (8U)    /**< Total number of TX Message Buffers */
#define CAN_RX_MB_START     (16U)   /**< First RX Message Buffer index */
#define CAN_RX_MB_COUNT     (16U)   /**< Total number of RX Message Buffers */
/** @} */

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief CAN Identifier Type
 * @details CAN protocol supports two types of identifiers:
 *          - Standard: 11-bit ID (CAN 2.0A)
 *          - Extended: 29-bit ID (CAN 2.0B)
 */
typedef enum {
    CAN_ID_STD = 0U,    /**< Standard 11-bit identifier */
    CAN_ID_EXT = 1U     /**< Extended 29-bit identifier */
} can_id_type_t;

/**
 * @brief CAN Frame Type
 * @details Specifies whether frame contains data or is a remote request
 */
typedef enum {
    CAN_FRAME_DATA = 0U,    /**< Data frame containing payload */
    CAN_FRAME_REMOTE = 1U   /**< Remote frame requesting data */
} can_frame_type_t;

/**
 * @brief CAN Operating Mode
 * @details Defines the operational mode of the CAN controller
 */
typedef enum {
    CAN_MODE_NORMAL = 0U,       /**< Normal operation mode for standard communication */
    CAN_MODE_LOOPBACK = 1U,     /**< Loopback mode for self-test (TX->RX internally) */
    CAN_MODE_LISTEN_ONLY = 2U   /**< Listen-only mode for bus monitoring (no ACK sent) */
} can_mode_t;

/**
 * @brief CAN Error State
 * @details Represents the fault confinement state as per CAN specification
 */
typedef enum {
    CAN_ERROR_ACTIVE = 0U,      /**< Error Active: TEC < 96 and REC < 96 */
    CAN_ERROR_PASSIVE = 1U,     /**< Error Passive: TEC or REC >= 128 */
    CAN_ERROR_BUS_OFF = 2U      /**< Bus Off: TEC > 255 */
} can_error_state_t;

/**
 * @brief CAN Message Structure
 * @details Complete representation of a CAN message including ID, type, and payload
 */
typedef struct {
    uint32_t id;                        /**< CAN Identifier (11-bit for STD, 29-bit for EXT) */
    can_id_type_t idType;               /**< Identifier type (Standard or Extended) */
    can_frame_type_t frameType;         /**< Frame type (Data or Remote) */
    uint8_t dataLength;                 /**< Number of data bytes (0-8) */
    uint8_t data[CAN_MAX_DATA_LENGTH];  /**< Payload data bytes */
} can_message_t;

/**
 * @brief CAN Bit Timing Configuration Structure
 * @details Configures the bit timing parameters to achieve desired baudrate.
 *          These parameters define the shape and duration of each CAN bit.
 * 
 * CAN Bit Time is divided into segments:
 * - SYNC_SEG: Always 1 Time Quantum (Tq)
 * - PROP_SEG: Propagation Segment (compensates for physical delays)
 * - PHASE_SEG1: Phase Segment 1 (before sample point)
 * - PHASE_SEG2: Phase Segment 2 (after sample point)
 * 
 * Formulas:
 * - Time Quantum (Tq) = (PRESDIV + 1) / CAN_Clock_Frequency
 * - Bit Time = (1 + PROPSEG + PSEG1 + PSEG2) * Tq
 * - Baudrate = 1 / Bit_Time = CAN_Clock / ((1 + PROPSEG + PSEG1 + PSEG2) * (PRESDIV + 1))
 * - Sample Point = (1 + PROPSEG + PSEG1) / (1 + PROPSEG + PSEG1 + PSEG2)
 * 
 * Example for 500 Kbps with 40 MHz CAN clock:
 * - Target: 500,000 bps (2 μs per bit)
 * - Configuration: PRESDIV=4, PROPSEG=6, PSEG1=7, PSEG2=2, RJW=1
 * - Tq = 5 / 40,000,000 = 125 ns
 * - Bit Time = 16 * 125 ns = 2 μs
 * - Baudrate = 1 / 2 μs = 500 Kbps
 * - Sample Point = (1+6+7)/16 = 87.5%
 */
typedef struct {
    uint8_t propSeg;    /**< Propagation Segment (0-7 Tq) */
    uint8_t phaseSeg1;  /**< Phase Segment 1 (0-7 Tq) */
    uint8_t phaseSeg2;  /**< Phase Segment 2 (0-7 Tq) */
    uint8_t rJumpWidth; /**< Resynchronization Jump Width (0-3 Tq) - Max phase error correction */
    uint8_t preDiv;     /**< Prescaler Division Factor (0-255) - Clock divider */
} can_timing_config_t;

/**
 * @brief CAN Module Configuration Structure
 * @details Main configuration structure for initializing a CAN instance
 */
typedef struct {
    uint8_t instance;               /**< CAN instance number (0, 1, or 2) */
    can_clk_src_t clockSource;      /**< Clock source selection for CAN peripheral */
    uint32_t baudRate;              /**< Desired baudrate in bps (e.g., 500000 = 500 Kbps) */
    can_mode_t mode;                /**< Operating mode (Normal/Loopback/Listen-Only) */
    bool enableSelfReception;       /**< Enable reception of own transmitted messages */
    bool useRxFifo;                 /**< Use RX FIFO mode instead of individual Message Buffers */
} can_config_t;

/**
 * @brief CAN RX Filter Configuration Structure
 * @details Configures acceptance filtering for received messages
 *          Filter uses AND operation: (Received_ID & mask) == (filter_ID & mask)
 */
typedef struct {
    uint32_t id;            /**< Filter ID value to match against */
    uint32_t mask;          /**< Filter mask (1=must match, 0=don't care) */
    can_id_type_t idType;   /**< ID type for this filter (Standard/Extended) */
} can_rx_filter_t;

/**
 * @brief CAN TX/RX Callback Function Type
 * @details Called when transmission completes or message is received
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] mbIndex Message Buffer index that triggered the callback
 * @param[in] userData User-defined data pointer passed during callback installation
 */
typedef void (*can_callback_t)(uint8_t instance, uint8_t mbIndex, void *userData);

/**
 * @brief CAN Error Callback Function Type
 * @details Called when CAN error or status change occurs
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] errorFlags Error flags from ESR1 register (see CAN_ESR1_xxx_MASK)
 * @param[in] userData User-defined data pointer passed during callback installation
 */
typedef void (*can_error_callback_t)(uint8_t instance, uint32_t errorFlags, void *userData);

/*******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * @brief Initialize CAN module
 * @details Configures and enables the CAN peripheral with specified parameters.
 *          This function performs complete module initialization including clock setup,
 *          soft reset, baudrate configuration, and Message Buffer initialization.
 * 
 * @param[in] config Pointer to CAN configuration structure
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Initialization completed successfully
 *         - STATUS_INVALID_PARAM: Invalid parameter (NULL pointer, invalid instance, etc.)
 *         - STATUS_ERROR: Initialization failed (clock enable failed, reset timeout, etc.)
 * 
 * @note This function must be called before using any other CAN API functions.
 *       The function performs the following steps:
 *       1. Enables peripheral clock for the CAN instance
 *       2. Selects clock source
 *       3. Performs soft reset and waits for completion
 *       4. Configures baudrate based on clock and desired rate
 *       5. Sets operating mode (Normal/Loopback/Listen-Only)
 *       6. Initializes Message Buffers (TX: MB8-15, RX: MB16-31)
 *       7. Enables the module
 * 
 * @warning Ensure pins are configured for CAN function before calling this function
 * 
 * @par Example:
 * @code
 * can_config_t config = {
 *     .instance = 0,
 *     .clockSource = CAN_CLK_SRC_SPLLDIV2,  // 40 MHz
 *     .baudRate = 500000,                    // 500 Kbps
 *     .mode = CAN_MODE_NORMAL,
 *     .enableSelfReception = false,
 *     .useRxFifo = false
 * };
 * if (CAN_Init(&config) == STATUS_SUCCESS) {
 *     // CAN ready to use
 * }
 * @endcode
 */
status_t CAN_Init(const can_config_t *config);

/**
 * @brief Deinitialize CAN module
 * @details Disables the CAN module and its peripheral clock to reduce power consumption.
 *          All Message Buffers are deactivated and the module enters disabled state.
 * 
 * @param[in] instance CAN instance number (0, 1, or 2)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Deinitialization completed successfully
 *         - STATUS_INVALID_PARAM: Invalid instance number
 * 
 * @note After calling this function, CAN_Init() must be called again before using the module
 * 
 * @warning Ensure no ongoing transmissions before deinitializing
 */
status_t CAN_Deinit(uint8_t instance);

/**
 * @brief Send CAN message (non-blocking)
 * @details Queues a message for transmission using the specified Message Buffer.
 *          This function returns immediately after queuing without waiting for transmission.
 *          Use callback or CAN_SendBlocking() for synchronous operation.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] mbIndex Message Buffer index (8-15 recommended for TX)
 * @param[in] message Pointer to message structure containing ID, data, and length
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Message successfully queued for transmission
 *         - STATUS_INVALID_PARAM: Invalid parameter (NULL pointer, invalid instance/MB, data length > 8)
 *         - STATUS_BUSY: Message Buffer is currently busy with another transmission
 *         - STATUS_NOT_INITIALIZED: CAN module not initialized
 * 
 * @note This function only queues the message. To wait for completion, use:
 *       - CAN_SendBlocking() for blocking operation
 *       - CAN_InstallTxCallback() for interrupt-driven notification
 * 
 * @warning Message Buffer must not be busy (check with CAN_IsMbBusy() if needed)
 * 
 * @par Example:
 * @code
 * can_message_t msg = {
 *     .id = 0x100,
 *     .idType = CAN_ID_STD,
 *     .frameType = CAN_FRAME_DATA,
 *     .dataLength = 8,
 *     .data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
 * };
 * CAN_Send(0, 8, &msg);  // Queue on MB8
 * @endcode
 */
status_t CAN_Send(uint8_t instance, uint8_t mbIndex, const can_message_t *message);

/**
 * @brief Send CAN message (blocking)
 * @details Sends a message and waits for transmission to complete or timeout.
 *          This function blocks execution until the message is successfully transmitted,
 *          an error occurs, or the specified timeout expires.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] mbIndex Message Buffer index (8-15 recommended for TX)
 * @param[in] message Pointer to message structure containing ID, data, and length
 * @param[in] timeoutMs Timeout in milliseconds (0 = wait forever, not recommended)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Message transmitted successfully
 *         - STATUS_TIMEOUT: Transmission did not complete within timeout period
 *         - STATUS_INVALID_PARAM: Invalid parameter
 *         - STATUS_NOT_INITIALIZED: CAN module not initialized
 * 
 * @note This function polls the transmission complete flag.
 *       For interrupt-driven operation, use CAN_Send() with callback.
 * 
 * @warning Long timeouts will block other operations. Consider using non-blocking CAN_Send().
 * 
 * @par Example:
 * @code
 * can_message_t msg = {
 *     .id = 0x200,
 *     .idType = CAN_ID_STD,
 *     .frameType = CAN_FRAME_DATA,
 *     .dataLength = 4,
 *     .data = {0xAA, 0xBB, 0xCC, 0xDD}
 * };
 * if (CAN_SendBlocking(0, 8, &msg, 100) == STATUS_SUCCESS) {
 *     // Message sent successfully within 100ms
 * }
 * @endcode
 */
status_t CAN_SendBlocking(uint8_t instance, uint8_t mbIndex, 
                          const can_message_t *message, uint32_t timeoutMs);

/**
 * @brief Receive CAN message (non-blocking)
 * @details Reads a message from the specified Message Buffer if available.
 *          Returns immediately whether a message is present or not.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] mbIndex Message Buffer index (16-31 recommended for RX)
 * @param[out] message Pointer to store received message data
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Message received and copied to buffer
 *         - STATUS_ERROR: No message available in Message Buffer
 *         - STATUS_INVALID_PARAM: Invalid parameter (NULL pointer, invalid instance/MB)
 *         - STATUS_NOT_INITIALIZED: CAN module not initialized
 * 
 * @note This function does not wait for a message. For blocking operation, use CAN_ReceiveBlocking().
 *       Message Buffer must be configured for reception (CODE = RX_EMPTY).
 * 
 * @par Example:
 * @code
 * can_message_t rxMsg;
 * if (CAN_Receive(0, 16, &rxMsg) == STATUS_SUCCESS) {
 *     // Process received message
 *     printf("Received ID: 0x%X, Data: ", rxMsg.id);
 *     for (int i = 0; i < rxMsg.dataLength; i++) {
 *         printf("%02X ", rxMsg.data[i]);
 *     }
 * }
 * @endcode
 */
status_t CAN_Receive(uint8_t instance, uint8_t mbIndex, can_message_t *message);

/**
 * @brief Receive CAN message (blocking)
 * @details Waits for a message to be received or until timeout expires.
 *          Function blocks execution until a message arrives, an error occurs, or timeout.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] mbIndex Message Buffer index (16-31 recommended for RX)
 * @param[out] message Pointer to store received message data
 * @param[in] timeoutMs Timeout in milliseconds (0 = wait forever, not recommended)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Message received successfully within timeout
 *         - STATUS_TIMEOUT: No message received within timeout period
 *         - STATUS_INVALID_PARAM: Invalid parameter
 *         - STATUS_NOT_INITIALIZED: CAN module not initialized
 * 
 * @note This function polls the receive complete flag.
 *       For interrupt-driven operation, use CAN_Receive() with callback.
 * 
 * @warning Long timeouts will block other operations. Consider using non-blocking CAN_Receive().
 * 
 * @par Example:
 * @code
 * can_message_t rxMsg;
 * if (CAN_ReceiveBlocking(0, 16, &rxMsg, 1000) == STATUS_SUCCESS) {
 *     // Message received within 1 second
 *     ProcessMessage(&rxMsg);
 * } else {
 *     // Timeout - no message received
 * }
 * @endcode
 */
status_t CAN_ReceiveBlocking(uint8_t instance, uint8_t mbIndex, 
                             can_message_t *message, uint32_t timeoutMs);

/**
 * @brief Configure RX filter for Message Buffer
 * @details Sets up acceptance filtering to receive only messages matching specific criteria.
 *          Filter operation: Message accepted if (Received_ID & mask) == (filter_ID & mask)
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] mbIndex Message Buffer index to configure
 * @param[in] filter Pointer to filter configuration (ID, mask, type)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Filter configured successfully
 *         - STATUS_INVALID_PARAM: Invalid parameter (NULL pointer, invalid instance/MB)
 * 
 * @note Mask bit interpretation:
 *       - Mask bit = 1: Corresponding ID bit must match
 *       - Mask bit = 0: Corresponding ID bit is "don't care"
 * 
 * @par Examples:
 * @code
 * // Example 1: Accept only ID 0x123 (exact match)
 * can_rx_filter_t filter1 = {
 *     .id = 0x123,
 *     .mask = 0x7FF,  // All 11 bits must match for Standard ID
 *     .idType = CAN_ID_STD
 * };
 * CAN_ConfigRxFilter(0, 16, &filter1);
 * 
 * // Example 2: Accept any ID from 0x200-0x20F (last 4 bits don't care)
 * can_rx_filter_t filter2 = {
 *     .id = 0x200,
 *     .mask = 0x7F0,  // Only upper 7 bits must match
 *     .idType = CAN_ID_STD
 * };
 * CAN_ConfigRxFilter(0, 17, &filter2);
 * 
 * // Example 3: Accept all messages (no filtering)
 * can_rx_filter_t filter3 = {
 *     .id = 0x000,
 *     .mask = 0x000,  // All bits don't care
 *     .idType = CAN_ID_STD
 * };
 * CAN_ConfigRxFilter(0, 18, &filter3);
 * @endcode
 */
status_t CAN_ConfigRxFilter(uint8_t instance, uint8_t mbIndex, 
                             const can_rx_filter_t *filter);

/**
 * @brief Install TX complete callback
 * @details Registers a callback function to be invoked when transmission completes.
 *          Enables interrupt-driven transmission notification.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] mbIndex Message Buffer index
 * @param[in] callback Callback function pointer (or NULL to uninstall)
 * @param[in] userData User-defined data passed to callback (can be NULL)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Callback installed successfully
 *         - STATUS_INVALID_PARAM: Invalid instance or MB index
 * 
 * @note Callback is called from ISR context. Keep callback execution time short.
 * 
 * @par Example:
 * @code
 * void TxCompleteCallback(uint8_t inst, uint8_t mb, void *userData) {
 *     // Called when transmission completes
 *     SetTxCompleteFlag();
 * }
 * 
 * CAN_InstallTxCallback(0, 8, TxCompleteCallback, NULL);
 * @endcode
 */
status_t CAN_InstallTxCallback(uint8_t instance, uint8_t mbIndex,
                                can_callback_t callback, void *userData);

/**
 * @brief Install RX message callback
 * @details Registers a callback function to be invoked when a message is received.
 *          Enables interrupt-driven reception notification.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] mbIndex Message Buffer index
 * @param[in] callback Callback function pointer (or NULL to uninstall)
 * @param[in] userData User-defined data passed to callback (can be NULL)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Callback installed successfully
 *         - STATUS_INVALID_PARAM: Invalid instance or MB index
 * 
 * @note Callback is called from ISR context. Keep callback execution time short.
 *       Read message data in callback using CAN_Receive().
 * 
 * @par Example:
 * @code
 * void RxCallback(uint8_t inst, uint8_t mb, void *userData) {
 *     can_message_t msg;
 *     if (CAN_Receive(inst, mb, &msg) == STATUS_SUCCESS) {
 *         // Process received message
 *         ProcessMessage(&msg);
 *     }
 * }
 * 
 * CAN_InstallRxCallback(0, 16, RxCallback, NULL);
 * @endcode
 */
status_t CAN_InstallRxCallback(uint8_t instance, uint8_t mbIndex,
                                can_callback_t callback, void *userData);

/**
 * @brief Install error callback
 * @details Registers a callback function to be invoked on CAN errors or status changes.
 *          Useful for monitoring bus-off, error warning, and error passive states.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] callback Callback function pointer (or NULL to uninstall)
 * @param[in] userData User-defined data passed to callback (can be NULL)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Callback installed successfully
 *         - STATUS_INVALID_PARAM: Invalid instance number
 * 
 * @note Callback is called from ISR context.
 *       Error flags parameter indicates which errors occurred (see CAN_ESR1_xxx_MASK).
 * 
 * @par Example:
 * @code
 * void ErrorCallback(uint8_t inst, uint32_t errorFlags, void *userData) {
 *     if (errorFlags & CAN_ESR1_BOFFINT_MASK) {
 *         // Bus-off condition
 *         HandleBusOff();
 *     }
 *     if (errorFlags & CAN_ESR1_ERRINT_MASK) {
 *         // General error
 *         HandleError();
 *     }
 * }
 * 
 * CAN_InstallErrorCallback(0, ErrorCallback, NULL);
 * @endcode
 */
status_t CAN_InstallErrorCallback(uint8_t instance, 
                                   can_error_callback_t callback, void *userData);

/**
 * @brief Get current CAN error state
 * @details Retrieves the fault confinement state according to CAN specification.
 *          Error state is determined by TX and RX error counters.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[out] errorState Pointer to store current error state
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Error state retrieved successfully
 *         - STATUS_INVALID_PARAM: Invalid parameter (NULL pointer, invalid instance)
 * 
 * @note Error States:
 *       - ERROR_ACTIVE: TEC < 96 and REC < 96 (normal operation)
 *       - ERROR_PASSIVE: TEC >= 128 or REC >= 128 (limited bus interaction)
 *       - BUS_OFF: TEC > 255 (node disconnected from bus)
 * 
 * @par Example:
 * @code
 * can_error_state_t state;
 * CAN_GetErrorState(0, &state);
 * if (state == CAN_ERROR_BUS_OFF) {
 *     // Node is in bus-off state, requires recovery
 *     RecoverFromBusOff();
 * }
 * @endcode
 */
status_t CAN_GetErrorState(uint8_t instance, can_error_state_t *errorState);

/**
 * @brief Get TX and RX error counters
 * @details Retrieves current values of Transmit and Receive Error Counters.
 *          These counters track error occurrences and determine error state.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[out] txErrorCount Pointer to store TX Error Counter (0-255)
 * @param[out] rxErrorCount Pointer to store RX Error Counter (0-255)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Error counters retrieved successfully
 *         - STATUS_INVALID_PARAM: Invalid parameter (NULL pointer, invalid instance)
 * 
 * @note Error counter thresholds:
 *       - TEC/REC < 96: Error Active
 *       - 96 <= TEC/REC < 128: Error Warning
 *       - 128 <= TEC/REC < 256: Error Passive
 *       - TEC > 255: Bus Off
 * 
 * @par Example:
 * @code
 * uint8_t txErr, rxErr;
 * CAN_GetErrorCounters(0, &txErr, &rxErr);
 * if (txErr > 96 || rxErr > 96) {
 *     // Error warning condition
 *     LogErrorWarning(txErr, rxErr);
 * }
 * @endcode
 */
status_t CAN_GetErrorCounters(uint8_t instance, 
                               uint8_t *txErrorCount, uint8_t *rxErrorCount);

/**
 * @brief Abort pending transmission
 * @details Cancels a transmission that is queued but not yet sent.
 *          Message Buffer is returned to inactive state.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] mbIndex Message Buffer index
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Transmission aborted successfully
 *         - STATUS_INVALID_PARAM: Invalid instance or MB index
 * 
 * @note If transmission has already started, abort may not take effect immediately.
 *       Message Buffer will be available for new transmission after abort completes.
 * 
 * @par Example:
 * @code
 * // Queue message
 * CAN_Send(0, 8, &msg);
 * 
 * // Changed mind, abort transmission
 * CAN_AbortTransmission(0, 8);
 * @endcode
 */
status_t CAN_AbortTransmission(uint8_t instance, uint8_t mbIndex);

/**
 * @brief Check if Message Buffer is busy
 * @details Determines if a Message Buffer is currently in use for TX or RX operation.
 *          Useful before attempting to send or reconfigure a Message Buffer.
 * 
 * @param[in] instance CAN instance number (0-2)
 * @param[in] mbIndex Message Buffer index
 * @param[out] isBusy Pointer to store busy status (true=busy, false=available)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Status checked successfully
 *         - STATUS_INVALID_PARAM: Invalid parameter (NULL pointer, invalid instance/MB)
 * 
 * @note A TX MB is busy from when message is queued until transmission completes.
 *       An RX MB is busy when it contains an unread received message.
 * 
 * @par Example:
 * @code
 * bool busy;
 * CAN_IsMbBusy(0, 8, &busy);
 * if (!busy) {
 *     // Safe to send new message
 *     CAN_Send(0, 8, &newMsg);
 * }
 * @endcode
 */
status_t CAN_IsMbBusy(uint8_t instance, uint8_t mbIndex, bool *isBusy);

/**
 * @brief Calculate bit timing parameters for target baudrate
 * @details Automatically calculates optimal timing parameters (prescaler, phase segments)
 *          to achieve the desired baudrate with given CAN clock frequency.
 *          Uses standard CAN timing recommendations (sample point at 87.5%).
 * 
 * @param[in] canClockHz CAN peripheral clock frequency in Hz
 * @param[in] baudRate Desired baudrate in bps (e.g., 500000 for 500 Kbps)
 * @param[out] timing Pointer to store calculated timing parameters
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Timing parameters calculated successfully
 *         - STATUS_INVALID_PARAM: Invalid parameter or impossible baudrate for given clock
 * 
 * @note This function is automatically called by CAN_Init().
 *       Calculated parameters aim for:
 *       - Sample point at approximately 87.5% of bit time
 *       - Maximum number of time quanta per bit (better accuracy)
 *       - RJW = 1 (standard resynchronization)
 * 
 * @par Example:
 * @code
 * can_timing_config_t timing;
 * // Calculate for 500 Kbps with 40 MHz CAN clock
 * if (CAN_CalculateTiming(40000000, 500000, &timing) == STATUS_SUCCESS) {
 *     // timing.preDiv, timing.propSeg, etc. now contain optimal values
 * }
 * @endcode
 */
status_t CAN_CalculateTiming(uint32_t canClockHz, uint32_t baudRate, 
                             can_timing_config_t *timing);

#endif /* CAN_H */
