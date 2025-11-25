/**
 * @file    clock.h
 * @brief   Clock Driver Header File for S32K144
 * @details This file contains the clock driver interface declarations, macros,
 *          and data structures for S32K144 microcontroller.
 * 
 * @author  PhucPH32
 * @date    25/11/2025
 * @version 1.0
 * 
 * @note This driver provides clock configuration and management
 * @warning Ensure proper clock sequence when switching clock sources
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 25, 2025): Initial version
 * 
 * @copyright Copyright (c) 2025
 * Licensed under [Your License]
 */

#ifndef CLOCK_H
#define CLOCK_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "clock_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup Clock_Definitions Clock Definitions
 * @{
 */

/** @brief Clock source enumeration */
typedef enum {
    CLOCK_SRC_SIRC = 0U,    /**< Slow Internal Reference Clock (8MHz) */
    CLOCK_SRC_FIRC = 1U,    /**< Fast Internal Reference Clock (48MHz) */
    CLOCK_SRC_SOSC = 2U,    /**< System Oscillator Clock (external) */
    CLOCK_SRC_SPLL = 3U     /**< System Phase-Locked Loop */
} clock_source_t;

/** @brief Clock divider enumeration */
typedef enum {
    CLOCK_DIV_BY_1 = 0U,    /**< Divide by 1 */
    CLOCK_DIV_BY_2 = 1U,    /**< Divide by 2 */
    CLOCK_DIV_BY_3 = 2U,    /**< Divide by 3 */
    CLOCK_DIV_BY_4 = 3U,    /**< Divide by 4 */
    CLOCK_DIV_BY_5 = 4U,    /**< Divide by 5 */
    CLOCK_DIV_BY_6 = 5U,    /**< Divide by 6 */
    CLOCK_DIV_BY_7 = 6U,    /**< Divide by 7 */
    CLOCK_DIV_BY_8 = 7U     /**< Divide by 8 */
} clock_divider_t;

/** @brief System clock configuration structure */
typedef struct {
    clock_source_t  clockSource;    /**< Clock source selection */
    clock_divider_t divCore;        /**< Core clock divider */
    clock_divider_t divBus;         /**< Bus clock divider */
    clock_divider_t divSlow;        /**< Slow clock divider */
} clock_config_t;

/** @brief Clock frequencies structure */
typedef struct {
    uint32_t coreClockFreq;         /**< Core clock frequency in Hz */
    uint32_t busClockFreq;          /**< Bus clock frequency in Hz */
    uint32_t slowClockFreq;         /**< Slow clock frequency in Hz */
    uint32_t systemClockFreq;       /**< System clock frequency in Hz */
} clock_frequencies_t;

/** @} */ /* End of Clock_Definitions */

/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @defgroup Clock_Functions Clock Driver Functions
 * @{
 */

/**
 * @brief Initialize system clock with specified configuration
 * @param[in] config Pointer to clock configuration structure
 * @return true if successful, false if failed
 * 
 * @note This function configures the system clock source and dividers
 * @warning Ensure the selected clock source is valid before switching
 * 
 * Example usage:
 * @code
 * clock_config_t clockConfig = {
 *     .clockSource = CLOCK_SRC_FIRC,
 *     .divCore = CLOCK_DIV_BY_1,
 *     .divBus = CLOCK_DIV_BY_2,
 *     .divSlow = CLOCK_DIV_BY_4
 * };
 * CLOCK_Init(&clockConfig);
 * @endcode
 */
bool CLOCK_Init(const clock_config_t *config);

/**
 * @brief Enable SIRC (Slow Internal Reference Clock)
 * @return true if SIRC is enabled and valid, false otherwise
 * 
 * @note SIRC is typically 8MHz
 * @warning Wait for SIRC to be valid before using it
 */
bool CLOCK_EnableSIRC(void);

/**
 * @brief Enable FIRC (Fast Internal Reference Clock)
 * @return true if FIRC is enabled and valid, false otherwise
 * 
 * @note FIRC is typically 48MHz
 * @warning Wait for FIRC to be valid before using it
 */
bool CLOCK_EnableFIRC(void);

/**
 * @brief Enable SOSC (System Oscillator Clock)
 * @param[in] extFreq External oscillator frequency in Hz
 * @return true if SOSC is enabled and valid, false otherwise
 * 
 * @note Requires external crystal/oscillator
 * @warning Ensure external clock source is connected
 */
bool CLOCK_EnableSOSC(uint32_t extFreq);

/**
 * @brief Enable SPLL (System Phase-Locked Loop)
 * @param[in] inputFreq PLL input frequency in Hz
 * @param[in] outputFreq Desired PLL output frequency in Hz
 * @return true if SPLL is enabled and locked, false otherwise
 * 
 * @note PLL requires SOSC as input source
 * @warning Ensure SOSC is enabled before enabling SPLL
 */
bool CLOCK_EnableSPLL(uint32_t inputFreq, uint32_t outputFreq);

/**
 * @brief Disable specified clock source
 * @param[in] source Clock source to disable
 * 
 * @note Cannot disable the currently selected system clock source
 * @warning Check system clock source before disabling
 */
void CLOCK_DisableClockSource(clock_source_t source);

/**
 * @brief Set system clock source
 * @param[in] source Clock source to use for system clock
 * @return true if successful, false if failed
 * 
 * @note Ensure the clock source is enabled and valid first
 * @warning Switching clock sources may cause temporary system interruption
 */
bool CLOCK_SetSystemClockSource(clock_source_t source);

/**
 * @brief Get current system clock source
 * @return Current system clock source
 */
clock_source_t CLOCK_GetSystemClockSource(void);

/**
 * @brief Get system clock frequencies
 * @param[out] frequencies Pointer to structure to store frequencies
 * 
 * @note Calculates current core, bus, and slow clock frequencies
 */
void CLOCK_GetFrequencies(clock_frequencies_t *frequencies);

/**
 * @brief Get core clock frequency
 * @return Core clock frequency in Hz
 */
uint32_t CLOCK_GetCoreClockFreq(void);

/**
 * @brief Get bus clock frequency
 * @return Bus clock frequency in Hz
 */
uint32_t CLOCK_GetBusClockFreq(void);

/**
 * @brief Get slow clock frequency
 * @return Slow clock frequency in Hz
 */
uint32_t CLOCK_GetSlowClockFreq(void);

/**
 * @brief Set clock dividers for system clocks
 * @param[in] divCore Core clock divider
 * @param[in] divBus Bus clock divider
 * @param[in] divSlow Slow clock divider
 * @return true if successful, false if failed
 * 
 * @note Dividers affect the system clock distribution
 * @warning Improper divider settings may cause system instability
 */
bool CLOCK_SetDividers(clock_divider_t divCore, clock_divider_t divBus, clock_divider_t divSlow);

/**
 * @brief Configure system clock to default settings
 * @return true if successful, false if failed
 * 
 * @note Uses FIRC at 48MHz with standard dividers
 * @note Core = 48MHz, Bus = 24MHz, Slow = 12MHz
 */
bool CLOCK_SetDefaultConfig(void);

/** @} */ /* End of Clock_Functions */

/*******************************************************************************
 * Inline Functions
 ******************************************************************************/

/**
 * @brief Check if SIRC is valid
 * @return true if SIRC is valid, false otherwise
 */
static inline bool CLOCK_IsSIRCValid(void)
{
    return SCG_IS_SIRC_VALID();
}

/**
 * @brief Check if FIRC is valid
 * @return true if FIRC is valid, false otherwise
 */
static inline bool CLOCK_IsFIRCValid(void)
{
    return SCG_IS_FIRC_VALID();
}

/**
 * @brief Check if SOSC is valid
 * @return true if SOSC is valid, false otherwise
 */
static inline bool CLOCK_IsSOSCValid(void)
{
    return SCG_IS_SOSC_VALID();
}

/**
 * @brief Check if SPLL is valid
 * @return true if SPLL is valid, false otherwise
 */
static inline bool CLOCK_IsSPLLValid(void)
{
    return SCG_IS_SPLL_VALID();
}

#endif /* CLOCK_H */
