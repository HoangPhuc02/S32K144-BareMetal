/**
 * @file    clock_srv_v2.h
 * @brief   Clock Service Layer V2 - Enhanced Abstraction API
 * @details
 * Service layer version 2 với thiết kế linh hoạt và powerful hơn.
 * Cho phép custom clock configuration thay vì chỉ pre-defined modes.
 * 
 * Features:
 * - Custom clock source configuration
 * - Flexible divider settings
 * - SPLL/SOSC/FIRC/SIRC individual control
 * - Runtime clock switching
 * - Clock monitoring và validation
 * - Peripheral clock source selection
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 2.0
 */

#ifndef CLOCK_SRV_V2_H
#define CLOCK_SRV_V2_H

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
    CLOCK_SRV_V2_SUCCESS = 0,
    CLOCK_SRV_V2_ERROR,
    CLOCK_SRV_V2_INVALID_CONFIG,
    CLOCK_SRV_V2_NOT_INITIALIZED,
    CLOCK_SRV_V2_TIMEOUT
} clock_srv_v2_status_t;

/**
 * @brief Clock source types
 */
typedef enum {
    CLOCK_SRV_V2_SOURCE_SOSC = 0,   /**< System OSC (External 4-40MHz) */
    CLOCK_SRV_V2_SOURCE_SIRC,       /**< Slow IRC (2-8MHz) */
    CLOCK_SRV_V2_SOURCE_FIRC,       /**< Fast IRC (48MHz) */
    CLOCK_SRV_V2_SOURCE_SPLL,       /**< System PLL (output up to 160MHz) */
    CLOCK_SRV_V2_SOURCE_NONE        /**< No clock source */
} clock_srv_v2_source_t;

/**
 * @brief Clock divider values
 */
typedef enum {
    CLOCK_SRV_V2_DIV_DISABLE = 0,
    CLOCK_SRV_V2_DIV_1,
    CLOCK_SRV_V2_DIV_2,
    CLOCK_SRV_V2_DIV_4,
    CLOCK_SRV_V2_DIV_8,
    CLOCK_SRV_V2_DIV_16,
    CLOCK_SRV_V2_DIV_32,
    CLOCK_SRV_V2_DIV_64
} clock_srv_v2_divider_t;

/**
 * @brief SOSC frequency range
 */
typedef enum {
    CLOCK_SRV_V2_SOSC_RANGE_LOW = 1,    /**< < 4MHz */
    CLOCK_SRV_V2_SOSC_RANGE_MEDIUM,     /**< 4-8 MHz */
    CLOCK_SRV_V2_SOSC_RANGE_HIGH        /**< 8-40 MHz */
} clock_srv_v2_sosc_range_t;

/**
 * @brief SPLL predivider (1-8)
 */
typedef enum {
    CLOCK_SRV_V2_SPLL_PREDIV_1 = 0,
    CLOCK_SRV_V2_SPLL_PREDIV_2,
    CLOCK_SRV_V2_SPLL_PREDIV_3,
    CLOCK_SRV_V2_SPLL_PREDIV_4,
    CLOCK_SRV_V2_SPLL_PREDIV_5,
    CLOCK_SRV_V2_SPLL_PREDIV_6,
    CLOCK_SRV_V2_SPLL_PREDIV_7,
    CLOCK_SRV_V2_SPLL_PREDIV_8
} clock_srv_v2_spll_prediv_t;

/**
 * @brief Peripheral clock source
 */
typedef enum {
    CLOCK_SRV_V2_PCS_NONE = 0,
    CLOCK_SRV_V2_PCS_SOSCDIV2,
    CLOCK_SRV_V2_PCS_SIRCDIV2,
    CLOCK_SRV_V2_PCS_FIRCDIV2,
    CLOCK_SRV_V2_PCS_SPLLDIV2,
    CLOCK_SRV_V2_PCS_LPOCLK
} clock_srv_v2_pcs_t;

/**
 * @brief SOSC configuration
 */
typedef struct {
    bool enable;                        /**< Enable SOSC */
    uint32_t freq_hz;                   /**< SOSC frequency in Hz (4-40MHz) */
    clock_srv_v2_sosc_range_t range;    /**< Frequency range */
    bool use_external_ref;              /**< true=external, false=crystal */
    clock_srv_v2_divider_t div1;        /**< SOSCDIV1 */
    clock_srv_v2_divider_t div2;        /**< SOSCDIV2 */
} clock_srv_v2_sosc_config_t;

