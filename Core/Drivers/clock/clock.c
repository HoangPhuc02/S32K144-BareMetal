/**
 * @file    clock.c
 * @brief   Clock Driver Implementation for S32K144
 * @details This file contains the implementation of clock driver functions
 *          for S32K144 microcontroller.
 * 
 * @author  PhucPH32
 * @date    25/11/2025
 * @version 1.0
 * 
 * @note    This implementation provides system clock configuration and management
 * @warning Always ensure clock sources are valid before switching
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 25, 2025): Initial version
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "clock.h"
#include "clock_reg.h"
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Default clock frequencies */
#define SIRC_DEFAULT_FREQ       (8000000U)      /* 8 MHz */
#define FIRC_DEFAULT_FREQ       (48000000U)     /* 48 MHz */
#define SOSC_DEFAULT_FREQ       (8000000U)      /* 8 MHz (typical) */

/* Timeout values */
#define CLOCK_TIMEOUT           (10000U)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief External oscillator frequency (set by CLOCK_EnableSOSC) */
static uint32_t s_soscFreq = SOSC_DEFAULT_FREQ;

/** @brief SPLL output frequency (set by CLOCK_EnableSPLL) */
static uint32_t s_spllFreq = 0U;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Get clock source frequency
 * @param[in] source Clock source
 * @return Clock frequency in Hz
 */
static uint32_t CLOCK_GetSourceFreq(clock_source_t source);

/**
 * @brief Convert divider enum to actual divider value
 * @param[in] divider Divider enumeration
 * @return Actual divider value (1-8)
 */
static uint32_t CLOCK_GetDividerValue(clock_divider_t divider);

/**
 * @brief Wait for clock source to be valid
 * @param[in] source Clock source to check
 * @return true if valid, false if timeout
 */
static bool CLOCK_WaitForValid(clock_source_t source);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static uint32_t CLOCK_GetSourceFreq(clock_source_t source)
{
    uint32_t freq = 0U;
    
    switch (source) {
        case CLOCK_SRC_SIRC:
            freq = SIRC_DEFAULT_FREQ;
            break;
        case CLOCK_SRC_FIRC:
            freq = FIRC_DEFAULT_FREQ;
            break;
        case CLOCK_SRC_SOSC:
            freq = s_soscFreq;
            break;
        case CLOCK_SRC_SPLL:
            freq = s_spllFreq;
            break;
        default:
            freq = 0U;
            break;
    }
    
    return freq;
}

static uint32_t CLOCK_GetDividerValue(clock_divider_t divider)
{
    return (uint32_t)divider + 1U;
}

