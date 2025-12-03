/**
 * @file board2_main.c
 * @brief Board 2 Main Application
 * @details Control Hub - Button control, CAN/UART bridge
 * 
 * System Overview:
 * - Button 1: Send Start ADC command via CAN
 * - Button 2: Send Stop ADC command via CAN
 * - Receive ADC data via CAN from Board 1
 * - Forward ADC data to PC via UART
 * 
 * Data Flow:
 * Button → CAN Command → Board 1 → ADC Data → CAN → UART → PC
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#include "board2_button_handler.h"
#include "board2_can_handler.h"
#include "board2_uart_handler.h"
#include "app_config.h"
#include "app_types.h"

/* Driver API includes */
// #include "scg.h"
// #include "pcc.h"
// #include "gpio.h"
// #include "port.h"

/*******************************************************************************
 * PRIVATE VARIABLES
 ******************************************************************************/

/** Board 2 system state */
static board2_state_t s_systemState = BOARD2_STATE_IDLE;

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/

/**
 * @brief Initialize system clocks
 * @return APP_STATUS_SUCCESS if successful
 */
static app_status_t Board2_InitClocks(void);

/**
 * @brief Initialize debug LED
 * @return APP_STATUS_SUCCESS if successful
 */
static app_status_t Board2_InitDebugLed(void);

/**
 * @brief Update system state
 */
static void Board2_UpdateState(void);

/**
 * @brief Button event callback
 * @param[in] button Button ID
 * @param[in] event Button event type
 */
static void Board2_ButtonCallback(button_id_t button, button_event_t event);

/*******************************************************************************
 * MAIN FUNCTION
 ******************************************************************************/

/**
 * @brief Main function for Board 2
 */
