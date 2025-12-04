/**
 * @file    adc.c
 * @brief   Enhanced ADC Driver Implementation for S32K144
 * @version 2.0
 * @date    2025-12-03
 */

#include "adc.h"
#include <stddef.h>

/* ============================================================================
   Private Data Structures
   ============================================================================ */

/**
 * @brief Per-instance ADC state
 */
typedef struct {
    uint32_t referenceVoltage;  /**< Reference voltage in mV */
    uint16_t maxAdcValue;       /**< Maximum ADC value based on resolution */
    bool isCalibrated;          /**< Calibration status */
    uint8_t resolution;         /**< Current resolution mode */
} adc_instance_state_t;

/* ============================================================================
   Private Variables
   ============================================================================ */

static adc_instance_state_t s_adcState[ADC_NUM_INSTANCES] = {0};

/* ============================================================================
   Private Function Prototypes
   ============================================================================ */

static uint8_t ADC_GetInstanceIdx(ADC_Type *adc);
static uint16_t ADC_GetMaxValueForResolution(uint8_t resolution);
static bool ADC_ValidateSlot(uint8_t slot);
static bool ADC_ValidateChannel(uint8_t channel);

/* ============================================================================
   Private Functions
   ============================================================================ */

/**
 * @brief Get ADC instance index
 */
static uint8_t ADC_GetInstanceIdx(ADC_Type *adc)
{
    if (adc == ADC0) return 0;
    if (adc == ADC1) return 1;
    return 0xFF;
}

/**
 * @brief Get maximum ADC value for resolution
 */
static uint16_t ADC_GetMaxValueForResolution(uint8_t resolution)
{
    switch (resolution) {
        case ADC_MODE_8_BIT:  return 0xFFU;    // 255
        case ADC_MODE_10_BIT: return 0x3FFU;   // 1023
        case ADC_MODE_12_BIT: return 0xFFFU;   // 4095
        default: return 0xFFFU;
    }
}

/**
 * @brief Validate conversion slot
 */
static bool ADC_ValidateSlot(uint8_t slot)
{
    return (slot < ADC_MAX_CONVERSION_SLOTS);
}

/**
 * @brief Validate channel number
 */
static bool ADC_ValidateChannel(uint8_t channel)
{
    return (channel < ADC_MAX_EXT_CHANNELS);
}

/* ============================================================================
   Public Functions - Basic Configuration & Initialization
   ============================================================================ */

adc_status_t ADC_Init(ADC_Type *adc, const adc_config_t *config)
{
    /* Validate parameters */
    if (adc == NULL || config == NULL) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    uint8_t idx = ADC_GetInstanceIdx(adc);
    if (idx == 0xFF) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    if (config->referenceVoltage == 0 || config->referenceVoltage > 5000) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    /* Configure CFG1 register */
    adc->CFG1 = ADC_CFG1_ADICLK(config->clockSource) |
                ADC_CFG1_MODE(config->resolution) |
                ADC_CFG1_ADIV(config->clockDivider);
    
    /* Configure SC2 register for voltage reference */
    adc->SC2 = ADC_SC2_REFSEL(config->vrefSel);
    
    /* Store instance state */
    s_adcState[idx].referenceVoltage = config->referenceVoltage;
    s_adcState[idx].maxAdcValue = ADC_GetMaxValueForResolution(config->resolution);
    s_adcState[idx].resolution = config->resolution;
    s_adcState[idx].isCalibrated = false;
    
    return ADC_STATUS_SUCCESS;
}

adc_status_t ADC_InitDefault(ADC_Type *adc)
{
    adc_config_t config = {
        .clockSource = ADC_CLK_ALTCLK1,
        .resolution = ADC_MODE_12_BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .referenceVoltage = 3300,  // 3.3V
        .vrefSel = ADC_VREF_DEFAULT
    };
    
    return ADC_Init(adc, &config);
}

adc_status_t ADC_Deinit(ADC_Type *adc)
{
    if (adc == NULL) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    uint8_t idx = ADC_GetInstanceIdx(adc);
    if (idx == 0xFF) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    /* Reset all registers to default */
    adc->CFG1 = 0;
    adc->CFG2 = 0;
    adc->SC2 = 0;
    adc->SC3 = 0;
    
    /* Clear state */
    s_adcState[idx].referenceVoltage = 0;
    s_adcState[idx].maxAdcValue = 0;
    s_adcState[idx].isCalibrated = false;
    
    return ADC_STATUS_SUCCESS;
}

/* ============================================================================
   Calibration Functions
   ============================================================================ */

