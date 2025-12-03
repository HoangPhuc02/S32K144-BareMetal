/*
 * @file    pcc.c
 * @brief   PCC Driver Implementation for S32K144
 */

#include <pcc.h>

/**
 * @brief Configure Peripheral Clock Control (PCC) for a specific module.
 *
 * This function enables or disables the clock for a peripheral and sets its clock source.
 *
 * @param p_config Pointer to pcc_config_t structure containing:
 *        - index: PCC register index for the peripheral.
 *        - source: Clock source selection.
 *        - enable: Boolean flag to enable (true) or disable (false) the clock.
 *
 * @note When enabling, the function sets CGC and PCS fields. When disabling, it clears CGC.
 */
void PCC_Config(pcc_config_t *p_config)
{
    if (p_config->enable)
    {
        /* Enable clock and set source */
        PCC->PCCn[p_config->index] |= PCC_PCCn_CGC_MASK |
                                         PCC_PCCn_PCS(p_config->source);
    }
    else
    {
        /* Disable clock */
        PCC->PCCn[p_config->index] &= ~PCC_PCCn_CGC_MASK;
    }
}



