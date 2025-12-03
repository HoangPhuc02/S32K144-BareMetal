/*
 * @file    lpit.c
 * @brief   LPIT Driver Implementation for S32K144
 */

#include "lpit.h"

/**
 * @brief Array of LPIT callback functions.
 *
 * This static array holds the registered callback functions for each LPIT channel (0�3).
 * Each element corresponds to one LPIT channel:
 *   - Index 0 - Channel 0
 *   - Index 1 - Channel 1
 *   - Index 2 - Channel 2
 *   - Index 3 - Channel 3
 *
 * @note If an element is NULL, no callback will be executed for that channel.
 */
static p_lpit_callback_t g_lpit_callbacks[4] = {NULL, NULL, NULL, NULL};

/**
 * @brief Configure and start an LPIT timer channel.
 *
 * This function enables the LPIT clock, configures the module, sets the timer value,
 * enables interrupts for the specified channel, and enables the NVIC interrupt.
 *
 * @param p_config Pointer to lpit_config_value_t structure containing:
 *        - source: Clock source for LPIT (via PCC).
 *        - channel: LPIT channel number (0�3).
 *        - value: Timer reload value (number of ticks).

 *
 * @note The function assumes that PCC and NVIC macros are correctly defined.
 *       Ensure that the clock source frequency is known when calculating `value`.
 */
void LPIT_ConfigValue(lpit_config_value_t *p_config)
{
	/* Step 1: Enable LPIT clock and set clock source */
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK | PCC_PCCn_PCS(p_config->source);

	/* Step 2: Enable LPIT module and allow debug mode */
	LPIT0->MCR |= LPIT_MCR_DBG_EN_MASK | LPIT_MCR_M_CEN_MASK;

	/* Step 3: Enable interrupt for the selected channel */
	SET_BIT(LPIT0->MIER, p_config->channel);

	/* Step 4: Set timer reload value (subtract 1 as per hardware requirement) */
	LPIT0->TMR[p_config->channel].TVAL = p_config->value;

	/* Step 5: Enable the selected channel */
	SET_BIT(LPIT0->SETTEN, p_config->channel);

	/* Step 6: Register a callback function */
	g_lpit_callbacks[p_config->channel] = p_config->func_callback;

	/* Step 7: Enable NVIC interrupt for the channel */
	uint32_t id_channel = ID_LPIT0_BASE + p_config->channel;
	SET_BIT(NVIC_ISER->ISERn[id_channel / 32], id_channel % 32);
}

/**
 * @brief LPIT Channel 0 Interrupt Handler.
 *
 * This ISR is executed when LPIT channel 0 timer expires.
 * It performs the following actions:
 *   - Calls the registered callback function for channel 0 (if not NULL).
 *   - Clears the interrupt flag for channel 0.
 */
void LPIT0_Ch0_IRQHandler(void)
{
    if (g_lpit_callbacks[0] != NULL)
    {
        g_lpit_callbacks[0]();	/**< Invoke callback for channel 0 */
    }
    LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /**< Clear interrupt flag */
}

/**
 * @brief LPIT Channel 1 Interrupt Handler.
 *
 * This ISR is executed when LPIT channel 1 timer expires.
 * It calls the registered callback and clears the interrupt flag.
 */
void LPIT0_Ch1_IRQHandler(void)
{
    if (g_lpit_callbacks[1] != NULL)
    {
        g_lpit_callbacks[1]();	/**< Invoke callback for channel 1 */
    }
    LPIT0->MSR |= LPIT_MSR_TIF1_MASK;  /**< Clear interrupt flag */
}

/**
 * @brief LPIT Channel 2 Interrupt Handler.
 *
 * Executes when LPIT channel 2 timer expires.
 * Calls the registered callback and clears the interrupt flag.
 */
void LPIT0_Ch2_IRQHandler(void)
{
    if (g_lpit_callbacks[2] != NULL)
    {
        g_lpit_callbacks[2]();	/**< Invoke callback for channel 2 */
    }
    LPIT0->MSR |= LPIT_MSR_TIF2_MASK; /**< Clear interrupt flag */
}

/**
 * @brief LPIT Channel 3 Interrupt Handler.
 *
 * Executes when LPIT channel 3 timer expires.
 * Calls the registered callback and clears the interrupt flag.
 */
void LPIT0_Ch3_IRQHandler(void)
{
    if (g_lpit_callbacks[3] != NULL)
    {
        g_lpit_callbacks[3]();	/**< Invoke callback for channel 3 */
    }
    LPIT0->MSR |= LPIT_MSR_TIF3_MASK;  /**< Clear interrupt flag */
}

/* ----------------------------------------------------------------------------
   -- Timer Control Functions (Priority 1)
   ---------------------------------------------------------------------------- */

/**
 * @brief Start the specified LPIT timer channel.
 *
 * This function enables the timer channel to start counting.
 *
 * @param channel LPIT channel to start (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 */
void LPIT_StartTimer(lpit_channel_t channel)
{
    /* Enable the timer channel by setting the corresponding bit in SETTEN */
    SET_BIT(LPIT0->SETTEN, channel);
}

