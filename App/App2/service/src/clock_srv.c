/**
 * @file    clock_srv.c
 * @brief   Clock Service Layer Implementation
 * @details Implementation của Clock service layer, wrapper cho SCG driver
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../inc/clock_srv.h"
#include "../../../../Core/BareMetal/scg/scg.h"
#include "../../../../Core/BareMetal/pcc/pcc.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static bool s_clock_initialized = false;
static clock_srv_config_t s_current_config;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Configure clock for RUN mode 80MHz (SPLL)
 * @details SOSC 8MHz -> SPLL (x40) -> 160MHz -> DIV2 -> 80MHz
 */
static void CLOCK_SRV_ConfigRUN_80MHz(void)
{
    /* 1. Configure SOSC (8 MHz external crystal) */
    scg_sosc_config_t sosc_cfg = {
        .range = HIGH_FREQ,
        .source = SCG_SOSC_SOURCE_INTERNAL,
        .divider1 = DIV_BY_1,
        .divider2 = DIV_BY_1
    };
    SCG_SOSCInit(&sosc_cfg);
    
    /* 2. Configure SPLL: 8MHz / 1 * 20 = 160MHz */
    scg_spll_config_t spll_cfg = {
        .prediv = SCG_SPLL_PREDIV_DIV1,
        .multi = SCG_SPLL_MULT_20,
        .divider1 = DIV_BY_2,  /* 160/2 = 80MHz */
        .divider2 = DIV_BY_4   /* 160/4 = 40MHz */
    };
    SCG_SPLLInit(&spll_cfg);
    
    /* 3. Switch to SPLL as system clock */
    scg_rccr_config_t rccr_cfg = {
        .scs = SCG_SYSTEM_CLOCK_SRC_SYS_PLL,
        .divcore = DIV_BY_1,   /* Core = 80MHz */
        .divbus = DIV_BY_2,    /* Bus = 40MHz */
        .divslow = DIV_BY_2    /* Flash = 40MHz (max 26.67MHz in RUN) */
    };
    SCG_SysClkConfig(&rccr_cfg);
    
    /* Update configuration */
    s_current_config.mode = CLOCK_SRV_MODE_RUN_80MHZ;
    s_current_config.core_clock_hz = 80000000;
    s_current_config.bus_clock_hz = 40000000;
    s_current_config.flash_clock_hz = 40000000;
}

/**
 * @brief Configure clock for RUN mode 48MHz (FIRC)
 * @details FIRC 48MHz -> Core 48MHz
 */
static void CLOCK_SRV_ConfigRUN_48MHz(void)
{
    /* 1. Configure FIRC (48 MHz internal) */
    scg_firc_config_t firc_cfg = {
        .divider1 = DIV_BY_1,
        .divider2 = DIV_BY_1
    };
    SCG_FIRCInit(&firc_cfg);
    
    /* 2. Switch to FIRC as system clock */
    scg_rccr_config_t rccr_cfg = {
        .scs = SCG_SYSTEM_CLOCK_SRC_FIRC,
        .divcore = DIV_BY_1,   /* Core = 48MHz */
        .divbus = DIV_BY_1,    /* Bus = 48MHz */
        .divslow = DIV_BY_2    /* Flash = 24MHz */
    };
    SCG_SysClkConfig(&rccr_cfg);
    
    /* Update configuration */
    s_current_config.mode = CLOCK_SRV_MODE_RUN_48MHZ;
    s_current_config.core_clock_hz = 48000000;
    s_current_config.bus_clock_hz = 48000000;
    s_current_config.flash_clock_hz = 24000000;
}

/**
 * @brief Configure clock for HSRUN mode 112MHz (SPLL)
 * @details SOSC 8MHz -> SPLL (x28) -> 112MHz
 */
static void CLOCK_SRV_ConfigHSRUN_112MHz(void)
{
    /* 1. Configure SOSC (8 MHz external crystal) */
    scg_sosc_config_t sosc_cfg = {
        .range = HIGH_FREQ,
        .source = SCG_SOSC_SOURCE_INTERNAL,
        .divider1 = DIV_BY_1,
        .divider2 = DIV_BY_1
    };
    SCG_SOSCInit(&sosc_cfg);
    
    /* 2. Configure SPLL: 8MHz / 2 * 28 = 112MHz */
    scg_spll_config_t spll_cfg = {
        .prediv = SCG_SPLL_PREDIV_DIV2,
        .multi = SCG_SPLL_MULT_28,
        .divider1 = DIV_BY_1,  /* 112MHz */
        .divider2 = DIV_BY_2   /* 56MHz */
    };
    SCG_SPLLInit(&spll_cfg);
    
    /* 3. Switch to HSRUN mode first (if not already) */
    /* Note: Requires SMC configuration - not implemented here */
    
    /* 4. Configure system clock */
    scg_rccr_config_t rccr_cfg = {
        .scs = SCG_SYSTEM_CLOCK_SRC_SYS_PLL,
        .divcore = DIV_BY_1,   /* Core = 112MHz */
        .divbus = DIV_BY_2,    /* Bus = 56MHz */
        .divslow = DIV_BY_4    /* Flash = 28MHz (max 28MHz in HSRUN) */
    };
    SCG_SysClkConfig(&rccr_cfg);
    
    /* Update configuration */
    s_current_config.mode = CLOCK_SRV_MODE_HSRUN_112MHZ;
    s_current_config.core_clock_hz = 112000000;
    s_current_config.bus_clock_hz = 56000000;
    s_current_config.flash_clock_hz = 28000000;
}

