/**
 * @file    lpit.c
 * @brief   LPIT (Low Power Interrupt Timer) Driver Implementation for S32K144
 * @details
 * LPIT driver implementation allows:
 * - Create periodic interrupts with high precision
 * - Timing and delay functions
 * - Support 4 independent channels
 * - Chain mode to create 64-bit timer
 * - Operate in low-power modes
 * 
 * LPIT Operation:
 * - Timer counts down from TVAL to 0
 * - When counting to 0, interrupt flag is set and timer reloads TVAL
 * - Can chain 2 channels to create 64-bit timer
 * 
 * @author  PhucPH32
 * @date    28/11/2025
 * @version 1.0
 * 
 * @note    Clock source must be configured before initialization
 * @warning Do not change TVAL while timer is active
 * 
 * @par Change Log:
 * - Version 1.0 (28/11/2025): Initialize LPIT driver
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "lpit.h"
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Clock frequency tracking */
static uint32_t s_lpitClockFreq = 8000000U;  /* Default: 8 MHz (SIRC) */

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Callback function pointers for each channel */
static lpit_callback_t s_lpitCallbacks[LPIT_MAX_CHANNELS] = {NULL};

/** @brief User data pointers for each channel */
static void *s_lpitUserData[LPIT_MAX_CHANNELS] = {NULL};

/** @brief Flag indicating LPIT has been initialized */
static bool s_lpitInitialized = false;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Validate channel number
 * @param channel LPIT channel number
 * @return true if channel is valid (0-3)
 */
static inline bool LPIT_IsValidChannel(uint8_t channel);

/**
 * @brief Enable clock for LPIT module
 * @param clockSource Clock source selection
 * @return STATUS_SUCCESS if successful
 */
static status_t LPIT_EnableClock(lpit_clock_source_t clockSource);

/**
 * @brief Get clock frequency by clock source
 * @param clockSource Clock source
 * @return Clock frequency in Hz
 */
static uint32_t LPIT_GetClockFrequency(lpit_clock_source_t clockSource);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Check if channel is valid
 */
static inline bool LPIT_IsValidChannel(uint8_t channel)
{
    return (channel < LPIT_MAX_CHANNELS);
}

/**
 * @brief Enable clock for LPIT
 */
