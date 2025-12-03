/**
 * @file board1_adc_task.h
 * @brief Board 1 ADC Sampling Task (Header)
 * @details Manages periodic ADC sampling with LPIT timer
 * 
 * Responsibilities:
 * - Initialize ADC peripheral
 * - Configure LPIT timer for 1Hz sampling
 * - Read ADC values on timer interrupt
 * - Send ADC data via CAN bus
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#ifndef BOARD1_ADC_TASK_H
#define BOARD1_ADC_TASK_H

#include "app_types.h"

/*******************************************************************************
 * API FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize ADC task
 * 
 * Initializes:
 * - ADC peripheral with calibration
 * - LPIT timer (stopped state)
 * - Internal state
 * 
 * @return APP_STATUS_SUCCESS if successful
 * @return APP_STATUS_ERROR if initialization failed
 * 
 * @note Must be called before AdcTask_Start()
 */
app_status_t AdcTask_Init(void);

/**
 * @brief Start ADC sampling task
 * 
 * Actions:
 * - Starts LPIT timer (1 Hz)
 * - Enables ADC sampling
 * - Enables LPIT interrupt
 * 
 * @return APP_STATUS_SUCCESS if started
 * @return APP_STATUS_ALREADY_INITIALIZED if already running
 * @return APP_STATUS_NOT_INITIALIZED if not initialized
 * 
 * @note Sends ACK_START_SUCCESS via CAN after starting
 */
app_status_t AdcTask_Start(void);

/**
 * @brief Stop ADC sampling task
 * 
 * Actions:
 * - Stops LPIT timer
 * - Disables ADC sampling
 * - Disables LPIT interrupt
 * 
 * @return APP_STATUS_SUCCESS if stopped
 * @return APP_STATUS_NOT_INITIALIZED if not initialized
 * 
 * @note Sends ACK_STOP_SUCCESS via CAN after stopping
 */
app_status_t AdcTask_Stop(void);

/**
 * @brief Check if ADC task is running
 * 
 * @return true if sampling active
 * @return false if sampling stopped
 */
bool AdcTask_IsRunning(void);

/**
 * @brief Get last ADC sample
 * 
 * @param[out] sample Pointer to store sample data
 * 
 * @return APP_STATUS_SUCCESS if valid sample
 * @return APP_STATUS_ERROR if no valid sample
 * @return APP_STATUS_INVALID_PARAM if sample is NULL
 */
app_status_t AdcTask_GetLastSample(adc_sample_t *sample);

/**
 * @brief LPIT interrupt handler (called from LPIT0_Ch0_IRQHandler)
 * 
 * Actions:
 * 1. Read ADC value (blocking)
 * 2. Convert to voltage
 * 3. Pack into CAN frame
 * 4. Send via CAN bus
 * 5. Clear LPIT interrupt flag
 * 
 * @note This function is called every 1 second
 * @note Should execute quickly (<1ms)
 */
void AdcTask_LpitIrqHandler(void);

/**
 * @brief Process ADC task (called from main loop)
 * 
 * Handles:
 * - State management
 * - Error recovery
 * - Debug LED toggle
 * 
 * @note Should be called periodically from main()
 */
void AdcTask_Process(void);

#endif /* BOARD1_ADC_TASK_H */
