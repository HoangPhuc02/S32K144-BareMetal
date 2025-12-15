/**
 * @file    can_tx_rx_mode.c
 * @brief   CAN TX/RX Mode Selection Example
 * @details ChÆ°Æ¡ng trÃ¬nh cho phÃ©p chá»�n giá»¯a TX mode vÃ  RX mode
 *          - TX Mode: Nháº¥n nÃºt gá»­i CAN message
 *          - RX Mode: Nháº­n CAN message vÃ  toggle Green LED
 *
 * Hardware Setup:
 * - Button SW2 (PTC12): Trigger TX trong TX mode
 * - Button SW3 (PTC13): Chuyá»ƒn Ä‘á»•i giá»¯a TX/RX mode
 * - Red LED (PTD15): BÃ¡o hiá»‡u TX mode
 * - Green LED (PTD16): Toggle khi RX nháº­n Ä‘Æ°á»£c message
 * - CAN TX: PTE5
 * - CAN RX: PTE4
 *
 * @author  PhucPH32
 * @date    04/12/2025
 * @version 1.0
 */

#include "can.h"
#include "gpio.h"
#include "port.h"
#include "nvic.h"
#include "pcc_reg.h"
#include "clocks_and_modes.h"
#include <stdio.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Button Configuration */
#define BUTTON_TX_PORT          PORT_C
#define BUTTON_TX_GPIO_PORT     GPIO_PORT_C
#define BUTTON_TX_PIN           (12U)           /* SW2 - TX trigger */

#define BUTTON_MODE_PORT        PORT_C
#define BUTTON_MODE_GPIO_PORT   GPIO_PORT_C
#define BUTTON_MODE_PIN         (13U)           /* SW3 - Mode select */

#define BUTTON_IRQn             PORTC_IRQn

/* LED Configuration */
#define LED_RED_PORT            PORT_D
#define LED_RED_GPIO_PORT       GPIO_PORT_D
#define LED_RED_PIN             (15U)           /* Red LED - TX indicator */

#define LED_GREEN_PORT          PORT_D
#define LED_GREEN_GPIO_PORT     GPIO_PORT_D
#define LED_GREEN_PIN           (16U)           /* Green LED - RX indicator */

/* CAN Configuration */
#define CAN_GPIO_PORT           GPIO_PORT_E
#define CAN_TX_PIN              (5U)
#define CAN_RX_PIN              (4U)
#define CAN_MESSAGE_ID          (0x123U)
#define CAN_TX_MAILBOX          (8U)
#define CAN_RX_MAILBOX          (16U)
#define CAN_BAUDRATE            (500000U)       /* 500 kbps */

/* Configuration */
#define DEBOUNCE_DELAY_MS       (50U)

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

typedef enum {
    MODE_TX = 0,    /* Transmit mode */
    MODE_RX = 1     /* Receive mode */
} operation_mode_t;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

static volatile operation_mode_t g_currentMode = MODE_TX;
static volatile bool g_buttonTxPressed = false;
static volatile bool g_buttonModePressed = false;
static volatile uint32_t g_messageCounter = 0;
static volatile bool g_messageReceived = false;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void InitClocks(void);
static void InitLEDs(void);
static void InitButtons(void);
static void InitCAN(void);
static void SendCANMessage(void);
static void UpdateModeLEDs(void);
static void SimpleDelay(uint32_t ms);

/*******************************************************************************
 * Interrupt Handlers
 ******************************************************************************/

/**
 * @brief PORTC interrupt handler (Buttons)
 */
void PORTC_IRQHandler(void)
{
    /* Check TX button (SW2 - PTC12) */
    if (PORT_GetPinInterruptFlag(BUTTON_TX_PORT, BUTTON_TX_PIN)) {
        /* Clear interrupt flag */
        PORT_ClearPinInterruptFlag(BUTTON_TX_PORT, BUTTON_TX_PIN);
        
        /* Debounce - check if button is actually pressed (low) */
        if (GPIO_ReadPin(BUTTON_TX_GPIO_PORT, BUTTON_TX_PIN) == 0U) {
            g_buttonTxPressed = true;
        }
    }
    
    /* Check Mode button (SW3 - PTC13) */
    if (PORT_GetPinInterruptFlag(BUTTON_MODE_PORT, BUTTON_MODE_PIN)) {
        /* Clear interrupt flag */
        PORT_ClearPinInterruptFlag(BUTTON_MODE_PORT, BUTTON_MODE_PIN);
        
        /* Debounce - check if button is actually pressed (low) */
        if (GPIO_ReadPin(BUTTON_MODE_GPIO_PORT, BUTTON_MODE_PIN) == 0U) {
            g_buttonModePressed = true;
        }
    }
}

