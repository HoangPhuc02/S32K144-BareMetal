/**
 * @file    adc.h
 * @brief   ADC Driver Header File for S32K144
 * @details This file contains the ADC driver interface declarations, macros,
 *          and data structures for S32K144 microcontroller.
 * 
 * @author  PhucPH32
 * @date    26/11/2025
 * @version 1.0
 * 
 * @note This driver provides register-level access to ADC peripherals
 * @warning Ensure proper clock configuration before using ADC functions
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 26, 2025): Initial version
 * 
 * @copyright Copyright (c) 2025
 * Licensed under [Your License]
 */

#ifndef ADC_H
#define ADC_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "adc_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup ADC_Definitions ADC Definitions
 * @{
 */

/** @brief ADC instance enumeration */
typedef enum {
    ADC_INSTANCE_0 = 0U,    /**< ADC0 instance */
    ADC_INSTANCE_1 = 1U     /**< ADC1 instance */
} ADC_Instance_t;

/** @brief ADC clock source enumeration */
typedef enum {
    ADC_CLK_ALT1 = 0U,      /**< Alternate clock 1 (ALTCLK1) */
    ADC_CLK_ALT2 = 1U,      /**< Alternate clock 2 (ALTCLK2) */
    ADC_CLK_ALT3 = 2U,      /**< Alternate clock 3 (ALTCLK3) */
    ADC_CLK_ALT4 = 3U       /**< Alternate clock 4 (ALTCLK4) */
} ADC_ClockSource_t;

/** @brief ADC resolution mode enumeration */
typedef enum {
    ADC_RESOLUTION_8BIT  = 0U,    /**< 8-bit resolution */
    ADC_RESOLUTION_10BIT = 1U,    /**< 10-bit resolution */
    ADC_RESOLUTION_12BIT = 2U     /**< 12-bit resolution */
} ADC_Resolution_t;

/** @brief ADC clock divider enumeration */
typedef enum {
    ADC_CLK_DIV_1 = 0U,     /**< Divide by 1 */
    ADC_CLK_DIV_2 = 1U,     /**< Divide by 2 */
    ADC_CLK_DIV_4 = 2U,     /**< Divide by 4 */
    ADC_CLK_DIV_8 = 3U      /**< Divide by 8 */
} ADC_ClockDivider_t;

/** @brief ADC voltage reference enumeration */
typedef enum {
    ADC_VREF_VREFH_VREFL = 0U,    /**< Default: VREFH/VREFL as Vref */
    ADC_VREF_VALTH       = 1U     /**< Alternate: VALTH as Vref */
} ADC_VoltageRef_t;

/** @brief ADC trigger source enumeration */
typedef enum {
    ADC_TRIGGER_SOFTWARE = 0U,    /**< Software trigger */
    ADC_TRIGGER_HARDWARE = 1U     /**< Hardware trigger */
} ADC_TriggerSource_t;

/** @brief ADC hardware average enumeration */
typedef enum {
    ADC_AVERAGE_4   = 0U,    /**< Average 4 samples */
    ADC_AVERAGE_8   = 1U,    /**< Average 8 samples */
    ADC_AVERAGE_16  = 2U,    /**< Average 16 samples */
    ADC_AVERAGE_32  = 3U     /**< Average 32 samples */
} ADC_AverageMode_t;

/** @brief ADC channel enumeration */
typedef enum {
    ADC_CHANNEL_AD0  = 0U,     /**< ADC channel 0 */
    ADC_CHANNEL_AD1  = 1U,     /**< ADC channel 1 */
    ADC_CHANNEL_AD2  = 2U,     /**< ADC channel 2 */
    ADC_CHANNEL_AD3  = 3U,     /**< ADC channel 3 */
    ADC_CHANNEL_AD4  = 4U,     /**< ADC channel 4 */
    ADC_CHANNEL_AD5  = 5U,     /**< ADC channel 5 */
    ADC_CHANNEL_AD6  = 6U,     /**< ADC channel 6 */
    ADC_CHANNEL_AD7  = 7U,     /**< ADC channel 7 */
    ADC_CHANNEL_AD8  = 8U,     /**< ADC channel 8 */
    ADC_CHANNEL_AD9  = 9U,     /**< ADC channel 9 */
    ADC_CHANNEL_AD10 = 10U,    /**< ADC channel 10 */
    ADC_CHANNEL_AD11 = 11U,    /**< ADC channel 11 */
    ADC_CHANNEL_AD12 = 12U,    /**< ADC channel 12 */
    ADC_CHANNEL_AD13 = 13U,    /**< ADC channel 13 */
    ADC_CHANNEL_AD14 = 14U,    /**< ADC channel 14 */
    ADC_CHANNEL_AD15 = 15U,    /**< ADC channel 15 */
    ADC_CHANNEL_DISABLED = 31U /**< Channel disabled */
} ADC_Channel_t;

/** @brief ADC status enumeration */
typedef enum {
    ADC_STATUS_SUCCESS = 0U,        /**< Operation successful */
    ADC_STATUS_ERROR = 1U,          /**< General error */
    ADC_STATUS_BUSY = 2U,           /**< ADC is busy */
    ADC_STATUS_TIMEOUT = 3U,        /**< Timeout occurred */
    ADC_STATUS_CALIBRATION_FAILED = 4U  /**< Calibration failed */
} ADC_Status_t;

/**
 * @brief ADC configuration structure
 */
typedef struct {
    ADC_ClockSource_t   clockSource;      /**< ADC clock source selection */
    ADC_Resolution_t    resolution;       /**< ADC resolution mode */
    ADC_ClockDivider_t  clockDivider;     /**< ADC clock divider */
    ADC_VoltageRef_t    voltageRef;       /**< Voltage reference selection */
    ADC_TriggerSource_t triggerSource;    /**< Trigger source selection */
    bool                continuousMode;   /**< Enable continuous conversion mode */
    bool                dmaEnable;        /**< Enable DMA for ADC */
    bool                interruptEnable;  /**< Enable conversion complete interrupt */
} ADC_Config_t;

/**
 * @brief ADC hardware average configuration structure
 */
typedef struct {
    bool               enable;       /**< Enable hardware averaging */
    ADC_AverageMode_t  averageMode;  /**< Hardware average mode */
} ADC_AverageConfig_t;

/**
 * @brief ADC callback function type
 * @param instance ADC instance that triggered the callback
 * @param result Conversion result value
 */
typedef void (*ADC_Callback_t)(ADC_Instance_t instance, uint16_t result);

/** @} */

/*******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * @defgroup ADC_Functions ADC Functions
 * @{
 */

/**
 * @brief Initialize ADC peripheral with specified configuration
 * 
 * This function initializes the ADC peripheral with the provided configuration.
 * It configures clock source, resolution, voltage reference, and other settings.
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @param[in] config Pointer to ADC configuration structure
 * @return ADC_Status_t Status of the operation
 * 
 * @note Clock for ADC peripheral must be enabled before calling this function
 * 
 * @par Example:
 * @code
 * ADC_Config_t adcConfig = {
 *     .clockSource = ADC_CLK_ALT1,
 *     .resolution = ADC_RESOLUTION_12BIT,
 *     .clockDivider = ADC_CLK_DIV_4,
 *     .voltageRef = ADC_VREF_VREFH_VREFL,
 *     .triggerSource = ADC_TRIGGER_SOFTWARE,
 *     .continuousMode = false,
 *     .dmaEnable = false,
 *     .interruptEnable = false
 * };
 * ADC_Init(ADC_INSTANCE_0, &adcConfig);
 * @endcode
 */
ADC_Status_t ADC_Init(ADC_Instance_t instance, const ADC_Config_t *config);

/**
 * @brief Perform ADC calibration
 * 
 * This function performs a calibration sequence for the ADC. Calibration should
 * be performed after initialization and whenever operating conditions change
 * significantly (temperature, voltage, etc.).
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @return ADC_Status_t Status of the calibration
 * @retval ADC_STATUS_SUCCESS Calibration successful
 * @retval ADC_STATUS_CALIBRATION_FAILED Calibration failed
 * @retval ADC_STATUS_TIMEOUT Calibration timeout
 * 
 * @note ADC must be initialized before calling this function
 * @warning Calibration can take several milliseconds to complete
 * 
 * @par Example:
 * @code
 * if (ADC_Calibrate(ADC_INSTANCE_0) != ADC_STATUS_SUCCESS) {
 *     // Handle calibration error
 * }
 * @endcode
 */
ADC_Status_t ADC_Calibrate(ADC_Instance_t instance);

/**
 * @brief Configure hardware averaging for ADC
 * 
 * This function enables or disables hardware averaging and sets the number
 * of samples to average.
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @param[in] avgConfig Pointer to averaging configuration structure
 * @return ADC_Status_t Status of the operation
 * 
 * @par Example:
 * @code
 * ADC_AverageConfig_t avgConfig = {
 *     .enable = true,
 *     .averageMode = ADC_AVERAGE_16
 * };
 * ADC_ConfigureAveraging(ADC_INSTANCE_0, &avgConfig);
 * @endcode
 */
ADC_Status_t ADC_ConfigureAveraging(ADC_Instance_t instance, const ADC_AverageConfig_t *avgConfig);

/**
 * @brief Start ADC conversion on specified channel
 * 
 * This function starts a conversion on the specified ADC channel. For software
 * trigger mode, this initiates the conversion. For hardware trigger mode, this
 * configures the channel and waits for the trigger.
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @param[in] channel ADC channel to convert
 * @return ADC_Status_t Status of the operation
 * 
 * @note In continuous mode, conversion will continue until stopped
 * 
 * @par Example:
 * @code
 * ADC_StartConversion(ADC_INSTANCE_0, ADC_CHANNEL_AD0);
 * @endcode
 */
ADC_Status_t ADC_StartConversion(ADC_Instance_t instance, ADC_Channel_t channel);

/**
 * @brief Stop ADC conversion
 * 
 * This function stops the current ADC conversion by disabling the channel.
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @return ADC_Status_t Status of the operation
 * 
 * @par Example:
 * @code
 * ADC_StopConversion(ADC_INSTANCE_0);
 * @endcode
 */
ADC_Status_t ADC_StopConversion(ADC_Instance_t instance);

/**
 * @brief Check if ADC conversion is complete
 * 
 * This function checks if the current ADC conversion has completed.
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @return bool Conversion complete status
 * @retval true Conversion is complete
 * @retval false Conversion is in progress
 * 
 * @par Example:
 * @code
 * while (!ADC_IsConversionComplete(ADC_INSTANCE_0)) {
 *     // Wait for conversion
 * }
 * @endcode
 */
bool ADC_IsConversionComplete(ADC_Instance_t instance);

/**
 * @brief Wait for ADC conversion to complete
 * 
 * This function blocks until the ADC conversion completes or timeout occurs.
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @param[in] timeout Maximum time to wait in microseconds (0 = wait forever)
 * @return ADC_Status_t Status of the operation
 * @retval ADC_STATUS_SUCCESS Conversion completed
 * @retval ADC_STATUS_TIMEOUT Timeout occurred
 * 
 * @par Example:
 * @code
 * if (ADC_WaitForConversion(ADC_INSTANCE_0, 10000) == ADC_STATUS_SUCCESS) {
 *     uint16_t result = ADC_GetResult(ADC_INSTANCE_0);
 * }
 * @endcode
 */
ADC_Status_t ADC_WaitForConversion(ADC_Instance_t instance, uint32_t timeout);

/**
 * @brief Get ADC conversion result
 * 
 * This function returns the result of the last completed ADC conversion.
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @return uint16_t Conversion result (resolution-dependent)
 * 
 * @note Check conversion complete status before reading result
 * 
 * @par Example:
 * @code
 * if (ADC_IsConversionComplete(ADC_INSTANCE_0)) {
 *     uint16_t result = ADC_GetResult(ADC_INSTANCE_0);
 * }
 * @endcode
 */
uint16_t ADC_GetResult(ADC_Instance_t instance);

/**
 * @brief Perform blocking ADC conversion
 * 
 * This function performs a complete ADC conversion sequence: start conversion,
 * wait for completion, and return the result.
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @param[in] channel ADC channel to convert
 * @param[out] result Pointer to store conversion result
 * @return ADC_Status_t Status of the operation
 * 
 * @par Example:
 * @code
 * uint16_t adcValue;
 * if (ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD0, &adcValue) == ADC_STATUS_SUCCESS) {
 *     // Use adcValue
 * }
 * @endcode
 */
ADC_Status_t ADC_ReadBlocking(ADC_Instance_t instance, ADC_Channel_t channel, uint16_t *result);

/**
 * @brief Convert ADC result to voltage
 * 
 * This function converts the ADC digital result to voltage in millivolts.
 * 
 * @param[in] adcValue ADC conversion result
 * @param[in] resolution ADC resolution mode used
 * @param[in] vrefMillivolts Reference voltage in millivolts
 * @return uint32_t Voltage in millivolts
 * 
 * @par Example:
 * @code
 * uint16_t adcValue = ADC_GetResult(ADC_INSTANCE_0);
 * uint32_t voltage = ADC_ConvertToVoltage(adcValue, ADC_RESOLUTION_12BIT, 3300);
 * @endcode
 */
uint32_t ADC_ConvertToVoltage(uint16_t adcValue, ADC_Resolution_t resolution, uint32_t vrefMillivolts);

/**
 * @brief Enable ADC peripheral clock
 * 
 * This function enables the clock for the specified ADC peripheral.
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @return ADC_Status_t Status of the operation
 * 
 * @note This must be called before ADC_Init()
 * 
 * @par Example:
 * @code
 * ADC_EnableClock(ADC_INSTANCE_0);
 * @endcode
 */
ADC_Status_t ADC_EnableClock(ADC_Instance_t instance);

/**
 * @brief Disable ADC peripheral clock
 * 
 * This function disables the clock for the specified ADC peripheral.
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @return ADC_Status_t Status of the operation
 * 
 * @par Example:
 * @code
 * ADC_DisableClock(ADC_INSTANCE_0);
 * @endcode
 */
ADC_Status_t ADC_DisableClock(ADC_Instance_t instance);

/**
 * @brief Register callback function for ADC interrupt
 * 
 * This function registers a callback function to be called when ADC conversion
 * completes (if interrupts are enabled).
 * 
 * @param[in] instance ADC instance (ADC0 or ADC1)
 * @param[in] callback Pointer to callback function
 * @return ADC_Status_t Status of the operation
 * 
 * @note Interrupt must be enabled in ADC configuration
 * 
 * @par Example:
 * @code
 * void MyADCCallback(ADC_Instance_t instance, uint16_t result) {
 *     // Handle conversion result
 * }
 * 
 * ADC_RegisterCallback(ADC_INSTANCE_0, MyADCCallback);
 * @endcode
 */
ADC_Status_t ADC_RegisterCallback(ADC_Instance_t instance, ADC_Callback_t callback);

/** @} */

#endif /* ADC_H */
