/**
 * @file    adc.h
 * @brief   Enhanced ADC Driver API for S32K144
 * @version 2.0
 * @date    2025-12-03
 * 
 * Enhanced features:
 * - Hardware calibration support
 * - Hardware averaging configuration
 * - Sample time configuration
 * - Improved API design (slot = 0-15 instead of 'A'-'P')
 * - Per-instance state management
 * - Better error checking
 */

#ifndef ADC_H
#define ADC_H

#include "S32K144.h"
#include <stdbool.h>

/* Constants */
#define ADC_MAX_EXT_CHANNELS     (32U)
#define ADC_MAX_CONVERSION_SLOTS (16U)
#define ADC_NUM_INSTANCES        (2U)

/* ADC Instance Aliases */
#define ADC_0 ADC0
#define ADC_1 ADC1

/* Clock Source Selection */
#define ADC_CLK_ALTCLK1  0x0U  /**< Alternate clock 1 (ALTCLK1) */
#define ADC_CLK_ALTCLK2  0x1U  /**< Alternate clock 2 (ALTCLK2) */
#define ADC_CLK_ALTCLK3  0x2U  /**< Alternate clock 3 (ALTCLK3) */
#define ADC_CLK_ALTCLK4  0x3U  /**< Alternate clock 4 (ALTCLK4) */

/* Resolution Modes */
#define ADC_MODE_8_BIT   0x0U  /**< 8-bit conversion */
#define ADC_MODE_12_BIT  0x1U  /**< 12-bit conversion */
#define ADC_MODE_10_BIT  0x2U  /**< 10-bit conversion */

/* Clock Divider */
#define ADC_CLK_DIV_1    0x0U  /**< Divide by 1 */
#define ADC_CLK_DIV_2    0x1U  /**< Divide by 2 */
#define ADC_CLK_DIV_4    0x2U  /**< Divide by 4 */
#define ADC_CLK_DIV_8    0x3U  /**< Divide by 8 */

/* Status Codes */
typedef enum {
    ADC_STATUS_SUCCESS = 0x00U,              /**< Operation successful */
    ADC_STATUS_ERROR = 0x01U,                /**< Generic error */
    ADC_STATUS_BUSY = 0x02U,                 /**< ADC is busy */
    ADC_STATUS_TIMEOUT = 0x03U,              /**< Operation timeout */
    ADC_STATUS_INVALID_PARAM = 0x04U,        /**< Invalid parameter */
    ADC_STATUS_CONVERSION_COMPLETED = 0x05U, /**< Conversion completed */
    ADC_STATUS_CONVERSION_WAITING = 0x06U,   /**< Conversion in progress */
    ADC_STATUS_CALIBRATION_FAILED = 0x07U    /**< Calibration failed */
} adc_status_t;

/* Hardware Average Selection */
typedef enum {
    ADC_AVG_DISABLED = 0x0U,  /**< Hardware average disabled */
    ADC_AVG_4_SAMPLES = 0x0U, /**< 4 samples averaged */
    ADC_AVG_8_SAMPLES = 0x1U, /**< 8 samples averaged */
    ADC_AVG_16_SAMPLES = 0x2U, /**< 16 samples averaged */
    ADC_AVG_32_SAMPLES = 0x3U  /**< 32 samples averaged */
} adc_hw_average_t;

/* Reference Voltage Selection */
typedef enum {
    ADC_VREF_DEFAULT = 0x0U,   /**< Default voltage reference (VREFH/VREFL) */
    ADC_VREF_ALTERNATE = 0x1U  /**< Alternate voltage reference */
} adc_vref_select_t;

/**
 * @brief ADC Configuration Structure
 * 
 * This structure contains all parameters needed to configure an ADC module.
 */
typedef struct {
    uint8_t clockSource;        /**< Clock source selection (ADC_CLK_xxx) */
    uint8_t resolution;         /**< Resolution mode (ADC_MODE_xxx) */
    uint8_t clockDivider;       /**< Clock divider (ADC_CLK_DIV_xxx) */
    uint32_t referenceVoltage;  /**< Reference voltage in mV (e.g., 3300 for 3.3V) */
    adc_vref_select_t vrefSel;  /**< Voltage reference selection */
} adc_config_t;

