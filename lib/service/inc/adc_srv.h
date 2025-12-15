/**
 * @file    adc_srv.h
 * @brief   ADC Service Layer - Abstraction API
 * @details
 * Service layer cung cấp interface abstraction cho ADC operations.
 * Che giấu chi tiết implementation và cung cấp API dễ sử dụng cho application.
 * 
 * Features:
 * - ADC initialization và configuration
 * - Single-shot conversion
 * - Calibration support
 * - Multi-channel management
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

#ifndef ADC_SRV_H
#define ADC_SRV_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief ADC service status codes
 */
typedef enum {
    ADC_SRV_SUCCESS = 0,
    ADC_SRV_ERROR,
    ADC_SRV_NOT_INITIALIZED
} adc_srv_status_t;

/**
 * @brief ADC configuration structure
 */
typedef struct {
    uint8_t channel;                /**< ADC channel number (0-15) */
    uint16_t raw_value;             /**< Raw ADC value (0-4095) */
    uint32_t voltage_mv;            /**< Voltage in millivolts */
    bool is_calibrated;             /**< Calibration status */
} adc_srv_config_t;

/*******************************************************************************
 * API Function Declarations
 ******************************************************************************/

/**
 * @brief Initialize ADC service
 * @return adc_srv_status_t Status of initialization
 */
adc_srv_status_t ADC_SRV_Init(void);

/**
 * @brief Start ADC conversion
 * @param channel Channel number (0-15)
 * @return adc_srv_status_t Status of operation
 */
adc_srv_status_t ADC_SRV_Start(uint8_t channel);

/**
 * @brief Read ADC channel value
 * @param config Pointer to ADC configuration structure
 * @return adc_srv_status_t Status of operation
 */
adc_srv_status_t ADC_SRV_Read(adc_srv_config_t *config);

/**
 * @brief Calibrate ADC
 * @return adc_srv_status_t Status of calibration
 */
adc_srv_status_t ADC_SRV_Calibrate(void);

#endif /* ADC_SRV_H */
