/**
 * @file    clock_srv_v2.c
 * @brief   Clock Service Layer V2 Implementation
 * @details Enhanced implementation với flexible configuration
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 2.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../inc/clock_srv_v2.h"
#include "../../../../Core/BareMetal/scg/scg.h"
#include "../../../../Core/BareMetal/pcc/pcc.h"
#include <string.h>

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static bool s_clock_initialized = false;
static clock_srv_v2_config_t s_current_config;
static clock_srv_v2_frequencies_t s_current_freq;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Convert service divider enum to driver enum
 */
static uint8_t ConvertDivider(clock_srv_v2_divider_t div)
{
    return (uint8_t)div;
}

/**
 * @brief Calculate actual frequency after divider
 */
static uint32_t ApplyDivider(uint32_t freq, clock_srv_v2_divider_t div)
{
    if (div == CLOCK_SRV_V2_DIV_DISABLE) return 0;
    uint32_t div_value = (1U << ((uint8_t)div - 1));
    return freq / div_value;
}

/**
 * @brief Update frequency tracking
 */
static void UpdateFrequencies(void)
{
    /* Calculate SOSC frequency */
    if (s_current_config.sosc.enable) {
        s_current_freq.sosc_hz = s_current_config.sosc.freq_hz;
    } else {
        s_current_freq.sosc_hz = 0;
    }
    
    /* Calculate SIRC frequency */
    if (s_current_config.sirc.enable) {
        s_current_freq.sirc_hz = s_current_config.sirc.use_8mhz ? 8000000 : 2000000;
    } else {
        s_current_freq.sirc_hz = 0;
    }
    
    /* Calculate FIRC frequency */
    if (s_current_config.firc.enable) {
        s_current_freq.firc_hz = 48000000;
    } else {
        s_current_freq.firc_hz = 0;
    }
    
    /* Calculate SPLL frequency */
    if (s_current_config.spll.enable) {
        uint32_t spll_input = s_current_freq.sosc_hz / (s_current_config.spll.prediv + 1);
        s_current_freq.spll_hz = spll_input * s_current_config.spll.mult;
    } else {
        s_current_freq.spll_hz = 0;
    }
    
    /* Calculate system clocks based on source */
    uint32_t sys_clk = 0;
    switch (s_current_config.sys.source) {
        case CLOCK_SRV_V2_SOURCE_SOSC:
            sys_clk = s_current_freq.sosc_hz;
            break;
        case CLOCK_SRV_V2_SOURCE_SIRC:
            sys_clk = s_current_freq.sirc_hz;
            break;
        case CLOCK_SRV_V2_SOURCE_FIRC:
            sys_clk = s_current_freq.firc_hz;
            break;
        case CLOCK_SRV_V2_SOURCE_SPLL:
            sys_clk = s_current_freq.spll_hz;
            break;
        default:
            sys_clk = 0;
            break;
    }
    
    s_current_freq.core_hz = ApplyDivider(sys_clk, s_current_config.sys.core_div);
    s_current_freq.bus_hz = ApplyDivider(sys_clk, s_current_config.sys.bus_div);
    s_current_freq.slow_hz = ApplyDivider(sys_clk, s_current_config.sys.slow_div);
}

/**
 * @brief Map peripheral enum to PCC index
 */
