/**
 * @file    lpit.h
 * @brief   LPIT (Low Power Interrupt Timer) driver for S32K144
 * @details
 * LPIT driver provides the following APIs:
 * - Configure and control 4 independent timer channels
 * - Create periodic interrupts with high accuracy
 * - Support chain mode to form 64-bit timers
 * - Timing, blocking delay and basic PWM support
 * - Operation in low-power modes
 *
 * LPIT Features:
 * - 4 independent 32-bit timer channels
 * - Ability to chain channels into 64-bit timers
 * - Periodic counter mode
 * - Dual 16-bit periodic counter mode
 * - Operates during Doze and Debug modes
 *
 * @author  PhucPH32
 * @date    28/11/2025
 * @version 1.0
 *
 * @note
 * - The LPIT clock must be configured before use
 * - Timers count down from TVAL to 0
 * - Each channel can generate its own interrupt
 *
 * @warning
 * - Enable the LPIT clock before using the module
 * - Do not modify TVAL while the timer is running
 *
 * @par Change Log:
 * - Version 1.0 (28/11/2025): Initial LPIT driver
 */

#ifndef LPIT_H
#define LPIT_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "lpit_reg.h"
#include "pcc_reg.h"
#include "status.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup LPIT_Definitions LPIT Definitions
 * @{
 */

/** @brief Maximum number of LPIT channels */
#define LPIT_MAX_CHANNELS   (4U)

/**
 * @brief LPIT Timer Mode
 * @details Timer operating modes
 */
typedef enum {
    LPIT_MODE_32BIT_PERIODIC    = 0U,   /**< 32-bit Periodic Counter */
    LPIT_MODE_DUAL_16BIT        = 1U,   /**< Dual 16-bit Periodic Counter */
    LPIT_MODE_TRIGGER_ACCUM     = 2U,   /**< Trigger Accumulator */
    LPIT_MODE_INPUT_CAPTURE     = 3U    /**< Input Capture */
} lpit_mode_t;

/*******************************************************************************
 * Timer Mode Definitions
 ******************************************************************************/

/** @brief Clock source options for LPIT */
typedef enum {
    LPIT_CLK_SRC_NONE       = 0U,   /**< Clock disabled */
    LPIT_CLK_SRC_SOSC       = 1U,   /**< System Oscillator (8 MHz) */
    LPIT_CLK_SRC_SIRC       = 2U,   /**< Slow IRC (8 MHz) */
    LPIT_CLK_SRC_FIRC       = 3U,   /**< Fast IRC (48 MHz) */
    LPIT_CLK_SRC_SPLL       = 6U    /**< System PLL */
} lpit_clock_source_t;
/**
 * @brief LPIT Channel Configuration
 * @details Configuration structure for one LPIT channel
 */
typedef struct {
    uint8_t channel;                /**< LPIT channel number (0-3) */
    lpit_mode_t mode;               /**< Timer mode */
    uint32_t period;                /**< Period value (timer reload value) */
    bool enableInterrupt;           /**< Enable interrupt when timer expires */
    bool chainChannel;              /**< Chain with previous channel (to create 64-bit timer) */
    bool startOnTrigger;            /**< Start timer on trigger */
    bool stopOnInterrupt;           /**< Stop timer on interrupt */
    bool reloadOnTrigger;           /**< Reload timer on trigger */
} lpit_channel_config_t;

/**
 * @brief LPIT Callback Function Type
 * @details Function pointer for callback when timer expires
 * @param channel LPIT channel number
 * @param userData Pointer to user data
 */
typedef void (*lpit_callback_t)(uint8_t channel, void *userData);

/** @} */ /* End of LPIT_Definitions */

/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @defgroup LPIT_Functions LPIT Functions
 * @{
 */

/**
 * @brief Initialize LPIT module
 * @details
 * - Enable clock for LPIT
 * - Reset LPIT module
 * - Enable module clock
 * - Configure debug mode
 * 
 * @param[in] clockSource Clock source for LPIT (SIRC, FIRC, SOSC, SPLL)
 * 
 * @return STATUS_SUCCESS if successful
 * 
 * @note
 * - Clock source must be configured in PCC
 * - SIRC (8 MHz) is a good choice for low-power applications
 * - FIRC (48 MHz) for high accuracy timing
 * 
 * @code
 * // Initialize LPIT with SIRC (8 MHz)
 * LPIT_Init(LPIT_CLK_SRC_SIRC);
 * @endcode
 */
status_t LPIT_Init(lpit_clock_source_t clockSource);

/**
 * @brief Deinitialize LPIT module
 * @details
 * - Disable all channels
 * - Disable module clock
 * - Disable clock gating
 * 
 * @return STATUS_SUCCESS if successful
 */
status_t LPIT_Deinit(void);

/**
 * @brief Configure an LPIT channel
 * @details
 * Set up all parameters for a timer channel:
 * - Timer mode
 * - Period (reload value)
 * - Interrupt enable
 * - Chain mode
 * - Trigger options
 * 
 * @param[in] config Pointer to configuration structure
 * 
 * @return STATUS_SUCCESS if configuration successful
 * @return STATUS_ERROR if parameters are invalid
 * 
 * @note
 * - Timer must be stopped before configuration
 * - Period = 0 will not generate interrupt
 * 
 * @code
 * // Configure channel 0 for 1ms interrupt with SIRC 8MHz
 * lpit_channel_config_t config = {
 *     .channel = 0,
 *     .mode = LPIT_MODE_32BIT_PERIODIC,
 *     .period = 8000 - 1,              // 8MHz / 8000 = 1ms
 *     .enableInterrupt = true,
 *     .chainChannel = false,
 *     .startOnTrigger = false,
 *     .stopOnInterrupt = false,
 *     .reloadOnTrigger = false
 * };
 * LPIT_ConfigChannel(&config);
 * @endcode
 */
status_t LPIT_ConfigChannel(const lpit_channel_config_t *config);

/**
 * @brief Start a timer channel
 * @details Enable timer channel to start counting
 * 
 * @param[in] channel LPIT channel number (0-3)
 * 
 * @return STATUS_SUCCESS if start successful
 * @return STATUS_ERROR if channel invalid
 * 
 * @code
 * // Start channel 0
 * LPIT_StartChannel(0);
 * @endcode
 */
status_t LPIT_StartChannel(uint8_t channel);

/**
 * @brief Stop a timer channel
 * @details Disable timer channel to stop counting
 * 
 * @param[in] channel LPIT channel number (0-3)
 * 
 * @return STATUS_SUCCESS if stop successful
 * @return STATUS_ERROR if channel invalid
 */
status_t LPIT_StopChannel(uint8_t channel);

/**
 * @brief Set period for a channel
 * @details
 * Update timer reload value.
 * This value will be loaded into the counter when timer expires.
 * 
 * @param[in] channel LPIT channel number (0-3)
 * @param[in] period Period value (0 to 0xFFFFFFFF)
 * 
 * @return STATUS_SUCCESS if set successful
 * @return STATUS_ERROR if channel invalid
 * 
 * @note
 * - New period only takes effect after timer reload
 * - Should stop timer before changing period
 * 
 * @code
 * // Change period to 2ms (SIRC 8MHz)
 * LPIT_SetPeriod(0, 16000 - 1);
 * @endcode
 */
status_t LPIT_SetPeriod(uint8_t channel, uint32_t period);

/**
 * @brief Get current timer value
 * @details Read the current counting value of the timer
 * 
 * @param[in] channel LPIT channel number (0-3)
 * @param[out] value Pointer to store timer value
 * 
 * @return STATUS_SUCCESS if read successful
 * @return STATUS_ERROR if channel invalid or value NULL
 * 
 * @note Timer counts down from TVAL to 0
 * 
 * @code
 * uint32_t currentValue;
 * LPIT_GetCurrentValue(0, &currentValue);
 * printf("Current timer value: %lu\n", currentValue);
 * @endcode
 */
status_t LPIT_GetCurrentValue(uint8_t channel, uint32_t *value);

/**
 * @brief Check interrupt flag of channel
 * @details Check if timer has expired
 * 
 * @param[in] channel LPIT channel number (0-3)
 * 
 * @return true if interrupt flag is set
 * @return false if no interrupt or channel invalid
 */
bool LPIT_GetInterruptFlag(uint8_t channel);

/**
 * @brief Clear interrupt flag of channel
 * @details Clear interrupt flag after processing
 * 
 * @param[in] channel LPIT channel number (0-3)
 * 
 * @return STATUS_SUCCESS if clear successful
 * @return STATUS_ERROR if channel invalid
 * 
 * @note Must clear flag in ISR to avoid re-trigger
 */
status_t LPIT_ClearInterruptFlag(uint8_t channel);

/**
 * @brief Enable interrupt for channel
 * @details Enable interrupt when timer expires
 * 
 * @param[in] channel LPIT channel number (0-3)
 * 
 * @return STATUS_SUCCESS if enable successful
 * @return STATUS_ERROR if channel invalid
 */
status_t LPIT_EnableInterrupt(uint8_t channel);

/**
 * @brief Disable interrupt for channel
 * @details Disable interrupt when timer expires
 * 
 * @param[in] channel LPIT channel number (0-3)
 * 
 * @return STATUS_SUCCESS if disable successful
 * @return STATUS_ERROR if channel invalid
 */
status_t LPIT_DisableInterrupt(uint8_t channel);

/**
 * @brief Register callback function for channel
 * @details
 * Callback will be called from ISR when timer expires
 * 
 * @param[in] channel LPIT channel number (0-3)
 * @param[in] callback Function pointer for callback
 * @param[in] userData Pointer to user data (can be NULL)
 * 
 * @return STATUS_SUCCESS if registration successful
 * @return STATUS_ERROR if channel invalid or callback NULL
 * 
 * @code
 * void myTimerCallback(uint8_t channel, void *userData) {
 *     // Toggle LED or perform other action
 *     GPIO_TogglePin(LED_PORT, LED_PIN);
 * }
 * 
 * LPIT_InstallCallback(0, myTimerCallback, NULL);
 * @endcode
 */
status_t LPIT_InstallCallback(uint8_t channel, lpit_callback_t callback, void *userData);

/**
 * @brief Handle LPIT interrupt (called from ISR)
 * @details
 * This function clears interrupt flag and calls registered callback.
 * Must be called in LPIT ISR.
 * 
 * @param[in] channel LPIT channel number (0-3)
 * 
 * @note This function automatically clears interrupt flag
 * 
 * @code
 * void LPIT0_Ch0_IRQHandler(void) {
 *     LPIT_IRQHandler(0);
 * }
 * @endcode
 */
void LPIT_IRQHandler(uint8_t channel);

/**
 * @brief Create delay using LPIT (blocking)
 * @details
 * Use LPIT to create accurate delay.
 * This function is blocking (waits until delay ends).
 * 
 * @param[in] channel LPIT channel number to use (0-3)
 * @param[in] delayUs Delay time in microseconds
 * 
 * @return STATUS_SUCCESS if delay successful
 * @return STATUS_ERROR if channel invalid
 * 
 * @note
 * - This function is blocking
 * - Should not be used in interrupt context
 * - Clock source must be known for accurate calculation
 * 
 * @code
 * // Delay 1000us (1ms)
 * LPIT_DelayUs(0, 1000);
 * @endcode
 */
status_t LPIT_DelayUs(uint8_t channel, uint32_t delayUs);

/**
 * @brief Calculate period value from frequency and desired frequency
 * @details
 * Helper function to calculate period value for timer
 * 
 * @param[in] clockFreq Clock frequency (Hz)
 * @param[in] desiredFreq Desired frequency (Hz)
 * 
 * @return Period value to load into TVAL register
 * 
 * @note Period = (clockFreq / desiredFreq) - 1
 * 
 * @code
 * // Calculate period for 1kHz with 8MHz clock
 * uint32_t period = LPIT_CalculatePeriod(8000000, 1000);
 * // period = 7999 (for 1ms period)
 * @endcode
 */
uint32_t LPIT_CalculatePeriod(uint32_t clockFreq, uint32_t desiredFreq);

/** @} */ /* End of LPIT_Functions */

#endif /* LPIT_H */