adc_status_t ADC_Calibrate(ADC_Type *adc)
{
    if (adc == NULL) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    uint8_t idx = ADC_GetInstanceIdx(adc);
    if (idx == 0xFF) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    /* Clear calibration failed flag if set */
    adc->SC3 &= ~ADC_SC3_CALF_MASK;
    
    /* Start calibration */
    adc->SC3 |= ADC_SC3_CAL_MASK;
    
    /* Wait for calibration to complete (timeout ~4000 ADC clocks) */
    uint32_t timeout = 100000;
    while ((adc->SC3 & ADC_SC3_CAL_MASK) && (timeout > 0)) {
        timeout--;
    }
    
    if (timeout == 0) {
        s_adcState[idx].isCalibrated = false;
        return ADC_STATUS_TIMEOUT;
    }
    
    /* Check if calibration failed */
    if (adc->SC3 & ADC_SC3_CALF_MASK) {
        s_adcState[idx].isCalibrated = false;
        return ADC_STATUS_CALIBRATION_FAILED;
    }
    
    /* Calculate calibration values */
    uint16_t cal_var = 0;
    
    /* Plus-Side Gain Calibration */
    cal_var = adc->CLP0;
    cal_var += adc->CLP1;
    cal_var += adc->CLP2;
    cal_var += adc->CLP3;
    cal_var += adc->CLP4;
    cal_var += adc->CLPS;
    cal_var = cal_var / 2;
    cal_var |= 0x8000; // Set MSB
    adc->PG = cal_var;
    
    /* Minus-Side Gain Calibration */
    cal_var = 0;
    cal_var = adc->CLM0;
    cal_var += adc->CLM1;
    cal_var += adc->CLM2;
    cal_var += adc->CLM3;
    cal_var += adc->CLM4;
    cal_var += adc->CLMS;
    cal_var = cal_var / 2;
    cal_var |= 0x8000; // Set MSB
    adc->MG = cal_var;
    
    s_adcState[idx].isCalibrated = true;
    return ADC_STATUS_SUCCESS;
}

bool ADC_IsCalibrated(ADC_Type *adc)
{
    if (adc == NULL) {
        return false;
    }
    
    uint8_t idx = ADC_GetInstanceIdx(adc);
    if (idx == 0xFF) {
        return false;
    }
    
    return s_adcState[idx].isCalibrated;
}

/* ============================================================================
   Hardware Averaging Functions
   ============================================================================ */

adc_status_t ADC_SetHardwareAverage(ADC_Type *adc, const adc_hw_avg_config_t *config)
{
    if (adc == NULL || config == NULL) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    if (config->enabled) {
        /* Enable hardware average and set sample count */
        adc->SC3 = (adc->SC3 & ~ADC_SC3_AVGS_MASK) | 
                   ADC_SC3_AVGE_MASK | 
                   ADC_SC3_AVGS(config->numSamples);
    } else {
        /* Disable hardware average */
        adc->SC3 &= ~ADC_SC3_AVGE_MASK;
    }
    
    return ADC_STATUS_SUCCESS;
}

adc_status_t ADC_EnableHardwareAverage(ADC_Type *adc, adc_hw_average_t numSamples)
{
    adc_hw_avg_config_t config = {
        .enabled = true,
        .numSamples = numSamples
    };
    
    return ADC_SetHardwareAverage(adc, &config);
}

adc_status_t ADC_DisableHardwareAverage(ADC_Type *adc)
{
    if (adc == NULL) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    adc->SC3 &= ~ADC_SC3_AVGE_MASK;
    return ADC_STATUS_SUCCESS;
}

/* ============================================================================
   Sample Time Configuration
   ============================================================================ */

adc_status_t ADC_SetSampleTime(ADC_Type *adc, uint8_t sampleClocks)
{
    if (adc == NULL) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    /* Configure sample time in CFG2 register */
    adc->CFG2 = (adc->CFG2 & ~ADC_CFG2_SMPLTS_MASK) | 
                ADC_CFG2_SMPLTS(sampleClocks);
    
    return ADC_STATUS_SUCCESS;
}

uint8_t ADC_GetSampleTime(ADC_Type *adc)
{
    if (adc == NULL) {
        return 0;
    }
    
    return (uint8_t)((adc->CFG2 & ADC_CFG2_SMPLTS_MASK) >> ADC_CFG2_SMPLTS_SHIFT);
}

/* ============================================================================
   Conversion Control Functions
   ============================================================================ */

