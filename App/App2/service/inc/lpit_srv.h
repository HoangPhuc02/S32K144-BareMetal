/**
 * @file    lpit_srv.h
 * @brief   LPIT Service Layer - Abstraction API
 * @details
 * Service layer cung cấp interface abstraction cho LPIT timer operations.
 * Che giấu chi tiết implementation và cung cấp API dễ sử dụng cho application.
 * 
 * Features:
 * - Timer initialization và configuration
 * - Periodic timer với callback
 * - Timer start/stop control
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

#ifndef LPIT_SRV_H
#define LPIT_SRV_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief LPIT service status codes
 */
typedef enum {
    LPIT_SRV_SUCCESS = 0,
    LPIT_SRV_ERROR,
    LPIT_SRV_NOT_INITIALIZED
} lpit_srv_status_t;

/**
 * @brief LPIT timer configuration structure
 */
typedef struct {
    uint8_t channel;                /**< Channel number (0-3) */
    uint32_t period_us;             /**< Period in microseconds */
    bool is_running;                /**< Timer running status */
} lpit_srv_config_t;

/**
 * @brief LPIT service callback type
 */
typedef void (*lpit_srv_callback_t)(uint8_t channel);

/*******************************************************************************
 * API Function Declarations
 ******************************************************************************/

/**
 * @brief Initialize LPIT service
 * @return lpit_srv_status_t Status of initialization
 */
lpit_srv_status_t LPIT_SRV_Init(void);

/**
 * @brief Configure timer channel với config struct
 * @param config Pointer to LPIT configuration structure
 * @param callback Callback function (NULL for no callback)
 * @return lpit_srv_status_t Status of operation
 */
lpit_srv_status_t LPIT_SRV_Config(lpit_srv_config_t *config, lpit_srv_callback_t callback);

/**
 * @brief Start timer channel
 * @param channel Channel number (0-3)
 * @return lpit_srv_status_t Status of operation
 */
lpit_srv_status_t LPIT_SRV_Start(uint8_t channel);

/**
 * @brief Stop timer channel
 * @param channel Channel number (0-3)
 * @return lpit_srv_status_t Status of operation
 */
lpit_srv_status_t LPIT_SRV_Stop(uint8_t channel);

#endif /* LPIT_SRV_H */
