/**
 * @file app_types.h
 * @brief Common Type Definitions
 * @details Shared types, enums, and macros for the application
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * STATUS CODES
 ******************************************************************************/

/**
 * @brief Application status codes
 */
typedef enum {
    APP_STATUS_SUCCESS = 0,                 /**< Operation successful */
    APP_STATUS_ERROR,                       /**< Generic error */
    APP_STATUS_TIMEOUT,                     /**< Operation timeout */
    APP_STATUS_BUSY,                        /**< Resource busy */
    APP_STATUS_INVALID_PARAM,               /**< Invalid parameter */
    APP_STATUS_NOT_INITIALIZED,             /**< Not initialized */
    APP_STATUS_ALREADY_INITIALIZED          /**< Already initialized */
} app_status_t;

/*******************************************************************************
 * SYSTEM STATE
 ******************************************************************************/

/**
 * @brief Board 1 system state
 */
typedef enum {
    BOARD1_STATE_IDLE = 0,                  /**< Idle, waiting for commands */
    BOARD1_STATE_ADC_SAMPLING,              /**< ADC sampling active */
    BOARD1_STATE_ERROR                      /**< Error state */
} board1_state_t;

/**
 * @brief Board 2 system state
 */
typedef enum {
    BOARD2_STATE_IDLE = 0,                  /**< Idle, no ADC data */
    BOARD2_STATE_RECEIVING_DATA,            /**< Receiving ADC data */
    BOARD2_STATE_ERROR                      /**< Error state */
} board2_state_t;

/*******************************************************************************
 * BUTTON STATE
 ******************************************************************************/

/**
 * @brief Button event types
 */
typedef enum {
    BUTTON_EVENT_NONE = 0,                  /**< No event */
    BUTTON_EVENT_PRESSED,                   /**< Button pressed */
    BUTTON_EVENT_RELEASED                   /**< Button released */
} button_event_t;

/**
 * @brief Button ID
 */
typedef enum {
    BUTTON_1 = 0,                           /**< Button 1 (Start ADC) */
    BUTTON_2                                /**< Button 2 (Stop ADC) */
} button_id_t;

/*******************************************************************************
 * ADC DATA STRUCTURE
 ******************************************************************************/

/**
 * @brief ADC sample data
 */
typedef struct {
    uint16_t raw_value;                     /**< Raw ADC value (0-4095) */
    uint32_t voltage_mv;                    /**< Voltage in millivolts */
    uint32_t timestamp_ms;                  /**< Timestamp in milliseconds */
    bool valid;                             /**< Data valid flag */
} adc_sample_t;

/*******************************************************************************
 * CALLBACK TYPES
 ******************************************************************************/

/**
 * @brief Button callback function type
 * 
 * @param[in] button Button ID that triggered event
 * @param[in] event Button event type
 */
typedef void (*button_callback_t)(button_id_t button, button_event_t event);

/**
 * @brief CAN receive callback function type
 * 
 * @param[in] msg_id CAN message ID
 * @param[in] data Pointer to received data
 * @param[in] length Data length
 */
typedef void (*can_rx_callback_t)(uint32_t msg_id, const uint8_t *data, uint8_t length);

/**
 * @brief ADC sample callback function type
 * 
 * @param[in] sample ADC sample data
 */
typedef void (*adc_sample_callback_t)(const adc_sample_t *sample);

/*******************************************************************************
 * UTILITY MACROS
 ******************************************************************************/

/** Convert boolean to string */
#define BOOL_TO_STR(b)      ((b) ? "TRUE" : "FALSE")

/** Get array size */
#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))

/** Min/Max macros */
#define MIN(a, b)           (((a) < (b)) ? (a) : (b))
#define MAX(a, b)           (((a) > (b)) ? (a) : (b))

/** Clamp value to range */
#define CLAMP(val, min, max)    (MIN(MAX(val, min), max))

/** Unused parameter */
#define UNUSED(x)           ((void)(x))

/** Bit manipulation */
#define BIT_SET(reg, bit)       ((reg) |= (1U << (bit)))
#define BIT_CLEAR(reg, bit)     ((reg) &= ~(1U << (bit)))
#define BIT_TOGGLE(reg, bit)    ((reg) ^= (1U << (bit)))
#define BIT_CHECK(reg, bit)     (((reg) >> (bit)) & 1U)

#endif /* APP_TYPES_H */
