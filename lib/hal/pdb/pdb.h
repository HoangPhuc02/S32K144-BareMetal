/**
 * @file    pdb.h
 * @brief   PDB (Programmable Delay Block) Driver API for S32K144
 * @details This driver provides API for configuring and using the PDB module.
 *          PDB is used for precise timing and triggering of ADC conversions.
 * 
 * Features:
 * - Configurable prescaler and multiplication factor for flexible timing
 * - Software and hardware trigger support
 * - Multiple ADC pre-trigger outputs
 * - Continuous or one-shot operation modes
 * - Interrupt and DMA support
 * 
 * @author  PhucPH32
 * @date    14/12/2025
 * @version 1.0
 * 
 * @note    Clock must be enabled via PCC before using PDB
 * @warning PDB counter runs on bus clock frequency
 * 
 * @par Change Log:
 * - Version 1.0 (Dec 14, 2025): Initial PDB driver implementation
 */

#ifndef PDB_H
#define PDB_H

#include <stdint.h>
#include <stdbool.h>
#include "pdb_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief PDB instances */
typedef enum {
    PDB_INSTANCE_0 = 0,
    PDB_INSTANCE_1 = 1
} pdb_instance_t;

/** @brief PDB prescaler */
typedef enum {
    PDB_PRESCALER_1 = 0,
    PDB_PRESCALER_2 = 1,
    PDB_PRESCALER_4 = 2,
    PDB_PRESCALER_8 = 3,
    PDB_PRESCALER_16 = 4,
    PDB_PRESCALER_32 = 5,
    PDB_PRESCALER_64 = 6,
    PDB_PRESCALER_128 = 7
} pdb_prescaler_t;

/** @brief PDB multiplication factor */
typedef enum {
    PDB_MULT_1 = 0,
    PDB_MULT_10 = 1,
    PDB_MULT_20 = 2,
    PDB_MULT_40 = 3
} pdb_mult_t;

/** @brief PDB trigger source */
typedef enum {
    PDB_TRIGGER_SOFTWARE = 15  /* Software trigger */
} pdb_trigger_t;

/** @brief PDB configuration */
typedef struct {
    pdb_prescaler_t prescaler;
    pdb_mult_t mult_factor;
    pdb_trigger_t trigger_source;
    uint16_t mod_value;
    bool continuous_mode;
    bool enable_interrupt;
} pdb_config_t;

/*******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * @brief Initialize PDB with configuration
 */
void PDB_Init(pdb_instance_t instance, const pdb_config_t *config);

/**
 * @brief Enable PDB
 */
void PDB_Enable(pdb_instance_t instance);

/**
 * @brief Disable PDB
 */
void PDB_Disable(pdb_instance_t instance);

/**
 * @brief Configure ADC pre-trigger
 */
void PDB_ConfigADCTrigger(pdb_instance_t instance, uint8_t channel, uint8_t pretrigger, uint16_t delay);

/**
 * @brief Load configuration (LDOK)
 */
void PDB_LoadConfig(pdb_instance_t instance);

/**
 * @brief Software trigger
 */
void PDB_SoftwareTrigger(pdb_instance_t instance);

#endif /* PDB_H */