int main(void)
{
    app_status_t status;
    
    /***************************************************************************
     * SYSTEM INITIALIZATION
     **************************************************************************/
    
    /* Initialize system clocks (80MHz) */
    status = Board2_InitClocks();
    if (status != APP_STATUS_SUCCESS) {
        /* Clock initialization failed - enter error state */
        while (1) {
            /* Blink LED fast to indicate error */
        }
    }
    
    /* Initialize debug LED */
    #if DEBUG_LED_ENABLE
    status = Board2_InitDebugLed();
    if (status != APP_STATUS_SUCCESS) {
        /* LED init failed - not critical, continue */
    }
    #endif
    
    /* Initialize UART handler */
    status = Board2_UartHandler_Init();
    if (status != APP_STATUS_SUCCESS) {
        s_systemState = BOARD2_STATE_ERROR;
        while (1) {
            /* UART initialization failed */
        }
    }
    
    /* Initialize CAN handler */
    status = Board2_CanHandler_Init();
    if (status != APP_STATUS_SUCCESS) {
        Board2_UartHandler_SendString("Error: CAN init failed\r\n");
        s_systemState = BOARD2_STATE_ERROR;
        while (1) {
            /* CAN initialization failed */
        }
    }
    
    /* Initialize button handler */
    status = ButtonHandler_Init();
    if (status != APP_STATUS_SUCCESS) {
        Board2_UartHandler_SendString("Error: Button init failed\r\n");
        s_systemState = BOARD2_STATE_ERROR;
        while (1) {
            /* Button initialization failed */
        }
    }
    
    /* Register button callback */
    ButtonHandler_RegisterCallback(Board2_ButtonCallback);
    
    /* Initialization complete - enter idle state */
    s_systemState = BOARD2_STATE_IDLE;
    Board2_UartHandler_SendString("System initialized\r\n\r\n");
    
    /***************************************************************************
     * MAIN LOOP
     **************************************************************************/
    
    while (1) {
        /* Process button events */
        ButtonHandler_Process();
        
        /* Process CAN messages (receive ADC data) */
        Board2_CanHandler_ProcessMessages();
        
        /* Process UART transmission */
        Board2_UartHandler_Process();
        
        /* Update system state */
        Board2_UpdateState();
        
        /* Small delay to prevent busy-wait */
        // Delay_Ms(10);
    }
    
    /* Should never reach here */
    return 0;
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize system clocks
 * 
 * Clock configuration:
 * - System clock: 80 MHz (SPLL)
 * - Bus clock: 40 MHz
 * - Flash clock: 26.67 MHz
 */
static app_status_t Board2_InitClocks(void)
{
    /* Configure SOSC (8 MHz external crystal) */
    // scg_sosc_config_t soscConfig;
    // soscConfig.frequency = 8000000;
    // soscConfig.divider1 = 1;  /* 8 MHz / 1 = 8 MHz */
    // soscConfig.divider2 = 1;  /* 8 MHz / 1 = 8 MHz */
    // SCG_ConfigureSOSC(&soscConfig);
    
    /* Configure SPLL (80 MHz from 8 MHz SOSC) */
    // scg_spll_config_t spllConfig;
    // spllConfig.prediv = 0;    /* Input = 8 MHz / 1 = 8 MHz */
    // spllConfig.mult = 20;     /* VCO = 8 MHz * 20 = 160 MHz */
    // spllConfig.postdiv = 1;   /* Output = 160 MHz / 2 = 80 MHz */
    // spllConfig.divider1 = 1;  /* 80 MHz / 1 = 80 MHz */
    // spllConfig.divider2 = 1;  /* 80 MHz / 1 = 80 MHz */
    // SCG_ConfigureSPLL(&spllConfig);
    
    /* Configure system clock dividers */
    // scg_system_clock_config_t sysClkConfig;
    // sysClkConfig.divCore = 1;     /* Core: 80 MHz / 1 = 80 MHz */
    // sysClkConfig.divBus = 2;      /* Bus: 80 MHz / 2 = 40 MHz */
    // sysClkConfig.divSlow = 3;     /* Flash: 80 MHz / 3 = 26.67 MHz */
    // SCG_SetSystemClockConfig(SCG_SYSTEM_CLOCK_RUN, &sysClkConfig);
    
    /* Switch to SPLL as system clock source */
    // SCG_SetSystemClockSource(SCG_SYSTEM_CLOCK_SRC_SPLL);
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Initialize debug LED
 */
static app_status_t Board2_InitDebugLed(void)
{
    #if DEBUG_LED_ENABLE
    /* Enable PORT clock */
    // PCC_EnableClock(PCC_PORTD_INDEX);
    
    /* Configure pin as GPIO output */
    // PORT_SetPinMux(DEBUG_LED_PORT, DEBUG_LED_PIN, PORT_MUX_GPIO);
    // GPIO_SetPinDirection(DEBUG_LED_GPIO, DEBUG_LED_PIN, GPIO_OUTPUT);
    // GPIO_WritePin(DEBUG_LED_GPIO, DEBUG_LED_PIN, 0);  /* LED off */
    #endif
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Update system state
 */
static void Board2_UpdateState(void)
{
    /* Update state based on CAN data reception */
    if (Board2_CanHandler_IsReceivingData()) {
        s_systemState = BOARD2_STATE_RECEIVING_DATA;
    } else {
        if (s_systemState != BOARD2_STATE_ERROR) {
            s_systemState = BOARD2_STATE_IDLE;
        }
    }
}

/**
 * @brief Button event callback
 */
static void Board2_ButtonCallback(button_id_t button, button_event_t event)
{
    /* Only handle button press events */
    if (event != BUTTON_EVENT_PRESSED) {
        return;
    }
    
    /* Send UART notification */
    if (button == BUTTON_1) {
        Board2_UartHandler_SendString("\r\n>>> Button 1 pressed: Starting ADC...\r\n");
        
        /* LED indication */
        #if DEBUG_LED_ENABLE
        // GPIO_WritePin(DEBUG_LED_GPIO, DEBUG_LED_PIN, 1);  /* LED on */
        #endif
    } else if (button == BUTTON_2) {
        Board2_UartHandler_SendString("\r\n>>> Button 2 pressed: Stopping ADC...\r\n");
        
        /* LED indication */
        #if DEBUG_LED_ENABLE
        // GPIO_WritePin(DEBUG_LED_GPIO, DEBUG_LED_PIN, 0);  /* LED off */
        #endif
    }
}

/*******************************************************************************
 * SYSTEM HOOKS (Optional)
 ******************************************************************************/

/**
 * @brief Hard fault handler (for debugging)
 */
void HardFault_Handler(void)
{
    /* Send error via UART */
    Board2_UartHandler_SendString("\r\n!!! HARD FAULT !!!\r\n");
    
    /* Enter infinite loop on hard fault */
    while (1) {
        #if DEBUG_LED_ENABLE
        /* Blink LED very fast to indicate fault */
        // GPIO_TogglePin(DEBUG_LED_GPIO, DEBUG_LED_PIN);
        // for (volatile int i = 0; i < 100000; i++);
        #endif
    }
}
