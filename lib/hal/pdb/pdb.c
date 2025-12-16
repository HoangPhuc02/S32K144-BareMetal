/**
 * @file    pdb.c
 * @brief   PDB Driver Implementation for S32K144
 * @details Implementation of PDB driver functions for ADC triggering
 * 
 * @author  PhucPH32
 * @date    14/12/2025
 * @version 1.0
 */

#include "pdb.h"
#include "lib/hal/pcc/pcc_reg.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Array of PDB base addresses */
static PDB_Type * const s_pdbBases[] = PDB_BASE_PTRS;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Get PDB base address from instance
 * @param instance PDB instance number
 * @return PDB_Type* Pointer to PDB base address
 */
static PDB_Type* PDB_GetBase(pdb_instance_t instance)
{
    return s_pdbBases[instance];
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize PDB module with configuration
 * @param instance PDB instance (PDB_INSTANCE_0 or PDB_INSTANCE_1)
 * @param config Pointer to PDB configuration structure
 */
void PDB_Init(pdb_instance_t instance, const pdb_config_t *config)
{
    PDB_Type *base = PDB_GetBase(instance);
    
    /* Enable PDB clock via PCC */
    uint32_t pcc_idx = (instance == PDB_INSTANCE_0) ? PCC_PDB0_INDEX : PCC_PDB1_INDEX;
    PCC->PCCn[pcc_idx] |= PCC_PCCn_CGC_MASK;
    
    /* Configure PDB Status and Control register */
    base->SC = PDB_SC_PRESCALER(config->prescaler) |
               PDB_SC_MULT(config->mult_factor) |
/**
 * @brief Enable PDB module
 * @param instance PDB instance
 */
void PDB_Enable(pdb_instance_t instance)
{
    PDB_Type *base = PDB_GetBase(instance);
    base->SC |= PDB_SC_PDBEN_MASK;
}

/**
 * @brief Disable PDB module
 * @param instance PDB instance
 */
void PDB_Disable(pdb_instance_t instance)
{
    PDB_Type *base = PDB_GetBase(instance);
    base->SC &= ~PDB_SC_PDBEN_MASK;
}

/**
 * @brief Configure ADC pre-trigger
 * @param instance PDB instance
 * @param channel Channel number (0-1)
 * @param pretrigger Pre-trigger number (0-7)
 * @param delay Delay value for the pre-trigger
 */
void PDB_ConfigADCTrigger(pdb_instance_t instance, uint8_t channel, uint8_t pretrigger, uint16_t delay)
{
    PDB_Type *base = PDB_GetBase(instance);
    
    /* Enable and configure pre-trigger */
    base->CH[channel].C1 |= PDB_C1_EN(1U << pretrigger) | 
                             PDB_C1_TOS(1U << pretrigger);
    
    /* Set pre-trigger delay value */
    base->CH[channel].DLY[pretrigger] = delay;
}

/**
 * @brief Load PDB configuration (set LDOK bit)
 * @param instance PDB instance
 * @note This must be called after configuration changes to load them into buffers
 */
void PDB_LoadConfig(pdb_instance_t instance)
{
    PDB_Type *base = PDB_GetBase(instance);
    base->SC |= PDB_SC_LDOK_MASK;
}

/**
 * @brief Trigger PDB via software
 * @param instance PDB instance
 * @note Only effective when trigger source is set to software trigger
 */
void PDB_SoftwareTrigger(pdb_instance_t instance)
{
    PDB_
void PDB_LoadConfig(pdb_instance_t instance)
{
    PDB_RegType *base = PDB_GetBase(instance);
    base->SC |= PDB_SC_LDOK_MASK;
}

void PDB_SoftwareTrigger(pdb_instance_t instance)
{
    PDB_RegType *base = PDB_GetBase(instance);
    base->SC |= PDB_SC_SWTRIG_MASK;
}
