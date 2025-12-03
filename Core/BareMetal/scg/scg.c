/*
 * @file    scg.c
 * @brief   SCG Driver Implementation for S32K144
 */

#include "scg.h"

/* ----------------------------------------------------------------------------
   -- SOSC - System OSC
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable or disable the System OSC (SOSC) clock.
 *
 * Unlocks the SOSC control register and sets the enable state based on the input parameter.
 * If enabled, waits until the SOSC clock is valid before returning.
 *
 * @param enable Boolean flag:
 *        - true: Enable SOSC
 *        - false: Disable SOSC
 *
 * @return void This function does not return a value.
 *
 * @note This function does not configure dividers or range. Call configuration functions before enabling.
 */
void SCG_SOSCEnable(bool enable)
{
	/* Step 1: Unlock SOSC control register */
    SCG->SOSCCSR &= ~SCG_SOSCCSR_LK_MASK;
    /* Enable or disable SOSC */
    if (enable)
    {
    	/* Step 2: Enable */
        SCG->SOSCCSR |= SCG_SOSCCSR_SOSCEN_MASK;

        /* Step 3: Wait until SOSC is valid*/
        while (READ_BIT(SCG->SOSCCSR, SCG_SOSCCSR_SOSCVLD_SHIFT) != 1);
    }
    else
    {
    	/* Step 2: Disable */
        SCG->SOSCCSR &= ~SCG_SOSCCSR_SOSCEN_MASK;
    }
}

/**
 * @brief Configure the SOSC source and frequency range.
 *
 * Unlocks the SOSC control register and sets the oscillator source type (internal or external)
 * and frequency range according to the provided parameters.
 *
 * @param source Clock source type (scg_sosc_source_t):
 *        - SCG_SOSC_SOURCE_EXTERNAL: External clock source
 *        - SCG_SOSC_SOURCE_INTERNAL: Internal crystal oscillator
 * @param range Frequency range selection (scg_sosc_range_t):
 *        - LOW_FREQ: < 4 MHz
 *        - MEDIUM_FREQ: 4_8 MHz
 *        - HIGH_FREQ: 8_40 MHz
 *
 * @return void This function does not return a value.
 *
 * @note This function does not enable SOSC. Call SCG_SOSCEnable() after configuration.
 */
void SCG_SOSCSetSourceAndRange(scg_sosc_source_t source, scg_sosc_range_t range)
{
	/* Step 1: Unlock SOSC control register */
	SCG->SOSCCSR &= ~SCG_SOSCCSR_LK_MASK;

	/* Step 2: Disable SOSC before configuration */
	SCG->SOSCCSR &= ~SCG_SOSCCSR_SOSCEN_MASK;

	/* Step 3: Configure range and source */
	SCG->SOSCCFG |= SCG_SOSCCFG_HGO_MASK |
					   SCG_SOSCCFG_RANGE(range) |
					   SCG_SOSCCFG_EREFS(source);

    /* Step 4: Enable SOSC */
    SCG->SOSCCSR |= SCG_SOSCCSR_SOSCEN_MASK;

    /* Step 5: Wait until SOSC is valid*/
    while (READ_BIT(SCG->SOSCCSR, SCG_SOSCCSR_SOSCVLD_SHIFT) != 1);
}

/**
 * @brief Initialize the System Oscillator (SOSC).
 *
 * This function unlocks the SOSC control register, disables SOSC,
 * configures its source, range, and dividers, then enables SOSC
 * and waits until it becomes valid.
 *
 * @param config Pointer to SCG_SOSCConfig structure containing:
 *        - source: External reference source selection.
 *        - range: Frequency range selection.
 *        - divider1: Divider for SOSCDIV1.
 *        - divider2: Divider for SOSCDIV2.
 *
 * @note The function blocks until SOSC is valid.
*/
void SCG_SOSCInit(scg_sosc_config_t *p_config)
{
	/* Step 1: Unlock SOSC control register */
	SCG->SOSCCSR &= ~SCG_SOSCCSR_LK_MASK;

	/* Step 2: Disable SOSC before configuration */
	SCG->SOSCCSR &= ~SCG_SOSCCSR_SOSCEN_MASK;

	/* Step 3: Configure range and source */
	SCG->SOSCCFG |= SCG_SOSCCFG_HGO_MASK |
					   SCG_SOSCCFG_RANGE(p_config->range) |
					   SCG_SOSCCFG_EREFS(p_config->source);

	/* Step 4: Configure divider */
	SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(p_config->divider1) | SCG_SOSCDIV_SOSCDIV2(p_config->divider2);

    /* Step 5: Enable SOSC */
    SCG->SOSCCSR |= SCG_SOSCCSR_SOSCEN_MASK;

    /* Step 6: Wait until SOSC is valid*/
    while (READ_BIT(SCG->SOSCCSR, SCG_SOSCCSR_SOSCVLD_SHIFT) != 1);
}