adc_status_t ADC_StartConversion(ADC_Type *adc, uint8_t slot, uint8_t channel)
{
    /* Validate parameters */
    if (adc == NULL) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    if (!ADC_ValidateSlot(slot)) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    if (!ADC_ValidateChannel(channel)) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    /* Handle channel gap (16-31 mapped to 32-47 in hardware) */
    uint8_t hwChannel = channel;
    if (channel >= 16) {
        hwChannel = channel + 16;
    }
    
    /* Clear previous channel and start new conversion */
    adc->SC1[slot] = (adc->SC1[slot] & ~ADC_SC1_ADCH_MASK) | ADC_SC1_ADCH(hwChannel);
    
    return ADC_STATUS_SUCCESS;
}

adc_status_t ADC_ConvertBlocking(ADC_Type *adc, uint8_t channel, adc_result_t *result)
{
    if (adc == NULL || result == NULL) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    /* Use slot 0 for blocking conversions */
    const uint8_t slot = 0;
    
    /* Start conversion */
    adc_status_t status = ADC_StartConversion(adc, slot, channel);
    if (status != ADC_STATUS_SUCCESS) {
        return status;
    }
    
    /* Wait for completion with timeout */
    uint32_t timeout = 100000;
    while ((ADC_IsConversionComplete(adc, slot) == ADC_STATUS_CONVERSION_WAITING) && 
           (timeout > 0)) {
        timeout--;
    }
    
    if (timeout == 0) {
        return ADC_STATUS_TIMEOUT;
    }
    
    /* Read result */
    return ADC_ReadResult(adc, slot, result);
}

adc_status_t ADC_IsConversionComplete(ADC_Type *adc, uint8_t slot)
{
    if (adc == NULL || !ADC_ValidateSlot(slot)) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    if (adc->SC1[slot] & ADC_SC1_COCO_MASK) {
        return ADC_STATUS_CONVERSION_COMPLETED;
    }
    
    return ADC_STATUS_CONVERSION_WAITING;
}

/* ============================================================================
   Data Reading Functions
   ============================================================================ */

uint32_t ADC_ReadVoltage(ADC_Type *adc, uint8_t slot)
{
    adc_result_t result;
    
    if (ADC_ReadResult(adc, slot, &result) == ADC_STATUS_SUCCESS) {
        return result.voltageMillivolts;
    }
    
    return 0;
}

uint16_t ADC_ReadRawValue(ADC_Type *adc, uint8_t slot)
{
    if (adc == NULL || !ADC_ValidateSlot(slot)) {
        return 0;
    }
    
    return (uint16_t)(adc->R[slot] & ADC_R_D_MASK);
}

adc_status_t ADC_ReadResult(ADC_Type *adc, uint8_t slot, adc_result_t *result)
{
    if (adc == NULL || result == NULL || !ADC_ValidateSlot(slot)) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    uint8_t idx = ADC_GetInstanceIdx(adc);
    if (idx == 0xFF) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    /* Read raw value */
    result->rawValue = ADC_ReadRawValue(adc, slot);
    
    /* Convert to voltage */
    result->voltageMillivolts = 
        (uint32_t)((uint64_t)s_adcState[idx].referenceVoltage * result->rawValue / 
                   s_adcState[idx].maxAdcValue);
    
    return ADC_STATUS_SUCCESS;
}

/* ============================================================================
   Interrupt Management
   ============================================================================ */

adc_status_t ADC_EnableInterrupt(ADC_Type *adc, uint8_t slot)
{
    if (adc == NULL || !ADC_ValidateSlot(slot)) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    adc->SC1[slot] |= ADC_SC1_AIEN_MASK;
    return ADC_STATUS_SUCCESS;
}

adc_status_t ADC_DisableInterrupt(ADC_Type *adc, uint8_t slot)
{
    if (adc == NULL || !ADC_ValidateSlot(slot)) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    adc->SC1[slot] &= ~ADC_SC1_AIEN_MASK;
    return ADC_STATUS_SUCCESS;
}

/* ============================================================================
   Continuous Conversion Mode
   ============================================================================ */

adc_status_t ADC_EnableContinuousMode(ADC_Type *adc)
{
    if (adc == NULL) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    adc->SC3 |= ADC_SC3_ADCO_MASK;
    return ADC_STATUS_SUCCESS;
}

adc_status_t ADC_DisableContinuousMode(ADC_Type *adc)
{
    if (adc == NULL) {
        return ADC_STATUS_INVALID_PARAM;
    }
    
    adc->SC3 &= ~ADC_SC3_ADCO_MASK;
    return ADC_STATUS_SUCCESS;
}

/* ============================================================================
   Utility Functions
   ============================================================================ */

uint8_t ADC_GetInstanceIndex(ADC_Type *adc)
{
    return ADC_GetInstanceIdx(adc);
}