/**
 * @brief SIRC configuration
 */
typedef struct {
    bool enable;                        /**< Enable SIRC */
    bool use_8mhz;                      /**< true=8MHz, false=2MHz */
    clock_srv_v2_divider_t div1;        /**< SIRCDIV1 */
    clock_srv_v2_divider_t div2;        /**< SIRCDIV2 */
} clock_srv_v2_sirc_config_t;

/**
 * @brief FIRC configuration
 */
typedef struct {
    bool enable;                        /**< Enable FIRC (always 48MHz) */
    clock_srv_v2_divider_t div1;        /**< FIRCDIV1 */
    clock_srv_v2_divider_t div2;        /**< FIRCDIV2 */
} clock_srv_v2_firc_config_t;

/**
 * @brief SPLL configuration
 */
typedef struct {
    bool enable;                        /**< Enable SPLL */
    clock_srv_v2_spll_prediv_t prediv;  /**< Pre-divider (1-8) */
    uint8_t mult;                       /**< Multiplier (16-47) */
    clock_srv_v2_divider_t div1;        /**< SPLLDIV1 */
    clock_srv_v2_divider_t div2;        /**< SPLLDIV2 */
} clock_srv_v2_spll_config_t;

/**
 * @brief System clock configuration
 */
typedef struct {
    clock_srv_v2_source_t source;       /**< System clock source */
    clock_srv_v2_divider_t core_div;    /**< Core clock divider */
    clock_srv_v2_divider_t bus_div;     /**< Bus clock divider */
    clock_srv_v2_divider_t slow_div;    /**< Slow (Flash) clock divider */
} clock_srv_v2_sysclk_config_t;

/**
 * @brief Complete clock configuration
 */
typedef struct {
    clock_srv_v2_sosc_config_t sosc;    /**< SOSC configuration */
    clock_srv_v2_sirc_config_t sirc;    /**< SIRC configuration */
    clock_srv_v2_firc_config_t firc;    /**< FIRC configuration */
    clock_srv_v2_spll_config_t spll;    /**< SPLL configuration */
    clock_srv_v2_sysclk_config_t sys;   /**< System clock configuration */
} clock_srv_v2_config_t;

/**
 * @brief Clock frequencies structure
 */
typedef struct {
    uint32_t core_hz;                   /**< Core clock frequency */
    uint32_t bus_hz;                    /**< Bus clock frequency */
    uint32_t slow_hz;                   /**< Slow/Flash clock frequency */
    uint32_t sosc_hz;                   /**< SOSC frequency */
    uint32_t sirc_hz;                   /**< SIRC frequency */
    uint32_t firc_hz;                   /**< FIRC frequency */
    uint32_t spll_hz;                   /**< SPLL output frequency */
} clock_srv_v2_frequencies_t;

/**
 * @brief Peripheral list
 */
typedef enum {
    CLOCK_SRV_V2_PORTA = 0,
    CLOCK_SRV_V2_PORTB,
    CLOCK_SRV_V2_PORTC,
    CLOCK_SRV_V2_PORTD,
    CLOCK_SRV_V2_PORTE,
    CLOCK_SRV_V2_LPIT,
    CLOCK_SRV_V2_ADC0,
    CLOCK_SRV_V2_ADC1,
    CLOCK_SRV_V2_FLEXCAN0,
    CLOCK_SRV_V2_FLEXCAN1,
    CLOCK_SRV_V2_FLEXCAN2,
    CLOCK_SRV_V2_LPUART0,
    CLOCK_SRV_V2_LPUART1,
    CLOCK_SRV_V2_LPUART2
} clock_srv_v2_peripheral_t;

/*******************************************************************************
 * API Function Declarations
 ******************************************************************************/

/**
 * @brief Initialize clock system với custom configuration
 * @param config Pointer to complete clock configuration
 * @return clock_srv_v2_status_t Status of initialization
 */
clock_srv_v2_status_t CLOCK_SRV_V2_Init(const clock_srv_v2_config_t *config);

