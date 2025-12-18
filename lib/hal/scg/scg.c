/**
 * @file    scg.c
 * @brief   SCG (System Clock Generator) Driver Implementation for S32K144
 * @details This file contains the implementation of SCG driver functions
 *          for S32K144 microcontroller clock management.
 * 
 * @author  PhucPH32
 * @date    18/12/2025
 * @version 1.0
 * 
 * @note    This implementation provides system clock configuration and management
 * @warning Always ensure clock sources are valid before switching
 * 
 * @par Reference:
 * - S32K1XXRM Rev. 12.1, Chapter 28: System Clock Generator (SCG)
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "scg.h"
#include "scg_reg.h"
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Default clock frequencies */
#define SIRC_LOW_RANGE_FREQ     (2000000U)      /* 2 MHz */
#define SIRC_HIGH_RANGE_FREQ    (8000000U)      /* 8 MHz */
#define FIRC_48M_FREQ           (48000000U)     /* 48 MHz */
#define FIRC_52M_FREQ           (52000000U)     /* 52 MHz */
#define FIRC_56M_FREQ           (56000000U)     /* 56 MHz */
#define FIRC_60M_FREQ           (60000000U)     /* 60 MHz */

/* Timeout values */
#define SCG_TIMEOUT             (10000U)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief SOSC frequency (set by SCG_InitSOSC) */
static uint32_t s_soscFreq = 0U;

/** @brief SPLL output frequency (set by SCG_InitSPLL) */
static uint32_t s_spllFreq = 0U;

/** @brief SIRC frequency */
static uint32_t s_sircFreq = SIRC_HIGH_RANGE_FREQ;

/** @brief FIRC frequency */
static uint32_t s_fircFreq = FIRC_48M_FREQ;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static uint32_t SCG_GetSourceFrequency(scg_clock_source_t source);
static uint32_t SCG_ConvertDividerToValue(scg_clock_divider_t divider);
static bool SCG_WaitForClockValid(scg_clock_source_t source);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Get clock source base frequency
 */
static uint32_t SCG_GetSourceFrequency(scg_clock_source_t source)
{
    uint32_t freq = 0U;
    
    switch (source) {
        case SCG_CLOCK_SRC_SIRC:
            freq = s_sircFreq;
            break;
        case SCG_CLOCK_SRC_FIRC:
            freq = s_fircFreq;
            break;
        case SCG_CLOCK_SRC_SOSC:
            freq = s_soscFreq;
            break;
        case SCG_CLOCK_SRC_SPLL:
            freq = s_spllFreq;
            break;
        default:
            freq = 0U;
            break;
    }
    
    return freq;
}

/**
 * @brief Convert divider enum to actual divider value
 */
static uint32_t SCG_ConvertDividerToValue(scg_clock_divider_t divider)
{
    return (uint32_t)divider + 1U;
}

/**
 * @brief Wait for clock source to become valid
 */
