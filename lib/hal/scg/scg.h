/**
 * @file    scg.h
 * @brief   SCG (System Clock Generator) Driver Header for S32K144
 * @details This file contains the SCG driver interface declarations, macros,
 *          and data structures for S32K144 microcontroller clock management.
 * 
 * @author  PhucPH32
 * @date    18/12/2025
 * @version 1.0
 * 
 * @note    This driver provides system clock configuration and management
 * @warning Ensure proper clock sequence when switching clock sources
 * 
 * @par Reference:
 * - S32K1XXRM Rev. 12.1, Chapter 28: System Clock Generator (SCG)
 * 
 */

#ifndef SCG_H
#define SCG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "scg_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief Clock source enumeration
 */
typedef enum {
    SCG_CLOCK_SRC_SIRC = 0U,    /**< Slow Internal Reference Clock (8MHz) */
    SCG_CLOCK_SRC_FIRC = 1U,    /**< Fast Internal Reference Clock (48MHz) */
    SCG_CLOCK_SRC_SOSC = 2U,    /**< System Oscillator Clock (external) */
    SCG_CLOCK_SRC_SPLL = 3U     /**< System Phase-Locked Loop */
} scg_clock_source_t;

/**
 * @brief System clock mode
 */
typedef enum {
    SCG_SYSTEM_CLOCK_MODE_RUN   = 0U,  /**< RUN mode */
    SCG_SYSTEM_CLOCK_MODE_VLPR  = 1U,  /**< VLPR mode */
    SCG_SYSTEM_CLOCK_MODE_HSRUN = 2U   /**< HSRUN mode */
} scg_system_clock_mode_t;

/**
 * @brief Clock divider values
 */
typedef enum {
    SCG_CLOCK_DIV_BY_1  = 0U,   /**< Divide by 1 */
    SCG_CLOCK_DIV_BY_2  = 1U,   /**< Divide by 2 */
    SCG_CLOCK_DIV_BY_3  = 2U,   /**< Divide by 3 */
    SCG_CLOCK_DIV_BY_4  = 3U,   /**< Divide by 4 */
    SCG_CLOCK_DIV_BY_5  = 4U,   /**< Divide by 5 */
    SCG_CLOCK_DIV_BY_6  = 5U,   /**< Divide by 6 */
    SCG_CLOCK_DIV_BY_7  = 6U,   /**< Divide by 7 */
    SCG_CLOCK_DIV_BY_8  = 7U,   /**< Divide by 8 */
    SCG_CLOCK_DIV_BY_9  = 8U,   /**< Divide by 9 */
    SCG_CLOCK_DIV_BY_10 = 9U,   /**< Divide by 10 */
    SCG_CLOCK_DIV_BY_11 = 10U,  /**< Divide by 11 */
    SCG_CLOCK_DIV_BY_12 = 11U,  /**< Divide by 12 */
    SCG_CLOCK_DIV_BY_13 = 12U,  /**< Divide by 13 */
    SCG_CLOCK_DIV_BY_14 = 13U,  /**< Divide by 14 */
    SCG_CLOCK_DIV_BY_15 = 14U,  /**< Divide by 15 */
    SCG_CLOCK_DIV_BY_16 = 15U   /**< Divide by 16 */
} scg_clock_divider_t;

/**
 * @brief SOSC (System Oscillator) frequency range
 */
typedef enum {
    SCG_SOSC_RANGE_LOW    = 0U, /**< Low frequency range (32kHz - 40kHz) */
    SCG_SOSC_RANGE_MID    = 1U, /**< Mid frequency range (1MHz - 8MHz) */
    SCG_SOSC_RANGE_HIGH   = 2U  /**< High frequency range (8MHz - 40MHz) */
} scg_sosc_range_t;

/**
 * @brief SIRC range selection
 */
typedef enum {
    SCG_SIRC_RANGE_LOW  = 0U,   /**< Slow IRC low range (2MHz) */
    SCG_SIRC_RANGE_HIGH = 1U    /**< Slow IRC high range (8MHz) */
} scg_sirc_range_t;

/**
 * @brief FIRC range selection
 */
typedef enum {
    SCG_FIRC_RANGE_48M = 0U,    /**< 48 MHz */
    SCG_FIRC_RANGE_52M = 1U,    /**< 52 MHz */
    SCG_FIRC_RANGE_56M = 2U,    /**< 56 MHz */
    SCG_FIRC_RANGE_60M = 3U     /**< 60 MHz */
} scg_firc_range_t;

/**
 * @brief SOSC configuration structure
 */
typedef struct {
    uint32_t freq;              /**< External oscillator frequency in Hz */
    scg_sosc_range_t range;     /**< Frequency range */
    bool enableInStop;          /**< Enable in STOP mode */
    bool enableInLowPower;      /**< Enable in low power mode */
    bool useExternalRef;        /**< true: external clock, false: crystal */
    bool highGainOsc;           /**< High gain oscillator */
} scg_sosc_config_t;

/**
 * @brief SIRC configuration structure
 */
typedef struct {
    scg_sirc_range_t range;     /**< SIRC range */
    bool enableInStop;          /**< Enable in STOP mode */
    bool enableInLowPower;      /**< Enable in low power mode */
    scg_clock_divider_t div1;   /**< SIRCDIV1 divider */
    scg_clock_divider_t div2;   /**< SIRCDIV2 divider */
} scg_sirc_config_t;

/**
 * @brief FIRC configuration structure
 */