/**
 * @brief Stop the specified LPIT timer channel.
 *
 * This function disables the timer channel to stop counting.
 *
 * @param channel LPIT channel to stop (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 */
void LPIT_StopTimer(lpit_channel_t channel)
{
    /* Disable the timer channel by setting the corresponding bit in CLRTEN */
    SET_BIT(LPIT0->CLRTEN, channel);
}

/**
 * @brief Reset the specified LPIT timer channel.
 *
 * This function stops the timer and reloads the initial timer value.
 *
 * @param channel LPIT channel to reset (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 */
void LPIT_ResetTimer(lpit_channel_t channel)
{
    /* Stop the timer */
    LPIT_StopTimer(channel);
    
    /* Clear interrupt flag if any */
    LPIT_ClearInterruptFlag(channel);
    
    /* Restart the timer - this will reload TVAL into CVAL */
    LPIT_StartTimer(channel);
}

/**
 * @brief Set a new timer value for the specified LPIT channel.
 *
 * This function updates the timer reload value. The new value will take effect
 * on the next timer reload (after current period completes).
 *
 * @param channel LPIT channel (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 * @param value New timer reload value in ticks.
 *
 * @note To apply immediately, stop and restart the timer after calling this function.
 */
void LPIT_SetTimerValue(lpit_channel_t channel, uint32_t value)
{
    /* Set new timer reload value */
    LPIT0->TMR[channel].TVAL = value;
}

/* ----------------------------------------------------------------------------
   -- Timer Status & Monitoring Functions (Priority 2)
   ---------------------------------------------------------------------------- */

/**
 * @brief Get the current timer value for the specified LPIT channel.
 *
 * This function reads the current count value of the timer.
 *
 * @param channel LPIT channel (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 *
 * @return Current timer count value.
 */
uint32_t LPIT_GetCurrentValue(lpit_channel_t channel)
{
    /* Read current timer value from CVAL register */
    return LPIT0->TMR[channel].CVAL;
}

/**
 * @brief Check if the specified LPIT timer channel is running.
 *
 * This function checks if the timer channel is currently enabled and counting.
 *
 * @param channel LPIT channel (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 *
 * @return true if timer is running, false otherwise.
 */
bool LPIT_IsTimerRunning(lpit_channel_t channel)
{
    /* Check if timer enable bit is set in TCTRL register */
    return (LPIT0->TMR[channel].TCTRL & LPIT_TCTRL_T_EN_MASK) != 0;
}

/**
 * @brief Check if interrupt is pending for the specified LPIT channel.
 *
 * This function checks if the timer interrupt flag is set.
 *
 * @param channel LPIT channel (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 *
 * @return true if interrupt is pending, false otherwise.
 */
bool LPIT_IsInterruptPending(lpit_channel_t channel)
{
    /* Check interrupt flag in MSR register */
    return READ_BIT(LPIT0->MSR, channel) != 0;
}

/**
 * @brief Get the elapsed time (number of ticks) since timer started.
 *
 * This function calculates how many ticks have elapsed from the reload value.
 *
 * @param channel LPIT channel (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 *
 * @return Number of ticks elapsed since last reload.
 *
 * @note LPIT is a down counter, so elapsed = TVAL - CVAL.
 */
uint32_t LPIT_GetTimeElapsed(lpit_channel_t channel)
{
    uint32_t tval = LPIT0->TMR[channel].TVAL;
    uint32_t cval = LPIT0->TMR[channel].CVAL;
    
    /* LPIT counts down from TVAL to 0 */
    return (tval - cval);
}

/* ----------------------------------------------------------------------------
   -- Interrupt Management Functions (Priority 3)
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable interrupt for the specified LPIT channel.
 *
 * This function enables the timer interrupt for the specified channel.
 *
 * @param channel LPIT channel (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 */
void LPIT_EnableInterrupt(lpit_channel_t channel)
{
    /* Enable interrupt in MIER register */
    SET_BIT(LPIT0->MIER, channel);
    
    /* Enable NVIC interrupt for the channel */
    uint32_t id_channel = ID_LPIT0_BASE + channel;
    SET_BIT(NVIC_ISER->ISERn[id_channel / 32], id_channel % 32);
}

/**
 * @brief Disable interrupt for the specified LPIT channel.
 *
 * This function disables the timer interrupt for the specified channel.
 *
 * @param channel LPIT channel (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 */
void LPIT_DisableInterrupt(lpit_channel_t channel)
{
    /* Disable interrupt in MIER register */
    CLEAR_BIT(LPIT0->MIER, channel);
}

/**
 * @brief Clear the interrupt flag for the specified LPIT channel.
 *
 * This function clears the interrupt pending flag by writing 1 to the MSR register.
 *
 * @param channel LPIT channel (LPIT_CHANNEL0 to LPIT_CHANNEL3).
 */
void LPIT_ClearInterruptFlag(lpit_channel_t channel)
{
    /* Clear interrupt flag by writing 1 to MSR */
    SET_BIT(LPIT0->MSR, channel);
}
