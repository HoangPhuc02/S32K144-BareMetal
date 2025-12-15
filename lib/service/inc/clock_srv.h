/**
 * @file    clock_srv.h
 * @brief   Clock Service Layer - Abstraction API
 * @details
 * Service layer cung cấp interface abstraction cho System Clock configuration.
 * Che giấu chi tiết implementation và cung cấp API dễ sử dụng cho application.
 * 
 * Features:
 * - System clock initialization
 * - Pre-configured clock modes
 * - Clock source selection
 * - Peripheral clock management
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

#ifndef CLOCK_SRV_H
#define CLOCK_SRV_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief Clock service status codes
 */
typedef enum {
    CLOCK_SRV_SUCCESS = 0,
    CLOCK_SRV_ERROR,
    CLOCK_SRV_NOT_INITIALIZED
} clock_srv_status_t;

/**
 * @brief Pre-defined clock modes
 */
typedef enum {
    CLOCK_SRV_MODE_RUN_80MHZ = 0,   /**< RUN mode: 80MHz (SPLL) */
    CLOCK_SRV_MODE_RUN_48MHZ,       /**< RUN mode: 48MHz (FIRC) */
    CLOCK_SRV_MODE_HSRUN_112MHZ,    /**< HSRUN mode: 112MHz (SPLL) */
    CLOCK_SRV_MODE_VLPR_4MHZ        /**< VLPR mode: 4MHz (SIRC) */
} clock_srv_mode_t;

/**
 * @brief Clock source selection
 */
typedef enum {
    CLOCK_SRV_SOURCE_SOSC = 0,      /**< System OSC (External crystal) */
    CLOCK_SRV_SOURCE_SIRC,          /**< Slow IRC (2-8 MHz) */
    CLOCK_SRV_SOURCE_FIRC,          /**< Fast IRC (48 MHz) */
    CLOCK_SRV_SOURCE_SPLL           /**< System PLL */
} clock_srv_source_t;

/**
 * @brief Clock configuration structure
 */
typedef struct {
    clock_srv_mode_t mode;          /**< Clock mode selection */
    uint32_t core_clock_hz;         /**< Core clock frequency in Hz */
    uint32_t bus_clock_hz;          /**< Bus clock frequency in Hz */
    uint32_t flash_clock_hz;        /**< Flash clock frequency in Hz */
} clock_srv_config_t;

/**
 * @brief Peripheral clock gate
 */
typedef enum {
    CLOCK_SRV_PCC_PORTA = 0,
    CLOCK_SRV_PCC_PORTB,
    CLOCK_SRV_PCC_PORTC,
    CLOCK_SRV_PCC_PORTD,
    CLOCK_SRV_PCC_PORTE,
    CLOCK_SRV_PCC_LPIT,
    CLOCK_SRV_PCC_ADC0,
    CLOCK_SRV_PCC_ADC1,
    CLOCK_SRV_PCC_FLEXCAN0,
    CLOCK_SRV_PCC_FLEXCAN1,
    CLOCK_SRV_PCC_FLEXCAN2,
    CLOCK_SRV_PCC_LPUART0,
    CLOCK_SRV_PCC_LPUART1,
    CLOCK_SRV_PCC_LPUART2
} clock_srv_peripheral_t;

/*******************************************************************************
 * API Function Declarations
 ******************************************************************************/

/**
 * @brief Initialize clock system với mode
 * @param mode Clock mode (RUN_80MHz, RUN_48MHz, HSRUN_112MHz, VLPR_4MHz)
 * @return clock_srv_status_t Status of initialization
 */
clock_srv_status_t CLOCK_SRV_Init(clock_srv_mode_t mode);

/**
 * @brief Get current clock configuration
 * @param config Pointer to store clock configuration
 * @return clock_srv_status_t Status of operation
 */
clock_srv_status_t CLOCK_SRV_GetConfig(clock_srv_config_t *config);

/**
 * @brief Enable peripheral clock
 * @param peripheral Peripheral to enable
 * @return clock_srv_status_t Status of operation
 */
clock_srv_status_t CLOCK_SRV_EnablePeripheral(clock_srv_peripheral_t peripheral);

/**
 * @brief Disable peripheral clock
 * @param peripheral Peripheral to disable
 * @return clock_srv_status_t Status of operation
 */
clock_srv_status_t CLOCK_SRV_DisablePeripheral(clock_srv_peripheral_t peripheral);

/**
 * @brief Get core clock frequency
 * @return uint32_t Core clock frequency in Hz
 */
uint32_t CLOCK_SRV_GetCoreFreq(void);

/**
 * @brief Get bus clock frequency
 * @return uint32_t Bus clock frequency in Hz
 */
uint32_t CLOCK_SRV_GetBusFreq(void);

#endif /* CLOCK_SRV_H */