/**
 * @brief CAN0 mailbox interrupt handler (RX)
 */
void CAN0_ORed_0_15_MB_IRQHandler(void)
{
    can_message_t rxMsg;
    
    /* Only process in RX mode */
    if (g_currentMode == MODE_RX) {
        /* Try to receive message */
        if (CAN_Receive(0, CAN_RX_MAILBOX, &rxMsg) == STATUS_SUCCESS) {
            /* Toggle Green LED */
            GPIO_TogglePin(LED_GREEN_GPIO_PORT, LED_GREEN_PIN);
            
            /* Print received message */
            printf("RX: ID=0x%03lX, Data=", rxMsg.id);
            for (uint8_t i = 0; i < rxMsg.dataLength; i++) {
                printf("%02X ", rxMsg.data[i]);
            }
            printf("\n");
            
            g_messageReceived = true;
        }
    }
}


/**
 * @brief CAN0 mailbox interrupt handler (RX)
 */
void CAN0_ORed_16_31_MB_IRQHandler(void)
{
    can_message_t rxMsg;

    /* Only process in RX mode */
    if (g_currentMode == MODE_RX) {
        /* Try to receive message */
        if (CAN_Receive(0, CAN_RX_MAILBOX, &rxMsg) == STATUS_SUCCESS) {
            /* Toggle Green LED */
            GPIO_TogglePin(LED_GREEN_GPIO_PORT, LED_GREEN_PIN);

            /* Print received message */
            printf("RX: ID=0x%03lX, Data=", rxMsg.id);
            for (uint8_t i = 0; i < rxMsg.dataLength; i++) {
                printf("%02X ", rxMsg.data[i]);
            }
            printf("\n");

            g_messageReceived = true;
        }
    }
}
/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Initialize system clocks
 */
static void InitClocks(void)
{
    /* Enable clocks for PORTC (Buttons) */
    PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;
    
    /* Enable clocks for PORTD (LEDs) */
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
    
    /* Enable clocks for PORTE (CAN) */
    PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK;
    
    /* Enable CAN0 clock */
    PCC->PCCn[PCC_FLEXCAN0_INDEX] = PCC_PCCn_CGC_MASK;
}

/**
 * @brief Initialize LEDs
 */
