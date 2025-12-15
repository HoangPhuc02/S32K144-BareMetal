/**
 * @file    adc.c
 * @brief   ADC Driver Implementation for S32K144
 * @details This file contains the implementation of ADC driver functions
 *          for S32K144 microcontroller including initialization, calibration,
 *          conversion control, and result retrieval.
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "adc.h"
#include "pcc_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Timeout for ADC operations (in loop iterations) */
#define ADC_TIMEOUT_COUNT       (100000UL)

/** @brief Calibration timeout (in loop iterations) */
#define ADC_CALIBRATION_TIMEOUT (500000UL)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/** @brief Array of ADC base addresses */
static ADC_RegType * const s_adcBases[] = {ADC0, ADC1};

/** @brief Array of ADC callback functions */
static ADC_Callback_t s_adcCallbacks[2] = {NULL, NULL};

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static ADC_RegType* ADC_GetBase(ADC_Instance_t instance);

/*******************************************************************************
 * Code
 ******************************************************************************/

/**
 * @brief Get ADC base address from instance
 * 
 * @param[in] instance ADC instance
 * @return ADC_RegType* Pointer to ADC base address
 */
static ADC_RegType* ADC_GetBase(ADC_Instance_t instance)
{
    return s_adcBases[instance];
}

/**
 * @brief Enable ADC peripheral clock
 */
