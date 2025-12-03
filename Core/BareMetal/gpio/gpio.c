/**
 * @file    gpio.c
 * @brief   GPIO Driver Implementation
 */

#include "gpio.h"
// #include <devassert.h>
/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Public Functions
 ******************************************************************************/
gpio_status_t GPIO_Init(GPIO_Type *gpio, uint8_t pin, uint32_t direction) {
    if (pin >= GPIO_MAX_PINS) {
        return GPIO_STATUS_INVALID_PARAM;
    }
    gpio->PDDR |= (direction << pin);
    return GPIO_STATUS_SUCCESS;
}
gpio_status_t GPIO_SetPin(GPIO_Type *gpio, uint8_t pin) {
    if (pin >= GPIO_MAX_PINS) {
        return GPIO_STATUS_INVALID_PARAM;
    }
    gpio->PSOR |= (1U << pin);
    return GPIO_STATUS_SUCCESS;
}
gpio_status_t GPIO_ClearPin(GPIO_Type *gpio, uint8_t pin) {
    if (pin >= GPIO_MAX_PINS) {
        return GPIO_STATUS_INVALID_PARAM;
    }
    gpio->PCOR |= (1U << pin);
    return GPIO_STATUS_SUCCESS;
}
gpio_status_t GPIO_TogglePin(GPIO_Type *gpio, uint8_t pin) {
    if (pin >= GPIO_MAX_PINS) {
        return GPIO_STATUS_INVALID_PARAM;
    }
    gpio->PTOR |= (1U << pin);
    return GPIO_STATUS_SUCCESS;
}