static void InitLEDs(void)
{
    /* Configure Red LED pin as GPIO */
    PORT_SetPinMux(LED_RED_GPIO_PORT, LED_RED_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(LED_RED_GPIO_PORT, LED_RED_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_RED_GPIO_PORT, LED_RED_PIN, 1U);  /* Off (active low) */
    
    /* Configure Green LED pin as GPIO */
    PORT_SetPinMux(LED_GREEN_GPIO_PORT, LED_GREEN_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(LED_GREEN_GPIO_PORT, LED_GREEN_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_PIN, 1U);  /* Off (active low) */
}

/**
 * @brief Initialize Buttons with interrupt
 */
static void InitButtons(void)
{
    /* Configure TX button (SW2 - PTC12) */
    port_pin_config_t buttonTxConfig = {
        .base = PORTC,
        .mux = PORT_MUX_GPIO,
        .pull = PORT_PULL_UP,
        .drive = PORT_DRIVE_LOW,
        .interrupt = PORT_INT_FALLING_EDGE,
        .passiveFilter = false,
        .digitalFilter = true
    };
    PORT_ConfigurePin(BUTTON_TX_PORT, BUTTON_TX_PIN, &buttonTxConfig);
    GPIO_SetPinDirection(BUTTON_TX_GPIO_PORT, BUTTON_TX_PIN, GPIO_DIR_INPUT);
    
    /* Configure Mode button (SW3 - PTC13) */
    port_pin_config_t buttonModeConfig = {
        .base = PORTC,
        .mux = PORT_MUX_GPIO,
        .pull = PORT_PULL_UP,
        .drive = PORT_DRIVE_LOW,
        .interrupt = PORT_INT_FALLING_EDGE,
        .passiveFilter = false,
        .digitalFilter = true
    };
    PORT_ConfigurePin(BUTTON_MODE_PORT, BUTTON_MODE_PIN, &buttonModeConfig);
    GPIO_SetPinDirection(BUTTON_MODE_GPIO_PORT, BUTTON_MODE_PIN, GPIO_DIR_INPUT);
    
    /* Enable PORTC interrupt in NVIC */
    NVIC_SetPriority(BUTTON_IRQn, 3);
    NVIC_EnableIRQ(BUTTON_IRQn);
}

/**
 * @brief Initialize CAN peripheral
 */
static void InitCAN(void)
{
    status_t status;
    
    /* Configure CAN pins */
    PORT_SetPinMux(CAN_GPIO_PORT, CAN_RX_PIN, PORT_MUX_ALT5);  /* CAN0_RX */
    PORT_SetPinMux(CAN_GPIO_PORT, CAN_TX_PIN, PORT_MUX_ALT5);  /* CAN0_TX */
    
    /* Configure CAN */
    can_config_t canConfig = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_SOSCDIV2,    /* 40 MHz */
        .baudRate = CAN_BAUDRATE,                /* 500 kbps */
        .mode = CAN_MODE_NORMAL,                 /* Normal mode for actual communication */
        .enableSelfReception = false,
        .useRxFifo = false
    };
    
    status = CAN_Init(&canConfig);
    if (status != STATUS_SUCCESS) {
        printf("CAN Init failed!\n");
        return;
    }
    
    /* Configure RX filter to receive messages with ID 0x123 */
    can_rx_filter_t rxFilter = {
        .id = CAN_MESSAGE_ID,
        .mask = 0x7FF,                           /* Match all 11 bits */
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, CAN_RX_MAILBOX, &rxFilter);
    
    /* Install RX callback to enable mailbox interrupt */
    CAN_InstallRxCallback(0, CAN_RX_MAILBOX, NULL, NULL);  /* Enable interrupt without callback */
    
    /* Enable CAN interrupt in NVIC */
    if(CAN_RX_MAILBOX < 16)
    {
        NVIC_SetPriority(CAN0_ORed_0_15_MB_IRQn, 4);
        NVIC_EnableIRQ(CAN0_ORed_0_15_MB_IRQn);
    }
    else
    {
        NVIC_SetPriority(CAN0_ORed_16_31_MB_IRQn, 4);
        NVIC_EnableIRQ(CAN0_ORed_16_31_MB_IRQn);
    }

    
    printf("CAN initialized: 500 kbps, ID=0x%03X\n", CAN_MESSAGE_ID);
}

/**
 * @brief Send a CAN message
 */
static void SendCANMessage(void)
{
    status_t status;
    can_message_t txMsg;
    
    /* Prepare message */
    txMsg.id = CAN_MESSAGE_ID;
    txMsg.idType = CAN_ID_STD;
    txMsg.frameType = CAN_FRAME_DATA;
    txMsg.dataLength = 8;
    
    /* Fill data with counter and pattern */
    txMsg.data[0] = (uint8_t)(g_messageCounter >> 24);
    txMsg.data[1] = (uint8_t)(g_messageCounter >> 16);
    txMsg.data[2] = (uint8_t)(g_messageCounter >> 8);
    txMsg.data[3] = (uint8_t)(g_messageCounter);
    txMsg.data[4] = 0xAA;
    txMsg.data[5] = 0x55;
    txMsg.data[6] = 0xDE;
    txMsg.data[7] = 0xAD;
    
    /* Brief Red LED blink during transmission */
    GPIO_WritePin(LED_RED_GPIO_PORT, LED_RED_PIN, 0U);  /* On (active low) */
    
    /* Send message */
    status = CAN_SendBlocking(0, CAN_TX_MAILBOX, &txMsg, 100);
    
    if (status == STATUS_SUCCESS) {
        printf("TX [%lu]: ", g_messageCounter);
        for (uint8_t i = 0; i < txMsg.dataLength; i++) {
            printf("%02X ", txMsg.data[i]);
        }
        printf("\n");
        g_messageCounter++;
    } else {
        printf("TX failed: status=%d\n", status);
    }
    
    SimpleDelay(50);  /* Keep LED on for 50ms */
    
    /* Update LED based on mode */
    UpdateModeLEDs();
}

/**
 * @brief Update LEDs based on current mode
 */
static void UpdateModeLEDs(void)
{
    if (g_currentMode == MODE_TX) {
        /* TX mode: Red LED solid ON, Green LED OFF */
        GPIO_WritePin(LED_RED_GPIO_PORT, LED_RED_PIN, 0U);    /* On */
        GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_PIN, 1U); /* Off */
    } else {
        /* RX mode: Red LED OFF, Green LED solid ON (will toggle on RX) */
        GPIO_WritePin(LED_RED_GPIO_PORT, LED_RED_PIN, 1U);    /* Off */
        GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_PIN, 0U); /* On */
    }
}