static bool SCG_WaitForClockValid(scg_clock_source_t source)
{
    uint32_t timeout = SCG_TIMEOUT;
    bool isValid = false;
    
    while ((timeout > 0U) && (!isValid)) {
        switch (source) {
            case SCG_CLOCK_SRC_SIRC:
                isValid = (SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK) != 0U;
                break;
            case SCG_CLOCK_SRC_FIRC:
                isValid = (SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK) != 0U;
                break;
            case SCG_CLOCK_SRC_SOSC:
                isValid = (SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK) != 0U;
                break;
            case SCG_CLOCK_SRC_SPLL:
                isValid = (SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK) != 0U;
                break;
            default:
                return false;
        }
        timeout--;
    }
    
    return isValid;
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

bool SCG_InitSIRC(const scg_sirc_config_t *config)
{
    if (config == NULL) {
        return false;
    }
    
    /* Disable SIRC first */
    SCG->SIRCCSR &= ~SCG_SIRCCSR_SIRCEN_MASK;
    
    /* Configure SIRC range */
    SCG->SIRCCFG = SCG_SIRCCFG_RANGE(config->range);
    
    /* Update frequency variable */
    s_sircFreq = (config->range == SCG_SIRC_RANGE_HIGH) ? 
                  SIRC_HIGH_RANGE_FREQ : SIRC_LOW_RANGE_FREQ;
    
    /* Configure dividers */
    SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(config->div1) |
                   SCG_SIRCDIV_SIRCDIV2(config->div2);
    
    /* Configure control/status register */
    uint32_t csr = SCG_SIRCCSR_SIRCEN_MASK;
    if (config->enableInStop) {
        csr |= SCG_SIRCCSR_SIRCSTEN_MASK;
    }
    if (config->enableInLowPower) {
        csr |= SCG_SIRCCSR_SIRCLPEN_MASK;
    }
    
    SCG->SIRCCSR = csr;
    
    /* Wait for SIRC to be valid */
    return SCG_WaitForClockValid(SCG_CLOCK_SRC_SIRC);
}

bool SCG_InitFIRC(const scg_firc_config_t *config)
{
    if (config == NULL) {
        return false;
    }
    
    /* Disable FIRC first (if not system clock) */
    if ((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT != SCG_SYSTEM_CLOCK_SRC_FIRC) {
        SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;
    }
    
    /* Configure FIRC range */
    SCG->FIRCCFG = SCG_FIRCCFG_RANGE(config->range);
    
    /* Update frequency variable */
    switch (config->range) {
        case SCG_FIRC_RANGE_48M:
            s_fircFreq = FIRC_48M_FREQ;
            break;
        case SCG_FIRC_RANGE_52M:
            s_fircFreq = FIRC_52M_FREQ;
            break;
        case SCG_FIRC_RANGE_56M:
            s_fircFreq = FIRC_56M_FREQ;
            break;
        case SCG_FIRC_RANGE_60M:
            s_fircFreq = FIRC_60M_FREQ;
            break;
        default:
            s_fircFreq = FIRC_48M_FREQ;
            break;
    }
    
    /* Configure dividers */
    SCG->FIRCDIV = SCG_FIRCDIV_FIRCDIV1(config->div1) |
                   SCG_FIRCDIV_FIRCDIV2(config->div2);
    
    /* Configure control/status register */
    uint32_t csr = SCG_FIRCCSR_FIRCEN_MASK;
    if (!config->regulator) {
        csr |= SCG_FIRCCSR_FIRCREGOFF_MASK;
    }
    
    SCG->FIRCCSR = csr;
    
    /* Wait for FIRC to be valid */
    return SCG_WaitForClockValid(SCG_CLOCK_SRC_FIRC);
}

bool SCG_InitSOSC(const scg_sosc_config_t *config)
{
    if (config == NULL) {
        return false;
    }
    
    /* Disable SOSC first */
    SCG->SOSCCSR &= ~SCG_SOSCCSR_SOSCEN_MASK;
    
    /* Store SOSC frequency */
    s_soscFreq = config->freq;
    
    /* Configure SOSC */
    uint32_t cfg = SCG_SOSCCFG_RANGE(config->range);
    if (config->useExternalRef) {
        cfg |= SCG_SOSCCFG_EREFS_MASK;
    }
    if (config->highGainOsc) {
        cfg |= SCG_SOSCCFG_HGO_MASK;
    }
    
    SCG->SOSCCFG = cfg;
    
    /* Configure control/status register */
    uint32_t csr = SCG_SOSCCSR_SOSCEN_MASK;
    if (config->enableInStop) {
        csr |= SCG_SOSCCSR_SOSCSTEN_MASK;
    }
    if (config->enableInLowPower) {
        csr |= SCG_SOSCCSR_SOSCLPEN_MASK;
    }
    
    SCG->SOSCCSR = csr;
    
    /* Wait for SOSC to be valid */
    return SCG_WaitForClockValid(SCG_CLOCK_SRC_SOSC);
}

bool SCG_InitSPLL(const scg_spll_config_t *config)
{
    if (config == NULL) {
        return false;
    }
    
    /* Check if SOSC is valid (SPLL requires SOSC as input) */
    if (!SCG_IsClockValid(SCG_CLOCK_SRC_SOSC)) {
        return false;
    }
    
    /* Disable SPLL first */
    SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;
    
    /* Configure SPLL */
    SCG->SPLLCFG = SCG_SPLLCFG_PREDIV(config->prediv) |
                   SCG_SPLLCFG_MULT(config->mult);
    
    /* Calculate SPLL frequency: (SOSC_freq / (prediv+1)) * (mult+16) */
    s_spllFreq = (s_soscFreq / (config->prediv + 1U)) * (config->mult + 16U);
    
    /* Configure dividers */
    SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(config->div1) |
                   SCG_SPLLDIV_SPLLDIV2(config->div2);
    
    /* Enable SPLL */
    SCG->SPLLCSR = SCG_SPLLCSR_SPLLEN_MASK;
    
    /* Wait for SPLL to lock */
    return SCG_WaitForClockValid(SCG_CLOCK_SRC_SPLL);
}

bool SCG_SetSystemClockConfig(scg_system_clock_mode_t mode, const scg_system_clock_config_t *config)
{
    if (config == NULL) {
        return false;
    }
    
    /* Verify clock source is valid */
    if (!SCG_IsClockValid(config->source)) {
        return false;
    }
    
    /* Map internal clock source enum to SCG register values */
    uint32_t scsValue;
    switch (config->source) {
        case SCG_CLOCK_SRC_SIRC:
            scsValue = SCG_SYSTEM_CLOCK_SRC_SIRC;
            break;
        case SCG_CLOCK_SRC_FIRC:
            scsValue = SCG_SYSTEM_CLOCK_SRC_FIRC;
            break;
        case SCG_CLOCK_SRC_SOSC:
            scsValue = SCG_SYSTEM_CLOCK_SRC_SOSC;
            break;
        case SCG_CLOCK_SRC_SPLL:
            scsValue = SCG_SYSTEM_CLOCK_SRC_SPLL;
            break;
        default:
            return false;
    }
    
    /* Configure clock control register based on mode */
    uint32_t ccr = SCG_RCCR_SCS(scsValue) |
                   SCG_RCCR_DIVCORE(config->divCore) |
                   SCG_RCCR_DIVBUS(config->divBus) |
                   SCG_RCCR_DIVSLOW(config->divSlow);
    
    switch (mode) {
        case SCG_SYSTEM_CLOCK_MODE_RUN:
            SCG->RCCR = ccr;
            break;
        case SCG_SYSTEM_CLOCK_MODE_VLPR:
            SCG->VCCR = ccr;
            break;
        case SCG_SYSTEM_CLOCK_MODE_HSRUN:
            SCG->HCCR = ccr;
            break;
        default:
            return false;
    }
    
    /* Wait for system clock source to switch */
    uint32_t timeout = SCG_TIMEOUT;
    while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT != scsValue) && (timeout > 0U)) {
        timeout--;
    }
    
    return (timeout > 0U);
}

