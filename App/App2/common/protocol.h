/**
 * @file protocol.h
 * @brief CAN Protocol Definition for Two-Board System
 * @details Defines message structures, frame formats, and protocol helpers
 * 
 * CAN Protocol Specification:
 * - Baudrate: 500 kbps
 * - Frame format: Standard (11-bit ID)
 * - Data length: 8 bytes
 * - Byte order: Big-endian (MSB first)
 * 
 * Message Types:
 * 1. Command Messages (Board2 → Board1): Start/Stop ADC
 * 2. Data Messages (Board1 → Board2): ADC values
 * 3. Acknowledge Messages (Board1 → Board2): Command confirmations
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CAN FRAME FORMAT
 ******************************************************************************/

/**
 * @brief CAN frame data length
 */
#define CAN_DATA_LENGTH             (8U)

/**
 * @brief CAN message structure
 */
typedef struct {
    uint32_t id;                            /**< CAN message ID (11-bit) */
    uint8_t data[CAN_DATA_LENGTH];          /**< Data bytes (8 bytes) */
    uint8_t length;                         /**< Actual data length (0-8) */
} can_message_t;

/*******************************************************************************
 * COMMAND MESSAGES (Board2 → Board1)
 ******************************************************************************/

/**
 * @brief Command types
 */
typedef enum {
    CMD_START_ADC = 0x01,                   /**< Start ADC sampling */
    CMD_STOP_ADC  = 0x02                    /**< Stop ADC sampling */
} command_type_t;

/**
 * @brief Command message format
 * 
 * Frame format:
 * | Byte 0 | Byte 1-7 |
 * |--------|----------|
 * | CMD    | Reserved |
 * 
 * Example - Start ADC:
 * ID: 0x100
 * Data: [0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
 */
typedef struct {
    command_type_t command;                 /**< Command type */
} command_message_t;

/*******************************************************************************
 * DATA MESSAGES (Board1 → Board2)
 ******************************************************************************/

/**
 * @brief ADC value format in CAN frame
 * 
 * Frame format (ASCII encoding):
 * | Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
 * |--------|--------|--------|--------|--------|--------|--------|--------|
 * | Digit  | Digit  | Digit  | Digit  | Digit  | Digit  | Digit  | Digit  |
 * 
 * ADC value (12-bit): 0 - 4095 → ASCII "00000000" to "00004095"
 * 
 * Examples:
 * - ADC = 1:
 *   Data: [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01]
 *        = ['0',  '0',  '0',  '0',  '0',  '0',  '0',  '1']
 * 
 * - ADC = 456:
 *   Data: [0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x05, 0x06]
 *        = ['0',  '0',  '0',  '0',  '0',  '4',  '5',  '6']
 * 
 * - ADC = 4095:
 *   Data: [0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x09, 0x05]
 *        = ['0',  '0',  '0',  '0',  '4',  '0',  '9',  '5']
 */
typedef struct {
    uint16_t adc_value;                     /**< ADC value (0-4095) */
    uint8_t adc_digits[8];                  /**< ASCII digits array */
} adc_data_message_t;

/*******************************************************************************
 * ACKNOWLEDGE MESSAGES (Board1 → Board2)
 ******************************************************************************/

/**
 * @brief Acknowledge types
 */
typedef enum {
    ACK_START_SUCCESS = 0x01,               /**< Start command accepted */
    ACK_STOP_SUCCESS  = 0x02,               /**< Stop command accepted */
    ACK_ERROR         = 0xFF                /**< Command error */
} ack_type_t;

/**
 * @brief Acknowledge message format
 * 
 * Frame format:
 * | Byte 0 | Byte 1-7 |
 * |--------|----------|
 * | ACK    | Reserved |
 */
typedef struct {
    ack_type_t ack;                         /**< Acknowledge type */
} ack_message_t;

/*******************************************************************************
 * PROTOCOL HELPER FUNCTIONS
 ******************************************************************************/

