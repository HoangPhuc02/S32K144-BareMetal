/**
 * @file app_config.h
 * @brief Application Configuration for Two-Board CAN/UART System
 * @details Defines baudrates, pins, timings, and hardware configuration
 * 
 * System Overview:
 * - Board 1: ADC Sampler (controlled via CAN)
 * - Board 2: Control Hub (CAN ↔ UART bridge)
 * - Communication: CAN 500kbps, UART 9600bps
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*******************************************************************************
 * COMMUNICATION SETTINGS
 ******************************************************************************/

/** CAN bus configuration */
#define CAN_BAUDRATE_KBPS           (500U)      /**< CAN baudrate: 500 kbps */
#define CAN_INSTANCE                (0U)        /**< CAN0 peripheral */

/** UART configuration */
#define UART_BAUDRATE               (9600U)     /**< UART baudrate: 9600 bps */
#define UART_INSTANCE               (1U)        /**< LPUART1 peripheral */

/*******************************************************************************
 * CAN PROTOCOL SETTINGS
 ******************************************************************************/

/** CAN Message IDs */
#define CAN_ID_CMD_START_ADC        (0x100U)    /**< Command: Start ADC sampling */
#define CAN_ID_CMD_STOP_ADC         (0x101U)    /**< Command: Stop ADC sampling */
#define CAN_ID_DATA_ADC_VALUE       (0x200U)    /**< Data: ADC value (Board1 → Board2) */
#define CAN_ID_ACK_START            (0x300U)    /**< Acknowledge: Start accepted */
#define CAN_ID_ACK_STOP             (0x301U)    /**< Acknowledge: Stop accepted */

/** CAN mailbox configuration */
#define CAN_MB_TX_CMD               (0U)        /**< Mailbox for TX commands (Board2) */
#define CAN_MB_RX_CMD               (1U)        /**< Mailbox for RX commands (Board1) */
#define CAN_MB_TX_DATA              (2U)        /**< Mailbox for TX data (Board1) */
#define CAN_MB_RX_DATA              (3U)        /**< Mailbox for RX data (Board2) */
#define CAN_MB_TX_ACK               (4U)        /**< Mailbox for TX ack (Board1) */
#define CAN_MB_RX_ACK               (5U)        /**< Mailbox for RX ack (Board2) */

/*******************************************************************************
 * TIMING SETTINGS
 ******************************************************************************/

/** ADC sampling timing */
#define ADC_SAMPLING_INTERVAL_MS    (1000U)     /**< Sample every 1 second */

/** LPIT timer configuration */
#define LPIT_CHANNEL                (0U)        /**< LPIT channel 0 */
#define LPIT_FREQUENCY_HZ           (1U)        /**< 1 Hz (1 second period) */

/** Debounce timing for buttons */
#define BUTTON_DEBOUNCE_MS          (50U)       /**< 50ms debounce */

/*******************************************************************************
 * BOARD 1: ADC SAMPLER CONFIGURATION
 ******************************************************************************/

/** ADC configuration */
#define ADC_INSTANCE                (0U)        /**< ADC0 peripheral */
#define ADC_CHANNEL                 (12U)       /**< ADC channel 12 (PTB13) */
#define ADC_RESOLUTION_BITS         (12U)       /**< 12-bit resolution */
#define ADC_VREF_MV                 (3300U)     /**< 3.3V reference */

/** Port/Pin for ADC input */
#define ADC_PORT                    PORTB       /**< Port B */
#define ADC_GPIO                    PTB         /**< GPIO B */
#define ADC_PIN                     (13U)       /**< Pin 13 (ADC0_SE12) */

/** CAN pins (Board 1) */
#define CAN_TX_PORT                 PORTE       /**< CAN TX port */
#define CAN_TX_PIN                  (5U)        /**< CAN TX pin (PTE5) */
#define CAN_RX_PORT                 PORTE       /**< CAN RX port */
#define CAN_RX_PIN                  (4U)        /**< CAN RX pin (PTE4) */

/*******************************************************************************
 * BOARD 2: CONTROL HUB CONFIGURATION
 ******************************************************************************/

/** Button 1 configuration (Start ADC) */
#define BUTTON1_PORT                PORTC       /**< Button 1 port */
#define BUTTON1_GPIO                PTC         /**< Button 1 GPIO */
#define BUTTON1_PIN                 (12U)       /**< Button 1 pin (PTC12) */
#define BUTTON1_IRQ                 PORTC_IRQn  /**< Button 1 interrupt */

/** Button 2 configuration (Stop ADC) */
#define BUTTON2_PORT                PORTC       /**< Button 2 port */
#define BUTTON2_GPIO                PTC         /**< Button 2 GPIO */
#define BUTTON2_PIN                 (13U)       /**< Button 2 pin (PTC13) */
#define BUTTON2_IRQ                 PORTC_IRQn  /**< Button 2 interrupt */

/** UART pins (Board 2) */
#define UART_TX_PORT                PORTC       /**< UART TX port */
#define UART_TX_PIN                 (7U)        /**< UART TX pin (PTC7) */
#define UART_RX_PORT                PORTC       /**< UART RX port */
#define UART_RX_PIN                 (6U)        /**< UART RX pin (PTC6) */

/** CAN pins (Board 2) - Same as Board 1 */
#define CAN_TX_PORT_B2              PORTE       /**< CAN TX port */
#define CAN_TX_PIN_B2               (5U)        /**< CAN TX pin (PTE5) */
#define CAN_RX_PORT_B2              PORTE       /**< CAN RX port */
#define CAN_RX_PIN_B2               (4U)        /**< CAN RX pin (PTE4) */

/*******************************************************************************
 * SYSTEM SETTINGS
 ******************************************************************************/

/** Clock configuration */
#define SYSTEM_CLOCK_HZ             (80000000U) /**< 80 MHz system clock */
#define LPIT_CLOCK_HZ               (8000000U)  /**< 8 MHz LPIT clock (SIRC) */

/** Buffer sizes */
#define UART_TX_BUFFER_SIZE         (128U)      /**< UART transmit buffer */
#define CAN_RX_QUEUE_SIZE           (8U)        /**< CAN receive queue */

/** Timeout values */
#define CAN_TIMEOUT_MS              (100U)      /**< CAN transmission timeout */
#define UART_TIMEOUT_MS             (100U)      /**< UART transmission timeout */
#define ADC_TIMEOUT_MS              (10U)       /**< ADC conversion timeout */

/*******************************************************************************
 * DEBUG SETTINGS
 ******************************************************************************/

/** Enable/disable debug features */
#define DEBUG_ENABLE                (1U)        /**< Enable debug output */
#define DEBUG_LED_ENABLE            (1U)        /**< Enable debug LED */

#if DEBUG_LED_ENABLE
/** Debug LED (Board 1 & 2) */
#define DEBUG_LED_PORT              PORTD       /**< LED port */
#define DEBUG_LED_GPIO              PTD         /**< LED GPIO */
#define DEBUG_LED_PIN               (15U)       /**< Green LED (PTD15) */
#endif

#endif /* APP_CONFIG_H */