static status_t LPIT_EnableClock(lpit_clock_source_t clockSource)
{

    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_PCS(clockSource) | PCC_PCCn_CGC_MASK;
    /* Save clock frequency */
    s_lpitClockFreq = LPIT_GetClockFrequency(clockSource);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get clock frequency
 */
static uint32_t LPIT_GetClockFrequency(lpit_clock_source_t clockSource)
{
    uint32_t freq = 8000000U;  /* Default: 8 MHz */
    
    switch (clockSource) {
        case LPIT_CLK_SRC_SOSC:
            freq = 8000000U;   /* System Oscillator: 8 MHz */
            break;
        case LPIT_CLK_SRC_SIRC:
            freq = 8000000U;   /* Slow IRC: 8 MHz */
            break;
        case LPIT_CLK_SRC_FIRC:
            freq = 48000000U;  /* Fast IRC: 48 MHz */
            break;
        case LPIT_CLK_SRC_SPLL:
            freq = 80000000U;  /* System PLL: 80 MHz (typical) */
            break;
        default:
            freq = 8000000U;
            break;
    }
    
    return freq;
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Khá»Ÿi táº¡o LPIT module
 */
status_t LPIT_Init(lpit_clock_source_t clockSource)
{
    uint8_t i;
    
    /* Enable clock for LPIT */
    LPIT_EnableClock(clockSource);
    
    /* Disable module clock to configure */
    LPIT0->MCR &= ~LPIT_MCR_M_CEN_MASK;
    
    /* Software reset */
    LPIT0->MCR |= LPIT_MCR_SW_RST_MASK;
    LPIT0->MCR &= ~LPIT_MCR_SW_RST_MASK;
    
    /* Enable Debug mode - LPIT operates when CPU is in debug mode */
    LPIT0->MCR |= LPIT_MCR_DBG_EN_MASK;
    
    /* Disable Doze mode - LPIT stops in Doze mode to save power */
    LPIT0->MCR &= ~LPIT_MCR_DOZE_EN_MASK;
    
    /* Clear táº¥t cáº£ interrupt flags */
    LPIT0->MSR = 0x0FU;  /* Write 1 to clear */
    
    /* Disable táº¥t cáº£ interrupts */
    LPIT0->MIER = 0x00U;
    
    /* Disable vÃ  reset táº¥t cáº£ cÃ¡c kÃªnh */
    for (i = 0U; i < LPIT_MAX_CHANNELS; i++) {
        LPIT0->CHANNEL[i].TCTRL = 0U;
        LPIT0->CHANNEL[i].TVAL = 0U;
        s_lpitCallbacks[i] = NULL;
        s_lpitUserData[i] = NULL;
    }
    
    /* Enable module clock */
    LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;
    
    s_lpitInitialized = true;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Deinitialize LPIT module
 */
status_t LPIT_Deinit(void)
{
    uint8_t i;
    
    if (!s_lpitInitialized) {
        return STATUS_ERROR;
    }
    
    /* Disable all channels */
    for (i = 0U; i < LPIT_MAX_CHANNELS; i++) {
        LPIT_StopChannel(i);
        LPIT0->CHANNEL[i].TCTRL = 0U;
        LPIT0->CHANNEL[i].TVAL = 0U;
    }
    
    /* Disable module clock */
    LPIT0->MCR &= ~LPIT_MCR_M_CEN_MASK;
    
    /* Disable clock gating */

    PCC->PCCn[PCC_LPIT_INDEX] &= ~PCC_PCCn_CGC_MASK;
    
    s_lpitInitialized = false;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Configure one LPIT channel
 */
status_t LPIT_ConfigChannel(const lpit_channel_config_t *config)
{
    uint32_t tctrl;
    uint8_t channel;
    
    /* Validate parameters */
    if (config == NULL) {
        return STATUS_ERROR;
    }
    
    channel = config->channel;
    
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (!s_lpitInitialized) {
        return STATUS_ERROR;
    }
    
    /* Disable channel before configuring */
    LPIT_StopChannel(channel);
    
    /* Set timer value (period) */
    LPIT0->CHANNEL[channel].TVAL = config->period;
    
    /* Configure Timer Control Register */
    tctrl = 0U;
    
    /* Set timer mode */
    tctrl |= ((uint32_t)config->mode << LPIT_TCTRL_MODE_SHIFT) & LPIT_TCTRL_MODE_MASK;
    
    /* Chain channel if needed */
    if (config->chainChannel) {
        tctrl |= LPIT_TCTRL_CHAIN_MASK;
    }
    
    /* Start on trigger */
    if (config->startOnTrigger) {
        tctrl |= LPIT_TCTRL_TSOT_MASK;
    }
    
    /* Stop on interrupt */
    if (config->stopOnInterrupt) {
        tctrl |= LPIT_TCTRL_TSOI_MASK;
    }
    
    /* Reload on trigger */
    if (config->reloadOnTrigger) {
        tctrl |= LPIT_TCTRL_TROT_MASK;
    }
    
    /* Write to TCTRL register */
    LPIT0->CHANNEL[channel].TCTRL = tctrl;
    
    /* Enable/disable interrupt */
    if (config->enableInterrupt) {
        LPIT_EnableInterrupt(channel);
    } else {
        LPIT_DisableInterrupt(channel);
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Start one timer channel
 */
status_t LPIT_StartChannel(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (!s_lpitInitialized) {
        return STATUS_ERROR;
    }
    
    /* Clear interrupt flag if any */
    LPIT_ClearInterruptFlag(channel);
    
    /* Enable timer */
    LPIT0->CHANNEL[channel].TCTRL |= LPIT_TCTRL_T_EN_MASK;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Stop má»™t kÃªnh timer
 */
status_t LPIT_StopChannel(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Disable timer */
    LPIT0->CHANNEL[channel].TCTRL &= ~LPIT_TCTRL_T_EN_MASK;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Set period for one channel
 */
status_t LPIT_SetPeriod(uint8_t channel, uint32_t period)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Set timer value */
    LPIT0->CHANNEL[channel].TVAL = period;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get current value of timer
 */
status_t LPIT_GetCurrentValue(uint8_t channel, uint32_t *value)
{
    if (!LPIT_IsValidChannel(channel) || (value == NULL)) {
        return STATUS_ERROR;
    }
    
    /* Read current timer value */
    *value = LPIT0->CHANNEL[channel].CVAL;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Check interrupt flag of channel
 */
bool LPIT_GetInterruptFlag(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return false;
    }
    
    return ((LPIT0->MSR & (1U << channel)) != 0U);
}

/**
 * @brief Clear interrupt flag of channel
 */
status_t LPIT_ClearInterruptFlag(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Write 1 to clear */
    LPIT0->MSR = (1U << channel);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Enable interrupt for channel
 */
status_t LPIT_EnableInterrupt(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Set interrupt enable bit */
    LPIT0->MIER |= (1U << channel);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Disable interrupt for channel
 */
status_t LPIT_DisableInterrupt(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Clear interrupt enable bit */
    LPIT0->MIER &= ~(1U << channel);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Register callback function for channel
 */
status_t LPIT_InstallCallback(uint8_t channel, lpit_callback_t callback, void *userData)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (callback == NULL) {
        return STATUS_ERROR;
    }
    
    s_lpitCallbacks[channel] = callback;
    s_lpitUserData[channel] = userData;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Handle LPIT interrupt (called from ISR)
 */
void LPIT_IRQHandler(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return;
    }
    
    /* Clear interrupt flag */
    LPIT_ClearInterruptFlag(channel);
    
    /* Call callback if available */
    if (s_lpitCallbacks[channel] != NULL) {
        s_lpitCallbacks[channel](channel, s_lpitUserData[channel]);
    }
}

/**
 * @brief Táº¡o delay báº±ng LPIT (blocking)
 */
status_t LPIT_DelayUs(uint8_t channel, uint32_t delayUs)
{
    uint32_t ticks;
    uint32_t timeout = 0xFFFFFFFFU;
    lpit_channel_config_t config;
    
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (!s_lpitInitialized) {
        return STATUS_ERROR;
    }
    
    if (delayUs == 0U) {
        return STATUS_SUCCESS;
    }
    
    /* Calculate required ticks */
    /* ticks = (delayUs * clockFreq) / 1000000 */
    ticks = (delayUs * (s_lpitClockFreq / 1000000U));
    
    if (ticks == 0U) {
        ticks = 1U;
    }
    
    /* Configure timer for one-shot mode */
    config.channel = channel;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = ticks - 1U;
    config.enableInterrupt = false;  /* Polling mode */
    config.chainChannel = false;
    config.startOnTrigger = false;
    config.stopOnInterrupt = true;   /* Stop after one timeout */
    config.reloadOnTrigger = false;
    
    LPIT_ConfigChannel(&config);
    
    /* Clear flag before start */
    LPIT_ClearInterruptFlag(channel);
    
    /* Start timer */
    LPIT_StartChannel(channel);
    
    /* Wait until timer expires (polling) */
    while (!LPIT_GetInterruptFlag(channel) && (timeout > 0U)) {
        timeout--;
    }
    
    /* Clear flag */
    LPIT_ClearInterruptFlag(channel);
    
    if (timeout == 0U) {
        return STATUS_TIMEOUT;
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Calculate period value from clock frequency and desired frequency
 */
uint32_t LPIT_CalculatePeriod(uint32_t clockFreq, uint32_t desiredFreq)
{
    uint32_t period;
    
    if (desiredFreq == 0U) {
        return 0U;
    }
    
    /* Period = (clockFreq / desiredFreq) - 1 */
    period = (clockFreq / desiredFreq);
    
    if (period > 0U) {
        period -= 1U;
    }
    
    return period;
}
