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
#include "scg.h"
#include "scg_reg.h"
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

bool PCC_EnableCANClock(uint8_t instance, can_clk_src_t clockSource)
{
    uint8_t pccIndex;
    pcc_config_t config;
    
    /* Determine PCC index for CAN instance */
    if (instance == 0U) {
        pccIndex = PCC_FlexCAN0_INDEX;
    } else if (instance == 1U) {
        pccIndex = PCC_FlexCAN1_INDEX;
    } else if (instance == 2U) {
        pccIndex = PCC_FlexCAN2_INDEX;
    } else {
        return false;  /* Invalid instance */
    }
    
    /* Configure peripheral clock */
    config.clockSource = (clockSource == CAN_CLK_SRC_BUSCLOCK) ? PCC_CLK_SRC_FIRC_DIV2 : PCC_CLK_SRC_SOSC_DIV2;
    config.enableClock = true;
    config.divider = 0U;  /* No division */
    config.fractionalDivider = false;
    
    return PCC_SetPeripheralClockConfig(pccIndex, &config);
}

bool PCC_DisableCANClock(uint8_t instance)
{
    uint8_t pccIndex;
    
    /* Determine PCC index for CAN instance */
    if (instance == 0U) {
        pccIndex = PCC_FlexCAN0_INDEX;
    } else if (instance == 1U) {
        pccIndex = PCC_FlexCAN1_INDEX;
    } else if (instance == 2U) {
        pccIndex = PCC_FlexCAN2_INDEX;
    } else {
        return false;  /* Invalid instance */
    }
    
    return PCC_DisablePeripheralClock(pccIndex);
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
    scg_clock_frequencies_t freqs;
    
    /* Get actual frequencies from SCG module */
    if (!SCG_GetClockFrequencies(&freqs)) {
        return 0U;  /* SCG not properly initialized */
    }
    
    /* Get source frequency based on PCC clock source selection */
    /* PCC uses DIV2 outputs from SCG */
    switch (source) {
        case PCC_CLK_SRC_SOSC_DIV2:
            /* SOSCDIV2 = SOSC / DIV2 (configured in SCG) */
            sourceFreq = PCC_GetSoscDiv2Freq();
            break;
        case PCC_CLK_SRC_SIRC_DIV2:
            /* SIRCDIV2 = SIRC / DIV2 (configured in SCG) */
            sourceFreq = PCC_GetSircDiv2Freq();
            break;
        case PCC_CLK_SRC_FIRC_DIV2:
            /* FIRCDIV2 = FIRC / DIV2 (configured in SCG) */
            sourceFreq = PCC_GetFircDiv2Freq();
            break;
        case PCC_CLK_SRC_SPLL_DIV2:
            /* SPLLDIV2 = SPLL / DIV2 (configured in SCG) */
            sourceFreq = PCC_GetSpllDiv2Freq();
            break;
        case PCC_CLK_SRC_OFF:
        default:
            sourceFreq = 0U;
            break;
    }
    
    /* Apply PCC divider if configured */
    uint32_t regValue = PCC->PCCn[peripheral];
    uint8_t divider = (uint8_t)((regValue & PCC_PCCn_PCD_MASK) >> PCC_PCCn_PCD_SHIFT);
    bool fractional = ((regValue & PCC_PCCn_FRAC_MASK) != 0U);
    
    if (divider > 0U) {
        if (fractional) {
            /* Fractional divider: divide by (divider + 1 + 0.5) */
            /* Multiply by 2 first to avoid fraction */
            sourceFreq = (sourceFreq * 2U) / ((divider + 1U) * 2U + 1U);
        } else {
            /* Integer divider: divide by (divider + 1) */
            sourceFreq = sourceFreq / (divider + 1U);
        }
    }
    
    return sourceFreq;
}

/*******************************************************************************
 * Clock Frequency Query Functions (using SCG)
 ******************************************************************************/

uint32_t PCC_GetSoscDiv2Freq(void)
{
    scg_clock_frequencies_t freqs;
    
    if (!SCG_GetClockFrequencies(&freqs)) {
        return 0U;
    }
    
    /* Read SOSCDIV2 divider from SCG register */
    uint32_t div2 = (SCG->SOSCDIV & SCG_SOSCDIV_SOSCDIV2_MASK) >> SCG_SOSCDIV_SOSCDIV2_SHIFT;
    
    if (div2 == 0U) {
        return 0U;  /* DIV2 output disabled */
    }
    
    /* DIV2 value: 0=disabled, 1=/1, 2=/2, 3=/4, 4=/8, 5=/16, 6=/32, 7=/64 */
    return freqs.soscClk >> (div2 - 1U);
}

