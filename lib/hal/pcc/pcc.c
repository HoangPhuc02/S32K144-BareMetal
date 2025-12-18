/**
 * @file    pcc.c
 * @brief   PCC (Peripheral Clock Control) Driver Implementation for S32K144
 * @details This file contains the implementation of PCC driver functions
 *          for controlling peripheral clocks on S32K144 microcontroller.
 * 
 * @author  PhucPH32
 * @date    18/12/2025
 * @version 1.0
 * 
 * @note    This implementation provides peripheral clock enable/disable and configuration
 * @warning Peripheral must be disabled before changing clock source
 * 
 * @par Reference:
 * - S32K1XXRM Rev. 12.1, Chapter 29: Peripheral Clock Control (PCC)
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "pcc.h"
#include "pcc_reg.h"
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Maximum peripheral index */
#define PCC_MAX_PERIPHERAL_INDEX    (PCC_PCCn_COUNT - 1U)

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static bool PCC_IsValidPeripheralIndex(uint8_t peripheral);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Validate peripheral index
 */
static bool PCC_IsValidPeripheralIndex(uint8_t peripheral)
{
    return (peripheral <= PCC_MAX_PERIPHERAL_INDEX);
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

bool PCC_SetPeripheralClockConfig(uint8_t peripheral, const pcc_config_t *config)
{
    if ((config == NULL) || (!PCC_IsValidPeripheralIndex(peripheral))) {
        return false;
    }
    
    /* Build register value */
    uint32_t regValue = 0U;
    
    /* Set clock source */
    regValue |= PCC_PCCn_PCS(config->clockSource);
    
    /* Set divider */
    if (config->divider <= 7U) {
        regValue |= PCC_PCCn_PCD(config->divider);
    }
    
    /* Set fractional divider */
    if (config->fractionalDivider) {
        regValue |= PCC_PCCn_FRAC_MASK;
    }
    
    /* Enable clock gate if requested */
    if (config->enableClock) {
        regValue |= PCC_PCCn_CGC_MASK;
    }
    
    /* Write to PCC register */
    PCC->PCCn[peripheral] = regValue;
    
    return true;
}

bool PCC_GetPeripheralClockConfig(uint8_t peripheral, pcc_config_t *config)
{
    if ((config == NULL) || (!PCC_IsValidPeripheralIndex(peripheral))) {
        return false;
    }
    
    /* Read PCC register */
    uint32_t regValue = PCC->PCCn[peripheral];
    
    /* Extract configuration */
    config->clockSource = (pcc_clock_source_t)((regValue & PCC_PCCn_PCS_MASK) >> PCC_PCCn_PCS_SHIFT);
    config->divider = (uint8_t)((regValue & PCC_PCCn_PCD_MASK) >> PCC_PCCn_PCD_SHIFT);
    config->fractionalDivider = ((regValue & PCC_PCCn_FRAC_MASK) != 0U);
    config->enableClock = ((regValue & PCC_PCCn_CGC_MASK) != 0U);
    
    return true;
}

bool PCC_EnablePeripheralClock(uint8_t peripheral)
{
    if (!PCC_IsValidPeripheralIndex(peripheral)) {
        return false;
    }
    
    /* Set clock gate control bit */
    PCC->PCCn[peripheral] |= PCC_PCCn_CGC_MASK;
    
    return true;
}

bool PCC_DisablePeripheralClock(uint8_t peripheral)
{
    if (!PCC_IsValidPeripheralIndex(peripheral)) {
        return false;
    }
    
    /* Clear clock gate control bit */
    PCC->PCCn[peripheral] &= ~PCC_PCCn_CGC_MASK;
    
    return true;
}

bool PCC_SetPeripheralClockSource(uint8_t peripheral, pcc_clock_source_t source)
{
    if (!PCC_IsValidPeripheralIndex(peripheral)) {
        return false;
    }
    
    /* Check if peripheral clock is disabled (required before changing source) */
    if ((PCC->PCCn[peripheral] & PCC_PCCn_CGC_MASK) != 0U) {
        return false;  /* Clock must be disabled first */
    }
    
    /* Clear and set clock source */
    uint32_t regValue = PCC->PCCn[peripheral];
    regValue &= ~PCC_PCCn_PCS_MASK;
    regValue |= PCC_PCCn_PCS(source);
    PCC->PCCn[peripheral] = regValue;
    
    return true;
}

pcc_clock_source_t PCC_GetPeripheralClockSource(uint8_t peripheral)
{
    if (!PCC_IsValidPeripheralIndex(peripheral)) {
        return PCC_CLK_SRC_OFF;
    }
    
    uint32_t regValue = PCC->PCCn[peripheral];
    return (pcc_clock_source_t)((regValue & PCC_PCCn_PCS_MASK) >> PCC_PCCn_PCS_SHIFT);
}

bool PCC_IsPeripheralClockEnabled(uint8_t peripheral)
{
    if (!PCC_IsValidPeripheralIndex(peripheral)) {
        return false;
    }
    
    return ((PCC->PCCn[peripheral] & PCC_PCCn_CGC_MASK) != 0U);
}

bool PCC_IsPeripheralPresent(uint8_t peripheral)
{
    if (!PCC_IsValidPeripheralIndex(peripheral)) {
        return false;
    }
    
    /* Check PR (Present) bit */
    return ((PCC->PCCn[peripheral] & PCC_PCCn_PR_MASK) != 0U);
}

bool PCC_SetPeripheralClockDivider(uint8_t peripheral, uint8_t divider, bool fractional)
{
    if (!PCC_IsValidPeripheralIndex(peripheral)) {
        return false;
    }
    
    if (divider > 7U) {
        return false;
    }
    
    /* Read current value */
    uint32_t regValue = PCC->PCCn[peripheral];
    
    /* Clear divider bits */
    regValue &= ~(PCC_PCCn_PCD_MASK | PCC_PCCn_FRAC_MASK);
    
    /* Set new divider */
    regValue |= PCC_PCCn_PCD(divider);
    
    if (fractional) {
        regValue |= PCC_PCCn_FRAC_MASK;
    }
    
    /* Write back */
    PCC->PCCn[peripheral] = regValue;
    
    return true;
}

bool PCC_Init(void)
{
    /* PCC doesn't require explicit initialization */
    /* All registers are accessible after power-on reset */
    
    /* Optional: Disable all peripheral clocks for power saving */
    /* This is typically done on a per-application basis */
    
    return true;
}

uint32_t PCC_GetPeripheralClockFreq(uint8_t peripheral)
{
    if (!PCC_IsValidPeripheralIndex(peripheral)) {
        return 0U;
    }
    
    /* Check if peripheral clock is enabled */
    if (!PCC_IsPeripheralClockEnabled(peripheral)) {
        return 0U;
    }
    
    /* Get clock source */
    pcc_clock_source_t source = PCC_GetPeripheralClockSource(peripheral);
    
    uint32_t sourceFreq = 0U;
    
    /* Get source frequency from SCG */
    /* Note: This requires SCG driver to be properly initialized */
    /* For now, we'll return typical frequencies */
    /* In a real implementation, you would call SCG functions */
    
    switch (source) {
        case PCC_CLK_SRC_SOSC_DIV2:
            sourceFreq = 4000000U;  /* Typical 8MHz SOSC / 2 */
            break;
        case PCC_CLK_SRC_SIRC_DIV2:
            sourceFreq = 4000000U;  /* 8MHz SIRC / 2 */
            break;
        case PCC_CLK_SRC_FIRC_DIV2:
            sourceFreq = 24000000U; /* 48MHz FIRC / 2 */
            break;
        case PCC_CLK_SRC_SPLL_DIV2:
            sourceFreq = 80000000U; /* Typical 160MHz SPLL / 2 */
            break;
        case PCC_CLK_SRC_OFF:
        default:
            sourceFreq = 0U;
            break;
    }
    
    /* Apply divider if configured */
    uint32_t regValue = PCC->PCCn[peripheral];
    uint8_t divider = (uint8_t)((regValue & PCC_PCCn_PCD_MASK) >> PCC_PCCn_PCD_SHIFT);
    bool fractional = ((regValue & PCC_PCCn_FRAC_MASK) != 0U);
    
    if (divider > 0U) {
        if (fractional) {
            /* Fractional divider: divide by (divider + 1 + 0.5) */
            /* For simplicity, we'll approximate */
            sourceFreq = (sourceFreq * 2U) / ((divider + 1U) * 2U + 1U);
        } else {
            /* Integer divider: divide by (divider + 1) */
            sourceFreq = sourceFreq / (divider + 1U);
        }
    }
    
    return sourceFreq;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