/**
 * @brief Hardware Average Configuration Structure
 */
typedef struct {
    bool enabled;               /**< Enable/disable hardware averaging */
    adc_hw_average_t numSamples; /**< Number of samples to average */
} adc_hw_avg_config_t;

/**
 * @brief ADC Conversion Result Structure
 */
typedef struct {
    uint16_t rawValue;          /**< Raw ADC value (0-255, 0-1023, or 0-4095) */
    uint32_t voltageMillivolts; /**< Converted voltage in mV */
} adc_result_t;

/* ============================================================================
   Basic Configuration & Initialization
   ============================================================================ */

/**
 * @brief Initialize ADC module with configuration
 * 
 * @param adc Pointer to ADC instance (ADC0 or ADC1)
 * @param config Pointer to configuration structure
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_Init(ADC_Type *adc, const adc_config_t *config);

/**
 * @brief Initialize ADC with default settings
 * 
 * Default: 12-bit, 3.3V reference, clock div 4, no averaging
 * 
 * @param adc Pointer to ADC instance
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_InitDefault(ADC_Type *adc);

/**
 * @brief Deinitialize ADC module
 * 
 * @param adc Pointer to ADC instance
 * @return ADC_STATUS_SUCCESS on success
 */
adc_status_t ADC_Deinit(ADC_Type *adc);

/* ============================================================================
   Calibration Functions (Priority 1)
   ============================================================================ */

/**
 * @brief Perform ADC hardware calibration
 * 
 * This function executes the automatic calibration sequence to improve
 * accuracy. Should be called after initialization and before conversions.
 * 
 * @param adc Pointer to ADC instance
 * @return ADC_STATUS_SUCCESS if calibration succeeded
 *         ADC_STATUS_CALIBRATION_FAILED if calibration failed
 *         ADC_STATUS_INVALID_PARAM if adc is NULL
 * 
 * @note Calibration takes approximately 4000 ADC clock cycles
 * @note Call this function after ADC_Init() and before starting conversions
 */
adc_status_t ADC_Calibrate(ADC_Type *adc);

/**
 * @brief Check if ADC is calibrated
 * 
 * @param adc Pointer to ADC instance
 * @return true if calibrated, false otherwise
 */
bool ADC_IsCalibrated(ADC_Type *adc);

/* ============================================================================
   Hardware Averaging Functions (Priority 1)
   ============================================================================ */

/**
 * @brief Configure hardware averaging
 * 
 * Hardware averaging reduces noise by averaging multiple samples automatically.
 * 
 * @param adc Pointer to ADC instance
 * @param config Pointer to hardware average configuration
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 * 
 * @note Enabling averaging increases conversion time proportionally
 * @note Example: 32 samples averaging = 32x longer conversion time
 */
adc_status_t ADC_SetHardwareAverage(ADC_Type *adc, const adc_hw_avg_config_t *config);

/**
 * @brief Enable hardware averaging with specified sample count
 * 
 * @param adc Pointer to ADC instance
 * @param numSamples Number of samples to average (4, 8, 16, or 32)
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_EnableHardwareAverage(ADC_Type *adc, adc_hw_average_t numSamples);

/**
 * @brief Disable hardware averaging
 * 
 * @param adc Pointer to ADC instance
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_DisableHardwareAverage(ADC_Type *adc);

/* ============================================================================
   Sample Time Configuration (Priority 1)
   ============================================================================ */

/**
 * @brief Set ADC sample time
 * 
 * Sample time affects accuracy for high-impedance sources.
 * Longer sample time = better accuracy but slower conversion.
 * 
 * @param adc Pointer to ADC instance
 * @param sampleClocks Sample time in ADC clock cycles (0-255)
 *                     Recommended: 12-24 for most applications
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 * 
 * @note Default sample time is typically too short for high-impedance sources
 * @note Formula: Total conversion time = Sample time + Resolution clocks
 */
