/**
 * @file    gpio_srv.h
 * @brief   GPIO Service Layer - Abstraction API
 * @details
 * Service layer cung cấp interface abstraction cho GPIO operations.
 * Che giấu chi tiết implementation và cung cấp API dễ sử dụng cho application.
 * 
 * Features:
 * - GPIO initialization và configuration
 * - Digital I/O operations
 * - Pin toggle operation
 * - External interrupt support (for buttons, sensors)
 * - Callback registration for interrupt handling
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.1
 */

#ifndef GPIO_SRV_H
#define GPIO_SRV_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief GPIO service status codes
 */
typedef enum {
    GPIO_SRV_SUCCESS = 0,
    GPIO_SRV_ERROR,
    GPIO_SRV_NOT_INITIALIZED
} gpio_srv_status_t;

/**
 * @brief GPIO interrupt trigger types
 */
typedef enum {
    GPIO_SRV_INT_DISABLE = 0,      /**< Interrupt disabled */
    GPIO_SRV_INT_RISING_EDGE,      /**< Rising edge trigger */
    GPIO_SRV_INT_FALLING_EDGE,     /**< Falling edge trigger */
    GPIO_SRV_INT_BOTH_EDGES,       /**< Both edges trigger */
    GPIO_SRV_INT_LOGIC_LOW,        /**< Logic 0 trigger */
    GPIO_SRV_INT_LOGIC_HIGH        /**< Logic 1 trigger */
} gpio_srv_interrupt_t;

/**
 * @brief GPIO callback function pointer type
 * @param port Port identifier
 * @param pin Pin number that triggered interrupt
 */
typedef void (*gpio_srv_callback_t)(uint8_t port, uint8_t pin);

/*******************************************************************************
 * API Function Declarations
 ******************************************************************************/

/**
 * @brief Initialize GPIO service
 * @return gpio_srv_status_t Status of initialization
 */
gpio_srv_status_t GPIO_SRV_Init(void);

/**
 * @brief Configure pin as input
 * @param port Port identifier (PORTA, PORTB, PORTC, PORTD, PORTE)
 * @param pin Pin number (0-31)
 * @return gpio_srv_status_t Status of operation
 */
gpio_srv_status_t GPIO_SRV_ConfigInput(uint8_t port, uint8_t pin);

/**
 * @brief Configure pin as output
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @return gpio_srv_status_t Status of operation
 */
gpio_srv_status_t GPIO_SRV_ConfigOutput(uint8_t port, uint8_t pin);

/**
 * @brief Read pin value
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @return uint8_t Pin value (0 or 1)
 */
uint8_t GPIO_SRV_Read(uint8_t port, uint8_t pin);

/**
 * @brief Write pin value
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @param value Value to write (0 or 1)
 * @return gpio_srv_status_t Status of operation
 */
gpio_srv_status_t GPIO_SRV_Write(uint8_t port, uint8_t pin, uint8_t value);

/**
 * @brief Toggle pin value
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @return gpio_srv_status_t Status of operation
 */
gpio_srv_status_t GPIO_SRV_Toggle(uint8_t port, uint8_t pin);

/**
 * @brief Enable interrupt on pin (for buttons, external events)
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @param trigger Interrupt trigger type
 * @param callback Callback function to execute on interrupt (can be NULL)
 * @return gpio_srv_status_t Status of operation
 */
gpio_srv_status_t GPIO_SRV_EnableInterrupt(uint8_t port, uint8_t pin, 
                                            gpio_srv_interrupt_t trigger,
                                            gpio_srv_callback_t callback);

/**
 * @brief Disable interrupt on pin
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @return gpio_srv_status_t Status of operation
 */
gpio_srv_status_t GPIO_SRV_DisableInterrupt(uint8_t port, uint8_t pin);

/**
 * @brief Check if interrupt flag is set
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @return bool true if interrupt flag set, false otherwise
 */
bool GPIO_SRV_IsInterruptPending(uint8_t port, uint8_t pin);

/**
 * @brief Clear interrupt flag
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @return gpio_srv_status_t Status of operation
 */
gpio_srv_status_t GPIO_SRV_ClearInterrupt(uint8_t port, uint8_t pin);

/**
 * @brief Read pin value (alias for GPIO_SRV_Read)
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @return uint8_t Pin value (0 or 1)
 */
uint8_t GPIO_SRV_ReadPin(uint8_t port, uint8_t pin);

/**
 * @brief Write pin value (alias for GPIO_SRV_Write)
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @param value Value to write (0 or 1)
 * @return gpio_srv_status_t Status of operation
 */
gpio_srv_status_t GPIO_SRV_WritePin(uint8_t port, uint8_t pin, uint8_t value);

/**
 * @brief Toggle pin value (alias for GPIO_SRV_Toggle)
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @return gpio_srv_status_t Status of operation
 */
gpio_srv_status_t GPIO_SRV_TogglePin(uint8_t port, uint8_t pin);

#endif /* GPIO_SRV_H */