static uint8_t GetPCCIndex(clock_srv_v2_peripheral_t peripheral)
{
    switch (peripheral) {
        case CLOCK_SRV_V2_PORTA:    return PCC_PORTA_INDEX;
        case CLOCK_SRV_V2_PORTB:    return PCC_PORTB_INDEX;
        case CLOCK_SRV_V2_PORTC:    return PCC_PORTC_INDEX;
        case CLOCK_SRV_V2_PORTD:    return PCC_PORTD_INDEX;
        case CLOCK_SRV_V2_PORTE:    return PCC_PORTE_INDEX;
        case CLOCK_SRV_V2_LPIT:     return PCC_LPIT_INDEX;
        case CLOCK_SRV_V2_ADC0:     return PCC_ADC0_INDEX;
        case CLOCK_SRV_V2_ADC1:     return PCC_ADC1_INDEX;
        case CLOCK_SRV_V2_FLEXCAN0: return PCC_FlexCAN0_INDEX;
        case CLOCK_SRV_V2_FLEXCAN1: return PCC_FlexCAN1_INDEX;
        case CLOCK_SRV_V2_FLEXCAN2: return PCC_FlexCAN2_INDEX;
        case CLOCK_SRV_V2_LPUART0:  return PCC_LPUART0_INDEX;
        case CLOCK_SRV_V2_LPUART1:  return PCC_LPUART1_INDEX;
        case CLOCK_SRV_V2_LPUART2:  return PCC_LPUART2_INDEX;
        default: return 0;
    }
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

clock_srv_v2_status_t CLOCK_SRV_V2_Init(const clock_srv_v2_config_t *config)
{
    if (config == NULL) {
        return CLOCK_SRV_V2_ERROR;
    }
    
    /* Validate configuration */
    if (CLOCK_SRV_V2_ValidateConfig(config) != CLOCK_SRV_V2_SUCCESS) {
        return CLOCK_SRV_V2_INVALID_CONFIG;
    }
    
    /* Save configuration */
    memcpy(&s_current_config, config, sizeof(clock_srv_v2_config_t));
    
    /* 1. Configure SOSC if enabled */
    if (config->sosc.enable) {
        scg_sosc_config_t sosc_cfg = {
            .range = (scg_sosc_range_t)config->sosc.range,
            .source = config->sosc.use_external_ref ? SCG_SOSC_SOURCE_EXTERNAL : SCG_SOSC_SOURCE_INTERNAL,
            .divider1 = ConvertDivider(config->sosc.div1),
            .divider2 = ConvertDivider(config->sosc.div2)
        };
        SCG_SOSCInit(&sosc_cfg);
    }
    
    /* 2. Configure SIRC if enabled */
    if (config->sirc.enable) {
        scg_sirc_config_t sirc_cfg = {
            .divider1 = ConvertDivider(config->sirc.div1),
            .divider2 = ConvertDivider(config->sirc.div2)
        };
        SCG_SIRCInit(&sirc_cfg);
    }
    
    /* 3. Configure FIRC if enabled */
    if (config->firc.enable) {
        scg_firc_config_t firc_cfg = {
            .divider1 = ConvertDivider(config->firc.div1),
            .divider2 = ConvertDivider(config->firc.div2)
        };
        SCG_FIRCInit(&firc_cfg);
    }
    
    /* 4. Configure SPLL if enabled */
    if (config->spll.enable) {
        scg_spll_config_t spll_cfg = {
            .prediv = (scg_spll_prediv_t)config->spll.prediv,
            .multi = (scg_spll_multi_t)config->spll.mult,
            .divider1 = ConvertDivider(config->spll.div1),
            .divider2 = ConvertDivider(config->spll.div2)
        };
        SCG_SPLLInit(&spll_cfg);
    }
    
    /* 5. Configure system clock */
    scg_rccr_config_t sys_cfg = {
        .scs = (scg_system_clock_src_t)config->sys.source,
        .divcore = ConvertDivider(config->sys.core_div),
        .divbus = ConvertDivider(config->sys.bus_div),
        .divslow = ConvertDivider(config->sys.slow_div)
    };
    SCG_SysClkConfig(&sys_cfg);
    
    /* Update frequency tracking */
    UpdateFrequencies();
    
    s_clock_initialized = true;
    
    return CLOCK_SRV_V2_SUCCESS;
}

clock_srv_v2_status_t CLOCK_SRV_V2_InitPreset(const char *preset_name)
{
    clock_srv_v2_config_t preset_cfg = {0};
    
    if (strcmp(preset_name, "RUN_80MHz") == 0) {
        /* SOSC 8MHz -> SPLL x20 = 160MHz -> DIV2 = 80MHz */
        preset_cfg.sosc = CLOCK_SRV_V2_SOSC_DEFAULT();
        preset_cfg.spll = CLOCK_SRV_V2_SPLL_80MHZ();
        preset_cfg.sys.source = CLOCK_SRV_V2_SOURCE_SPLL;
        preset_cfg.sys.core_div = CLOCK_SRV_V2_DIV_1;
        preset_cfg.sys.bus_div = CLOCK_SRV_V2_DIV_2;
        preset_cfg.sys.slow_div = CLOCK_SRV_V2_DIV_2;
    }
    else if (strcmp(preset_name, "RUN_48MHz") == 0) {
        /* FIRC 48MHz */
        preset_cfg.firc = CLOCK_SRV_V2_FIRC_DEFAULT();
        preset_cfg.sys.source = CLOCK_SRV_V2_SOURCE_FIRC;
        preset_cfg.sys.core_div = CLOCK_SRV_V2_DIV_1;
        preset_cfg.sys.bus_div = CLOCK_SRV_V2_DIV_1;
        preset_cfg.sys.slow_div = CLOCK_SRV_V2_DIV_2;
    }
    else if (strcmp(preset_name, "HSRUN_112MHz") == 0) {
        /* SOSC 8MHz / 2 * 28 = 112MHz */
        preset_cfg.sosc = CLOCK_SRV_V2_SOSC_DEFAULT();
        preset_cfg.spll.enable = true;
        preset_cfg.spll.prediv = CLOCK_SRV_V2_SPLL_PREDIV_2;
        preset_cfg.spll.mult = 28;
        preset_cfg.spll.div1 = CLOCK_SRV_V2_DIV_1;
        preset_cfg.spll.div2 = CLOCK_SRV_V2_DIV_2;
        preset_cfg.sys.source = CLOCK_SRV_V2_SOURCE_SPLL;
        preset_cfg.sys.core_div = CLOCK_SRV_V2_DIV_1;
        preset_cfg.sys.bus_div = CLOCK_SRV_V2_DIV_2;
        preset_cfg.sys.slow_div = CLOCK_SRV_V2_DIV_4;
    }
    else if (strcmp(preset_name, "VLPR_4MHz") == 0) {
        /* SIRC 8MHz / 2 = 4MHz */
        preset_cfg.sirc.enable = true;
        preset_cfg.sirc.use_8mhz = true;
        preset_cfg.sirc.div1 = CLOCK_SRV_V2_DIV_1;
        preset_cfg.sirc.div2 = CLOCK_SRV_V2_DIV_1;
        preset_cfg.sys.source = CLOCK_SRV_V2_SOURCE_SIRC;
        preset_cfg.sys.core_div = CLOCK_SRV_V2_DIV_2;
        preset_cfg.sys.bus_div = CLOCK_SRV_V2_DIV_2;
        preset_cfg.sys.slow_div = CLOCK_SRV_V2_DIV_2;
    }
    else {
        return CLOCK_SRV_V2_INVALID_CONFIG;
    }
    
    return CLOCK_SRV_V2_Init(&preset_cfg);
}

clock_srv_v2_status_t CLOCK_SRV_V2_GetFrequencies(clock_srv_v2_frequencies_t *freq)
{
    if (!s_clock_initialized) {
        return CLOCK_SRV_V2_NOT_INITIALIZED;
    }
    
    if (freq == NULL) {
        return CLOCK_SRV_V2_ERROR;
    }
    
    memcpy(freq, &s_current_freq, sizeof(clock_srv_v2_frequencies_t));
    
    return CLOCK_SRV_V2_SUCCESS;
}

clock_srv_v2_status_t CLOCK_SRV_V2_EnablePeripheral(clock_srv_v2_peripheral_t peripheral, clock_srv_v2_pcs_t pcs)
{
    if (!s_clock_initialized) {
        return CLOCK_SRV_V2_NOT_INITIALIZED;
    }
    
    uint8_t pcc_index = GetPCCIndex(peripheral);
    
    /* Enable clock with source selection */
    PCC_EnableClock(pcc_index);
    
    /* Set peripheral clock source if applicable */
    if (pcs != CLOCK_SRV_V2_PCS_NONE) {
        PCC_SetClockSource(pcc_index, (uint8_t)pcs);
    }
    
    return CLOCK_SRV_V2_SUCCESS;
}

clock_srv_v2_status_t CLOCK_SRV_V2_DisablePeripheral(clock_srv_v2_peripheral_t peripheral)
{
    if (!s_clock_initialized) {
        return CLOCK_SRV_V2_NOT_INITIALIZED;
    }
    
    uint8_t pcc_index = GetPCCIndex(peripheral);
    
    PCC_DisableClock(pcc_index);
    
    return CLOCK_SRV_V2_SUCCESS;
}

clock_srv_v2_status_t CLOCK_SRV_V2_ValidateConfig(const clock_srv_v2_config_t *config)
{
    if (config == NULL) {
        return CLOCK_SRV_V2_ERROR;
    }
    
    /* Check SPLL configuration */
    if (config->spll.enable) {
        if (config->spll.mult < 16 || config->spll.mult > 47) {
            return CLOCK_SRV_V2_INVALID_CONFIG;
        }
        if (!config->sosc.enable) {
            return CLOCK_SRV_V2_INVALID_CONFIG; /* SPLL needs SOSC */
        }
    }
    
    /* Check system clock source is enabled */
    switch (config->sys.source) {
        case CLOCK_SRV_V2_SOURCE_SOSC:
            if (!config->sosc.enable) return CLOCK_SRV_V2_INVALID_CONFIG;
            break;
        case CLOCK_SRV_V2_SOURCE_SIRC:
            if (!config->sirc.enable) return CLOCK_SRV_V2_INVALID_CONFIG;
            break;
        case CLOCK_SRV_V2_SOURCE_FIRC:
            if (!config->firc.enable) return CLOCK_SRV_V2_INVALID_CONFIG;
            break;
        case CLOCK_SRV_V2_SOURCE_SPLL:
            if (!config->spll.enable) return CLOCK_SRV_V2_INVALID_CONFIG;
            break;
        default:
            return CLOCK_SRV_V2_INVALID_CONFIG;
    }
    
    return CLOCK_SRV_V2_SUCCESS;
}

clock_srv_v2_status_t CLOCK_SRV_V2_GetConfig(clock_srv_v2_config_t *config)
{
    if (!s_clock_initialized) {
        return CLOCK_SRV_V2_NOT_INITIALIZED;
    }
    
    if (config == NULL) {
        return CLOCK_SRV_V2_ERROR;
    }
    
    memcpy(config, &s_current_config, sizeof(clock_srv_v2_config_t));
    
    return CLOCK_SRV_V2_SUCCESS;
}

uint32_t CLOCK_SRV_V2_CalculateSPLLFreq(uint32_t sosc_freq, uint8_t prediv, uint8_t mult)
{
    return (sosc_freq / (prediv + 1)) * mult;
}
