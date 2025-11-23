/**
 * @file    gpio.h
 * @brief   GPIO Driver Header File for S32K144
 * @details This file contains the GPIO driver interface declarations, macros,
 *          and data structures for S32K144 microcontroller.
 * 
 * @author  PhucPH32
 * @date    November 22, 2025
 * @version 1.0
 * 
 * @note This driver provides register-level access to GPIO peripherals
 * @warning Ensure proper clock configuration before using GPIO functions
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 22, 2025): Initial version
 * 
 * @copyright Copyright (c) 2025
 * Licensed under [Your License]
 */

#ifndef GPIO_H
#define GPIO_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "gpio_reg.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup GPIO_Definitions GPIO Definitions
 * @{
 */

/** @brief GPIO port enumeration */
typedef enum {
    GPIO_PORT_A = 0U,    /**< Port A */
    GPIO_PORT_B = 1U,    /**< Port B */
    GPIO_PORT_C = 2U,    /**< Port C */
    GPIO_PORT_D = 3U,    /**< Port D */
    GPIO_PORT_E = 4U     /**< Port E */
} gpio_port_t;

/** @brief GPIO pin direction enumeration */
typedef enum {
    GPIO_DIR_INPUT  = 0U,    /**< Pin configured as input */
    GPIO_DIR_OUTPUT = 1U     /**< Pin configured as output */
} gpio_direction_t;

/** @brief GPIO pin level enumeration */
typedef enum {
    GPIO_LEVEL_LOW  = 0U,    /**< Logic level low */
    GPIO_LEVEL_HIGH = 1U     /**< Logic level high */
} gpio_level_t;

/** @} */ /* End of GPIO_Definitions */

/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @defgroup GPIO_Functions GPIO Functions
 * @{
 */

/**
 * @brief Initialize GPIO pin
 * @details Configures the specified pin as GPIO with the given direction
 * 
 * @param[in] port GPIO port (GPIO_PORT_A to GPIO_PORT_E)
 * @param[in] pin Pin number (0-31)
 * @param[in] direction Pin direction (GPIO_DIR_INPUT or GPIO_DIR_OUTPUT)
 * 
 * @return None
 * 
 * @note Ensure the port clock is enabled before calling this function
 * 
 * @par Example:
 * @code
 * GPIO_Init(GPIO_PORT_C, 0, GPIO_DIR_OUTPUT);
 * @endcode
 */
void GPIO_Init(gpio_port_t port, uint8_t pin, gpio_direction_t direction);

/**
 * @brief Write value to GPIO pin
 * @details Sets or clears the output value of the specified GPIO pin
 * 
 * @param[in] port GPIO port (GPIO_PORT_A to GPIO_PORT_E)
 * @param[in] pin Pin number (0-31)
 * @param[in] value Pin level (GPIO_LEVEL_LOW or GPIO_LEVEL_HIGH)
 * 
 * @return None
 * 
 * @note Pin must be configured as output before calling this function
 * 
 * @par Example:
 * @code
 * GPIO_WritePin(GPIO_PORT_C, 0, GPIO_LEVEL_HIGH);
 * @endcode
 */
void GPIO_WritePin(gpio_port_t port, uint8_t pin, gpio_level_t value);

/**
 * @brief Read value from GPIO pin
 * @details Reads the current input value of the specified GPIO pin
 * 
 * @param[in] port GPIO port (GPIO_PORT_A to GPIO_PORT_E)
 * @param[in] pin Pin number (0-31)
 * 
 * @return gpio_level_t Current pin level (GPIO_LEVEL_LOW or GPIO_LEVEL_HIGH)
 * 
 * @par Example:
 * @code
 * gpio_level_t button_state = GPIO_ReadPin(GPIO_PORT_D, 15);
 * @endcode
 */
gpio_level_t GPIO_ReadPin(gpio_port_t port, uint8_t pin);

/**
 * @brief Toggle GPIO pin
 * @details Inverts the output value of the specified GPIO pin
 * 
 * @param[in] port GPIO port (GPIO_PORT_A to GPIO_PORT_E)
 * @param[in] pin Pin number (0-31)
 * 
 * @return None
 * 
 * @note Pin must be configured as output before calling this function
 * 
 * @par Example:
 * @code
 * GPIO_TogglePin(GPIO_PORT_C, 0);
 * @endcode
 */
void GPIO_TogglePin(gpio_port_t port, uint8_t pin);

/**
 * @brief Set GPIO pin direction
 * @details Changes the direction of the specified GPIO pin
 * 
 * @param[in] port GPIO port (GPIO_PORT_A to GPIO_PORT_E)
 * @param[in] pin Pin number (0-31)
 * @param[in] direction Pin direction (GPIO_DIR_INPUT or GPIO_DIR_OUTPUT)
 * 
 * @return None
 * 
 * @par Example:
 * @code
 * GPIO_SetPinDirection(GPIO_PORT_C, 0, GPIO_DIR_INPUT);
 * @endcode
 */
void GPIO_SetPinDirection(gpio_port_t port, uint8_t pin, gpio_direction_t direction);

/** @} */ /* End of GPIO_Functions */

#endif /* GPIO_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/