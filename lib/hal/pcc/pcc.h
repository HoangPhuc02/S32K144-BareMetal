/**
 * @file    pcc.h
 * @brief   PCC (Peripheral Clock Control) Driver Header for S32K144
 * @details This file contains the PCC driver interface declarations for
 *          controlling peripheral clocks on S32K144 microcontroller.
 * 
 * @author  PhucPH32
 * @date    18/12/2025
 * @version 1.0
 * 
 * @note    This driver provides peripheral clock enable/disable and configuration
 * @warning Peripheral must be disabled before changing clock source
 * 
 * @par Reference:
 * - S32K1XXRM Rev. 12.1, Chapter 29: Peripheral Clock Control (PCC)
 * 
 */

#ifndef PCC_H
#define PCC_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "pcc_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief Peripheral clock source selection
 */
typedef enum {
    PCC_CLK_SRC_OFF         = 0U,   /**< Clock off (disabled) */
    PCC_CLK_SRC_SOSC_DIV2   = 1U,   /**< SOSC divided by 2 */
    PCC_CLK_SRC_SIRC_DIV2   = 2U,   /**< SIRC divided by 2 */
    PCC_CLK_SRC_FIRC_DIV2   = 3U,   /**< FIRC divided by 2 */
    PCC_CLK_SRC_SPLL_DIV2   = 6U    /**< SPLL divided by 2 */
} pcc_clock_source_t;

/**
 * @brief Peripheral clock configuration structure
 */
typedef struct {
    pcc_clock_source_t clockSource; /**< Clock source selection */
    bool enableClock;               /**< Enable peripheral clock */
    uint8_t divider;                /**< Clock divider (0-7, actual divider = value+1) */
    bool fractionalDivider;         /**< Enable fractional divider */
} pcc_config_t;




/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @brief Configure peripheral clock
 * @param[in] peripheral Peripheral index (0-115)
 * @param[in] config Pointer to clock configuration structure
 * @return true if successful, false if failed
 * 
 * @note Peripheral must be disabled before changing clock source
 * @warning Some peripherals don't support all clock sources
 * 
 * Example usage:
 * @code
 * pcc_config_t adcConfig = {
 *     .clockSource = PCC_CLK_SRC_FIRC_DIV2,
 *     .enableClock = true,
 *     .divider = 0,
 *     .fractionalDivider = false
 * };
 * PCC_SetPeripheralClockConfig(PCC_ADC0_INDEX, &adcConfig);
 * @endcode
 */
bool PCC_SetPeripheralClockConfig(uint8_t peripheral, const pcc_config_t *config);

/**
 * @brief Get peripheral clock configuration
 * @param[in] peripheral Peripheral index (0-115)
 * @param[out] config Pointer to store configuration
 * @return true if successful, false if failed
 */
bool PCC_GetPeripheralClockConfig(uint8_t peripheral, pcc_config_t *config);

/**
 * @brief Enable peripheral clock
 * @param[in] peripheral Peripheral index (0-115)
 * @return true if successful, false if failed
 * 
 * @note This is a convenience function for quick clock enable
 */
bool PCC_EnablePeripheralClock(uint8_t peripheral);

/**
 * @brief Disable peripheral clock
 * @param[in] peripheral Peripheral index (0-115)
 * @return true if successful, false if failed
 */
bool PCC_DisablePeripheralClock(uint8_t peripheral);

/**
 * @brief Set peripheral clock source
 * @param[in] peripheral Peripheral index (0-115)
 * @param[in] source Clock source selection
 * @return true if successful, false if failed
 * 
 * @note Peripheral clock must be disabled before changing source
 */
bool PCC_SetPeripheralClockSource(uint8_t peripheral, pcc_clock_source_t source);

/**
 * @brief Get peripheral clock source
 * @param[in] peripheral Peripheral index (0-115)
 * @return Current clock source, or PCC_CLK_SRC_OFF if error
 */
pcc_clock_source_t PCC_GetPeripheralClockSource(uint8_t peripheral);

/**
 * @brief Check if peripheral clock is enabled
 * @param[in] peripheral Peripheral index (0-115)
 * @return true if enabled, false if disabled
 */
bool PCC_IsPeripheralClockEnabled(uint8_t peripheral);

/**
 * @brief Check if peripheral is present
 * @param[in] peripheral Peripheral index (0-115)
 * @return true if present, false if not available
 * 
 * @note Some peripherals may not be available on all S32K144 variants
 */
bool PCC_IsPeripheralPresent(uint8_t peripheral);

/**
 * @brief Set peripheral clock divider
 * @param[in] peripheral Peripheral index (0-115)
 * @param[in] divider Divider value (0-7, actual divider = value+1)
 * @param[in] fractional Enable fractional divider
 * @return true if successful, false if failed
 * 
 * @note Not all peripherals support clock division
 */
bool PCC_SetPeripheralClockDivider(uint8_t peripheral, uint8_t divider, bool fractional);

/**
 * @brief Initialize PCC module
 * @return true if successful, false if failed
 * 
 * @note This function initializes the PCC module for safe operation
 */
bool PCC_Init(void);

/**
 * @brief Get peripheral clock frequency
 * @param[in] peripheral Peripheral index (0-115)
 * @return Clock frequency in Hz, or 0 if error
 * 
 * @note This function calculates frequency based on source and divider
 * @warning Requires SCG module to be properly initialized
 */
uint32_t PCC_GetPeripheralClockFreq(uint8_t peripheral);

#endif /* PCC_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
