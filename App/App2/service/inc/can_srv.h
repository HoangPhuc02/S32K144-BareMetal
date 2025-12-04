/**
 * @file    can_srv.h
 * @brief   CAN Service Layer - Abstraction API
 * @details
 * Service layer cung cấp interface abstraction cho CAN communication.
 * Che giấu chi tiết implementation và cung cấp API dễ sử dụng cho application.
 * 
 * Features:
 * - CAN initialization và configuration
 * - Message transmission
 * - Message reception
 * - RX callback support
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

#ifndef CAN_SRV_H
#define CAN_SRV_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief CAN service status codes
 */
typedef enum {
    CAN_SRV_SUCCESS = 0,
    CAN_SRV_ERROR,
    CAN_SRV_NOT_INITIALIZED
} can_srv_status_t;

/**
 * @brief CAN message structure
 */
typedef struct {
    uint32_t id;                    /**< Message ID (11-bit or 29-bit) */
    uint8_t data[8];                /**< Data bytes */
    uint8_t length;                 /**< Data length (0-8) */
    bool isExtended;                /**< true = 29-bit ID, false = 11-bit ID */
} can_srv_message_t;

/**
 * @brief CAN configuration structure
 */
typedef struct {
    uint32_t baudrate;              /**< Baudrate in bps (125000, 250000, 500000, 1000000) */
    uint32_t filter_id;             /**< RX filter ID */
    uint32_t filter_mask;           /**< RX filter mask */
    bool filter_extended;           /**< Extended ID filter */
} can_srv_config_t;

/**
 * @brief CAN RX callback type
 */
typedef void (*can_srv_rx_callback_t)(const can_srv_message_t *msg);

/*******************************************************************************
 * API Function Declarations
 ******************************************************************************/

/**
 * @brief Initialize CAN service với config struct
 * @param config Pointer to CAN configuration structure
 * @return can_srv_status_t Status of initialization
 */
can_srv_status_t CAN_SRV_Init(const can_srv_config_t *config);

/**
 * @brief Send CAN message
 * @param msg Pointer to message structure
 * @return can_srv_status_t Status of operation
 */
can_srv_status_t CAN_SRV_Send(const can_srv_message_t *msg);

/**
 * @brief Receive CAN message
 * @param msg Pointer to store received message
 * @return can_srv_status_t Status of operation
 */
can_srv_status_t CAN_SRV_Receive(can_srv_message_t *msg);

/**
 * @brief Register RX callback
 * @param callback Callback function for received messages
 * @return can_srv_status_t Status of operation
 */
can_srv_status_t CAN_SRV_RegisterCallback(can_srv_rx_callback_t callback);

#endif /* CAN_SRV_H */
