/*
 * @file    lpit.h
 * @brief   LPIT Driver API for S32K144
 */

#ifndef LPIT_H_
#define LPIT_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "bit_ultis.h"
#include "lpit_reg.h"
#include "pcc.h"

#ifndef NVIC_ISER_BASE
/* Configuration NVIC */
typedef struct {
	volatile uint32_t ISERn[8];
} NVIC_ISER_Type;

#define NVIC_ISER_BASE 	(0xE000E100U)
#define NVIC_ISER 		((NVIC_ISER_Type *)NVIC_ISER_BASE)
#define ID_LPIT0_BASE	48U
#endif

/**
 * @brief LPIT channel selection.
 *
 * This enum defines the available LPIT timer channels.
 * Each channel can be configured independently.
 */
typedef enum {
    LPIT_CHANNEL0,  /**< LPIT Channel 0 */
    LPIT_CHANNEL1,  /**< LPIT Channel 1 */
    LPIT_CHANNEL2,  /**< LPIT Channel 2 */
    LPIT_CHANNEL3   /**< LPIT Channel 3 */
} lpit_channel_t;

/**
 * @brief LPIT clock source selection.
 *
 * This enum specifies the possible clock sources for the LPIT module.
 * The clock source is configured via the PCC (Peripheral Clock Control).
 */
typedef enum {
    LPIT_NONE_PERIPHERAL_CLK_SOURCE = 0U, /**< No peripheral clock source */
    LPIT_SOSCDIV2_CLK_SOURCE        = 1U, /**< System OSC DIV2 clock source */
    LPIT_SIRCDIV2_CLK_SOURCE        = 2U, /**< Slow IRC DIV2 clock source */
    LPIT_FIRCDIV2_CLK_SOURCE        = 3U, /**< Fast IRC DIV2 clock source */
    LPIT_SPLLDIV2_CLK_SOURCE        = 6U  /**< System PLL DIV2 clock source */
} lpit_clk_source_t;

/**
 * @brief LPIT callback function type.
 *
 * This type defines the prototype for user-defined callback functions
 * that are called when an LPIT interrupt occurs.
 */
typedef void (*p_lpit_callback_t)(void);

/**
 * @brief LPIT configuration structure.
 *
 * This structure holds the configuration parameters for an LPIT channel.
 */
typedef struct {
    lpit_clk_source_t 	source;      /**< Clock source for LPIT (via PCC) */
    lpit_channel_t    	channel;     /**< LPIT channel number (0�3) */
    uint32_t          	value;       /**< Timer reload value (in ticks) */
    p_lpit_callback_t	func_callback; /**< Pointer to user callback function */
} lpit_config_value_t;


void LPIT_ConfigValue(lpit_config_value_t *p_config);

/* Timer Control Functions - Priority 1 */
void LPIT_StartTimer(lpit_channel_t channel);
void LPIT_StopTimer(lpit_channel_t channel);
void LPIT_ResetTimer(lpit_channel_t channel);
void LPIT_SetTimerValue(lpit_channel_t channel, uint32_t value);

/* Timer Status & Monitoring Functions - Priority 2 */
uint32_t LPIT_GetCurrentValue(lpit_channel_t channel);
bool LPIT_IsTimerRunning(lpit_channel_t channel);
bool LPIT_IsInterruptPending(lpit_channel_t channel);
uint32_t LPIT_GetTimeElapsed(lpit_channel_t channel);

/* Interrupt Management Functions - Priority 3 */
void LPIT_EnableInterrupt(lpit_channel_t channel);
void LPIT_DisableInterrupt(lpit_channel_t channel);
void LPIT_ClearInterruptFlag(lpit_channel_t channel);

#endif /* LPIT_H_ */
