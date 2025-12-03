/**
 * @file board1_main.c
 * @brief Board 1 Main Application
 * @details ADC Sampler Board - Receives CAN commands, samples ADC, sends data
 * 
 * System Overview:
 * - Receives Start/Stop commands via CAN from Board 2
 * - Samples ADC at 1Hz using LPIT timer when active
 * - Sends ADC values via CAN to Board 2
 * - Sends acknowledgements for commands
 * 
 * State Machine:
 * IDLE → (Start CMD) → SAMPLING → (Stop CMD) → IDLE
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#include "board1_adc_task.h"
#include "board1_can_handler.h"
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

/** Board 1 system state */
static board1_state_t s_systemState = BOARD1_STATE_IDLE;

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/

/**
 * @brief Initialize system clocks
 * @return APP_STATUS_SUCCESS if successful
 */
static app_status_t Board1_InitClocks(void);

/**
 * @brief Initialize debug LED
 * @return APP_STATUS_SUCCESS if successful
 */
static app_status_t Board1_InitDebugLed(void);

/**
 * @brief Update system state
 */
static void Board1_UpdateState(void);

/*******************************************************************************
 * MAIN FUNCTION
 ******************************************************************************/

/**
 * @brief Main function for Board 1
 */
int main(void)
{
    app_status_t status;
    
    /***************************************************************************
     * SYSTEM INITIALIZATION
     **************************************************************************/
    
    /* Initialize system clocks (80MHz) */
    status = Board1_InitClocks();
    if (status != APP_STATUS_SUCCESS) {
        /* Clock initialization failed - enter error state */
        while (1) {
            /* Blink LED fast to indicate error */
        }
    }
    
    /* Initialize debug LED */
    #if DEBUG_LED_ENABLE
    status = Board1_InitDebugLed();
    if (status != APP_STATUS_SUCCESS) {
        /* LED init failed - not critical, continue */
    }
    #endif
    
    /* Initialize CAN handler */
    status = CanHandler_Init();
    if (status != APP_STATUS_SUCCESS) {
        s_systemState = BOARD1_STATE_ERROR;
        while (1) {
            /* CAN initialization failed */
        }
    }
    
    /* Initialize ADC task */
    status = AdcTask_Init();
    if (status != APP_STATUS_SUCCESS) {
        s_systemState = BOARD1_STATE_ERROR;
        while (1) {
            /* ADC initialization failed */
        }
    }
    
    /* Initialization complete - enter idle state */
    s_systemState = BOARD1_STATE_IDLE;
    
    /***************************************************************************
     * MAIN LOOP
     **************************************************************************/
    
    while (1) {
        /* Process CAN messages (check for commands) */
        CanHandler_ProcessMessages();
        
        /* Process ADC task (state management) */
        AdcTask_Process();
        
        /* Update system state */
        Board1_UpdateState();
        
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
 * - LPIT clock: 8 MHz (SIRC)
 */
static app_status_t Board1_InitClocks(void)
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
    
    /* Enable SIRC for LPIT (8 MHz) */
    // SCG_EnableSIRC(true);
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Initialize debug LED
 */
static app_status_t Board1_InitDebugLed(void)
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
static void Board1_UpdateState(void)
{
    /* Update state based on ADC task status */
    if (AdcTask_IsRunning()) {
        s_systemState = BOARD1_STATE_ADC_SAMPLING;
    } else {
        if (s_systemState != BOARD1_STATE_ERROR) {
            s_systemState = BOARD1_STATE_IDLE;
        }
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
    /* Enter infinite loop on hard fault */
    while (1) {
        #if DEBUG_LED_ENABLE
        /* Blink LED very fast to indicate fault */
        // GPIO_TogglePin(DEBUG_LED_GPIO, DEBUG_LED_PIN);
        // for (volatile int i = 0; i < 100000; i++);
        #endif
    }
}
