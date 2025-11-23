/**
 * @file port.c
 * @brief PORT Driver Implementation for S32K144
 * @details This file contains the implementation of PORT driver functions
 *          for pin multiplexing and configuration.
 * 
 * @author [Your Name]
 * @date November 22, 2025
 * @version 1.0
 * 
 * @note This implementation provides PORT configuration functionality
 * @warning Always enable clock before accessing PORT registers
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 22, 2025): Initial version
 * 
 * @copyright Copyright (c) 2025
 * Licensed under [Your License]
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "port.h"
#include "port_reg.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/



/*******************************************************************************
 * Private Variables
 ******************************************************************************/

// /** @brief Array of PORT base addresses */
// static PORT_Type * const s_portBases[] = {
//     (PORT_Type *)PORTA_BASE,
//     (PORT_Type *)PORTB_BASE,
//     (PORT_Type *)PORTC_BASE,
//     (PORT_Type *)PORTD_BASE,
//     (PORT_Type *)PORTE_BASE
// };

// /** @brief Array of PCC indices for PORT modules */
// static const uint8_t s_portPccIndices[] = {
//     PCC_PORTA_INDEX,
//     PCC_PORTB_INDEX,
//     PCC_PORTC_INDEX,
//     PCC_PORTD_INDEX,
//     PCC_PORTE_INDEX
// };

// /** @brief PCC base pointer */
// static PCC_Type * const s_pccBase = (PCC_Type *)PCC_BASE;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Get PORT base address
 * @param[in] port PORT name
 * @return Pointer to PORT register structure
 */
static inline PORT_Type* PORT_GetBase(port_name_t port);

/**
 * @brief Get PCC index for PORT
 * @param[in] port PORT name
 * @return PCC register index
 */
static inline uint8_t PORT_GetPccIndex(port_name_t port);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static inline PORT_Type* PORT_GetBase(port_name_t port)
{
    return s_portBases[port];
}

static inline uint8_t PORT_GetPccIndex(port_name_t port)
{
    return s_portPccIndices[port];
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

void PORT_EnableClock(port_name_t port)
{
    uint8_t pccIndex = PORT_GetPccIndex(port);
    s_pccBase->PCCn[pccIndex] |= PCC_CGC_MASK;
}

void PORT_DisableClock(port_name_t port)
{
    uint8_t pccIndex = PORT_GetPccIndex(port);
    s_pccBase->PCCn[pccIndex] &= ~PCC_CGC_MASK;
}

void PORT_SetPinMux(port_name_t port, uint8_t pin, port_mux_t mux)
{
    PORT_Type *base = PORT_GetBase(port);
    uint32_t pcr = base->PCR[pin];
    
    /* Clear MUX field */
    pcr &= ~PORT_PCR_MUX_MASK;
    
    /* Set new MUX value */
    pcr |= ((uint32_t)mux << PORT_PCR_MUX_SHIFT) & PORT_PCR_MUX_MASK;
    
    base->PCR[pin] = pcr;
}

void PORT_SetPullConfig(port_name_t port, uint8_t pin, port_pull_config_t pull)
{
    PORT_Type *base = PORT_GetBase(port);
    uint32_t pcr = base->PCR[pin];
    
    if (pull == PORT_PULL_DISABLE) {
        /* Disable pull resistor */
        pcr &= ~PORT_PCR_PE_MASK;
    } else {
        /* Enable pull resistor */
        pcr |= PORT_PCR_PE_MASK;
        
        /* Configure pull direction */
        if (pull == PORT_PULL_UP) {
            pcr |= PORT_PCR_PS_MASK;   /* Pull-up */
        } else {
            pcr &= ~PORT_PCR_PS_MASK;  /* Pull-down */
        }
    }
    
    base->PCR[pin] = pcr;
}

void PORT_SetPinInterrupt(port_name_t port, uint8_t pin, port_interrupt_config_t config)
{
    PORT_Type *base = PORT_GetBase(port);
    uint32_t pcr = base->PCR[pin];
    
    /* Clear IRQC field */
    pcr &= ~PORT_PCR_IRQC_MASK;
    
    /* Set new interrupt configuration */
    pcr |= ((uint32_t)config << PORT_PCR_IRQC_SHIFT) & PORT_PCR_IRQC_MASK;
    
    base->PCR[pin] = pcr;
}

void PORT_ClearPinInterruptFlag(port_name_t port, uint8_t pin)
{
    PORT_Type *base = PORT_GetBase(port);
    
    /* Write 1 to clear interrupt flag */
    base->PCR[pin] |= PORT_PCR_ISF_MASK;
}

bool PORT_GetPinInterruptFlag(port_name_t port, uint8_t pin)
{
    PORT_Type *base = PORT_GetBase(port);
    
    return ((base->PCR[pin] & PORT_PCR_ISF_MASK) != 0U);
}

void PORT_ConfigurePin(port_name_t port, uint8_t pin, const port_pin_config_t *config)
{
    if (config == NULL) {
        return;
    }
    
    PORT_Type *base = PORT_GetBase(port);
    uint32_t pcr = 0U;
    
    /* Configure MUX */
    pcr |= ((uint32_t)config->mux << PORT_PCR_MUX_SHIFT) & PORT_PCR_MUX_MASK;
    
    /* Configure pull resistor */
    if (config->pull != PORT_PULL_DISABLE) {
        pcr |= PORT_PCR_PE_MASK;
        if (config->pull == PORT_PULL_UP) {
            pcr |= PORT_PCR_PS_MASK;
        }
    }
    
    /* Configure drive strength */
    if (config->drive == PORT_DRIVE_HIGH) {
        pcr |= PORT_PCR_DSE_MASK;
    }
    
    /* Configure passive filter */
    if (config->passiveFilter) {
        pcr |= PORT_PCR_PFE_MASK;
    }
    
    /* Configure interrupt */
    pcr |= ((uint32_t)config->interrupt << PORT_PCR_IRQC_SHIFT) & PORT_PCR_IRQC_MASK;
    
    /* Write configuration to register */
    base->PCR[pin] = pcr;
    
    /* Configure digital filter if needed */
    if (config->digitalFilter) {
        base->DFER |= (1U << pin);
    } else {
        base->DFER &= ~(1U << pin);
    }
}

void PORT_SetDriveStrength(port_name_t port, uint8_t pin, port_drive_strength_t drive)
{
    PORT_Type *base = PORT_GetBase(port);
    
    if (drive == PORT_DRIVE_HIGH) {
        base->PCR[pin] |= PORT_PCR_DSE_MASK;
    } else {
        base->PCR[pin] &= ~PORT_PCR_DSE_MASK;
    }
}

void PORT_EnablePassiveFilter(port_name_t port, uint8_t pin)
{
    PORT_Type *base = PORT_GetBase(port);
    base->PCR[pin] |= PORT_PCR_PFE_MASK;
}

void PORT_DisablePassiveFilter(port_name_t port, uint8_t pin)
{
    PORT_Type *base = PORT_GetBase(port);
    base->PCR[pin] &= ~PORT_PCR_PFE_MASK;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/