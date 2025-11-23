/**
 * @file    port.h
 * @brief   PORT Driver Header File for S32K144
 * @details This file contains the PORT driver interface declarations for pin
 *          multiplexing, configuration, and interrupt control.
 * 
 * @author  PhucPH32
 * @date    November 22, 2025
 * @version 1.0
 * 
 * @note    This driver provides PORT configuration and pin muxing functionality
 * @warning Ensure proper clock configuration before using PORT functions
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 22, 2025): Initial version
 * 
 */

#ifndef PORT_H
#define PORT_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "port_reg.h"
#include <stdint.h>
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup PORT_Definitions PORT Definitions
 * @{
 */

/** @brief PORT enumeration */
typedef enum {
    PORT_A = 0U,    /**< Port A */
    PORT_B = 1U,    /**< Port B */
    PORT_C = 2U,    /**< Port C */
    PORT_D = 3U,    /**< Port D */
    PORT_E = 4U     /**< Port E */
} port_name_t;

/** @brief Pin multiplexing options */
typedef enum {
    PORT_MUX_DISABLED = 0U,    /**< Pin disabled (analog) */
    PORT_MUX_GPIO     = 1U,    /**< Alternative 1 (GPIO) */
    PORT_MUX_ALT2     = 2U,    /**< Alternative 2 */
    PORT_MUX_ALT3     = 3U,    /**< Alternative 3 */
    PORT_MUX_ALT4     = 4U,    /**< Alternative 4 */
    PORT_MUX_ALT5     = 5U,    /**< Alternative 5 */
    PORT_MUX_ALT6     = 6U,    /**< Alternative 6 */
    PORT_MUX_ALT7     = 7U     /**< Alternative 7 */
} port_mux_t;

/** @brief Pull resistor configuration */
typedef enum {
    PORT_PULL_DISABLE   = 0U,    /**< Internal pull resistor disabled */
    PORT_PULL_DOWN      = 1U,    /**< Internal pull-down resistor enabled */
    PORT_PULL_UP        = 2U     /**< Internal pull-up resistor enabled */
} port_pull_config_t;

/** @brief Drive strength configuration */
typedef enum {
    PORT_DRIVE_LOW  = 0U,    /**< Low drive strength */
    PORT_DRIVE_HIGH = 1U     /**< High drive strength */
} port_drive_strength_t;

/** @brief Interrupt configuration */
typedef enum {
    PORT_INT_DISABLED       = 0U,     /**< Interrupt/DMA disabled */
    PORT_INT_DMA_RISING     = 1U,     /**< DMA request on rising edge */
    PORT_INT_DMA_FALLING    = 2U,     /**< DMA request on falling edge */
    PORT_INT_DMA_EITHER     = 3U,     /**< DMA request on either edge */
    PORT_INT_LOGIC_ZERO     = 8U,     /**< Interrupt when logic 0 */
    PORT_INT_RISING_EDGE    = 9U,     /**< Interrupt on rising edge */
    PORT_INT_FALLING_EDGE   = 10U,    /**< Interrupt on falling edge */
    PORT_INT_EITHER_EDGE    = 11U,    /**< Interrupt on either edge */
    PORT_INT_LOGIC_ONE      = 12U     /**< Interrupt when logic 1 */
} port_interrupt_config_t;

/**
 * @brief PORT pin configuration structure
 */
typedef struct {
    PORT_Type* base;                         /**< PORT base pointer */
    port_mux_t mux;                          /**< Pin multiplexing option */
    port_pull_config_t pull;                 /**< Pull resistor configuration */
    port_drive_strength_t drive;             /**< Drive strength */
    port_interrupt_config_t interrupt;       /**< Interrupt configuration */
    bool passiveFilter;                      /**< Passive filter enable */
    bool digitalFilter;                      /**< Digital filter enable */
} port_pin_config_t;

/** @} */ /* End of PORT_Definitions */

/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @defgroup PORT_Functions PORT Functions
 * @{
 */

/**
 * @brief Enable PORT clock
 * @details Enables the clock for the specified PORT module
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * 
 * @return None
 * 
 * @note Must be called before any PORT configuration
 * 
 * @par Example:
 * @code
 * PORT_EnableClock(PORT_C);
 * @endcode
 */
void PORT_EnableClock(port_name_t port);

/**
 * @brief Disable PORT clock
 * @details Disables the clock for the specified PORT module
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * 
 * @return None
 * 
 * @par Example:
 * @code
 * PORT_DisableClock(PORT_C);
 * @endcode
 */
void PORT_DisableClock(port_name_t port);

/**
 * @brief Set pin multiplexing
 * @details Configures the multiplexing option for the specified pin
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * @param[in] pin Pin number (0-31)
 * @param[in] mux Multiplexing option (PORT_MUX_DISABLED to PORT_MUX_ALT7)
 * 
 * @return None
 * 
 * @note PORT clock must be enabled before calling this function
 * 
 * @par Example:
 * @code
 * PORT_SetPinMux(PORT_C, 0, PORT_MUX_GPIO);
 * @endcode
 */
void PORT_SetPinMux(port_name_t port, uint8_t pin, port_mux_t mux);

/**
 * @brief Configure pull resistor
 * @details Configures the internal pull resistor for the specified pin
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * @param[in] pin Pin number (0-31)
 * @param[in] pull Pull configuration (PORT_PULL_DISABLE, PORT_PULL_DOWN, PORT_PULL_UP)
 * 
 * @return None
 * 
 * @par Example:
 * @code
 * PORT_SetPullConfig(PORT_D, 15, PORT_PULL_UP);
 * @endcode
 */
void PORT_SetPullConfig(port_name_t port, uint8_t pin, port_pull_config_t pull);

/**
 * @brief Configure pin interrupt
 * @details Sets the interrupt/DMA configuration for the specified pin
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * @param[in] pin Pin number (0-31)
 * @param[in] config Interrupt configuration
 * 
 * @return None
 * 
 * @note NVIC must be configured separately to enable PORT interrupts
 * 
 * @par Example:
 * @code
 * PORT_SetPinInterrupt(PORT_C, 12, PORT_INT_RISING_EDGE);
 * @endcode
 */
void PORT_SetPinInterrupt(port_name_t port, uint8_t pin, port_interrupt_config_t config);

/**
 * @brief Clear pin interrupt flag
 * @details Clears the interrupt flag for the specified pin
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * @param[in] pin Pin number (0-31)
 * 
 * @return None
 * 
 * @note Must be called in interrupt handler to clear the flag
 * 
 * @par Example:
 * @code
 * PORT_ClearPinInterruptFlag(PORT_C, 12);
 * @endcode
 */
void PORT_ClearPinInterruptFlag(port_name_t port, uint8_t pin);

/**
 * @brief Get pin interrupt flag status
 * @details Checks if the interrupt flag is set for the specified pin
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * @param[in] pin Pin number (0-31)
 * 
 * @return bool true if interrupt flag is set, false otherwise
 * 
 * @par Example:
 * @code
 * if (PORT_GetPinInterruptFlag(PORT_C, 12)) {
 *     // Handle interrupt
 * }
 * @endcode
 */
bool PORT_GetPinInterruptFlag(port_name_t port, uint8_t pin);

/**
 * @brief Configure pin with structure
 * @details Configures all pin parameters using a configuration structure
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * @param[in] pin Pin number (0-31)
 * @param[in] config Pointer to configuration structure
 * 
 * @return None
 * 
 * @par Example:
 * @code
 * port_pin_config_t config = {
 *     .mux = PORT_MUX_GPIO,
 *     .pull = PORT_PULL_UP,
 *     .drive = PORT_DRIVE_HIGH,
 *     .interrupt = PORT_INT_DISABLED,
 *     .passiveFilter = false,
 *     .digitalFilter = false
 * };
 * PORT_ConfigurePin(PORT_C, 0, &config);
 * @endcode
 */
void PORT_ConfigurePin(port_name_t port, uint8_t pin, const port_pin_config_t *config);

/**
 * @brief Set drive strength
 * @details Configures the drive strength for the specified pin
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * @param[in] pin Pin number (0-31)
 * @param[in] drive Drive strength (PORT_DRIVE_LOW or PORT_DRIVE_HIGH)
 * 
 * @return None
 * 
 * @par Example:
 * @code
 * PORT_SetDriveStrength(PORT_C, 0, PORT_DRIVE_HIGH);
 * @endcode
 */
void PORT_SetDriveStrength(port_name_t port, uint8_t pin, port_drive_strength_t drive);

/**
 * @brief Enable passive filter
 * @details Enables the passive filter for the specified pin
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * @param[in] pin Pin number (0-31)
 * 
 * @return None
 * 
 * @note Passive filter provides noise rejection
 * 
 * @par Example:
 * @code
 * PORT_EnablePassiveFilter(PORT_D, 15);
 * @endcode
 */
void PORT_EnablePassiveFilter(port_name_t port, uint8_t pin);

/**
 * @brief Disable passive filter
 * @details Disables the passive filter for the specified pin
 * 
 * @param[in] port PORT name (PORT_A to PORT_E)
 * @param[in] pin Pin number (0-31)
 * 
 * @return None
 * 
 * @par Example:
 * @code
 * PORT_DisablePassiveFilter(PORT_D, 15);
 * @endcode
 */
void PORT_DisablePassiveFilter(port_name_t port, uint8_t pin);

/** @} */ /* End of PORT_Functions */

#endif /* PORT_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/