static bool CLOCK_WaitForValid(clock_source_t source)
{
    uint32_t timeout = CLOCK_TIMEOUT;
    bool isValid = false;
    
    while ((timeout > 0U) && (!isValid)) {
        switch (source) {
            case CLOCK_SRC_SIRC:
                isValid = SCG_IS_SIRC_VALID();
                break;
            case CLOCK_SRC_FIRC:
                isValid = SCG_IS_FIRC_VALID();
                break;
            case CLOCK_SRC_SOSC:
                isValid = SCG_IS_SOSC_VALID();
                break;
            case CLOCK_SRC_SPLL:
                isValid = SCG_IS_SPLL_VALID();
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

bool CLOCK_Init(const clock_config_t *config)
{
    if (config == NULL) {
        return false;
    }
    
    bool status = true;
    
    /* Enable the selected clock source first */
    switch (config->clockSource) {
        case CLOCK_SRC_SIRC:
            status = CLOCK_EnableSIRC();
            break;
        case CLOCK_SRC_FIRC:
            status = CLOCK_EnableFIRC();
            break;
        case CLOCK_SRC_SOSC:
            status = CLOCK_EnableSOSC(SOSC_DEFAULT_FREQ);
            break;
        case CLOCK_SRC_SPLL:
            /* SPLL requires SOSC to be enabled first */
            status = CLOCK_EnableSOSC(SOSC_DEFAULT_FREQ);
            if (status) {
                status = CLOCK_EnableSPLL(SOSC_DEFAULT_FREQ, 80000000U);
            }
            break;
        default:
            status = false;
            break;
    }
    
    if (!status) {
        return false;
    }
    
    /* Set dividers */
    status = CLOCK_SetDividers(config->divCore, config->divBus, config->divSlow);
    if (!status) {
        return false;
    }
    
    /* Switch to the selected clock source */
    status = CLOCK_SetSystemClockSource(config->clockSource);
    
    return status;
}

bool CLOCK_EnableSIRC(void)
{
    /* Enable SIRC */
    SCG->SIRCCSR |= SCG_SIRCCSR_SIRCEN_MASK;
    
    /* Wait for SIRC to be valid */
    return CLOCK_WaitForValid(CLOCK_SRC_SIRC);
}

bool CLOCK_EnableFIRC(void)
{
    /* Enable FIRC */
    SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN_MASK;
    
    /* Wait for FIRC to be valid */
    return CLOCK_WaitForValid(CLOCK_SRC_FIRC);
}

bool CLOCK_EnableSOSC(uint32_t extFreq)
{
    /* Store external frequency */
    s_soscFreq = extFreq;
    
    /* Configure SOSC for medium frequency range (4-8 MHz typical) */
    SCG->SOSCCFG = 0x20U;  /* Range: Medium frequency range */
    
    /* Enable SOSC */
    SCG->SOSCCSR |= SCG_SOSCCSR_SOSCEN_MASK;
    
    /* Wait for SOSC to be valid */
    return CLOCK_WaitForValid(CLOCK_SRC_SOSC);
}

bool CLOCK_EnableSPLL(uint32_t inputFreq, uint32_t outputFreq)
{
    /* Ensure SOSC is enabled */
    if (!SCG_IS_SOSC_VALID()) {
        return false;
    }
    
    /* Calculate PLL multiplier and dividers
     * PLL = (Input * MULT) / (PREDIV * POSTDIV)
     * For simplicity, using fixed values for 80MHz from 8MHz input
     * MULT = 40, PREDIV = 2, POSTDIV = 2
     * PLL = (8MHz * 40) / (2 * 2) = 80MHz
     */
    
    /* Configure SPLL: PREDIV=1 (div by 2), MULT=24 (x40) */
    SCG->SPLLCFG = 0x00180000U;  /* MULT=24 (multiply by 40), PREDIV=1 (divide by 2) */
    
    /* Store output frequency */
    s_spllFreq = outputFreq;
    
    /* Enable SPLL */
    SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;
    
    /* Wait for SPLL to be valid */
    return CLOCK_WaitForValid(CLOCK_SRC_SPLL);
}

void CLOCK_DisableClockSource(clock_source_t source)
{
    /* Don't disable if it's the current system clock */
    clock_source_t currentSource = CLOCK_GetSystemClockSource();
    if (currentSource == source) {
        return;
    }
    
    switch (source) {
        case CLOCK_SRC_SIRC:
            SCG->SIRCCSR &= ~SCG_SIRCCSR_SIRCEN_MASK;
            break;
        case CLOCK_SRC_FIRC:
            SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;
            break;
        case CLOCK_SRC_SOSC:
            SCG->SOSCCSR &= ~SCG_SOSCCSR_SOSCEN_MASK;
            break;
        case CLOCK_SRC_SPLL:
            SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;
            break;
        default:
            break;
    }
}

bool CLOCK_SetSystemClockSource(clock_source_t source)
{
    /* Check if clock source is valid */
    if (!CLOCK_WaitForValid(source)) {
        return false;
    }
    
    uint32_t scsValue = 0U;
    
    /* Map clock source to SCS value */
    switch (source) {
        case CLOCK_SRC_SIRC:
            scsValue = SCG_SYSTEM_CLOCK_SRC_SIRC;
            break;
        case CLOCK_SRC_FIRC:
            scsValue = SCG_SYSTEM_CLOCK_SRC_FIRC;
            break;
        case CLOCK_SRC_SOSC:
            scsValue = SCG_SYSTEM_CLOCK_SRC_SOSC;
            break;
        case CLOCK_SRC_SPLL:
            scsValue = SCG_SYSTEM_CLOCK_SRC_SPLL;
            break;
        default:
            return false;
    }
    
    /* Set system clock source in RUN mode */
    SCG->RCCR = (SCG->RCCR & ~SCG_RCCR_SCS_MASK) | SCG_RCCR_SCS(scsValue);
    
    /* Wait for clock switch to complete */
    uint32_t timeout = CLOCK_TIMEOUT;
    while ((timeout > 0U) && (SCG_GET_SYSTEM_CLOCK_SRC() != scsValue)) {
        timeout--;
    }
    
    return (timeout > 0U);
}

clock_source_t CLOCK_GetSystemClockSource(void)
{
    uint32_t scs = SCG_GET_SYSTEM_CLOCK_SRC();
    clock_source_t source;
    
    switch (scs) {
        case SCG_SYSTEM_CLOCK_SRC_SIRC:
            source = CLOCK_SRC_SIRC;
            break;
        case SCG_SYSTEM_CLOCK_SRC_FIRC:
            source = CLOCK_SRC_FIRC;
            break;
        case SCG_SYSTEM_CLOCK_SRC_SOSC:
            source = CLOCK_SRC_SOSC;
            break;
        case SCG_SYSTEM_CLOCK_SRC_SPLL:
            source = CLOCK_SRC_SPLL;
            break;
        default:
            source = CLOCK_SRC_SIRC;  /* Default */
            break;
    }
    
    return source;
}

void CLOCK_GetFrequencies(clock_frequencies_t *frequencies)
{
    if (frequencies == NULL) {
        return;
    }
    
    /* Get current system clock source */
    clock_source_t source = CLOCK_GetSystemClockSource();
    uint32_t systemFreq = CLOCK_GetSourceFreq(source);
    
    /* Read dividers from RCCR register */
    uint32_t divCore = ((SCG->RCCR & SCG_RCCR_DIVCORE_MASK) >> SCG_RCCR_DIVCORE_SHIFT) + 1U;
    uint32_t divBus = ((SCG->RCCR & SCG_RCCR_DIVBUS_MASK) >> SCG_RCCR_DIVBUS_SHIFT) + 1U;
    uint32_t divSlow = ((SCG->RCCR & SCG_RCCR_DIVSLOW_MASK) >> SCG_RCCR_DIVSLOW_SHIFT) + 1U;
    
    /* Calculate frequencies */
    frequencies->systemClockFreq = systemFreq;
    frequencies->coreClockFreq = systemFreq / divCore;
    frequencies->busClockFreq = frequencies->coreClockFreq / divBus;
    frequencies->slowClockFreq = frequencies->coreClockFreq / divSlow;
}

uint32_t CLOCK_GetCoreClockFreq(void)
{
    clock_frequencies_t frequencies;
    CLOCK_GetFrequencies(&frequencies);
    return frequencies.coreClockFreq;
}

uint32_t CLOCK_GetBusClockFreq(void)
{
    clock_frequencies_t frequencies;
    CLOCK_GetFrequencies(&frequencies);
    return frequencies.busClockFreq;
}

uint32_t CLOCK_GetSlowClockFreq(void)
{
    clock_frequencies_t frequencies;
    CLOCK_GetFrequencies(&frequencies);
    return frequencies.slowClockFreq;
}

bool CLOCK_SetDividers(clock_divider_t divCore, clock_divider_t divBus, clock_divider_t divSlow)
{
    /* Clear and set dividers */
    uint32_t rccr = SCG->RCCR;
    
    rccr &= ~(SCG_RCCR_DIVCORE_MASK | SCG_RCCR_DIVBUS_MASK | SCG_RCCR_DIVSLOW_MASK);
    rccr |= SCG_RCCR_DIVCORE(divCore) | SCG_RCCR_DIVBUS(divBus) | SCG_RCCR_DIVSLOW(divSlow);
    
    SCG->RCCR = rccr;
    
    return true;
}

bool CLOCK_SetDefaultConfig(void)
{
    clock_config_t defaultConfig = {
        .clockSource = CLOCK_SRC_FIRC,      /* Use FIRC (48MHz) */
        .divCore = CLOCK_DIV_BY_1,          /* Core = 48MHz */
        .divBus = CLOCK_DIV_BY_2,           /* Bus = 24MHz */
        .divSlow = CLOCK_DIV_BY_4           /* Slow = 12MHz */
    };
    
    return CLOCK_Init(&defaultConfig);
}