/**
 * @brief Configure SOSC clock dividers.
 *
 * Unlocks the SOSC control register, disables SOSC temporarily, sets the divider values
 * for SOSCDIV1 and SOSCDIV2, then re-enables SOSC. Waits until SOSC is valid before returning.
 *
 * @param divider1 Divider value for SOSCDIV1 (type scg_sosc_div_t).
 * @param divider2 Divider value for SOSCDIV2 (type scg_sosc_div_t).
 *
 * @return void This function does not return a value.
 *
 * @note Changes to SOSCDIV must be done when SOSC is disabled to prevent glitches.
 */
void SCG_SOSCSetDivider(scg_sosc_div_t divider1, scg_sosc_div_t divider2)
{
	/* Step 1: Unlock SOSC control register */
    SCG->SOSCCSR &= ~SCG_SOSCCSR_LK_MASK;

    /* Step 2: Disable SOSC before configuration */
    SCG->SOSCCSR &= ~SCG_SOSCCSR_SOSCEN_MASK;

	/* Step 3: Configure divider */
    SCG->SOSCDIV |= SCG_SOSCDIV_SOSCDIV1(divider1) | SCG_SOSCDIV_SOSCDIV2(divider2);

    /* Step 4: Enable SOSC */
    SCG->SOSCCSR |= SCG_SOSCCSR_SOSCEN_MASK;

    /* Step 5: Wait until SOSC is valid*/
    while (READ_BIT(SCG->SOSCCSR, SCG_SOSCCSR_SOSCVLD_SHIFT) != 1);
}


/* ----------------------------------------------------------------------------
   -- SIRC - Slow IRC
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable or disable the SIRC (Slow IRC) clock.
 *
 * This function unlocks the SIRC control register and sets the enable state
 * based on the input parameter. It waits until the SIRC clock is valid if enabled.
 *
 * @param enable Boolean flag:
 *        - true: Enable SIRC
 *        - false: Disable SIRC
 *
 * @return void This function does not return a value.
 *
 * @note Default SIRC frequency is 8 MHz. This function does not configure dividers.
 */
void SCG_SIRCEnable(bool enable)
{
    /* Step 1: Unlock SIRC control register */
    SCG->SIRCCSR &= ~SCG_SIRCCSR_LK_MASK;

    /* Enable or disable SIRC */
    if (enable)
    {
    	/* Step 2: Enable */
    	SCG->SIRCCSR |= SCG_SIRCCSR_SIRCEN_MASK;

		/* Step 3: Wait until SIRC is valid */
		while (READ_BIT(SCG->SIRCCSR, SCG_SIRCCSR_SIRCVLD_SHIFT) != 1);
    }
    else
    {
    	/* Step 2: Disable */
    	SCG->SIRCCSR &= ~SCG_SIRCCSR_SIRCEN_MASK;
    }
}

/**
 * @brief Configure SIRC clock dividers.
 *
 * This function unlocks the SIRC control register, disables SIRC temporarily,
 * sets the divider values for SIRCDIV1 and SIRCDIV2, then re-enables SIRC.
 * It waits until the SIRC clock is valid before returning.
 *
 * @param divider1 Divider value for SIRCDIV1 (type scg_sirc_div_t).
 * @param divider2 Divider value for SIRCDIV2 (type scg_sirc_div_t).
 *
 * @return void This function does not return a value.
 *
 * @note Changes to SIRCDIV must be done when SIRC is disabled to prevent glitches.
 * 		 This function is setup SIRC disabled then setup divider.
 * 		 Default SIRC is 8MHz
 */
