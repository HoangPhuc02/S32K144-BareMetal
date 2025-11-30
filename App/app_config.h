/**
 * @file    app_config.h
 * @brief   Application Configuration Header
 * @details Common configuration for both Board 1 and Board 2 applications
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 * System Configuration
 ******************************************************************************/

/** @brief System clock frequency (Hz) */
#define SYSTEM_CLOCK_HZ         80000000U

/** @brief Bus clock frequency (Hz) */
#define BUS_CLOCK_HZ            40000000U

/*******************************************************************************
 * CAN Configuration
 ******************************************************************************/

/** @brief CAN baudrate (bits per second) */
#define APP_CAN_BAUDRATE        500000U

/** @brief CAN message ID for ADC data */
#define APP_CAN_MSG_ID          0x100U

/** @brief CAN instance used for communication */
#define APP_CAN_INSTANCE        0U

/*******************************************************************************
 * UART Configuration
 ******************************************************************************/

/** @brief UART baudrate (bits per second) */
#define APP_UART_BAUDRATE       9600U

/** @brief UART instance for PC communication */
#define APP_UART_INSTANCE       1U

/*******************************************************************************
 * ADC Configuration
 ******************************************************************************/

/** @brief ADC instance */
#define APP_ADC_INSTANCE        0U

/** @brief ADC channel */
#define APP_ADC_CHANNEL         12U

/** @brief ADC resolution (bits) */
#define APP_ADC_RESOLUTION      12U

/** @brief Maximum ADC value (12-bit = 4095) */
#define APP_ADC_MAX_VALUE       4095U

/*******************************************************************************
 * Timer Configuration
 ******************************************************************************/

/** @brief LPIT channel for periodic ADC reading */
#define APP_LPIT_CHANNEL        0U

/** @brief LPIT period (microseconds) - 1 second */
#define APP_LPIT_PERIOD_US      1000000U

/*******************************************************************************
 * GPIO Pin Definitions - Board 1
 ******************************************************************************/

/** @brief Button 1 pin (Start ADC) */
#define BOARD1_BTN1_PORT        PORTD
#define BOARD1_BTN1_GPIO        PTD
#define BOARD1_BTN1_PIN         3U

/** @brief Button 2 pin (Stop ADC) */
#define BOARD1_BTN2_PORT        PORTD
#define BOARD1_BTN2_GPIO        PTD
#define BOARD1_BTN2_PIN         5U

/** @brief Status LED pin */
#define BOARD1_LED_PORT         PORTD
#define BOARD1_LED_GPIO         PTD
#define BOARD1_LED_PIN          0U

/*******************************************************************************
 * GPIO Pin Definitions - Board 2
 ******************************************************************************/

/** @brief Status LED pin */
#define BOARD2_LED_PORT         PORTD
#define BOARD2_LED_GPIO         PTD
#define BOARD2_LED_PIN          0U

/*******************************************************************************
 * Message Buffer Configuration
 ******************************************************************************/

/** @brief CAN TX message buffer for Board 1 */
#define APP_CAN_TX_MB           8U

/** @brief CAN RX message buffer for Board 2 */
#define APP_CAN_RX_MB           16U

/*******************************************************************************
 * Protocol Configuration
 ******************************************************************************/

/**
 * @brief CAN Message Frame Format for ADC Data
 * 
 * The ADC value is transmitted as ASCII digits in the last 4 bytes:
 * 
 * Byte:   0    1    2    3    4    5    6    7
 * Data:  [0]  [0]  [0]  [0]  [T]  [H]  [D]  [U]
 * 
 * Where:
 * - Bytes 0-3: Reserved (set to 0)
 * - Byte 4 (T): Thousands digit (ASCII '0'-'9')
 * - Byte 5 (H): Hundreds digit (ASCII '0'-'9')
 * - Byte 6 (D): Tens digit (ASCII '0'-'9')
 * - Byte 7 (U): Units digit (ASCII '0'-'9')
 * 
 * Example:
 * - ADC = 1    -> [0][0][0][0]['0']['0']['0']['1']
 * - ADC = 456  -> [0][0][0][0]['0']['4']['5']['6']
 * - ADC = 3210 -> [0][0][0][0]['3']['2']['1']['0']
 */

/** @brief Offset for thousands digit in CAN frame */
#define CAN_FRAME_THOUSANDS_OFFSET  4U

/** @brief Offset for hundreds digit in CAN frame */
#define CAN_FRAME_HUNDREDS_OFFSET   5U

/** @brief Offset for tens digit in CAN frame */
#define CAN_FRAME_TENS_OFFSET       6U

/** @brief Offset for units digit in CAN frame */
#define CAN_FRAME_UNITS_OFFSET      7U

/*******************************************************************************
 * Interrupt Priority Configuration
 ******************************************************************************/

/** @brief Button interrupt priority (lower number = higher priority) */
#define APP_BTN_IRQ_PRIORITY        3U

/** @brief LPIT timer interrupt priority */
#define APP_LPIT_IRQ_PRIORITY       2U

/** @brief CAN interrupt priority */
#define APP_CAN_IRQ_PRIORITY        2U

/** @brief UART interrupt priority */
#define APP_UART_IRQ_PRIORITY       3U

#endif /* APP_CONFIG_H */
