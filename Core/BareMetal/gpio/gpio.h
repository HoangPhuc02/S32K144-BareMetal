/**
 * @file    gpio.h
 * @brief   GPIO Driver API for S32K144
 */

#ifndef GPIO_H
#define GPIO_H

#include "S32K144.h"
// #include "gpio_reg.h"

/* Constants */
#define GPIO_MAX_PINS (32U)
#define GPIO_A        PTA
#define GPIO_B        PTB
#define GPIO_C        PTC
#define GPIO_D        PTD
#define GPIO_E        PTE

#define GPIO_INPUT  0U
#define GPIO_OUTPUT 1U

/* Types */
typedef enum {
    GPIO_STATUS_SUCCESS = 0x00U,
    GPIO_STATUS_ERROR = 0x01U,
    GPIO_STATUS_BUSY = 0x02U,
    GPIO_STATUS_TIMEOUT = 0x03U,
    GPIO_STATUS_INVALID_PARAM = 0x04U
} gpio_status_t;

/* API Functions */
gpio_status_t GPIO_Init(GPIO_Type *gpio, uint8_t pin, uint32_t direction);
gpio_status_t GPIO_SetPin(GPIO_Type *gpio, uint8_t pin);
gpio_status_t GPIO_ClearPin(GPIO_Type *gpio, uint8_t pin);
gpio_status_t GPIO_TogglePin(GPIO_Type *gpio, uint8_t pin);

#endif /* GPIO_H */