/**
 * @brief Simple delay function
 */
static void SimpleDelay(uint32_t ms)
{
    /* Approximate delay at 80 MHz
     * Each loop iteration takes ~4 cycles
     * 80,000,000 / 4 = 20,000,000 iterations per second
     * 20,000 iterations per millisecond
     */
    volatile uint32_t count = ms * 20000;
    while (count--) {
        __asm("NOP");
    }
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Main function
 */
int main(void)
{
    printf("\n");
    printf("===========================================\n");
    printf("  CAN TX/RX Mode Selection Example\n");
    printf("===========================================\n");
    printf("Hardware:\n");
    printf("  - SW2 (PTC12): TX trigger button\n");
    printf("  - SW3 (PTC13): Mode switch button\n");
    printf("  - Red LED (PTD15): TX mode indicator\n");
    printf("  - Green LED (PTD16): RX toggle on message\n");
    printf("  - CAN: PTE4/PTE5\n");
    printf("\n");
    printf("Operation:\n");
    printf("  1. Press SW3 to switch TX/RX mode\n");
    printf("  2. TX Mode: Red LED ON, press SW2 to send\n");
    printf("  3. RX Mode: Green LED ON, toggles on RX\n");
    printf("\n");
    printf("===========================================\n\n");
    
    /* Initialize system clocks */
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    
    /* Initialize peripherals */
    InitClocks();
    InitLEDs();
    InitButtons();
    InitCAN();
    
    /* Set initial mode to TX */
    g_currentMode = MODE_TX;
    UpdateModeLEDs();
    if(g_currentMode == MODE_TX)
    {
        printf("Current Mode: TX\n");
        printf("Press SW3 to switch mode, SW2 to send (TX mode)\n\n");
    }
    else
    {
        printf("Current Mode: RX\n");
        printf("Press SW3 to switch mode, SW2 to send (TX mode)\n\n");
    }

    
    /* Main loop */
    while (1) {
        /* Handle Mode button press */
        if (g_buttonModePressed) {
            g_buttonModePressed = false;
            
            /* Toggle mode */
            if (g_currentMode == MODE_TX) {
                g_currentMode = MODE_RX;
                printf("\n>>> Switched to RX Mode <<<\n");
                printf("Waiting for CAN messages...\n\n");
            } else {
                g_currentMode = MODE_TX;
                printf("\n>>> Switched to TX Mode <<<\n");
                printf("Press SW2 to send messages\n\n");
            }
            
            /* Update LEDs */
            UpdateModeLEDs();
            
            /* Debounce delay */
            SimpleDelay(DEBOUNCE_DELAY_MS);
        }
        
        /* Handle TX button press (only in TX mode) */
        if (g_buttonTxPressed && g_currentMode == MODE_TX) {
            g_buttonTxPressed = false;
            
            /* Send CAN message */
            SendCANMessage();
            
            /* Debounce delay */
            SimpleDelay(DEBOUNCE_DELAY_MS);
        }
        
        /* Optional: Add low power mode here */
        __asm("WFI");  /* Wait for interrupt */
    }
    
    return 0;
}
