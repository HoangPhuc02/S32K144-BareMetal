/**
 * @file    systick.c
 * @brief   SysTick Timer Driver Implementation for S32K144
 * @details This file contains the implementation of SysTick timer driver functions
 *          for ARM Cortex-M4 core.
 * 
 * @author  PhucPH32
 * @date    25/11/2025
 * @version 1.0
 * 
 * @note    This implementation provides SysTick timer control and delay functions
 * @warning SysTick is a 24-bit down counter with maximum reload value of 0xFFFFFF
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 25, 2025): Initial version
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "systick.h"
#include "systick_reg.h"
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Maximum reload value for 24-bit counter */
#define SYSTICK_MAX_RELOAD      (0xFFFFFFU)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Callback function pointer */
static systick_callback_t s_systickCallback = NULL;

/** @brief Tick counter for millisecond tracking */
static volatile uint32_t s_tickCounter = 0U;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Validate reload value
 * @param[in] value Reload value to validate
 * @return true if valid, false otherwise
 */
static bool SYSTICK_ValidateReloadValue(uint32_t value);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static bool SYSTICK_ValidateReloadValue(uint32_t value)
{
    return ((value > 0U) && (value <= SYSTICK_MAX_RELOAD));
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

bool SYSTICK_Init(const systick_config_t *config)
{
    if (config == NULL) {
        return false;
    }
    
    /* Validate reload value */
    if (!SYSTICK_ValidateReloadValue(config->reloadValue)) {
        return false;
    }
    
    /* Disable SysTick during configuration */
    SYSTICK_DISABLE();
    
    /* Set reload value */
    SYSTICK->RVR = SYSTICK_RVR_RELOAD(config->reloadValue);
    
    /* Clear current value */
    SYSTICK->CVR = 0U;
    
    /* Configure clock source and interrupt */
    uint32_t csr = 0U;
    
    if (config->clockSource == SYSTICK_CLK_PROCESSOR) {
        csr |= SYSTICK_CSR_CLKSOURCE_MASK;
    }
    
    if (config->enableInterrupt) {
        csr |= SYSTICK_CSR_TICKINT_MASK;
    }
    
    SYSTICK->CSR = csr;
    
    /* Reset tick counter */
    s_tickCounter = 0U;
    
    return true;
}

void SYSTICK_Start(void)
{
    SYSTICK_ENABLE();
}

void SYSTICK_Stop(void)
{
    SYSTICK_DISABLE();
}

void SYSTICK_Reset(void)
{
    /* Writing any value to CVR clears it to 0 and clears COUNTFLAG */
    SYSTICK->CVR = 0U;
}

uint32_t SYSTICK_GetCurrentValue(void)
{
    return (SYSTICK->CVR & SYSTICK_CVR_CURRENT_MASK);
}

bool SYSTICK_HasCountedToZero(void)
{
    /* Reading CSR clears COUNTFLAG */
    return ((SYSTICK->CSR & SYSTICK_CSR_COUNTFLAG_MASK) != 0U);
}

bool SYSTICK_ConfigMillisecond(uint32_t systemClockHz, bool enableInterrupt)
{
    /* Calculate reload value for 1ms tick
     * reload = (clock_freq / 1000) - 1
     */
    uint32_t reloadValue;
    
    if (systemClockHz < 1000U) {
        return false;  /* Clock too slow for 1ms ticks */
    }
    
    reloadValue = (systemClockHz / 1000U) - 1U;
    
    /* Check if reload value fits in 24 bits */
    if (reloadValue > SYSTICK_MAX_RELOAD) {
        return false;
    }
    
    systick_config_t config = {
        .reloadValue = reloadValue,
        .clockSource = SYSTICK_CLK_PROCESSOR,
        .enableInterrupt = enableInterrupt
    };
    
    return SYSTICK_Init(&config);
}

bool SYSTICK_ConfigMicrosecond(uint32_t systemClockHz, uint32_t microseconds, bool enableInterrupt)
{
    /* Calculate reload value for microsecond tick
     * reload = (clock_freq * microseconds / 1000000) - 1
     */
    uint64_t temp = ((uint64_t)systemClockHz * (uint64_t)microseconds) / 1000000ULL;
    
    if ((temp == 0ULL) || (temp > (uint64_t)SYSTICK_MAX_RELOAD)) {
        return false;
    }
    
    uint32_t reloadValue = (uint32_t)temp - 1U;
    
    systick_config_t config = {
        .reloadValue = reloadValue,
        .clockSource = SYSTICK_CLK_PROCESSOR,
        .enableInterrupt = enableInterrupt
    };
    
    return SYSTICK_Init(&config);
}

void SYSTICK_DelayMs(uint32_t milliseconds)
{
    uint32_t startTick = s_tickCounter;
    uint32_t targetTick = startTick + milliseconds;
    
    /* Handle counter overflow */
    if (targetTick < startTick) {
        /* Wait for overflow */
        while (s_tickCounter >= startTick) {
            __asm("NOP");
        }
    }
    
    /* Wait for target tick */
    while (s_tickCounter < targetTick) {
        __asm("NOP");
    }
}

void SYSTICK_DelayUs(uint32_t microseconds, uint32_t systemClockHz)
{
    /* Calculate number of clock cycles for delay */
    uint64_t cycles = ((uint64_t)systemClockHz * (uint64_t)microseconds) / 1000000ULL;
    
    if (cycles == 0ULL) {
        return;
    }
    
    /* If cycles fit in one SysTick period, use it directly */
    if (cycles <= (uint64_t)SYSTICK_MAX_RELOAD) {
        /* Save current configuration */
        bool wasEnabled = SYSTICK_IsEnabled();
        uint32_t oldReload = SYSTICK_GetReloadValue();
        bool oldIntEnabled = SYSTICK_IsInterruptEnabled();
        
        /* Configure for delay */
        SYSTICK_Stop();
        SYSTICK->RVR = (uint32_t)cycles - 1U;
        SYSTICK->CVR = 0U;
        SYSTICK->CSR = SYSTICK_CSR_CLKSOURCE_MASK;  /* Processor clock, no interrupt */
        SYSTICK_Start();
        
        /* Wait for COUNTFLAG */
        while (!SYSTICK_HasCountedToZero()) {
            __asm("NOP");
        }
        
        /* Restore configuration */
        SYSTICK_Stop();
        SYSTICK->RVR = oldReload;
        SYSTICK->CVR = 0U;
        
        uint32_t csr = SYSTICK_CSR_CLKSOURCE_MASK;
        if (oldIntEnabled) {
            csr |= SYSTICK_CSR_TICKINT_MASK;
        }
        SYSTICK->CSR = csr;
        
        if (wasEnabled) {
            SYSTICK_Start();
        }
    } else {
        /* For longer delays, use multiple periods */
        uint32_t periods = (uint32_t)(cycles / (uint64_t)SYSTICK_MAX_RELOAD);
        uint32_t remainder = (uint32_t)(cycles % (uint64_t)SYSTICK_MAX_RELOAD);
        
        /* Save current configuration */
        bool wasEnabled = SYSTICK_IsEnabled();
        uint32_t oldReload = SYSTICK_GetReloadValue();
        bool oldIntEnabled = SYSTICK_IsInterruptEnabled();
        
        SYSTICK_Stop();
        SYSTICK->RVR = SYSTICK_MAX_RELOAD;
        SYSTICK->CVR = 0U;
        SYSTICK->CSR = SYSTICK_CSR_CLKSOURCE_MASK;
        SYSTICK_Start();
        
        /* Wait for full periods */
        for (uint32_t i = 0; i < periods; i++) {
            while (!SYSTICK_HasCountedToZero()) {
                __asm("NOP");
            }
        }
        
        /* Wait for remainder if any */
        if (remainder > 0U) {
            SYSTICK_Stop();
            SYSTICK->RVR = remainder - 1U;
            SYSTICK->CVR = 0U;
            SYSTICK_Start();
            
            while (!SYSTICK_HasCountedToZero()) {
                __asm("NOP");
            }
        }
        
        /* Restore configuration */
        SYSTICK_Stop();
        SYSTICK->RVR = oldReload;
        SYSTICK->CVR = 0U;
        
        uint32_t csr = SYSTICK_CSR_CLKSOURCE_MASK;
        if (oldIntEnabled) {
            csr |= SYSTICK_CSR_TICKINT_MASK;
        }
        SYSTICK->CSR = csr;
        
        if (wasEnabled) {
            SYSTICK_Start();
        }
    }
}

void SYSTICK_RegisterCallback(systick_callback_t callback)
{
    s_systickCallback = callback;
}

void SYSTICK_UnregisterCallback(void)
{
    s_systickCallback = NULL;
}

void SYSTICK_IRQHandler(void)
{
    /* Increment tick counter */
    s_tickCounter++;
    
    /* Call user callback if registered */
    if (s_systickCallback != NULL) {
        s_systickCallback();
    }
}

void SYSTICK_EnableInterrupt(void)
{
    SYSTICK_ENABLE_INT();
}

void SYSTICK_DisableInterrupt(void)
{
    SYSTICK_DISABLE_INT();
}

bool SYSTICK_SetReloadValue(uint32_t value)
{
    if (!SYSTICK_ValidateReloadValue(value)) {
        return false;
    }
    
    SYSTICK->RVR = SYSTICK_RVR_RELOAD(value);
    return true;
}

uint32_t SYSTICK_GetReloadValue(void)
{
    return (SYSTICK->RVR & SYSTICK_RVR_RELOAD_MASK);
}

void SYSTICK_SetClockSource(systick_clock_source_t source)
{
    if (source == SYSTICK_CLK_PROCESSOR) {
        SYSTICK_SET_CLK_PROCESSOR();
    } else {
        SYSTICK_SET_CLK_EXTERNAL();
    }
}

uint32_t SYSTICK_GetTicks(void)
{
    return s_tickCounter;
}

void SYSTICK_ResetTicks(void)
{
    s_tickCounter = 0U;
}
