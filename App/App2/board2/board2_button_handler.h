/**
 * @file board2_button_handler.h
 * @brief Board 2 Button Handler (Header)
 * @details Handles button press detection with debouncing
 * 
 * Responsibilities:
 * - Initialize button GPIO and interrupts
 * - Debounce button presses
 * - Trigger CAN commands on button events
 * 
 * Button Mapping:
 * - Button 1 (PTC12): Send Start ADC command
 * - Button 2 (PTC13): Send Stop ADC command
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#ifndef BOARD2_BUTTON_HANDLER_H
#define BOARD2_BUTTON_HANDLER_H

#include "app_types.h"

/*******************************************************************************
 * API FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize button handler
 * 
 * Initializes:
 * - GPIO pins for buttons
 * - PORT interrupts (falling edge)
 * - Debounce timer
 * 
 * @return APP_STATUS_SUCCESS if successful
 * @return APP_STATUS_ERROR if initialization failed
 */
app_status_t ButtonHandler_Init(void);

/**
 * @brief Process button events
 * 
 * Should be called from main loop
 * 
 * Actions:
 * - Check debounce timers
 * - Process pending button events
 * - Trigger callbacks
 */
void ButtonHandler_Process(void);

/**
 * @brief Register button callback
 * 
 * @param[in] callback Callback function
 * 
 * @note Callback is called when button is pressed (after debounce)
 */
void ButtonHandler_RegisterCallback(button_callback_t callback);

/**
 * @brief Get button state
 * 
 * @param[in] button Button ID
 * 
 * @return true if button is pressed (low)
 * @return false if button is released (high)
 */
bool ButtonHandler_IsPressed(button_id_t button);

/**
 * @brief PORT interrupt handler (internal)
 * 
 * Called from PORTC_IRQHandler
 * 
 * @note Handles interrupt flag clearing
 */
void ButtonHandler_PortIrqHandler(void);

#endif /* BOARD2_BUTTON_HANDLER_H */
