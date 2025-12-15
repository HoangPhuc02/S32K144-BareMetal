/**
 * @file    adc_srv.c
 * @brief   ADC Service Layer Implementation
 * @details Implementation của ADC service layer, wrapper cho ADC driver
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../inc/adc_srv.h"
#include "../../../../Core/BareMetal/adc/adc.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static bool s_adc_initialized = false;
static ADC_Type *s_adc_instance = ADC0;
static uint32_t s_ref_voltage_mv = 5000; /* Default 5V reference */

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

adc_srv_status_t ADC_SRV_Init(void)
{
    adc_module_config_1_t adc_cfg;
    
    /* Configure ADC: 12-bit mode, ALTCLK1, DIV1 */
    adc_cfg.field.ADICLK = ADC_ALTCLK1;
    adc_cfg.field.MODE = ADC_MODE_12_BIT;
    adc_cfg.field.ADIV = ADC_CLK_DIV_1;
    
    adc_status_t status = ADC_Init(s_adc_instance, &adc_cfg, s_ref_voltage_mv);
    
    if (status == ADC_STATUS_SUCCESS) {
        s_adc_initialized = true;
        return ADC_SRV_SUCCESS;
    }
    
    return ADC_SRV_ERROR;
}

adc_srv_status_t ADC_SRV_Start(uint8_t channel)
{
    if (!s_adc_initialized) {
        return ADC_SRV_NOT_INITIALIZED;
    }
    
    /* Start conversion on slot 0 */
    adc_status_t status = ADC_StartConversion(s_adc_instance, 0, channel);
    
    return (status == ADC_STATUS_SUCCESS) ? ADC_SRV_SUCCESS : ADC_SRV_ERROR;
}

adc_srv_status_t ADC_SRV_Read(adc_srv_config_t *config)
{
    if (!s_adc_initialized) {
        return ADC_SRV_NOT_INITIALIZED;
    }
    
    if (config == NULL) {
        return ADC_SRV_ERROR;
    }
    
    /* Wait for conversion complete */
    adc_status_t status;
    do {
        status = ADC_InterruptCheck(s_adc_instance, 0);
    } while (status == ADC_STATUS_CONVERSION_WAITING);
    
    /* Read raw value */
    config->raw_value = (uint16_t)ADC_ReadValue(s_adc_instance, 0);
    
    /* Calculate voltage in mV */
    /* For 12-bit: voltage = (raw_value * ref_voltage) / 4096 */
    config->voltage_mv = (uint32_t)((config->raw_value * s_ref_voltage_mv) / 4096U);
    
    return ADC_SRV_SUCCESS;
}

adc_srv_status_t ADC_SRV_Calibrate(void)
{
    if (!s_adc_initialized) {
        return ADC_SRV_NOT_INITIALIZED;
    }
    
    /* TODO: Implement calibration if driver supports */
    /* For now, just return success */
    
    return ADC_SRV_SUCCESS;
}