/**
 * @brief Initialize với preset configuration (helper)
 * @param preset_name Preset name: "RUN_80MHz", "RUN_48MHz", "HSRUN_112MHz", "VLPR_4MHz"
 * @return clock_srv_v2_status_t Status of initialization
 */
clock_srv_v2_status_t CLOCK_SRV_V2_InitPreset(const char *preset_name);

/**
 * @brief Configure individual clock source
 * @param source Clock source to configure
 * @param config Pointer to source-specific configuration
 * @return clock_srv_v2_status_t Status of operation
 */
clock_srv_v2_status_t CLOCK_SRV_V2_ConfigSource(clock_srv_v2_source_t source, const void *config);

/**
 * @brief Switch system clock source
 * @param source New system clock source
 * @return clock_srv_v2_status_t Status of operation
 */
clock_srv_v2_status_t CLOCK_SRV_V2_SwitchSystemClock(clock_srv_v2_source_t source);

/**
 * @brief Get current clock frequencies
 * @param freq Pointer to store frequencies
 * @return clock_srv_v2_status_t Status of operation
 */
clock_srv_v2_status_t CLOCK_SRV_V2_GetFrequencies(clock_srv_v2_frequencies_t *freq);

/**
 * @brief Enable peripheral với clock source selection
 * @param peripheral Peripheral to enable
 * @param pcs Peripheral clock source (for peripherals that support it)
 * @return clock_srv_v2_status_t Status of operation
 */
clock_srv_v2_status_t CLOCK_SRV_V2_EnablePeripheral(clock_srv_v2_peripheral_t peripheral, clock_srv_v2_pcs_t pcs);

/**
 * @brief Disable peripheral clock
 * @param peripheral Peripheral to disable
 * @return clock_srv_v2_status_t Status of operation
 */
clock_srv_v2_status_t CLOCK_SRV_V2_DisablePeripheral(clock_srv_v2_peripheral_t peripheral);

/**
 * @brief Validate clock configuration
 * @param config Configuration to validate
 * @return clock_srv_v2_status_t CLOCK_SRV_V2_SUCCESS if valid
 */
clock_srv_v2_status_t CLOCK_SRV_V2_ValidateConfig(const clock_srv_v2_config_t *config);

/**
 * @brief Get current configuration
 * @param config Pointer to store current configuration
 * @return clock_srv_v2_status_t Status of operation
 */
clock_srv_v2_status_t CLOCK_SRV_V2_GetConfig(clock_srv_v2_config_t *config);

/**
 * @brief Calculate SPLL output frequency
 * @param sosc_freq SOSC input frequency in Hz
 * @param prediv Pre-divider value
 * @param mult Multiplier value
 * @return uint32_t SPLL output frequency in Hz
 */
uint32_t CLOCK_SRV_V2_CalculateSPLLFreq(uint32_t sosc_freq, uint8_t prediv, uint8_t mult);

/*******************************************************************************
 * Helper Macros
 ******************************************************************************/

/**
 * @brief Create default SOSC configuration (8MHz external crystal)
 */
#define CLOCK_SRV_V2_SOSC_DEFAULT() {               \
    .enable = true,                                  \
    .freq_hz = 8000000,                              \
    .range = CLOCK_SRV_V2_SOSC_RANGE_HIGH,          \
    .use_external_ref = false,                       \
    .div1 = CLOCK_SRV_V2_DIV_1,                     \
    .div2 = CLOCK_SRV_V2_DIV_1                      \
}

/**
 * @brief Create default FIRC configuration (48MHz)
 */
#define CLOCK_SRV_V2_FIRC_DEFAULT() {               \
    .enable = true,                                  \
    .div1 = CLOCK_SRV_V2_DIV_1,                     \
    .div2 = CLOCK_SRV_V2_DIV_1                      \
}

/**
 * @brief Create SPLL configuration for 80MHz (8MHz * 20 / 2 = 80MHz)
 */
#define CLOCK_SRV_V2_SPLL_80MHZ() {                 \
    .enable = true,                                  \
    .prediv = CLOCK_SRV_V2_SPLL_PREDIV_1,           \
    .mult = 20,                                      \
    .div1 = CLOCK_SRV_V2_DIV_2,                     \
    .div2 = CLOCK_SRV_V2_DIV_4                      \
}

#endif /* CLOCK_SRV_V2_H */
