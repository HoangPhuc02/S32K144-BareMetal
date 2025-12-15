/**
 * @file    touch_sensor.h
 * @brief   Touch Sensor Middleware - Capacitive Touch Detection
 * @details Provides capacitive touch sensing using ADC measurements
 *          - Multi-channel touch detection
 *          - Baseline calibration and drift compensation
 *          - Touch threshold and debouncing
 *          - Support for both polling and interrupt modes
 * 
 * @author  PhucPH32
 * @date    14/12/2025
 * @version 1.0
 */

#ifndef TOUCH_SENSOR_H
#define TOUCH_SENSOR_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Maximum number of touch channels */
#define TOUCH_MAX_CHANNELS          (8U)

/** @brief Touch sensor states */
typedef enum {
    TOUCH_STATE_IDLE = 0,           /**< Sensor idle, not touched */
    TOUCH_STATE_TOUCHED,            /**< Sensor touched */
    TOUCH_STATE_RELEASED,           /**< Sensor just released */
    TOUCH_STATE_CALIBRATING         /**< Calibrating baseline */
} touch_state_t;

/** @brief Touch sensor status codes */
typedef enum {
    TOUCH_STATUS_SUCCESS = 0,       /**< Operation successful */
    TOUCH_STATUS_ERROR,             /**< General error */
    TOUCH_STATUS_NOT_INITIALIZED,   /**< Not initialized */
    TOUCH_STATUS_INVALID_PARAM,     /**< Invalid parameter */
    TOUCH_STATUS_BUSY               /**< Sensor busy */
} touch_status_t;

/** @brief Touch trigger mode */
typedef enum {
    TOUCH_TRIGGER_SW = 0,           /**< Software trigger (polling) */
    TOUCH_TRIGGER_HW_LPIT,          /**< Hardware trigger via LPIT */
    TOUCH_TRIGGER_HW_PDB            /**< Hardware trigger via PDB */
} touch_trigger_mode_t;

/** @brief Touch sensor configuration */
typedef struct {
    uint8_t adc_channel;            /**< ADC channel for this sensor */
    uint16_t baseline;              /**< Baseline capacitance value */
    uint16_t threshold;             /**< Touch detection threshold (delta from baseline) */
    uint8_t debounce_count;         /**< Number of samples for debouncing */
    bool enable_drift_compensation; /**< Enable baseline drift compensation */
} touch_channel_config_t;

/** @brief Touch sensor channel data */
typedef struct {
    uint8_t adc_channel;            /**< ADC channel */
    uint16_t raw_value;             /**< Current raw ADC value */
    uint16_t baseline;              /**< Baseline value */
    int16_t delta;                  /**< Delta from baseline */
    uint16_t threshold;             /**< Touch threshold */
    touch_state_t state;            /**< Current state */
    uint8_t debounce_counter;       /**< Debounce counter */
    uint8_t debounce_count;         /**< Debounce threshold */
    bool drift_comp_enabled;        /**< Drift compensation enabled */
} touch_channel_t;

/** @brief Touch sensor callback function type */
typedef void (*touch_callback_t)(uint8_t channel, touch_state_t state, int16_t delta);

/** @brief Touch sensor system configuration */
typedef struct {
    touch_trigger_mode_t trigger_mode;  /**< Trigger mode */
    uint16_t scan_period_us;            /**< Scan period in microseconds (for HW trigger) */
    uint8_t num_channels;               /**< Number of active channels */
    touch_callback_t callback;          /**< Touch event callback */
} touch_system_config_t;

/*******************************************************************************
 * API Function Declarations
 ******************************************************************************/

/**
 * @brief Initialize touch sensor system
 * @param config System configuration
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_Init(const touch_system_config_t *config);

/**
 * @brief Configure a touch sensor channel
 * @param channel_id Channel index (0-7)
 * @param config Channel configuration
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_ConfigChannel(uint8_t channel_id, const touch_channel_config_t *config);

/**
 * @brief Calibrate baseline for a channel
 * @param channel_id Channel index (0-7)
 * @param num_samples Number of samples to average for baseline
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_CalibrateChannel(uint8_t channel_id, uint16_t num_samples);

/**
 * @brief Calibrate all configured channels
 * @param num_samples Number of samples to average for baseline
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_CalibrateAll(uint16_t num_samples);

/**
 * @brief Start touch sensor scanning
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_StartScan(void);

/**
 * @brief Stop touch sensor scanning
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_StopScan(void);

/**
 * @brief Process touch sensor (call in main loop for SW trigger mode)
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_Process(void);

/**
 * @brief Get touch channel state
 * @param channel_id Channel index (0-7)
 * @param state Pointer to store state
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_GetChannelState(uint8_t channel_id, touch_state_t *state);

/**
 * @brief Get touch channel delta value
 * @param channel_id Channel index (0-7)
 * @param delta Pointer to store delta value
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_GetChannelDelta(uint8_t channel_id, int16_t *delta);

/**
 * @brief Get touch channel raw value
 * @param channel_id Channel index (0-7)
 * @param raw_value Pointer to store raw value
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_GetChannelRaw(uint8_t channel_id, uint16_t *raw_value);

/**
 * @brief Register callback for touch events
 * @param callback Callback function pointer
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_RegisterCallback(touch_callback_t callback);

/**
 * @brief Update touch threshold for a channel
 * @param channel_id Channel index (0-7)
 * @param threshold New threshold value
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_SetThreshold(uint8_t channel_id, uint16_t threshold);

/**
 * @brief Enable/disable drift compensation for a channel
 * @param channel_id Channel index (0-7)
 * @param enable Enable flag
 * @return touch_status_t Status of operation
 */
touch_status_t TOUCH_SetDriftCompensation(uint8_t channel_id, bool enable);

#endif /* TOUCH_SENSOR_H */
