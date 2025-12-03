/**
 * @file board1_can_handler.h
 * @brief Board 1 CAN Communication Handler (Header)
 * @details Handles CAN message transmission and reception for Board 1
 * 
 * Responsibilities:
 * - Initialize CAN peripheral (500kbps)
 * - Receive command messages (Start/Stop)
 * - Send ADC data messages
 * - Send acknowledge messages
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#ifndef BOARD1_CAN_HANDLER_H
#define BOARD1_CAN_HANDLER_H

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
app_status_t CanHandler_Init(void);

/**
 * @brief Send CAN message
 * 
 * @param[in] frame CAN frame to send
 * 
 * @return APP_STATUS_SUCCESS if sent
 * @return APP_STATUS_TIMEOUT if transmission timeout
 * @return APP_STATUS_INVALID_PARAM if frame is NULL
 */
app_status_t CanHandler_SendMessage(const can_message_t *frame);

/**
 * @brief Send ADC data message
 * 
 * Convenience function to pack and send ADC value
 * 
 * @param[in] adc_value ADC value (0-4095)
 * 
 * @return APP_STATUS_SUCCESS if sent
 * @return APP_STATUS_ERROR if packing/sending failed
 */
app_status_t CanHandler_SendAdcData(uint16_t adc_value);

/**
 * @brief Send acknowledge message
 * 
 * @param[in] ack Acknowledge type
 * 
 * @return APP_STATUS_SUCCESS if sent
 * @return APP_STATUS_ERROR if packing/sending failed
 */
app_status_t CanHandler_SendAcknowledge(ack_type_t ack);

/**
 * @brief Process received CAN messages
 * 
 * Should be called from main loop or CAN ISR
 * 
 * Actions:
 * - Check for received messages
 * - Decode command messages
 * - Trigger ADC task start/stop
 */
void CanHandler_ProcessMessages(void);

/**
 * @brief CAN receive callback (internal)
 * 
 * Called when CAN message is received
 * 
 * @param[in] frame Received CAN frame
 */
void CanHandler_RxCallback(const can_message_t *frame);

#endif /* BOARD1_CAN_HANDLER_H */
