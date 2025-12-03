/**
 * @file    adc.h
 * @brief   ADC Driver API for S32K144
 */

#ifndef ADC_H
#define ADC_H

#include "S32K144.h"
// #include "adc_reg.h"

/* Constants */
#define ADC_MAX_EXT_CHANNELS     (32U)
#define ADC_MAX_CONVERSION_SLOTS (16U)

#define ADC_0 ADC0
#define ADC_1 ADC1

#define ADC_CONVERSION_INTERRUPT_DISABLE 0x00000000U
#define ADC_CONVERSION_INTERRUPT_ENABLE  0x00000040U

#define ADC_MODULE_DISABLE 0x0000003FU

#define ADC_ALTCLK1 0x0U

#define ADC_MODE_8_BIT  0x0U
#define ADC_MODE_12_BIT 0x1U
#define ADC_MODE_10_BIT 0x2U

#define ADC_CLK_DIV_1 0x0U
#define ADC_CLK_DIV_2 0x1U
#define ADC_CLK_DIV_4 0x2U
#define ADC_CLK_DIV_8 0x3U

/* Types */
typedef enum {
    ADC_STATUS_SUCCESS = 0x00U,
    ADC_STATUS_ERROR = 0x01U,
    ADC_STATUS_BUSY = 0x02U,
    ADC_STATUS_TIMEOUT = 0x03U,
    ADC_STATUS_INVALID_PARAM = 0x04U,
    ADC_STATUS_CONVERSION_COMPLETED = 0x05,
    ADC_STATUS_CONVERSION_WAITING = 0x06
} adc_status_t;

typedef union {
    uint32_t value;
    struct {
        uint32_t ADICLK    : 2; /** Input Clock Select */
        uint32_t MODE      : 2; /** Conversion mode selection */
        uint32_t reserved0 : 1;
        uint32_t ADIV      : 2; /** Clock Divide Select */
        uint32_t reserved1 : 25;
    } field;
} adc_module_config_1_t;

typedef union {
    uint32_t value;
    struct {
        uint32_t ADCH      : 6; /** Input channel select */
        uint32_t AIEN      : 1; /** Interrupt Enable */
        uint32_t reserved0 : 25;
    } field;
} adc_slot_config_t;

/* API Functions */
adc_status_t ADC_Init(ADC_Type *adc, adc_module_config_1_t *cfg, uint32_t refVoltage);
adc_status_t ADC_InterruptConfig(ADC_Type *adc, uint8_t slot, uint32_t interruptEnable);
adc_status_t ADC_StartConversion(ADC_Type *adc, uint8_t slot, uint32_t channel);
uint32_t ADC_ReadValue(ADC_Type *adc, uint8_t slot);
adc_status_t ADC_InterruptCheck(ADC_Type *adc, uint8_t slot);

#endif /* ADC_H */