void SCG_SIRCSetDivider(scg_sirc_div_t divider1, scg_sirc_div_t divider2)
{
    /* Step 1: Unlock SIRC control register */
    SCG->SIRCCSR &= ~SCG_SIRCCSR_LK_MASK;

    /* Step 2: Disable SIRC before configuration */
    SCG->SIRCCSR &= ~SCG_SIRCCSR_SIRCEN_MASK;

    /* Step 3: Configure SIRC dividers */
    SCG->SIRCDIV |= SCG_SIRCDIV_SIRCDIV1(divider1) | SCG_SIRCDIV_SIRCDIV2(divider2);

    /* Step 4: Enable SIRC */
    SCG->SIRCCSR |= SCG_SIRCCSR_SIRCEN_MASK;

    /* Step 5: Wait until SIRC is valid */
    while (READ_BIT(SCG->SIRCCSR, SCG_SIRCCSR_SIRCVLD_SHIFT) != 1);
}


/* ----------------------------------------------------------------------------
   -- FIRC - Fast IRC
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable or disable the FIRC (Fast IRC) clock.
 *
 * This function unlocks the FIRC control register and sets the enable state
 * based on the input parameter. It waits until the FIRC clock is valid if enabled.
 *
 * @param enable Boolean flag:
 *        - true: Enable FIRC
 *        - false: Disable FIRC
 *
 * @return void This function does not return a value.
 *
 * @note Default FIRC frequency is 48 MHz. This function does not configure dividers.
 */
void SCG_FIRCEnable(bool enable)
{
    /* Step 1: Unlock FIRC control register */
    SCG->FIRCCSR &= ~SCG_FIRCCSR_LK_MASK;

    /* Enable or disable FIRC */
    if (enable)
    {
    	/* Step 2: Enable */
        SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN_MASK;

        /* Step 3: Wait until FIRC is valid */
        while (READ_BIT(SCG->FIRCCSR, SCG_FIRCCSR_FIRCVLD_SHIFT) != 1);
    }
    else
    {
    	/* Step 2: Disable */
        SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;
    }
}

/**
 * @brief Configure FIRC clock dividers.
 *
 * This function unlocks the FIRC control register, disables FIRC temporarily,
 * sets the divider values for FIRCDIV1 and FIRCDIV2, then re-enables FIRC.
 * It waits until the FIRC clock is valid before returning.
 *
 * @param divider1 Divider value for FIRCDIV1 (type scg_firc_div_t).
 * @param divider2 Divider value for FIRCDIV2 (type scg_firc_div_t).
 *
 * @return void This function does not return a value.
 *
 * @note Changes to FIRCDIV must be done when FIRC is disabled to prevent glitches.
 * 		 This function is setup FIRC disabled then setup divider.
 * 		 Default FIRC is 48MHz
 */
void SCG_FIRCSetDivider(scg_firc_div_t divider1, scg_firc_div_t divider2)
{
    /* Step 1: Unlock FIRC control register */
    SCG->FIRCCSR &= ~SCG_FIRCCSR_LK_MASK;

    /* Step 2: Disable FIRC before configuration */
    SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;

    /* Step 3: Configure FIRC dividers */
    SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV1(divider1) | SCG_FIRCDIV_FIRCDIV2(divider2);

    /* Step 4: Enable FIRC */
    SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN_MASK;

    /* Step 5: Wait until FIRC is valid */
    while (READ_BIT(SCG->FIRCCSR, SCG_FIRCCSR_FIRCVLD_SHIFT) != 1);
}


/* ----------------------------------------------------------------------------
   -- SPLL - System PLL
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable or disable the System PLL (SPLL) clock.
 *
 * Unlocks the SPLL control register and sets the enable state based on the input parameter.
 * If enabled, waits until the SPLL clock is valid before returning.
 *
 * @param enable Boolean flag:
 *        - true: Enable SPLL
 *        - false: Disable SPLL
 *
 * @return void This function does not return a value.
 *
 * @note This function does not configure dividers or multiplier. Call configuration functions before enabling.
 */
