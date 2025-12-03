/**
 * @file board2_can_handler.h
 * @brief Board 2 CAN Communication Handler (Header)
 * @details Handles CAN message transmission and reception for Board 2
 * 
 * Responsibilities:
 * - Initialize CAN peripheral (500kbps)
 * - Send command messages (Start/Stop) to Board 1
 * - Receive ADC data messages from Board 1
 * - Receive acknowledge messages from Board 1
 * - Forward ADC data to UART handler
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#ifndef BOARD2_CAN_HANDLER_H
#define BOARD2_CAN_HANDLER_H

#include "app_types.h"
#include "protocol.h"

/*******************************************************************************
 * API FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize CAN handler
 * 
 * Initializes:
 * - CAN peripheral (500kbps)
 * - Mailbox configuration (TX/RX)
 * - Interrupt configuration
 * 
 * @return APP_STATUS_SUCCESS if successful
 * @return APP_STATUS_ERROR if initialization failed
 */
app_status_t Board2_CanHandler_Init(void);

/**
 * @brief Send CAN command to Board 1
 * 
 * @param[in] command Command type (START/STOP)
 * 
 * @return APP_STATUS_SUCCESS if sent
 * @return APP_STATUS_ERROR if packing/sending failed
 */
app_status_t Board2_CanHandler_SendCommand(command_type_t command);

/**
 * @brief Process received CAN messages
 * 
 * Should be called from main loop or CAN ISR
 * 
 * Actions:
 * - Check for received messages
 * - Decode ADC data messages
 * - Decode acknowledge messages
 * - Forward ADC data to UART
 */
void Board2_CanHandler_ProcessMessages(void);

/**
 * @brief Check if Board 1 is actively sending data
 * 
 * @return true if receiving ADC data
 * @return false if no data received recently
 */
bool Board2_CanHandler_IsReceivingData(void);

/**
 * @brief Get last received ADC value
 * 
 * @param[out] adc_value Pointer to store ADC value
 * 
 * @return APP_STATUS_SUCCESS if valid data available
 * @return APP_STATUS_ERROR if no data or invalid
 */
app_status_t Board2_CanHandler_GetLastAdcValue(uint16_t *adc_value);

/**
 * @brief CAN receive callback (internal)
 * 
 * Called when CAN message is received
 * 
 * @param[in] frame Received CAN frame
 */
void Board2_CanHandler_RxCallback(const can_message_t *frame);

#endif /* BOARD2_CAN_HANDLER_H */