adc_status_t ADC_SetSampleTime(ADC_Type *adc, uint8_t sampleClocks);

/**
 * @brief Get current sample time setting
 * 
 * @param adc Pointer to ADC instance
 * @return Sample time in clock cycles, 0 if error
 */
uint8_t ADC_GetSampleTime(ADC_Type *adc);

/* ============================================================================
   Conversion Control Functions
   ============================================================================ */

/**
 * @brief Start ADC conversion on specified channel
 * 
 * @param adc Pointer to ADC instance
 * @param slot Conversion slot (0-15, replaces 'A'-'P' encoding)
 * @param channel Channel number (0-31)
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_StartConversion(ADC_Type *adc, uint8_t slot, uint8_t channel);

/**
 * @brief Perform blocking conversion (wait for completion)
 * 
 * This is a convenience function that starts conversion and waits for result.
 * 
 * @param adc Pointer to ADC instance
 * @param channel Channel number (0-31)
 * @param result Pointer to store conversion result
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_ConvertBlocking(ADC_Type *adc, uint8_t channel, adc_result_t *result);

/**
 * @brief Check if conversion is complete
 * 
 * @param adc Pointer to ADC instance
 * @param slot Conversion slot (0-15)
 * @return ADC_STATUS_CONVERSION_COMPLETED if done
 *         ADC_STATUS_CONVERSION_WAITING if still converting
 */
adc_status_t ADC_IsConversionComplete(ADC_Type *adc, uint8_t slot);

/* ============================================================================
   Data Reading Functions
   ============================================================================ */

/**
 * @brief Read conversion result (voltage in mV)
 * 
 * @param adc Pointer to ADC instance
 * @param slot Conversion slot (0-15)
 * @return Voltage in millivolts, 0 on error
 */
uint32_t ADC_ReadVoltage(ADC_Type *adc, uint8_t slot);

/**
 * @brief Read raw ADC value
 * 
 * @param adc Pointer to ADC instance
 * @param slot Conversion slot (0-15)
 * @return Raw ADC value (0-255, 0-1023, or 0-4095), 0 on error
 */
uint16_t ADC_ReadRawValue(ADC_Type *adc, uint8_t slot);

/**
 * @brief Read full conversion result (raw + voltage)
 * 
 * @param adc Pointer to ADC instance
 * @param slot Conversion slot (0-15)
 * @param result Pointer to store result
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_ReadResult(ADC_Type *adc, uint8_t slot, adc_result_t *result);

/* ============================================================================
   Interrupt Management
   ============================================================================ */

/**
 * @brief Enable conversion complete interrupt
 * 
 * @param adc Pointer to ADC instance
 * @param slot Conversion slot (0-15)
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_EnableInterrupt(ADC_Type *adc, uint8_t slot);

/**
 * @brief Disable conversion complete interrupt
 * 
 * @param adc Pointer to ADC instance
 * @param slot Conversion slot (0-15)
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_DisableInterrupt(ADC_Type *adc, uint8_t slot);

/* ============================================================================
   Continuous Conversion Mode
   ============================================================================ */

/**
 * @brief Enable continuous conversion mode
 * 
 * In continuous mode, ADC automatically re-triggers after each conversion.
 * 
 * @param adc Pointer to ADC instance
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_EnableContinuousMode(ADC_Type *adc);

/**
 * @brief Disable continuous conversion mode
 * 
 * @param adc Pointer to ADC instance
 * @return ADC_STATUS_SUCCESS on success, error code otherwise
 */
adc_status_t ADC_DisableContinuousMode(ADC_Type *adc);

/* ============================================================================
   Utility Functions
   ============================================================================ */

/**
 * @brief Get ADC instance index (0 for ADC0, 1 for ADC1)
 * 
 * @param adc Pointer to ADC instance
 * @return Instance index, 0xFF on error
 */
uint8_t ADC_GetInstanceIndex(ADC_Type *adc);

#endif /* ADC_H */