void SCG_SPLLEnable(bool enable)
{
    /* Step 1: Unlock SPLL control register */
    SCG->SPLLCSR &= ~SCG_SPLLCSR_LK_MASK;
    /* Enable or Disable PLL */
    if (enable)
    {
    	/* Step 2: Enable */
        SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;
        /* Step 3: Wait until SPLL is valid */
        while (READ_BIT(SCG->SPLLCSR, SCG_SPLLCSR_SPLLVLD_SHIFT) != 1);
    }
    else
    {
    	/* Step 2: Disable */
        SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;
    }
}

/**
 * @brief Configure SPLL clock dividers.
 *
 * Unlocks the SPLL control register, disables SPLL temporarily, sets the divider values
 * for SPLLDIV1 and SPLLDIV2, then re-enables SPLL. Waits until SPLL is valid before returning.
 *
 * @param divider1 Divider value for SPLLDIV1 (type scg_spll_div_t).
 * @param divider2 Divider value for SPLLDIV2 (type scg_spll_div_t).
 *
 * @return void This function does not return a value.
 *
 * @note Changes to SPLLDIV must be done when SPLL is disabled to prevent glitches.
 */
void SCG_SPLLSetDivider(scg_spll_div_t divider1, scg_spll_div_t divider2)
{
    /* Step 1: Unlock SPLL control register */
    SCG->SPLLCSR &= ~SCG_SPLLCSR_LK_MASK;

    /* Step 2: Disable SPLL before configuration */
    SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;

    /* Step 3: Configure SPLL dividers */
    SCG->SPLLDIV |= SCG_SPLLDIV_SPLLDIV1(divider1) | SCG_SPLLDIV_SPLLDIV2(divider2);

    /* Step 4: Enable SPLL */
    SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;

    /* Step 5: Wait until SPLL is valid */
    while (READ_BIT(SCG->SPLLCSR, SCG_SPLLCSR_SPLLVLD_SHIFT) != 1);
}

/**
 * @brief Initialize the System PLL (SPLL).
 *
 * This function unlocks the SPLL control register, disables SPLL,
 * configures multiplier, pre-divider, and output dividers, then enables SPLL
 * and waits until it becomes valid.
 * Output clock = (input clock * multi) / (prediv * 2)
 * input clock is SOSC = 8MHz
 *
 * @param p_config Pointer to SCG_SPLLConfig_t structure containing:
 *        - multi: Multiplier value for SPLL.
 *        - prediv: Pre-divider value for SPLL input clock.
 *        - divider1: Divider for SPLLDIV1 output.
 *        - divider2: Divider for SPLLDIV2 output.
 *
 * @note The function blocks until SPLL is valid.
 */
void SCG_SPLLInit(scg_spll_config_t *p_config)
{
    /* Step 1: Unlock SPLL control register */
    SCG->SPLLCSR &= ~SCG_SPLLCSR_LK_MASK;

    /* Step 2: Disable SPLL before configuration */
    SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;

    /* Step 3: Configure multiplier and pre-divider */
    SCG->SPLLCFG |= SCG_SPLLCFG_MULT(p_config->multi) |
                       SCG_SPLLCFG_PREDIV(p_config->prediv);

    /* Step 4: Configure output dividers */
    SCG->SPLLDIV |= SCG_SPLLDIV_SPLLDIV1(p_config->divider1) |
                       SCG_SPLLDIV_SPLLDIV2(p_config->divider2);

    /* Step 5: Enable SPLL */
    SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;

    /* Step 6: Wait until SPLL is valid */
    while (READ_BIT(SCG->SPLLCSR, SCG_SPLLCSR_SPLLVLD_SHIFT) != 1);
}


/* ----------------------------------------------------------------------------
   -- RCCR
   ---------------------------------------------------------------------------- */

/**
 * @brief Configure the system clock source and dividers.
 *
 * This function sets the system clock source and its associated dividers
 * (core, bus, and slow) in the SCG Run Clock Control Register (RCCR).
 *
 * @param p_config Pointer to SCG_RCCRConfig structure containing:
 *        - source: System clock source (SOSC, SIRC, FIRC, SPLL).
 *        - divcore: Core clock divider.
 *        - divbus: Bus clock divider.
 *        - divlow: Slow clock divider.
 *
 * @note This function updates RCCR register fields based on provided configuration.
 */
