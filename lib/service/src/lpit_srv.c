/**
 * @file    lpit_srv.c
 * @brief   LPIT Service Layer Implementation
 * @details Implementation của LPIT service layer, wrapper cho LPIT driver
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../inc/lpit_srv.h"
#include "../../../../Core/BareMetal/lpit/lpit.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static bool s_lpit_initialized = false;
static lpit_srv_callback_t s_callbacks[4] = {NULL, NULL, NULL, NULL};

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/* Callback wrappers for each channel */
static void LPIT_Channel0_Callback(void)
{
    if (s_callbacks[0] != NULL) {
        s_callbacks[0](0);
    }
}

static void LPIT_Channel1_Callback(void)
{
    if (s_callbacks[1] != NULL) {
        s_callbacks[1](1);
    }
}

static void LPIT_Channel2_Callback(void)
{
    if (s_callbacks[2] != NULL) {
        s_callbacks[2](2);
    }
}

static void LPIT_Channel3_Callback(void)
{
    if (s_callbacks[3] != NULL) {
        s_callbacks[3](3);
    }
}

static p_lpit_callback_t LPIT_SRV_GetDriverCallback(uint8_t channel)
{
    switch (channel) {
        case 0: return LPIT_Channel0_Callback;
        case 1: return LPIT_Channel1_Callback;
        case 2: return LPIT_Channel2_Callback;
        case 3: return LPIT_Channel3_Callback;
        default: return NULL;
    }
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

lpit_srv_status_t LPIT_SRV_Init(void)
{
    s_lpit_initialized = true;
    return LPIT_SRV_SUCCESS;
}

lpit_srv_status_t LPIT_SRV_Config(lpit_srv_config_t *config, lpit_srv_callback_t callback)
{
    if (!s_lpit_initialized) {
        return LPIT_SRV_NOT_INITIALIZED;
    }
    
    if (config == NULL || config->channel > 3) {
        return LPIT_SRV_ERROR;
    }
    
    /* Save user callback */
    s_callbacks[config->channel] = callback;
    
    /* Configure LPIT driver */
    lpit_config_value_t lpit_cfg;
    lpit_cfg.source = LPIT_FIRCDIV2_CLK_SOURCE; /* Use FIRC DIV2 as clock source */
    lpit_cfg.channel = (lpit_channel_t)config->channel;
    
    /* Calculate timer value from period_us */
    /* Assuming 48MHz FIRC, DIV2 = 24MHz, 1us = 24 ticks */
    lpit_cfg.value = config->period_us * 24U;
    
    /* Set callback */
    lpit_cfg.func_callback = LPIT_SRV_GetDriverCallback(config->channel);
    
    LPIT_ConfigValue(&lpit_cfg);
    
    config->is_running = false;
    
    return LPIT_SRV_SUCCESS;
}

lpit_srv_status_t LPIT_SRV_Start(uint8_t channel)
{
    if (!s_lpit_initialized) {
        return LPIT_SRV_NOT_INITIALIZED;
    }
    
    if (channel > 3) {
        return LPIT_SRV_ERROR;
    }
    
    LPIT_EnableInterrupt((lpit_channel_t)channel);
    LPIT_StartTimer((lpit_channel_t)channel);
    
    return LPIT_SRV_SUCCESS;
}

lpit_srv_status_t LPIT_SRV_Stop(uint8_t channel)
{
    if (!s_lpit_initialized) {
        return LPIT_SRV_NOT_INITIALIZED;
    }
    
    if (channel > 3) {
        return LPIT_SRV_ERROR;
    }
    
    LPIT_StopTimer((lpit_channel_t)channel);
    LPIT_DisableInterrupt((lpit_channel_t)channel);
    
    return LPIT_SRV_SUCCESS;
}