typedef struct {
    scg_firc_range_t range;     /**< FIRC range */
    bool enableInStop;          /**< Enable in STOP mode */
    bool enableInLowPower;      /**< Enable in low power mode */
    bool regulator;             /**< Regulator enable */
    scg_clock_divider_t div1;   /**< FIRCDIV1 divider */
    scg_clock_divider_t div2;   /**< FIRCDIV2 divider */
} scg_firc_config_t;

/**
 * @brief SPLL configuration structure
 */
typedef struct {
    uint8_t prediv;             /**< PLL pre-divider (0-7) -> divide by (prediv+1) */
    uint8_t mult;               /**< PLL multiplier (0-31) -> multiply by (mult+16) */
    bool enableInStop;          /**< Enable in STOP mode */
    scg_clock_divider_t div1;   /**< SPLLDIV1 divider */
    scg_clock_divider_t div2;   /**< SPLLDIV2 divider */
} scg_spll_config_t;

/**
 * @brief System clock configuration structure
 */
typedef struct {
    scg_clock_source_t source;  /**< Clock source selection */
    scg_clock_divider_t divCore;/**< Core clock divider */
    scg_clock_divider_t divBus; /**< Bus clock divider */
    scg_clock_divider_t divSlow;/**< Slow clock divider */
} scg_system_clock_config_t;

/**
 * @brief Clock frequencies structure
 */
typedef struct {
    uint32_t coreClk;           /**< Core clock frequency in Hz */
    uint32_t busClk;            /**< Bus clock frequency in Hz */
    uint32_t slowClk;           /**< Slow clock frequency in Hz */
    uint32_t sircClk;           /**< SIRC clock frequency in Hz */
    uint32_t fircClk;           /**< FIRC clock frequency in Hz */
    uint32_t soscClk;           /**< SOSC clock frequency in Hz */
    uint32_t spllClk;           /**< SPLL clock frequency in Hz */
} scg_clock_frequencies_t;

/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @brief Initialize SIRC (Slow Internal Reference Clock)
 * @param[in] config Pointer to SIRC configuration structure
 * @return true if successful, false if failed
 * 
 * @note SIRC is available in all power modes
 */
bool SCG_InitSIRC(const scg_sirc_config_t *config);

/**
 * @brief Initialize FIRC (Fast Internal Reference Clock)
 * @param[in] config Pointer to FIRC configuration structure
 * @return true if successful, false if failed
 * 
 * @note FIRC is the default system clock after reset
 */
bool SCG_InitFIRC(const scg_firc_config_t *config);

/**
 * @brief Initialize SOSC (System Oscillator)
 * @param[in] config Pointer to SOSC configuration structure
 * @return true if successful, false if failed
 * 
 * @note Requires external crystal or clock source
 */
bool SCG_InitSOSC(const scg_sosc_config_t *config);

/**
 * @brief Initialize SPLL (System Phase-Locked Loop)
 * @param[in] config Pointer to SPLL configuration structure
 * @return true if successful, false if failed
 * 
 * @note SOSC must be initialized and enabled before SPLL
 * @warning SPLL output frequency = (SOSC_freq / (prediv+1)) * (mult+16)
 */
bool SCG_InitSPLL(const scg_spll_config_t *config);

/**
 * @brief Set system clock configuration for specified mode
 * @param[in] mode System clock mode (RUN, VLPR, HSRUN)
 * @param[in] config Pointer to system clock configuration
 * @return true if successful, false if failed
 * 
 * @note This function switches the system clock source
 */
bool SCG_SetSystemClockConfig(scg_system_clock_mode_t mode, const scg_system_clock_config_t *config);

/**
 * @brief Get current system clock configuration
 * @param[in] mode System clock mode to query
 * @param[out] config Pointer to store configuration
 * @return true if successful, false if failed
 */
bool SCG_GetSystemClockConfig(scg_system_clock_mode_t mode, scg_system_clock_config_t *config);

/**
 * @brief Get current clock frequencies
 * @param[out] freqs Pointer to store clock frequencies
 * @return true if successful, false if failed
 * 
 * @note This function calculates all clock frequencies based on current configuration
 */
bool SCG_GetClockFrequencies(scg_clock_frequencies_t *freqs);

/**
 * @brief Enable clock source
 * @param[in] source Clock source to enable
 * @return true if successful, false if failed
 */
bool SCG_EnableClock(scg_clock_source_t source);

/**
 * @brief Disable clock source
 * @param[in] source Clock source to disable
 * @return true if successful, false if failed
 * 
 * @warning Cannot disable currently selected system clock source
 */
bool SCG_DisableClock(scg_clock_source_t source);

/**
 * @brief Check if clock source is valid (ready)
 * @param[in] source Clock source to check
 * @return true if valid, false otherwise
 */
bool SCG_IsClockValid(scg_clock_source_t source);

/**
 * @brief Get current system clock source
 * @return Current system clock source
 */
scg_clock_source_t SCG_GetSystemClockSource(void);

/**
 * @brief Get core clock frequency
 * @return Core clock frequency in Hz
 */
uint32_t SCG_GetCoreClockFreq(void);

/**
 * @brief Get bus clock frequency
 * @return Bus clock frequency in Hz
 */
uint32_t SCG_GetBusClockFreq(void);

/**
 * @brief Get slow clock frequency
 * @return Slow clock frequency in Hz
 */
uint32_t SCG_GetSlowClockFreq(void);

/**
 * @brief Configure CLKOUT (clock output pin)
 * @param[in] source Clock source to output (0-15)
 * @return true if successful, false if failed
 * 
 * @note CLKOUT values:
 *       0 = SCG SLOW Clock
 *       1 = SOSC
 *       2 = SIRC
 *       3 = FIRC
 *       6 = SPLL
 */
bool SCG_ConfigureClockout(uint8_t source);

#endif /* SCG_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