void SCG_RCCRConfigSource(scg_rccr_config_t *p_config)
{
    /* Read current RCCR value */
    uint32_t temp = SCG->RCCR;

    /* Prepare new configuration: source and dividers */
    temp = SCG_RCCR_SCS(p_config->source) |
           SCG_RCCR_DIVCORE(p_config->divcore) |
           SCG_RCCR_DIVBUS(p_config->divbus) |
           SCG_RCCR_DIVSLOW(p_config->divlow);

    /* Apply configuration to RCCR register */
    SCG->RCCR |= temp;
}

// NEW ADD ON

/* ----------------------------------------------------------------------------
   -- Clock Monitoring
   ---------------------------------------------------------------------------- */

/* Default clock frequencies in Hz */
#define SOSC_DEFAULT_FREQ    8000000U   /* 8 MHz external crystal */
#define SIRC_DEFAULT_FREQ    8000000U   /* 8 MHz slow IRC */
#define FIRC_DEFAULT_FREQ    48000000U  /* 48 MHz fast IRC */

/**
 * @brief Get the frequency of a specific clock.
 *
 * This function calculates and returns the frequency of the specified clock type.
 *
 * @param clockType The type of clock to query (scg_clock_type_t).
 *
 * @return Clock frequency in Hz, or 0 if clock is disabled or invalid.
 *
 * @note For SOSC, this function assumes 8MHz external crystal.
 *       For accurate results, you may need to pass the actual SOSC frequency.
 */