bool SCG_GetSystemClockConfig(scg_system_clock_mode_t mode, scg_system_clock_config_t *config)
{
    if (config == NULL) {
        return false;
    }
    
    uint32_t ccr;
    
    /* Read clock control register based on mode */
    switch (mode) {
        case SCG_SYSTEM_CLOCK_MODE_RUN:
            ccr = SCG->RCCR;
            break;
        case SCG_SYSTEM_CLOCK_MODE_VLPR:
            ccr = SCG->VCCR;
            break;
        case SCG_SYSTEM_CLOCK_MODE_HSRUN:
            ccr = SCG->HCCR;
            break;
        default:
            return false;
    }
    
    /* Extract configuration */
    uint32_t scs = (ccr & SCG_RCCR_SCS_MASK) >> SCG_RCCR_SCS_SHIFT;
    
    /* Map register value to clock source enum */
    switch (scs) {
        case SCG_SYSTEM_CLOCK_SRC_SIRC:
            config->source = SCG_CLOCK_SRC_SIRC;
            break;
        case SCG_SYSTEM_CLOCK_SRC_FIRC:
            config->source = SCG_CLOCK_SRC_FIRC;
            break;
        case SCG_SYSTEM_CLOCK_SRC_SOSC:
            config->source = SCG_CLOCK_SRC_SOSC;
            break;
        case SCG_SYSTEM_CLOCK_SRC_SPLL:
            config->source = SCG_CLOCK_SRC_SPLL;
            break;
        default:
            return false;
    }
    
    config->divCore = (scg_clock_divider_t)((ccr & SCG_RCCR_DIVCORE_MASK) >> SCG_RCCR_DIVCORE_SHIFT);
    config->divBus  = (scg_clock_divider_t)((ccr & SCG_RCCR_DIVBUS_MASK) >> SCG_RCCR_DIVBUS_SHIFT);
    config->divSlow = (scg_clock_divider_t)((ccr & SCG_RCCR_DIVSLOW_MASK) >> SCG_RCCR_DIVSLOW_SHIFT);
    
    return true;
}

