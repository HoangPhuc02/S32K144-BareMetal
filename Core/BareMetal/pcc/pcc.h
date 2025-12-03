/*
 * @file    pcc.h
 * @brief   PCC Driver API for S32K144
 */

#ifndef PCC_H_
#define PCC_H_

#include <stdbool.h>
#include "pcc_reg.h"

/**
 * @brief PCC index enumeration.
 *
 * This enum defines the PCC register index for each peripheral.
 */
typedef enum {
    PCC_FlexCAN0_INDEX = 36U,  /**< FlexCAN0 PCC index */
    PCC_FlexCAN1_INDEX = 37U,  /**< FlexCAN1 PCC index */
    PCC_ADC1_INDEX     = 39U,  /**< ADC1 PCC index */
    PCC_FlexCAN2_INDEX = 43U,  /**< FlexCAN2 PCC index */
    PCC_LPIT_INDEX     = 55U,  /**< LPIT PCC index */
    PCC_ADC0_INDEX     = 59U,  /**< ADC0 PCC index */
    PCC_PORTA_INDEX    = 73U,  /**< PORTA PCC index */
    PCC_PORTB_INDEX    = 74U,  /**< PORTB PCC index */
    PCC_PORTC_INDEX    = 75U,  /**< PORTC PCC index */
    PCC_PORTD_INDEX    = 76U,  /**< PORTD PCC index */
    PCC_PORTE_INDEX    = 77U,  /**< PORTE PCC index */
    PCC_LPI2C0_INDEX   = 102U, /**< LPI2C0 PCC index */
    PCC_LPI2C1_INDEX   = 103U, /**< LPI2C1 PCC index */
    PCC_LPUART0_INDEX  = 106U, /**< LPUART0 PCC index */
    PCC_LPUART1_INDEX  = 107U, /**< LPUART1 PCC index */
    PCC_LPUART2_INDEX  = 108U  /**< LPUART2 PCC index */
} pcc_index_t;

/**
 * @brief PCC clock source enumeration.
 *
 * This enum defines the possible clock sources for PCC.
 */
typedef enum {
    PCC_NONE_PERIPHERAL_CLK_SOURCE	= 0U, /**< None peripheral clock source */
    PCC_SOSCDIV2_CLK_SOURCE  		= 1U, /**< SOSC DIV2 clock source */
    PCC_SIRCDIV2_CLK_SOURCE  		= 2U, /**< SIRC DIV2 clock source */
    PCC_FIRCDIV2_CLK_SOURCE  		= 3U, /**< FIRC DIV2 clock source */
    PCC_SPLLDIV2_CLK_SOURCE  		= 6U  /**< SPLL DIV2 clock source */
} pcc_clk_source_t;

/**
 * @brief PCC configuration structure.
 *
 * This structure holds the PCC index, enable flag, and clock source for a peripheral.
 */
typedef struct {
    pcc_index_t       index;   /**< PCC register index for the peripheral */
    bool              enable;  /**< Enable (true) or disable (false) the clock */
    pcc_clk_source_t  source;  /**< Clock source selection */
} pcc_config_t;

void PCC_Config(pcc_config_t *p_config);
#endif /* PCC_H_ */