uint32_t SCG_GetClockFreq(scg_clock_type_t clockType)
{
    uint32_t freq = 0;
    uint32_t divider = 0;
    uint32_t mult = 0;
    uint32_t prediv = 0;
    
    switch(clockType) {
        case SCG_CLOCK_SOSC:
            /* Check if SOSC is enabled and valid */
            if ((SCG->SOSCCSR & SCG_SOSCCSR_SOSCEN_MASK) && 
                (SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK)) {
                freq = SOSC_DEFAULT_FREQ;
            }
            break;
            
        case SCG_CLOCK_SIRC:
            /* Check if SIRC is enabled and valid */
            if ((SCG->SIRCCSR & SCG_SIRCCSR_SIRCEN_MASK) && 
                (SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK)) {
                freq = SIRC_DEFAULT_FREQ;
            }
            break;
            
        case SCG_CLOCK_FIRC:
            /* Check if FIRC is enabled and valid */
            if ((SCG->FIRCCSR & SCG_FIRCCSR_FIRCEN_MASK) && 
                (SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK)) {
                freq = FIRC_DEFAULT_FREQ;
            }
            break;
            
        case SCG_CLOCK_SPLL:
            /* Check if SPLL is enabled and valid */
            if ((SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK) && 
                (SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)) {
                /* SPLL frequency = (SOSC * MULT) / (PREDIV + 1) / 2 */
                mult = (SCG->SPLLCFG & SCG_SPLLCFG_MULT_MASK) >> SCG_SPLLCFG_MULT_SHIFT;
                prediv = (SCG->SPLLCFG & SCG_SPLLCFG_PREDIV_MASK) >> SCG_SPLLCFG_PREDIV_SHIFT;
                freq = (SOSC_DEFAULT_FREQ * mult) / ((prediv + 1) * 2);
            }
            break;
            
        case SCG_CLOCK_CORE:
            /* Get system clock source */
            {
                scg_systems_source_t source = (scg_systems_source_t)((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT);
                uint32_t sourceFreq = 0;
                
                switch(source) {
                    case SCG_SYSTEM_SRC_SOSC:
                        sourceFreq = SCG_GetClockFreq(SCG_CLOCK_SOSC);
                        break;
                    case SCG_SYSTEM_SRC_SIRC:
                        sourceFreq = SCG_GetClockFreq(SCG_CLOCK_SIRC);
                        break;
                    case SCG_SYSTEM_SRC_FIRC:
                        sourceFreq = SCG_GetClockFreq(SCG_CLOCK_FIRC);
                        break;
                    case SCG_SYSTEM_SRC_SPLL:
                        sourceFreq = SCG_GetClockFreq(SCG_CLOCK_SPLL);
                        break;
                    default:
                        break;
                }
                
                divider = (SCG->CSR & SCG_CSR_DIVCORE_MASK) >> SCG_CSR_DIVCORE_SHIFT;
                freq = sourceFreq / (divider + 1);
            }
            break;
            
        case SCG_CLOCK_BUS:
            /* Bus clock = Core clock / (DIVBUS + 1) */
            {
                uint32_t coreFreq = SCG_GetClockFreq(SCG_CLOCK_CORE);
                divider = (SCG->CSR & SCG_CSR_DIVBUS_MASK) >> SCG_CSR_DIVBUS_SHIFT;
                freq = coreFreq / (divider + 1);
            }
            break;
            
        case SCG_CLOCK_SLOW:
            /* Slow clock = Core clock / (DIVSLOW + 1) */
            {
                uint32_t coreFreq = SCG_GetClockFreq(SCG_CLOCK_CORE);
                divider = (SCG->CSR & SCG_CSR_DIVSLOW_MASK) >> SCG_CSR_DIVSLOW_SHIFT;
                freq = coreFreq / (divider + 1);
            }
            break;
            
        case SCG_CLOCK_SOSCDIV1:
            divider = (SCG->SOSCDIV & SCG_SOSCDIV_SOSCDIV1_MASK) >> SCG_SOSCDIV_SOSCDIV1_SHIFT;
            if (divider != 0) {
                freq = SCG_GetClockFreq(SCG_CLOCK_SOSC) / (1U << (divider - 1));
            }
            break;
            
        case SCG_CLOCK_SOSCDIV2:
            divider = (SCG->SOSCDIV & SCG_SOSCDIV_SOSCDIV2_MASK) >> SCG_SOSCDIV_SOSCDIV2_SHIFT;
            if (divider != 0) {
                freq = SCG_GetClockFreq(SCG_CLOCK_SOSC) / (1U << (divider - 1));
            }
            break;
            
        case SCG_CLOCK_SIRCDIV1:
            divider = (SCG->SIRCDIV & SCG_SIRCDIV_SIRCDIV1_MASK) >> SCG_SIRCDIV_SIRCDIV1_SHIFT;
            if (divider != 0) {
                freq = SCG_GetClockFreq(SCG_CLOCK_SIRC) / (1U << (divider - 1));
            }
            break;
            
        case SCG_CLOCK_SIRCDIV2:
            divider = (SCG->SIRCDIV & SCG_SIRCDIV_SIRCDIV2_MASK) >> SCG_SIRCDIV_SIRCDIV2_SHIFT;
            if (divider != 0) {
                freq = SCG_GetClockFreq(SCG_CLOCK_SIRC) / (1U << (divider - 1));
            }
            break;
            
        case SCG_CLOCK_FIRCDIV1:
            divider = (SCG->FIRCDIV & SCG_FIRCDIV_FIRCDIV1_MASK) >> SCG_FIRCDIV_FIRCDIV1_SHIFT;
            if (divider != 0) {
                freq = SCG_GetClockFreq(SCG_CLOCK_FIRC) / (1U << (divider - 1));
            }
            break;
            
        case SCG_CLOCK_FIRCDIV2:
            divider = (SCG->FIRCDIV & SCG_FIRCDIV_FIRCDIV2_MASK) >> SCG_FIRCDIV_FIRCDIV2_SHIFT;
            if (divider != 0) {
                freq = SCG_GetClockFreq(SCG_CLOCK_FIRC) / (1U << (divider - 1));
            }
            break;
            
        case SCG_CLOCK_SPLLDIV1:
            divider = (SCG->SPLLDIV & SCG_SPLLDIV_SPLLDIV1_MASK) >> SCG_SPLLDIV_SPLLDIV1_SHIFT;
            if (divider != 0) {
                freq = SCG_GetClockFreq(SCG_CLOCK_SPLL) / (1U << (divider - 1));
            }
            break;
            
        case SCG_CLOCK_SPLLDIV2:
            divider = (SCG->SPLLDIV & SCG_SPLLDIV_SPLLDIV2_MASK) >> SCG_SPLLDIV_SPLLDIV2_SHIFT;
            if (divider != 0) {
                freq = SCG_GetClockFreq(SCG_CLOCK_SPLL) / (1U << (divider - 1));
            }
            break;
            
        default:
            freq = 0;
            break;
    }
    
    return freq;
}

/**
 * @brief Check if a clock is valid.
 *
 * This function checks if the specified clock is enabled and valid.
 *
 * @param clockType The type of clock to check (scg_clock_type_t).
 *
 * @return true if clock is valid, false otherwise.
 */
bool SCG_IsClockValid(scg_clock_type_t clockType)
{
    bool isValid = false;
    
    switch(clockType) {
        case SCG_CLOCK_SOSC:
            isValid = ((SCG->SOSCCSR & SCG_SOSCCSR_SOSCEN_MASK) != 0) && 
                      ((SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK) != 0);
            break;
            
        case SCG_CLOCK_SIRC:
            isValid = ((SCG->SIRCCSR & SCG_SIRCCSR_SIRCEN_MASK) != 0) && 
                      ((SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK) != 0);
            break;
            
        case SCG_CLOCK_FIRC:
            isValid = ((SCG->FIRCCSR & SCG_FIRCCSR_FIRCEN_MASK) != 0) && 
                      ((SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK) != 0);
            break;
            
        case SCG_CLOCK_SPLL:
            isValid = ((SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK) != 0) && 
                      ((SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK) != 0);
            break;
            
        default:
            isValid = false;
            break;
    }
    
    return isValid;
}

/**
 * @brief Get the current system clock source.
 *
 * This function returns the current system clock source as configured in CSR.
 *
 * @return Current system clock source (scg_systems_source_t).
 */
scg_systems_source_t SCG_GetSystemClockSource(void)
{
    return (scg_systems_source_t)((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT);
}

/**
 * @brief Get complete clock status.
 *
 * This function fills a clock status structure with information about
 * the specified clock.
 *
 * @param clockType The type of clock to query (scg_clock_type_t).
 * @param status Pointer to scg_clock_status_t structure to fill.
 */
void SCG_GetClockStatus(scg_clock_type_t clockType, scg_clock_status_t *status)
{
    if (status == NULL) {
        return;
    }
    
    status->valid = SCG_IsClockValid(clockType);
    status->frequency = SCG_GetClockFreq(clockType);
    
    /* Check if clock is enabled */
    switch(clockType) {
        case SCG_CLOCK_SOSC:
            status->enabled = (SCG->SOSCCSR & SCG_SOSCCSR_SOSCEN_MASK) != 0;
            break;
            
        case SCG_CLOCK_SIRC:
            status->enabled = (SCG->SIRCCSR & SCG_SIRCCSR_SIRCEN_MASK) != 0;
            break;
            
        case SCG_CLOCK_FIRC:
            status->enabled = (SCG->FIRCCSR & SCG_FIRCCSR_FIRCEN_MASK) != 0;
            break;
            
        case SCG_CLOCK_SPLL:
            status->enabled = (SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK) != 0;
            break;
            
        default:
            status->enabled = false;
            break;
    }
}

/**
 * @brief Get current clock configuration from CSR register.
 *
 * This function reads the current clock dividers from the Clock Status Register (CSR).
 *
 * @param divcore Pointer to store core clock divider.
 * @param divbus Pointer to store bus clock divider.
 * @param divlow Pointer to store slow clock divider.
 */
void SCG_GetCurrentClockConfig(scg_divcore_t *divcore, scg_divbus_t *divbus, scg_divlow_t *divlow)
{
    if (divcore != NULL) {
        *divcore = (scg_divcore_t)((SCG->CSR & SCG_CSR_DIVCORE_MASK) >> SCG_CSR_DIVCORE_SHIFT);
    }
    
    if (divbus != NULL) {
        *divbus = (scg_divbus_t)((SCG->CSR & SCG_CSR_DIVBUS_MASK) >> SCG_CSR_DIVBUS_SHIFT);
    }
    
    if (divlow != NULL) {
        *divlow = (scg_divlow_t)((SCG->CSR & SCG_CSR_DIVSLOW_MASK) >> SCG_CSR_DIVSLOW_SHIFT);
    }
}