uint32_t PCC_GetSircDiv2Freq(void)
{
    scg_clock_frequencies_t freqs;
    
    if (!SCG_GetClockFrequencies(&freqs)) {
        return 0U;
    }
    
    /* Read SIRCDIV2 divider from SCG register */
    uint32_t div2 = (SCG->SIRCDIV & SCG_SIRCDIV_SIRCDIV2_MASK) >> SCG_SIRCDIV_SIRCDIV2_SHIFT;
    
    if (div2 == 0U) {
        return 0U;  /* DIV2 output disabled */
    }
    
    return freqs.sircClk >> (div2 - 1U);
}

uint32_t PCC_GetFircDiv2Freq(void)
{
    scg_clock_frequencies_t freqs;
    
    if (!SCG_GetClockFrequencies(&freqs)) {
        return 0U;
    }
    
    /* Read FIRCDIV2 divider from SCG register */
    uint32_t div2 = (SCG->FIRCDIV & SCG_FIRCDIV_FIRCDIV2_MASK) >> SCG_FIRCDIV_FIRCDIV2_SHIFT;
    
    if (div2 == 0U) {
        return 0U;  /* DIV2 output disabled */
    }
    
    return freqs.fircClk >> (div2 - 1U);
}

uint32_t PCC_GetSpllDiv2Freq(void)
{
    scg_clock_frequencies_t freqs;
    
    if (!SCG_GetClockFrequencies(&freqs)) {
        return 0U;
    }
    
    /* Read SPLLDIV2 divider from SCG register */
    uint32_t div2 = (SCG->SPLLDIV & SCG_SPLLDIV_SPLLDIV2_MASK) >> SCG_SPLLDIV_SPLLDIV2_SHIFT;
    
    if (div2 == 0U) {
        return 0U;  /* DIV2 output disabled */
    }
    
    return freqs.spllClk >> (div2 - 1U);
}

/*******************************************************************************
 * Communication Peripheral Clock Helpers
 ******************************************************************************/

uint32_t PCC_GetLpuartClockFreq(uint8_t instance)
{
    uint8_t pccIndex;
    
    switch (instance) {
        case 0U:
            pccIndex = PCC_LPUART0_INDEX;
            break;
        case 1U:
            pccIndex = PCC_LPUART1_INDEX;
            break;
        case 2U:
            pccIndex = PCC_LPUART2_INDEX;
            break;
        default:
            return 0U;
    }
    
    return PCC_GetPeripheralClockFreq(pccIndex);
}

uint32_t PCC_GetFlexCanClockFreq(uint8_t instance)
{
    uint8_t pccIndex;
    
    switch (instance) {
        case 0U:
            pccIndex = PCC_FlexCAN0_INDEX;
            break;
        case 1U:
            pccIndex = PCC_FlexCAN1_INDEX;
            break;
        case 2U:
            pccIndex = PCC_FlexCAN2_INDEX;
            break;
        default:
            return 0U;
    }
    
    return PCC_GetPeripheralClockFreq(pccIndex);
}

uint32_t PCC_GetLpspiClockFreq(uint8_t instance)
{
    uint8_t pccIndex;
    
    switch (instance) {
        case 0U:
            pccIndex = PCC_LPSPI0_INDEX;
            break;
        case 1U:
            pccIndex = PCC_LPSPI1_INDEX;
            break;
        case 2U:
            pccIndex = PCC_LPSPI2_INDEX;
            break;
        default:
            return 0U;
    }
    
    return PCC_GetPeripheralClockFreq(pccIndex);
}

uint32_t PCC_GetLpi2cClockFreq(uint8_t instance)
{
    if (instance != 0U) {
        return 0U;  /* S32K144 only has LPI2C0 */
    }
    
    return PCC_GetPeripheralClockFreq(PCC_LPI2C0_INDEX);
}

uint32_t PCC_GetFtmClockFreq(uint8_t instance)
{
    uint8_t pccIndex;
    
    switch (instance) {
        case 0U:
            pccIndex = PCC_FTM0_INDEX;
            break;
        case 1U:
            pccIndex = PCC_FTM1_INDEX;
            break;
        case 2U:
            pccIndex = PCC_FTM2_INDEX;
            break;
        case 3U:
            pccIndex = PCC_FTM3_INDEX;
            break;
        default:
            return 0U;
    }
    
    return PCC_GetPeripheralClockFreq(pccIndex);
}

uint32_t PCC_GetAdcClockFreq(uint8_t instance)
{
    uint8_t pccIndex;
    
    switch (instance) {
        case 0U:
            pccIndex = PCC_ADC0_INDEX;
            break;
        case 1U:
            pccIndex = PCC_ADC1_INDEX;
            break;
        default:
            return 0U;
    }
    
    return PCC_GetPeripheralClockFreq(pccIndex);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
