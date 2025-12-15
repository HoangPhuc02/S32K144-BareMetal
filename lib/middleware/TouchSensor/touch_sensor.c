/**
 * @file    touch_sensor.c
 * @brief   Touch Sensor Middleware Implementation
 */

#include "touch_sensor.h"
#include "lib/hal/adc/adc.h"
#include "lib/hal/lpit/lpit.h"
#include "lib/hal/pcc/pcc.h"
#include <string.h>

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static touch_channel_t s_touch_channels[TOUCH_MAX_CHANNELS];
static touch_system_config_t s_system_config;
static bool s_is_initialized = false;
static bool s_is_scanning = false;
static uint8_t s_current_scan_channel = 0;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/
static void TOUCH_ProcessChannel(uint8_t channel_id);
static void TOUCH_UpdateBaseline(uint8_t channel_id);
static void TOUCH_ADC_Callback(uint8_t adc_channel, uint16_t raw_value);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

touch_status_t TOUCH_Init(const touch_system_config_t *config)
{
    if (config == NULL || config->num_channels > TOUCH_MAX_CHANNELS) {
        return TOUCH_STATUS_INVALID_PARAM;
    }
    
    /* Clear channel data */
    memset(s_touch_channels, 0, sizeof(s_touch_channels));
    
    /* Save configuration */
    memcpy(&s_system_config, config, sizeof(touch_system_config_t));
    
    /* Initialize ADC based on trigger mode */
    if (config->trigger_mode == TOUCH_TRIGGER_SW) {
        ADC_Init(ADC_INSTANCE_0, NULL);  /* SW trigger mode */
    } else if (config->trigger_mode == TOUCH_TRIGGER_HW_PDB) {
        /* Configure ADC for PDB trigger */
        adc_config_t adc_cfg = {
            .trigger_mode = ADC_TRIGGER_PDB,
            .resolution = ADC_RESOLUTION_12BIT,
            .clock_div = ADC_CLK_DIV_1,
            .enable_dma = false
        };
        ADC_Init(ADC_INSTANCE_0, &adc_cfg);
        
        /* Configure PDB for periodic trigger */
        ADC_ConfigPDB(config->scan_period_us);
    }
    
    s_is_initialized = true;
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_ConfigChannel(uint8_t channel_id, const touch_channel_config_t *config)
{
    if (!s_is_initialized || channel_id >= TOUCH_MAX_CHANNELS || config == NULL) {
        return TOUCH_STATUS_INVALID_PARAM;
    }
    
    touch_channel_t *ch = &s_touch_channels[channel_id];
    
    ch->adc_channel = config->adc_channel;
    ch->baseline = config->baseline;
    ch->threshold = config->threshold;
    ch->debounce_count = config->debounce_count;
    ch->drift_comp_enabled = config->enable_drift_compensation;
    ch->state = TOUCH_STATE_IDLE;
    ch->debounce_counter = 0;
    
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_CalibrateChannel(uint8_t channel_id, uint16_t num_samples)
{
    if (!s_is_initialized || channel_id >= TOUCH_MAX_CHANNELS) {
        return TOUCH_STATUS_INVALID_PARAM;
    }
    
    touch_channel_t *ch = &s_touch_channels[channel_id];
    uint32_t sum = 0;
    
    ch->state = TOUCH_STATE_CALIBRATING;
    
    /* Take multiple samples and average */
    for (uint16_t i = 0; i < num_samples; i++) {
        ADC_StartConversion(ADC_INSTANCE_0, ch->adc_channel);
        while (!ADC_IsConversionComplete(ADC_INSTANCE_0));
        sum += ADC_GetConversionResult(ADC_INSTANCE_0);
    }
    
    ch->baseline = sum / num_samples;
    ch->state = TOUCH_STATE_IDLE;
    
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_CalibrateAll(uint16_t num_samples)
{
    if (!s_is_initialized) {
        return TOUCH_STATUS_NOT_INITIALIZED;
    }
    
    for (uint8_t i = 0; i < s_system_config.num_channels; i++) {
        if (s_touch_channels[i].adc_channel != 0) {
            TOUCH_CalibrateChannel(i, num_samples);
        }
    }
    
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_StartScan(void)
{
    if (!s_is_initialized) {
        return TOUCH_STATUS_NOT_INITIALIZED;
    }
    
    if (s_system_config.trigger_mode == TOUCH_TRIGGER_HW_PDB) {
        ADC_StartPDB();
    }
    
    s_is_scanning = true;
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_StopScan(void)
{
    if (!s_is_initialized) {
        return TOUCH_STATUS_NOT_INITIALIZED;
    }
    
    if (s_system_config.trigger_mode == TOUCH_TRIGGER_HW_PDB) {
        ADC_StopPDB();
    }
    
    s_is_scanning = false;
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_Process(void)
{
    if (!s_is_initialized || !s_is_scanning) {
        return TOUCH_STATUS_NOT_INITIALIZED;
    }
    
    /* For SW trigger mode, manually scan channels */
    if (s_system_config.trigger_mode == TOUCH_TRIGGER_SW) {
        for (uint8_t i = 0; i < s_system_config.num_channels; i++) {
            touch_channel_t *ch = &s_touch_channels[i];
            if (ch->adc_channel != 0) {
                ADC_StartConversion(ADC_INSTANCE_0, ch->adc_channel);
                while (!ADC_IsConversionComplete(ADC_INSTANCE_0));
                ch->raw_value = ADC_GetConversionResult(ADC_INSTANCE_0);
                TOUCH_ProcessChannel(i);
            }
        }
    }
    
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_GetChannelState(uint8_t channel_id, touch_state_t *state)
{
    if (!s_is_initialized || channel_id >= TOUCH_MAX_CHANNELS || state == NULL) {
        return TOUCH_STATUS_INVALID_PARAM;
    }
    
    *state = s_touch_channels[channel_id].state;
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_GetChannelDelta(uint8_t channel_id, int16_t *delta)
{
    if (!s_is_initialized || channel_id >= TOUCH_MAX_CHANNELS || delta == NULL) {
        return TOUCH_STATUS_INVALID_PARAM;
    }
    
    *delta = s_touch_channels[channel_id].delta;
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_GetChannelRaw(uint8_t channel_id, uint16_t *raw_value)
{
    if (!s_is_initialized || channel_id >= TOUCH_MAX_CHANNELS || raw_value == NULL) {
        return TOUCH_STATUS_INVALID_PARAM;
    }
    
    *raw_value = s_touch_channels[channel_id].raw_value;
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_RegisterCallback(touch_callback_t callback)
{
    if (!s_is_initialized) {
        return TOUCH_STATUS_NOT_INITIALIZED;
    }
    
    s_system_config.callback = callback;
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_SetThreshold(uint8_t channel_id, uint16_t threshold)
{
    if (!s_is_initialized || channel_id >= TOUCH_MAX_CHANNELS) {
        return TOUCH_STATUS_INVALID_PARAM;
    }
    
    s_touch_channels[channel_id].threshold = threshold;
    return TOUCH_STATUS_SUCCESS;
}

touch_status_t TOUCH_SetDriftCompensation(uint8_t channel_id, bool enable)
{
    if (!s_is_initialized || channel_id >= TOUCH_MAX_CHANNELS) {
        return TOUCH_STATUS_INVALID_PARAM;
    }
    
    s_touch_channels[channel_id].drift_comp_enabled = enable;
    return TOUCH_STATUS_SUCCESS;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static void TOUCH_ProcessChannel(uint8_t channel_id)
{
    touch_channel_t *ch = &s_touch_channels[channel_id];
    
    /* Calculate delta from baseline */
    ch->delta = (int16_t)ch->baseline - (int16_t)ch->raw_value;
    
    /* Apply drift compensation */
    if (ch->drift_comp_enabled && ch->state == TOUCH_STATE_IDLE) {
        TOUCH_UpdateBaseline(channel_id);
    }
    
    /* Touch detection with debouncing */
    if (ch->delta > ch->threshold) {
        /* Touch detected */
        if (ch->state != TOUCH_STATE_TOUCHED) {
            ch->debounce_counter++;
            if (ch->debounce_counter >= ch->debounce_count) {
                ch->state = TOUCH_STATE_TOUCHED;
                ch->debounce_counter = 0;
                
                /* Trigger callback */
                if (s_system_config.callback != NULL) {
                    s_system_config.callback(channel_id, ch->state, ch->delta);
                }
            }
        }
    } else {
        /* No touch */
        if (ch->state == TOUCH_STATE_TOUCHED) {
            ch->debounce_counter++;
            if (ch->debounce_counter >= ch->debounce_count) {
                ch->state = TOUCH_STATE_RELEASED;
                ch->debounce_counter = 0;
                
                /* Trigger callback */
                if (s_system_config.callback != NULL) {
                    s_system_config.callback(channel_id, ch->state, ch->delta);
                }
                
                ch->state = TOUCH_STATE_IDLE;
            }
        } else {
            ch->debounce_counter = 0;
        }
    }
}

static void TOUCH_UpdateBaseline(uint8_t channel_id)
{
    touch_channel_t *ch = &s_touch_channels[channel_id];
    
    /* Slow baseline drift compensation (IIR filter) */
    /* baseline = baseline * 0.99 + raw_value * 0.01 */
    uint32_t new_baseline = ((uint32_t)ch->baseline * 99 + (uint32_t)ch->raw_value) / 100;
    ch->baseline = (uint16_t)new_baseline;
}

static void TOUCH_ADC_Callback(uint8_t adc_channel, uint16_t raw_value)
{
    /* Called from ADC ISR for HW trigger modes */
    for (uint8_t i = 0; i < s_system_config.num_channels; i++) {
        if (s_touch_channels[i].adc_channel == adc_channel) {
            s_touch_channels[i].raw_value = raw_value;
            TOUCH_ProcessChannel(i);
            break;
        }
    }
}