/**
 * @brief Configure clock for VLPR mode 4MHz (SIRC)
 * @details SIRC 8MHz -> DIV2 -> 4MHz
 */
static void CLOCK_SRV_ConfigVLPR_4MHz(void)
{
    /* 1. Configure SIRC (8 MHz internal) */
    scg_sirc_config_t sirc_cfg = {
        .divider1 = DIV_BY_1,
        .divider2 = DIV_BY_1
    };
    SCG_SIRCInit(&sirc_cfg);
    
    /* 2. Switch to VLPR mode first (if not already) */
    /* Note: Requires SMC configuration - not implemented here */
    
    /* 3. Configure system clock */
    scg_rccr_config_t rccr_cfg = {
        .scs = SCG_SYSTEM_CLOCK_SRC_SIRC,
        .divcore = DIV_BY_2,   /* Core = 4MHz */
        .divbus = DIV_BY_2,    /* Bus = 4MHz */
        .divslow = DIV_BY_2    /* Flash = 4MHz */
    };
    SCG_SysClkConfig(&rccr_cfg);
    
    /* Update configuration */
    s_current_config.mode = CLOCK_SRV_MODE_VLPR_4MHZ;
    s_current_config.core_clock_hz = 4000000;
    s_current_config.bus_clock_hz = 4000000;
    s_current_config.flash_clock_hz = 4000000;
}

/**
 * @brief Get PCC index from peripheral enum
 */
static uint8_t CLOCK_SRV_GetPCCIndex(clock_srv_peripheral_t peripheral)
{
    switch (peripheral) {
        case CLOCK_SRV_PCC_PORTA:    return PCC_PORTA_INDEX;
        case CLOCK_SRV_PCC_PORTB:    return PCC_PORTB_INDEX;
        case CLOCK_SRV_PCC_PORTC:    return PCC_PORTC_INDEX;
        case CLOCK_SRV_PCC_PORTD:    return PCC_PORTD_INDEX;
        case CLOCK_SRV_PCC_PORTE:    return PCC_PORTE_INDEX;
        case CLOCK_SRV_PCC_LPIT:     return PCC_LPIT_INDEX;
        case CLOCK_SRV_PCC_ADC0:     return PCC_ADC0_INDEX;
        case CLOCK_SRV_PCC_ADC1:     return PCC_ADC1_INDEX;
        case CLOCK_SRV_PCC_FLEXCAN0: return PCC_FlexCAN0_INDEX;
        case CLOCK_SRV_PCC_FLEXCAN1: return PCC_FlexCAN1_INDEX;
        case CLOCK_SRV_PCC_FLEXCAN2: return PCC_FlexCAN2_INDEX;
        case CLOCK_SRV_PCC_LPUART0:  return PCC_LPUART0_INDEX;
        case CLOCK_SRV_PCC_LPUART1:  return PCC_LPUART1_INDEX;
        case CLOCK_SRV_PCC_LPUART2:  return PCC_LPUART2_INDEX;
        default: return 0;
    }
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

clock_srv_status_t CLOCK_SRV_Init(clock_srv_mode_t mode)
{
    switch (mode) {
        case CLOCK_SRV_MODE_RUN_80MHZ:
            CLOCK_SRV_ConfigRUN_80MHz();
            break;
            
        case CLOCK_SRV_MODE_RUN_48MHZ:
            CLOCK_SRV_ConfigRUN_48MHz();
            break;
            
        case CLOCK_SRV_MODE_HSRUN_112MHZ:
            CLOCK_SRV_ConfigHSRUN_112MHz();
            break;
            
        case CLOCK_SRV_MODE_VLPR_4MHZ:
            CLOCK_SRV_ConfigVLPR_4MHz();
            break;
            
        default:
            return CLOCK_SRV_ERROR;
    }
    
    s_clock_initialized = true;
    
    return CLOCK_SRV_SUCCESS;
}

clock_srv_status_t CLOCK_SRV_GetConfig(clock_srv_config_t *config)
{
    if (!s_clock_initialized) {
        return CLOCK_SRV_NOT_INITIALIZED;
    }
    
    if (config == NULL) {
        return CLOCK_SRV_ERROR;
    }
    
    *config = s_current_config;
    
    return CLOCK_SRV_SUCCESS;
}

clock_srv_status_t CLOCK_SRV_EnablePeripheral(clock_srv_peripheral_t peripheral)
{
    if (!s_clock_initialized) {
        return CLOCK_SRV_NOT_INITIALIZED;
    }
    
    uint8_t pcc_index = CLOCK_SRV_GetPCCIndex(peripheral);
    
    PCC_EnableClock(pcc_index);
    
    return CLOCK_SRV_SUCCESS;
}

clock_srv_status_t CLOCK_SRV_DisablePeripheral(clock_srv_peripheral_t peripheral)
{
    if (!s_clock_initialized) {
        return CLOCK_SRV_NOT_INITIALIZED;
    }
    
    uint8_t pcc_index = CLOCK_SRV_GetPCCIndex(peripheral);
    
    PCC_DisableClock(pcc_index);
    
    return CLOCK_SRV_SUCCESS;
}

uint32_t CLOCK_SRV_GetCoreFreq(void)
{
    return s_current_config.core_clock_hz;
}

uint32_t CLOCK_SRV_GetBusFreq(void)
{
    return s_current_config.bus_clock_hz;
}