/**
 * @brief Pack command into CAN frame
 * 
 * @param[in] command Command type (START/STOP)
 * @param[out] frame Output CAN frame
 * 
 * @return true if successful, false otherwise
 * 
 * @note Sets appropriate CAN ID based on command type
 */
bool Protocol_PackCommand(command_type_t command, can_message_t *frame);

/**
 * @brief Unpack command from CAN frame
 * 
 * @param[in] frame Input CAN frame
 * @param[out] msg Output command message
 * 
 * @return true if valid command, false otherwise
 */
bool Protocol_UnpackCommand(const can_message_t *frame, command_message_t *msg);

/**
 * @brief Pack ADC value into CAN frame (ASCII encoding)
 * 
 * Converts 12-bit ADC value (0-4095) to 8-byte ASCII representation:
 * - Most significant digits in lower bytes
 * - Least significant digits in higher bytes
 * 
 * @param[in] adc_value ADC value (0-4095)
 * @param[out] frame Output CAN frame
 * 
 * @return true if successful, false if value out of range
 * 
 * Example:
 * @code
 * can_message_t frame;
 * Protocol_PackAdcValue(456, &frame);
 * // frame.data = [0, 0, 0, 0, 0, 4, 5, 6]
 * @endcode
 */
bool Protocol_PackAdcValue(uint16_t adc_value, can_message_t *frame);

/**
 * @brief Unpack ADC value from CAN frame (ASCII decoding)
 * 
 * Converts 8-byte ASCII representation to 12-bit ADC value
 * 
 * @param[in] frame Input CAN frame
 * @param[out] msg Output ADC data message
 * 
 * @return true if valid ADC data, false otherwise
 */
bool Protocol_UnpackAdcValue(const can_message_t *frame, adc_data_message_t *msg);

/**
 * @brief Pack acknowledge into CAN frame
 * 
 * @param[in] ack Acknowledge type
 * @param[out] frame Output CAN frame
 * 
 * @return true if successful, false otherwise
 */
bool Protocol_PackAcknowledge(ack_type_t ack, can_message_t *frame);

/**
 * @brief Unpack acknowledge from CAN frame
 * 
 * @param[in] frame Input CAN frame
 * @param[out] msg Output acknowledge message
 * 
 * @return true if valid acknowledge, false otherwise
 */
bool Protocol_UnpackAcknowledge(const can_message_t *frame, ack_message_t *msg);

/**
 * @brief Convert ADC value to ASCII digits (helper)
 * 
 * Converts uint16_t value to 8 ASCII digit bytes
 * 
 * @param[in] value ADC value (0-4095)
 * @param[out] digits Output array (8 bytes)
 * 
 * Example:
 * @code
 * uint8_t digits[8];
 * Protocol_ConvertAdcToAscii(456, digits);
 * // digits = [0, 0, 0, 0, 0, 4, 5, 6]
 * @endcode
 */
void Protocol_ConvertAdcToAscii(uint16_t value, uint8_t *digits);

/**
 * @brief Convert ASCII digits to ADC value (helper)
 * 
 * Converts 8 ASCII digit bytes to uint16_t value
 * 
 * @param[in] digits Input array (8 bytes)
 * @param[out] value Output ADC value
 * 
 * @return true if valid digits (0-9), false otherwise
 * 
 * Example:
 * @code
 * uint8_t digits[8] = {0, 0, 0, 0, 0, 4, 5, 6};
 * uint16_t value;
 * Protocol_ConvertAsciiToAdc(digits, &value);
 * // value = 456
 * @endcode
 */
bool Protocol_ConvertAsciiToAdc(const uint8_t *digits, uint16_t *value);

/**
 * @brief Validate CAN message ID
 * 
 * @param[in] id CAN message ID
 * @param[in] expected_id Expected message ID
 * 
 * @return true if ID matches, false otherwise
 */
bool Protocol_ValidateMessageId(uint32_t id, uint32_t expected_id);

#endif /* PROTOCOL_H */