bool SCG_GetClockFrequencies(scg_clock_frequencies_t *freqs)
{
    if (freqs == NULL) {
        return false;
    }
    
    /* Get current system clock configuration */
    scg_system_clock_config_t config;
    if (!SCG_GetSystemClockConfig(SCG_SYSTEM_CLOCK_MODE_RUN, &config)) {
        return false;
    }
    
    /* Get base frequency of current clock source */
    uint32_t srcFreq = SCG_GetSourceFrequency(config.source);
    
    /* Calculate divided frequencies */
    freqs->coreClk = srcFreq / SCG_ConvertDividerToValue(config.divCore);
    freqs->busClk  = srcFreq / SCG_ConvertDividerToValue(config.divBus);
    freqs->slowClk = srcFreq / SCG_ConvertDividerToValue(config.divSlow);
    
    /* Clock source frequencies */
    freqs->sircClk = s_sircFreq;
    freqs->fircClk = s_fircFreq;
    freqs->soscClk = s_soscFreq;
    freqs->spllClk = s_spllFreq;
    
    return true;
}

bool SCG_EnableClock(scg_clock_source_t source)
{
    switch (source) {
        case SCG_CLOCK_SRC_SIRC:
            SCG->SIRCCSR |= SCG_SIRCCSR_SIRCEN_MASK;
            break;
        case SCG_CLOCK_SRC_FIRC:
            SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN_MASK;
            break;
        case SCG_CLOCK_SRC_SOSC:
            SCG->SOSCCSR |= SCG_SOSCCSR_SOSCEN_MASK;
            break;
        case SCG_CLOCK_SRC_SPLL:
            SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;
            break;
        default:
            return false;
    }
    
    return SCG_WaitForClockValid(source);
}

bool SCG_DisableClock(scg_clock_source_t source)
{
    /* Check if this is the current system clock source */
    scg_clock_source_t currentSource = SCG_GetSystemClockSource();
    if (currentSource == source) {
        return false;  /* Cannot disable current system clock */
    }
    
    switch (source) {
        case SCG_CLOCK_SRC_SIRC:
            SCG->SIRCCSR &= ~SCG_SIRCCSR_SIRCEN_MASK;
            break;
        case SCG_CLOCK_SRC_FIRC:
            SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;
            break;
        case SCG_CLOCK_SRC_SOSC:
            SCG->SOSCCSR &= ~SCG_SOSCCSR_SOSCEN_MASK;
            break;
        case SCG_CLOCK_SRC_SPLL:
            SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;
            break;
        default:
            return false;
    }
    
    return true;
}

bool SCG_IsClockValid(scg_clock_source_t source)
{
    bool isValid = false;
    
    switch (source) {
        case SCG_CLOCK_SRC_SIRC:
            isValid = (SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK) != 0U;
            break;
        case SCG_CLOCK_SRC_FIRC:
            isValid = (SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK) != 0U;
            break;
        case SCG_CLOCK_SRC_SOSC:
            isValid = (SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK) != 0U;
            break;
        case SCG_CLOCK_SRC_SPLL:
            isValid = (SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK) != 0U;
            break;
        default:
            isValid = false;
            break;
    }
    
    return isValid;
}

scg_clock_source_t SCG_GetSystemClockSource(void)
{
    uint32_t scs = (SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT;
    scg_clock_source_t source;
    
    switch (scs) {
        case SCG_SYSTEM_CLOCK_SRC_SIRC:
            source = SCG_CLOCK_SRC_SIRC;
            break;
        case SCG_SYSTEM_CLOCK_SRC_FIRC:
            source = SCG_CLOCK_SRC_FIRC;
            break;
        case SCG_SYSTEM_CLOCK_SRC_SOSC:
            source = SCG_CLOCK_SRC_SOSC;
            break;
        case SCG_SYSTEM_CLOCK_SRC_SPLL:
            source = SCG_CLOCK_SRC_SPLL;
            break;
        default:
            source = SCG_CLOCK_SRC_FIRC;  /* Default */
            break;
    }
    
    return source;
}

uint32_t SCG_GetCoreClockFreq(void)
{
    scg_clock_frequencies_t freqs;
    
    if (SCG_GetClockFrequencies(&freqs)) {
        return freqs.coreClk;
    }
    
    return 0U;
}

uint32_t SCG_GetBusClockFreq(void)
{
    scg_clock_frequencies_t freqs;
    
    if (SCG_GetClockFrequencies(&freqs)) {
        return freqs.busClk;
    }
    
    return 0U;
}

uint32_t SCG_GetSlowClockFreq(void)
{
    scg_clock_frequencies_t freqs;
    
    if (SCG_GetClockFrequencies(&freqs)) {
        return freqs.slowClk;
    }
    
    return 0U;
}

bool SCG_ConfigureClockout(uint8_t source)
{
    if (source > 15U) {
        return false;
    }
    
    SCG->CLKOUTCNFG = SCG_CLKOUTCNFG_CLKOUTSEL(source);
    
    return true;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
