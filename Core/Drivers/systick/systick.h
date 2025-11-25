/**
 * @file    systick.h
 * @brief   SysTick Timer Driver Header File for S32K144
 * @details This file contains the SysTick timer driver interface declarations,
 *          macros, and data structures for ARM Cortex-M4 core.
 * 
 * @author  PhucPH32
 * @date    25/11/2025
 * @version 1.0
 * 
 * @note This driver provides SysTick timer configuration and delay functions
 * @warning Ensure proper system clock configuration before using SysTick
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 25, 2025): Initial version
 * 
 * @copyright Copyright (c) 2025
 * Licensed under [Your License]
 */

#ifndef SYSTICK_H
#define SYSTICK_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "systick_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup SysTick_Definitions SysTick Definitions
 * @{
 */

/** @brief SysTick clock source enumeration */
typedef enum {
    SYSTICK_CLK_EXTERNAL = 0U,    /**< External reference clock */
    SYSTICK_CLK_PROCESSOR = 1U    /**< Processor clock */
} systick_clock_source_t;

/** @brief SysTick configuration structure */
typedef struct {
    uint32_t reloadValue;                    /**< Reload value (24-bit, 0x1 to 0xFFFFFF) */
    systick_clock_source_t clockSource;      /**< Clock source selection */
    bool enableInterrupt;                    /**< Enable/disable SysTick interrupt */
} systick_config_t;

/** @brief SysTick callback function type */
typedef void (*systick_callback_t)(void);

/** @} */ /* End of SysTick_Definitions */

/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @defgroup SysTick_Functions SysTick Driver Functions
 * @{
 */

/**
 * @brief Initialize SysTick timer with specified configuration
 * @param[in] config Pointer to SysTick configuration structure
 * @return true if successful, false if failed
 * 
 * @note This function configures but does not start the SysTick timer
 * @warning Reload value must be between 0x1 and 0xFFFFFF
 * 
 * Example usage:
 * @code
 * systick_config_t config = {
 *     .reloadValue = 48000U - 1U,  // 1ms at 48MHz
 *     .clockSource = SYSTICK_CLK_PROCESSOR,
 *     .enableInterrupt = true
 * };
 * SYSTICK_Init(&config);
 * @endcode
 */
bool SYSTICK_Init(const systick_config_t *config);

/**
 * @brief Start SysTick timer
 * 
 * @note Timer must be initialized before calling this function
 */
void SYSTICK_Start(void);

/**
 * @brief Stop SysTick timer
 */
void SYSTICK_Stop(void);

/**
 * @brief Reset SysTick current value to reload value
 * 
 * @note This clears the current value register and COUNTFLAG
 */
void SYSTICK_Reset(void);

/**
 * @brief Get current SysTick counter value
 * @return Current counter value (24-bit)
 */
uint32_t SYSTICK_GetCurrentValue(void);

/**
 * @brief Check if SysTick counter has counted to 0
 * @return true if counted to 0, false otherwise
 * 
 * @note Reading this flag clears it automatically
 */
bool SYSTICK_HasCountedToZero(void);

/**
 * @brief Configure SysTick for millisecond delays
 * @param[in] systemClockHz System clock frequency in Hz
 * @param[in] enableInterrupt Enable/disable interrupt
 * @return true if successful, false if failed
 * 
 * @note Configures SysTick to generate 1ms ticks
 * @warning System clock must be known and passed correctly
 * 
 * Example usage:
 * @code
 * SYSTICK_ConfigMillisecond(48000000U, true);  // 48MHz system clock
 * SYSTICK_Start();
 * @endcode
 */
bool SYSTICK_ConfigMillisecond(uint32_t systemClockHz, bool enableInterrupt);

/**
 * @brief Configure SysTick for microsecond delays
 * @param[in] systemClockHz System clock frequency in Hz
 * @param[in] microseconds Desired tick period in microseconds
 * @param[in] enableInterrupt Enable/disable interrupt
 * @return true if successful, false if failed
 * 
 * @note Limited by 24-bit reload value and clock frequency
 */
bool SYSTICK_ConfigMicrosecond(uint32_t systemClockHz, uint32_t microseconds, bool enableInterrupt);

/**
 * @brief Blocking delay in milliseconds
 * @param[in] milliseconds Number of milliseconds to delay
 * 
 * @note SysTick must be configured with 1ms tick before using
 * @warning This is a blocking delay
 * 
 * Example usage:
 * @code
 * SYSTICK_ConfigMillisecond(48000000U, false);
 * SYSTICK_Start();
 * SYSTICK_DelayMs(1000U);  // Delay 1 second
 * @endcode
 */
void SYSTICK_DelayMs(uint32_t milliseconds);

/**
 * @brief Blocking delay in microseconds
 * @param[in] microseconds Number of microseconds to delay
 * @param[in] systemClockHz System clock frequency in Hz
 * 
 * @note This function temporarily reconfigures SysTick
 * @warning Not suitable for precise short delays (< 10us)
 */
void SYSTICK_DelayUs(uint32_t microseconds, uint32_t systemClockHz);

/**
 * @brief Register callback function for SysTick interrupt
 * @param[in] callback Pointer to callback function
 * 
 * @note Callback is called from SysTick_Handler ISR
 * @warning Keep callback function short and efficient
 * 
 * Example usage:
 * @code
 * void MyCallback(void) {
 *     // Toggle LED or increment counter
 * }
 * 
 * SYSTICK_RegisterCallback(MyCallback);
 * @endcode
 */
void SYSTICK_RegisterCallback(systick_callback_t callback);

/**
 * @brief Unregister SysTick callback function
 */
void SYSTICK_UnregisterCallback(void);

/**
 * @brief SysTick interrupt handler
 * 
 * @note This function should be called from SysTick_Handler in vector table
 * @note Automatically calls registered callback if available
 */
void SYSTICK_IRQHandler(void);

/**
 * @brief Enable SysTick interrupt
 */
void SYSTICK_EnableInterrupt(void);

/**
 * @brief Disable SysTick interrupt
 */
void SYSTICK_DisableInterrupt(void);

/**
 * @brief Set SysTick reload value
 * @param[in] value Reload value (1 to 0xFFFFFF)
 * @return true if successful, false if invalid value
 */
bool SYSTICK_SetReloadValue(uint32_t value);

/**
 * @brief Get SysTick reload value
 * @return Current reload value
 */
uint32_t SYSTICK_GetReloadValue(void);

/**
 * @brief Set SysTick clock source
 * @param[in] source Clock source selection
 */
void SYSTICK_SetClockSource(systick_clock_source_t source);

/**
 * @brief Get elapsed ticks since start
 * @return Number of ticks elapsed
 * 
 * @note Requires interrupt to be enabled for accurate counting
 * @note Counter wraps around at maximum uint32_t value
 */
uint32_t SYSTICK_GetTicks(void);

/**
 * @brief Reset tick counter
 */
void SYSTICK_ResetTicks(void);

/** @} */ /* End of SysTick_Functions */

/*******************************************************************************
 * Inline Functions
 ******************************************************************************/

/**
 * @brief Check if SysTick is enabled
 * @return true if enabled, false otherwise
 */
static inline bool SYSTICK_IsEnabled(void)
{
    return ((SYSTICK->CSR & SYSTICK_CSR_ENABLE_MASK) != 0U);
}

/**
 * @brief Check if SysTick interrupt is enabled
 * @return true if enabled, false otherwise
 */
static inline bool SYSTICK_IsInterruptEnabled(void)
{
    return ((SYSTICK->CSR & SYSTICK_CSR_TICKINT_MASK) != 0U);
}

/**
 * @brief Get SysTick clock source
 * @return Current clock source
 */
static inline systick_clock_source_t SYSTICK_GetClockSource(void)
{
    return (systick_clock_source_t)((SYSTICK->CSR & SYSTICK_CSR_CLKSOURCE_MASK) >> SYSTICK_CSR_CLKSOURCE_SHIFT);
}

#endif /* SYSTICK_H */
