/**
 * @file    port_srv.h
 * @brief   PORT Service Layer - Abstraction API
 * @details
 * Service layer cung cấp interface abstraction cho PORT pin multiplexing.
 * Che giấu chi tiết implementation và cung cấp API dễ sử dụng cho application.
 * 
 * Features:
 * - Pin mux configuration với struct tối ưu
 * - Pull-up/Pull-down configuration
 * - Pin interrupt configuration
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

#ifndef PORT_SRV_H
#define PORT_SRV_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief PORT service status codes
 */
typedef enum {
    PORT_SRV_SUCCESS = 0,
    PORT_SRV_ERROR,
    PORT_SRV_NOT_INITIALIZED
} port_srv_status_t;

/**
 * @brief Pin mux alternatives
 */
typedef enum {
    PORT_SRV_MUX_DISABLED = 0,      /**< Pin disabled (analog) */
    PORT_SRV_MUX_GPIO     = 1,      /**< GPIO functionality */
    PORT_SRV_MUX_ALT2     = 2,      /**< Alternative 2 */
    PORT_SRV_MUX_ALT3     = 3,      /**< Alternative 3 */
    PORT_SRV_MUX_ALT4     = 4,      /**< Alternative 4 */
    PORT_SRV_MUX_ALT5     = 5,      /**< Alternative 5 */
    PORT_SRV_MUX_ALT6     = 6,      /**< Alternative 6 */
    PORT_SRV_MUX_ALT7     = 7       /**< Alternative 7 */
} port_srv_mux_t;

/**
 * @brief Pull configuration
 */
typedef enum {
    PORT_SRV_PULL_DISABLE = 0,
    PORT_SRV_PULL_DOWN,
    PORT_SRV_PULL_UP
} port_srv_pull_t;

/**
 * @brief Interrupt configuration
 */
typedef enum {
    PORT_SRV_INT_DISABLE = 0,
    PORT_SRV_INT_RISING,
    PORT_SRV_INT_FALLING,
    PORT_SRV_INT_BOTH
} port_srv_interrupt_t;

/**
 * @brief Pin configuration structure (tối ưu hóa setting)
 */
typedef struct {
    uint8_t port;                   /**< Port identifier (PORTA, PORTB, PORTC, PORTD, PORTE) */
    uint8_t pin;                    /**< Pin number (0-31) */
    port_srv_mux_t mux;             /**< Mux alternative */
    port_srv_pull_t pull;           /**< Pull configuration */
    port_srv_interrupt_t interrupt; /**< Interrupt configuration */
} port_srv_pin_config_t;

/*******************************************************************************
 * API Function Declarations
 ******************************************************************************/

/**
 * @brief Initialize PORT service
 * @return port_srv_status_t Status of initialization
 */
port_srv_status_t PORT_SRV_Init(void);

/**
 * @brief Configure pin với struct (recommended)
 * @param config Pointer to pin configuration structure
 * @return port_srv_status_t Status of operation
 */
port_srv_status_t PORT_SRV_ConfigPin(const port_srv_pin_config_t *config);

/**
 * @brief Configure pin mux (simple)
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @param mux Mux alternative
 * @return port_srv_status_t Status of operation
 */
port_srv_status_t PORT_SRV_SetMux(uint8_t port, uint8_t pin, port_srv_mux_t mux);

/**
 * @brief Configure pin interrupt
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @param interrupt Interrupt configuration
 * @return port_srv_status_t Status of operation
 */
port_srv_status_t PORT_SRV_ConfigInterrupt(uint8_t port, uint8_t pin, port_srv_interrupt_t interrupt);

/**
 * @brief Clear port interrupt flag
 * @param port Port identifier
 * @param pin Pin number (0-31)
 * @return port_srv_status_t Status of operation
 */
port_srv_status_t PORT_SRV_ClearInterruptFlag(uint8_t port, uint8_t pin);

#endif /* PORT_SRV_H */