ADC_Status_t ADC_EnableClock(ADC_Instance_t instance)
{
    uint32_t pccIndex;
    
    /* Get PCC index based on ADC instance */
    if (instance == ADC_INSTANCE_0) {
        pccIndex = PCC_ADC0_INDEX;
    } else if (instance == ADC_INSTANCE_1) {
        pccIndex = PCC_ADC1_INDEX;
    } else {
        return ADC_STATUS_ERROR;
    }
    
    /* Enable clock gate */
    PCC->PCCn[pccIndex] |= PCC_PCCn_CGC_MASK;
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Disable ADC peripheral clock
 */
ADC_Status_t ADC_DisableClock(ADC_Instance_t instance)
{
    uint32_t pccIndex;
    
    /* Get PCC index based on ADC instance */
    if (instance == ADC_INSTANCE_0) {
        pccIndex = PCC_ADC0_INDEX;
    } else if (instance == ADC_INSTANCE_1) {
        pccIndex = PCC_ADC1_INDEX;
    } else {
        return ADC_STATUS_ERROR;
    }
    
    /* Disable clock gate */
    PCC->PCCn[pccIndex] &= ~PCC_PCCn_CGC_MASK;
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Initialize ADC peripheral
 */
ADC_Status_t ADC_Init(ADC_Instance_t instance, const ADC_Config_t *config)
{
    ADC_RegType *base;
    uint32_t pccIndex;
    uint32_t cfg1Value = 0U;
    uint32_t cfg2Value = 0U;
    uint32_t sc2Value = 0U;
    uint32_t sc3Value = 0U;
    
    if (config == NULL) {
        return ADC_STATUS_ERROR;
    }
    
    base = ADC_GetBase(instance);
    
    /* Get PCC index */
    if (instance == ADC_INSTANCE_0) {
        pccIndex = PCC_ADC0_INDEX;
    } else {
        pccIndex = PCC_ADC1_INDEX;
    }
    
    /* Configure PCC clock source */
    PCC->PCCn[pccIndex] &= ~PCC_PCCn_PCS_MASK;
    PCC->PCCn[pccIndex] |= PCC_PCCn_PCS(config->clockSource);
    
    /* Enable clock gate */
    PCC->PCCn[pccIndex] |= PCC_PCCn_CGC_MASK;
    
    /* Configure CFG1: clock source, resolution, clock divider */
    cfg1Value = ADC_CFG1_ADICLK(config->clockSource) |
                ADC_CFG1_MODE(config->resolution) |
                ADC_CFG1_ADIV(config->clockDivider);
    base->CFG1 = cfg1Value;
    
    /* Configure CFG2: sample time (default to maximum) */
    cfg2Value = ADC_CFG2_SMPLTS(0xFFU);
    base->CFG2 = cfg2Value;
    
    /* Configure SC2: voltage reference, DMA, trigger source */
    sc2Value = ADC_SC2_REFSEL(config->voltageRef);
    
    if (config->dmaEnable) {
        sc2Value |= ADC_SC2_DMAEN_MASK;
    }
    
    if (config->triggerSource == ADC_TRIGGER_HARDWARE) {
        sc2Value |= ADC_SC2_ADTRG_MASK;
    }
    
    base->SC2 = sc2Value;
    
    /* Configure SC3: continuous mode */
    if (config->continuousMode) {
        sc3Value |= ADC_SC3_ADCO_MASK;
    }
    
    base->SC3 = sc3Value;
    
    /* Configure SC1[0]: disable channel, configure interrupt */
    if (config->interruptEnable) {
        base->SC1[0] = ADC_SC1_ADCH(ADC_CHANNEL_DISABLED) | ADC_SC1_AIEN_MASK;
    } else {
        base->SC1[0] = ADC_SC1_ADCH(ADC_CHANNEL_DISABLED);
    }
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Perform ADC calibration
 */
ADC_Status_t ADC_Calibrate(ADC_Instance_t instance)
{
    ADC_RegType *base;
    uint32_t timeout;
    uint32_t calValue;
    
    base = ADC_GetBase(instance);
    
    /* Clear calibration failed flag */
    base->SC3 &= ~ADC_SC3_CALF_MASK;
    
    /* Start calibration */
    base->SC3 |= ADC_SC3_CAL_MASK;
    
    /* Wait for calibration to complete */
    timeout = ADC_CALIBRATION_TIMEOUT;
    while ((timeout > 0U) && ((base->SC3 & ADC_SC3_CAL_MASK) != 0U)) {
        timeout--;
    }
    
    if (timeout == 0U) {
        return ADC_STATUS_TIMEOUT;
    }
    
    /* Check calibration status */
    if (ADC_IS_CALIBRATION_FAILED(base)) {
        return ADC_STATUS_CALIBRATION_FAILED;
    }
    
    /* Calculate and store calibration values */
    calValue = base->CLP0 + base->CLP1 + base->CLP2 + 
               base->CLP3 + base->CLPS;
    calValue = calValue >> 1U;
    calValue |= 0x8000U;  /* Set MSB */
    base->G = calValue;
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Configure hardware averaging
 */
ADC_Status_t ADC_ConfigureAveraging(ADC_Instance_t instance, const ADC_AverageConfig_t *avgConfig)
{
    ADC_RegType *base;
    uint32_t sc3Value;
    
    if (avgConfig == NULL) {
        return ADC_STATUS_ERROR;
    }
    
    base = ADC_GetBase(instance);
    
    /* Read current SC3 value */
    sc3Value = base->SC3;
    
    /* Clear averaging bits */
    sc3Value &= ~(ADC_SC3_AVGE_MASK | ADC_SC3_AVGS_MASK);
    
    /* Configure averaging if enabled */
    if (avgConfig->enable) {
        sc3Value |= ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(avgConfig->averageMode);
    }
    
    /* Write back to SC3 */
    base->SC3 = sc3Value;
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Start ADC conversion
 */
ADC_Status_t ADC_StartConversion(ADC_Instance_t instance, ADC_Channel_t channel)
{
    ADC_RegType *base;
    uint32_t sc1Value;
    
    base = ADC_GetBase(instance);
    
    /* Check if conversion is active */
    if (ADC_IS_CONVERSION_ACTIVE(base)) {
        return ADC_STATUS_BUSY;
    }
    
    /* Read current SC1[0] to preserve interrupt enable bit */
    sc1Value = base->SC1[0] & ADC_SC1_AIEN_MASK;
    
    /* Start conversion by writing channel */
    sc1Value |= ADC_SC1_ADCH(channel);
    base->SC1[0] = sc1Value;
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Stop ADC conversion
 */
ADC_Status_t ADC_StopConversion(ADC_Instance_t instance)
{
    ADC_RegType *base;
    
    base = ADC_GetBase(instance);
    
    /* Stop conversion by disabling channel */
    ADC_DISABLE_CHANNEL(base);
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Check if conversion is complete
 */
bool ADC_IsConversionComplete(ADC_Instance_t instance)
{
    ADC_RegType *base;
    
    base = ADC_GetBase(instance);
    
    return ADC_IS_CONVERSION_COMPLETE(base);
}

/**
 * @brief Wait for conversion to complete
 */
ADC_Status_t ADC_WaitForConversion(ADC_Instance_t instance, uint32_t timeout)
{
    ADC_RegType *base;
    uint32_t timeoutCounter;
    
    base = ADC_GetBase(instance);
    
    if (timeout == 0U) {
        /* Wait forever */
        while (!ADC_IS_CONVERSION_COMPLETE(base)) {
            /* Wait */
        }
        return ADC_STATUS_SUCCESS;
    } else {
        /* Wait with timeout */
        timeoutCounter = timeout;
        while (!ADC_IS_CONVERSION_COMPLETE(base) && (timeoutCounter > 0U)) {
            timeoutCounter--;
        }
        
        if (timeoutCounter == 0U) {
            return ADC_STATUS_TIMEOUT;
        }
        
        return ADC_STATUS_SUCCESS;
    }
}

/**
 * @brief Get conversion result
 */
uint16_t ADC_GetResult(ADC_Instance_t instance)
{
    ADC_RegType *base;
    
    base = ADC_GetBase(instance);
    
    return (uint16_t)(ADC_GET_RESULT(base, 0) & ADC_R_D_MASK);
}

/**
 * @brief Perform blocking conversion
 */
ADC_Status_t ADC_ReadBlocking(ADC_Instance_t instance, ADC_Channel_t channel, uint16_t *result)
{
    ADC_Status_t status;
    
    if (result == NULL) {
        return ADC_STATUS_ERROR;
    }
    
    /* Start conversion */
    status = ADC_StartConversion(instance, channel);
    if (status != ADC_STATUS_SUCCESS) {
        return status;
    }
    
    /* Wait for conversion to complete */
    status = ADC_WaitForConversion(instance, ADC_TIMEOUT_COUNT);
    if (status != ADC_STATUS_SUCCESS) {
        return status;
    }
    
    /* Get result */
    *result = ADC_GetResult(instance);
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Convert ADC result to voltage
 */
uint32_t ADC_ConvertToVoltage(uint16_t adcValue, ADC_Resolution_t resolution, uint32_t vrefMillivolts)
{
    uint32_t maxValue;
    uint32_t voltage;
    
    /* Get maximum ADC value based on resolution */
    switch (resolution) {
        case ADC_RESOLUTION_8BIT:
            maxValue = 255U;
            break;
        case ADC_RESOLUTION_10BIT:
            maxValue = 1023U;
            break;
        case ADC_RESOLUTION_12BIT:
            maxValue = 4095U;
            break;
        default:
            maxValue = 4095U;
            break;
    }
    
    /* Calculate voltage: (adcValue * vref) / maxValue */
    voltage = ((uint32_t)adcValue * vrefMillivolts) / maxValue;
    
    return voltage;
}

/**
 * @brief Register callback function
 */
ADC_Status_t ADC_RegisterCallback(ADC_Instance_t instance, ADC_Callback_t callback)
{
    if (instance >= 2U) {
        return ADC_STATUS_ERROR;
    }
    
    s_adcCallbacks[instance] = callback;
    
    return ADC_STATUS_SUCCESS;
}

/*******************************************************************************
 * Interrupt Handlers (Optional - to be implemented by user)
 ******************************************************************************/

/**
 * @brief ADC0 interrupt handler
 * 
 * @note This function should be called from the ADC0 IRQ handler
 */
void ADC0_IRQHandler(void)
{
    uint16_t result;
    
    /* Check if conversion is complete */
    if (ADC_IS_CONVERSION_COMPLETE(ADC0)) {
        /* Get result */
        result = ADC_GetResult(ADC_INSTANCE_0);
        
        /* Call callback if registered */
        if (s_adcCallbacks[ADC_INSTANCE_0] != NULL) {
            s_adcCallbacks[ADC_INSTANCE_0](ADC_INSTANCE_0, result);
        }
    }
}

/**
 * @brief ADC1 interrupt handler
 * 
 * @note This function should be called from the ADC1 IRQ handler
 */
void ADC1_IRQHandler(void)
{
    uint16_t result;
    
    /* Check if conversion is complete */
    if (ADC_IS_CONVERSION_COMPLETE(ADC1)) {
        /* Get result */
        result = ADC_GetResult(ADC_INSTANCE_1);
        
        /* Call callback if registered */
        if (s_adcCallbacks[ADC_INSTANCE_1] != NULL) {
            s_adcCallbacks[ADC_INSTANCE_1](ADC_INSTANCE_1, result);
        }
    }
}

/*******************************************************************************
 * Hardware Trigger Functions (LPIT Trigger)
 ******************************************************************************/

/**
 * @brief Configure ADC for LPIT hardware trigger
 */
ADC_Status_t ADC_ConfigHardwareTrigger(ADC_Instance_t instance, ADC_Channel_t channel, bool enableInterrupt)
{
    ADC_RegType *base;
    
    if (instance >= 2U) {
        return ADC_STATUS_ERROR;
    }
    
    base = ADC_GetBase(instance);
    
    /* Enable hardware trigger in SC2 register */
    base->SC2 |= ADC_SC2_ADTRG_MASK;
    
    /* Configure channel with optional interrupt */
    if (enableInterrupt) {
        base->SC1[0] = ADC_SC1_ADCH(channel) | ADC_SC1_AIEN_MASK;
    } else {
        base->SC1[0] = ADC_SC1_ADCH(channel);
    }
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Start ADC in hardware trigger mode
 */
ADC_Status_t ADC_StartHardwareTrigger(ADC_Instance_t instance, ADC_Channel_t channel)
{
    ADC_RegType *base;
    
    if (instance >= 2U) {
        return ADC_STATUS_ERROR;
    }
    
    base = ADC_GetBase(instance);
    
    /* Enable hardware trigger */
    base->SC2 |= ADC_SC2_ADTRG_MASK;
    
    /* Set channel to start conversion on trigger */
    base->SC1[0] = ADC_SC1_ADCH(channel);
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Stop ADC hardware trigger mode
 */
ADC_Status_t ADC_StopHardwareTrigger(ADC_Instance_t instance)
{
    ADC_RegType *base;
    
    if (instance >= 2U) {
        return ADC_STATUS_ERROR;
    }
    
    base = ADC_GetBase(instance);
    
    /* Disable hardware trigger (switch to software trigger) */
    base->SC2 &= ~ADC_SC2_ADTRG_MASK;
    
    /* Disable channel */
    base->SC1[0] = ADC_SC1_ADCH(ADC_CHANNEL_DISABLED);
    
    return ADC_STATUS_SUCCESS;
}

/**
 * @brief Check if hardware trigger is enabled
 */
bool ADC_IsHardwareTriggerEnabled(ADC_Instance_t instance)
{
    ADC_RegType *base;
    
    if (instance >= 2U) {
        return false;
    }
    
    base = ADC_GetBase(instance);
    
    return ((base->SC2 & ADC_SC2_ADTRG_MASK) != 0U);
}
