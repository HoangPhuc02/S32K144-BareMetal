/**
 * @file protocol.c
 * @brief CAN Protocol Implementation
 * @details Implements message packing/unpacking functions
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#include "protocol.h"
#include "app_config.h"
#include <string.h>

/*******************************************************************************
 * COMMAND MESSAGE FUNCTIONS
 ******************************************************************************/

/**
 * @brief Pack command into CAN frame
 */
bool Protocol_PackCommand(command_type_t command, can_message_t *frame)
{
    /* Validate inputs */
    if (frame == NULL) {
        return false;
    }
    
    /* Clear frame */
    memset(frame, 0, sizeof(can_message_t));
    
    /* Set message ID based on command */
    switch (command) {
        case CMD_START_ADC:
            frame->id = CAN_ID_CMD_START_ADC;
            break;
        case CMD_STOP_ADC:
            frame->id = CAN_ID_CMD_STOP_ADC;
            break;
        default:
            return false;
    }
    
    /* Set command in byte 0 */
    frame->data[0] = (uint8_t)command;
    frame->length = CAN_DATA_LENGTH;
    
    return true;
}

/**
 * @brief Unpack command from CAN frame
 */
bool Protocol_UnpackCommand(const can_message_t *frame, command_message_t *msg)
{
    /* Validate inputs */
    if (frame == NULL || msg == NULL) {
        return false;
    }
    
    /* Validate message ID */
    if (frame->id != CAN_ID_CMD_START_ADC && 
        frame->id != CAN_ID_CMD_STOP_ADC) {
        return false;
    }
    
    /* Extract command */
    msg->command = (command_type_t)frame->data[0];
    
    /* Validate command type */
    if (msg->command != CMD_START_ADC && msg->command != CMD_STOP_ADC) {
        return false;
    }
    
    return true;
}

/*******************************************************************************
 * ADC DATA MESSAGE FUNCTIONS
 ******************************************************************************/

/**
 * @brief Convert ADC value to ASCII digits (helper)
 */
void Protocol_ConvertAdcToAscii(uint16_t value, uint8_t *digits)
{
    /* Convert each digit from right to left */
    digits[7] = value % 10;          /* Ones */
    value /= 10;
    digits[6] = value % 10;          /* Tens */
    value /= 10;
    digits[5] = value % 10;          /* Hundreds */
    value /= 10;
    digits[4] = value % 10;          /* Thousands */
    value /= 10;
    digits[3] = value % 10;          /* Ten thousands */
    value /= 10;
    digits[2] = value % 10;
    value /= 10;
    digits[1] = value % 10;
    value /= 10;
    digits[0] = value % 10;
}

/**
 * @brief Convert ASCII digits to ADC value (helper)
 */
bool Protocol_ConvertAsciiToAdc(const uint8_t *digits, uint16_t *value)
{
    uint16_t result = 0;
    uint32_t multiplier = 1;
    
    /* Validate inputs */
    if (digits == NULL || value == NULL) {
        return false;
    }
    
    /* Convert from right to left */
    for (int8_t i = 7; i >= 0; i--) {
        /* Validate digit range (0-9) */
        if (digits[i] > 9) {
            return false;
        }
        
        result += digits[i] * multiplier;
        multiplier *= 10;
        
        /* Prevent overflow */
        if (result > 4095) {
            return false;
        }
    }
    
    *value = result;
    return true;
}

/**
 * @brief Pack ADC value into CAN frame (ASCII encoding)
 */
bool Protocol_PackAdcValue(uint16_t adc_value, can_message_t *frame)
{
    /* Validate inputs */
    if (frame == NULL) {
        return false;
    }
    
    /* Validate ADC range (12-bit: 0-4095) */
    if (adc_value > 4095) {
        return false;
    }
    
    /* Clear frame */
    memset(frame, 0, sizeof(can_message_t));
    
    /* Set message ID */
    frame->id = CAN_ID_DATA_ADC_VALUE;
    
    /* Convert to ASCII digits */
    Protocol_ConvertAdcToAscii(adc_value, frame->data);
    
    frame->length = CAN_DATA_LENGTH;
    
    return true;
}

/**
 * @brief Unpack ADC value from CAN frame (ASCII decoding)
 */
bool Protocol_UnpackAdcValue(const can_message_t *frame, adc_data_message_t *msg)
{
    /* Validate inputs */
    if (frame == NULL || msg == NULL) {
        return false;
    }
    
    /* Validate message ID */
    if (!Protocol_ValidateMessageId(frame->id, CAN_ID_DATA_ADC_VALUE)) {
        return false;
    }
    
    /* Copy digits */
    memcpy(msg->adc_digits, frame->data, CAN_DATA_LENGTH);
    
    /* Convert to numeric value */
    if (!Protocol_ConvertAsciiToAdc(msg->adc_digits, &msg->adc_value)) {
        return false;
    }
    
    return true;
}

/*******************************************************************************
 * ACKNOWLEDGE MESSAGE FUNCTIONS
 ******************************************************************************/

/**
 * @brief Pack acknowledge into CAN frame
 */
bool Protocol_PackAcknowledge(ack_type_t ack, can_message_t *frame)
{
    /* Validate inputs */
    if (frame == NULL) {
        return false;
    }
    
    /* Clear frame */
    memset(frame, 0, sizeof(can_message_t));
    
    /* Set message ID based on ack type */
    switch (ack) {
        case ACK_START_SUCCESS:
            frame->id = CAN_ID_ACK_START;
            break;
        case ACK_STOP_SUCCESS:
            frame->id = CAN_ID_ACK_STOP;
            break;
        case ACK_ERROR:
            frame->id = CAN_ID_ACK_STOP;  /* Use stop ID for error */
            break;
        default:
            return false;
    }
    
    /* Set ack in byte 0 */
    frame->data[0] = (uint8_t)ack;
    frame->length = CAN_DATA_LENGTH;
    
    return true;
}

/**
 * @brief Unpack acknowledge from CAN frame
 */
bool Protocol_UnpackAcknowledge(const can_message_t *frame, ack_message_t *msg)
{
    /* Validate inputs */
    if (frame == NULL || msg == NULL) {
        return false;
    }
    
    /* Validate message ID */
    if (frame->id != CAN_ID_ACK_START && 
        frame->id != CAN_ID_ACK_STOP) {
        return false;
    }
    
    /* Extract ack */
    msg->ack = (ack_type_t)frame->data[0];
    
    return true;
}

/*******************************************************************************
 * UTILITY FUNCTIONS
 ******************************************************************************/

/**
 * @brief Validate CAN message ID
 */
bool Protocol_ValidateMessageId(uint32_t id, uint32_t expected_id)
{
    return (id == expected_id);
}
