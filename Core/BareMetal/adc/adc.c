/**
 * @file    adc.c
 * @brief   ADC Driver Implementation
 */

#include "adc.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static uint32_t s_referenceVoltage = 0;
static uint32_t s_referenceADCValue = 0;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Public Functions
 ******************************************************************************/
adc_status_t ADC_Init(ADC_Type *adc, adc_module_config_1_t *cfg, uint32_t refVoltage) {
    adc->CFG1 = cfg->value;
    s_referenceVoltage = refVoltage;
    if (cfg->field.MODE == ADC_MODE_8_BIT) {
        s_referenceADCValue = 0xFFU;            //255
    } else if (cfg->field.MODE == ADC_MODE_10_BIT) {
        s_referenceADCValue = 0x3FFU;            //1023
    } else if (cfg->field.MODE == ADC_MODE_12_BIT) {
        s_referenceADCValue = 0xFFFU;            //4095
    }
    return ADC_STATUS_SUCCESS;
}
adc_status_t ADC_InterruptConfig(ADC_Type *adc, uint8_t slot, uint32_t interruptEnable) {
    slot -= 65U; /* 65 represent letter A (uppercase) */
    if (slot >= ADC_MAX_CONVERSION_SLOTS) {
        return ADC_STATUS_INVALID_PARAM;
    }
    adc->SC1[slot] |= interruptEnable;
    return ADC_STATUS_SUCCESS;
}
adc_status_t ADC_StartConversion(ADC_Type *adc, uint8_t slot, uint32_t channel) {
    slot -= 65U; /* 65 represent letter A (uppercase) */
    if (slot >= ADC_MAX_CONVERSION_SLOTS) {
        return ADC_STATUS_INVALID_PARAM;
    }
    if (channel >= ADC_MAX_EXT_CHANNELS) {
        return ADC_STATUS_INVALID_PARAM;
    }
    if (channel >= 16U) {
        channel += 16U; /* Matching 16 higher channels */
    }
    adc->SC1[slot] &= ~ADC_MODULE_DISABLE; /* Clear previous channels */
    adc->SC1[slot] |= channel;
    return ADC_STATUS_SUCCESS;
}
uint32_t ADC_ReadValue(ADC_Type *adc, uint8_t slot) {
    slot -= 65U; /* 65 represent letter A (uppercase) */
    if (slot >= ADC_MAX_CONVERSION_SLOTS) {
        return ADC_STATUS_INVALID_PARAM;
    }
    uint32_t result = adc->R[slot];
    return ((s_referenceVoltage * result) / s_referenceADCValue); /* Convert result to mv for 0-5V range */
}
adc_status_t ADC_InterruptCheck(ADC_Type *adc, uint8_t slot) {
    slot -= 65U; /* 65 represent letter A (uppercase) */
    if (slot >= ADC_MAX_CONVERSION_SLOTS) {
        return ADC_STATUS_INVALID_PARAM;
    }
    if (((adc->SC1[slot] & ADC_SC1_COCO_MASK) >> ADC_SC1_COCO_SHIFT)) {
        return ADC_STATUS_CONVERSION_COMPLETED;
    } else {
        return ADC_STATUS_CONVERSION_WAITING;
    }
}