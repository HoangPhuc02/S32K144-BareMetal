/**
 * @file    port_reg.h
 * @brief   PORT Register Definitions for S32K144
 * @details This file contains low-level PORT register definitions and macros
 *          for direct hardware access including pin muxing and configuration.
 * 
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 * 
 * @note    These are raw register definitions for PORT peripheral
 * @warning Direct register access - use with caution
 * 
 * @par     Change Log:
 * - Version 1.0 (Nov 22, 2025): Initial version
 * 
 */

#ifndef PORT_REG_H
#define PORT_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include "def_reg.h"


/*******************************************************************************
 * PORT Register Structure
 ******************************************************************************/

/** @brief PORT - Size of Registers Arrays */
#define PORT_PCRn_COUNT        (32U)  /**< Number of PCR registers per PORT */

/**
 * @brief PORT register structure definition
 */
typedef struct {
    __IO uint32_t PCR[PORT_PCRn_COUNT];     /**< Offset: 0x000-0x07C - Pin Control Register array */
    __O  uint32_t GPCLR;                    /**< Offset: 0x080 - Global Pin Control Low Register */
    __O  uint32_t GPCHR;                    /**< Offset: 0x084 - Global Pin Control High Register */
    __O  uint32_t GICLR;                    /**< Offset: 0x088 - Global Interrupt Control Low Register */
    __O  uint32_t GICHR;                    /**< Offset: 0x08C - Global Interrupt Control High Register */
         uint32_t RESERVED_0[4];            /**< Reserved */
    __IO uint32_t ISFR;                     /**< Offset: 0x0A0 - Interrupt Status Flag Register */
         uint32_t RESERVED_1[7];            /**< Reserved */
    __IO uint32_t DFER;                 /**< Offset: 0x0C0 - Digital Filter Enable Register */
    __IO uint32_t DFCR;                 /**< Offset: 0x0C4 - Digital Filter Clock Register */
    __IO uint32_t DFWR;                 /**< Offset: 0x0C8 - Digital Filter Width Register */
} PORT_Type;

 /** Number of instances of the PORT module. */
#define PORT_INSTANCE_COUNT                      (5u)

/*******************************************************************************
 * PORT Base Addresses
 ******************************************************************************/

/** @brief PORT A base address */
#define PORTA_BASE_ADDR         (0x40049000UL)

/** @brief PORT B base address */
#define PORTB_BASE_ADDR         (0x4004A000UL)

/** @brief PORT C base address */
#define PORTC_BASE_ADDR         (0x4004B000UL)

/** @brief PORT D base address */
#define PORTD_BASE_ADDR         (0x4004C000UL)

/** @brief PORT E base address */
#define PORTE_BASE_ADDR         (0x4004D000UL)

/** Array initializer of PORT peripheral base addresses */
#define PORT_BASE_ADDRS         { PORTA_BASE_ADDR, PORTB_BASE_ADDR, PORTC_BASE_ADDR, PORTD_BASE_ADDR, PORTE_BASE_ADDR }

/*******************************************************************************
 * PORT Register Pointers
 ******************************************************************************/

/** @brief PORT A register pointer */
#define PORTA                   ((PORT_Type *)PORTA_BASE_ADDR)

/** @brief PORT B register pointer */
#define PORTB                   ((PORT_Type *)PORTB_BASE_ADDR)

/** @brief PORT C register pointer */
#define PORTC                   ((PORT_Type *)PORTC_BASE_ADDR)

/** @brief PORT D register pointer */
#define PORTD                   ((PORT_Type *)PORTD_BASE_ADDR)

/** @brief PORT E register pointer */
#define PORTE                   ((PORT_Type *)PORTE_BASE_ADDR)

/** Array initializer of PORT peripheral base pointers */
#define PORT_BASE_PTRS          { PORTA, PORTB, PORTC, PORTD, PORTE }

/*******************************************************************************
 * PCR (Pin Control Register) Bit Definitions
 ******************************************************************************/

/* Pull Select (PS) */
#define PORT_PCR_PS_MASK                         0x1u
#define PORT_PCR_PS_SHIFT                        0u
#define PORT_PCR_PS_WIDTH                        1u
#define PORT_PCR_PS(x)                           (((uint32_t)(((uint32_t)(x))<<PORT_PCR_PS_SHIFT))&PORT_PCR_PS_MASK)

/* Pull Enable (PE) */
#define PORT_PCR_PE_MASK                         0x2u
#define PORT_PCR_PE_SHIFT                        1u
#define PORT_PCR_PE_WIDTH                        1u
#define PORT_PCR_PE(x)                           (((uint32_t)(((uint32_t)(x))<<PORT_PCR_PE_SHIFT))&PORT_PCR_PE_MASK)

/* Passive Filter Enable (PFE) */
#define PORT_PCR_PFE_MASK                        0x10u
#define PORT_PCR_PFE_SHIFT                       4u
#define PORT_PCR_PFE_WIDTH                       1u
#define PORT_PCR_PFE(x)                          (((uint32_t)(((uint32_t)(x))<<PORT_PCR_PFE_SHIFT))&PORT_PCR_PFE_MASK)

/* Drive Strength Enable (DSE) */
#define PORT_PCR_DSE_MASK                        0x40u
#define PORT_PCR_DSE_SHIFT                       6u
#define PORT_PCR_DSE_WIDTH                       1u
#define PORT_PCR_DSE(x)                          (((uint32_t)(((uint32_t)(x))<<PORT_PCR_DSE_SHIFT))&PORT_PCR_DSE_MASK)

/* Pin Mux Control (MUX) */
#define PORT_PCR_MUX_MASK                        0x700u
#define PORT_PCR_MUX_SHIFT                       8u
#define PORT_PCR_MUX_WIDTH                       3u
#define PORT_PCR_MUX(x)                          (((uint32_t)(((uint32_t)(x))<<PORT_PCR_MUX_SHIFT))&PORT_PCR_MUX_MASK)

/* Lock Register (LK) */
#define PORT_PCR_LK_MASK                         0x8000u
#define PORT_PCR_LK_SHIFT                        15u
#define PORT_PCR_LK_WIDTH                        1u
#define PORT_PCR_LK(x)                           (((uint32_t)(((uint32_t)(x))<<PORT_PCR_LK_SHIFT))&PORT_PCR_LK_MASK)

/* Interrupt Configuration (IRQC) */
#define PORT_PCR_IRQC_MASK                       0xF0000u
#define PORT_PCR_IRQC_SHIFT                      16u
#define PORT_PCR_IRQC_WIDTH                      4u
#define PORT_PCR_IRQC(x)                         (((uint32_t)(((uint32_t)(x))<<PORT_PCR_IRQC_SHIFT))&PORT_PCR_IRQC_MASK)

/* Interrupt Status Flag (ISF) */
#define PORT_PCR_ISF_MASK                        0x1000000u
#define PORT_PCR_ISF_SHIFT                       24u
#define PORT_PCR_ISF_WIDTH                       1u
#define PORT_PCR_ISF(x)                          (((uint32_t)(((uint32_t)(x))<<PORT_PCR_ISF_SHIFT))&PORT_PCR_ISF_MASK)


/*******************************************************************************
 * GPCLR (Global Pin Control Low Register) Bit Definitions
 ******************************************************************************/

/* Global Pin Write Data (GPWD) */
#define PORT_GPCLR_GPWD_MASK                     0xFFFFu
#define PORT_GPCLR_GPWD_SHIFT                    0u
#define PORT_GPCLR_GPWD_WIDTH                    16u
#define PORT_GPCLR_GPWD(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GPCLR_GPWD_SHIFT))&PORT_GPCLR_GPWD_MASK)

/* Global Pin Write Enable (GPWE) */
#define PORT_GPCLR_GPWE_MASK                     0xFFFF0000u
#define PORT_GPCLR_GPWE_SHIFT                    16u
#define PORT_GPCLR_GPWE_WIDTH                    16u
#define PORT_GPCLR_GPWE(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GPCLR_GPWE_SHIFT))&PORT_GPCLR_GPWE_MASK)


/*******************************************************************************
 * GPCHR (Global Pin Control High Register) Bit Definitions
 ******************************************************************************/

/* Global Pin Write Data (GPWD) */
#define PORT_GPCHR_GPWD_MASK                     0xFFFFu
#define PORT_GPCHR_GPWD_SHIFT                    0u
#define PORT_GPCHR_GPWD_WIDTH                    16u
#define PORT_GPCHR_GPWD(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GPCHR_GPWD_SHIFT))&PORT_GPCHR_GPWD_MASK)

/* Global Pin Write Enable (GPWE) */
#define PORT_GPCHR_GPWE_MASK                     0xFFFF0000u
#define PORT_GPCHR_GPWE_SHIFT                    16u
#define PORT_GPCHR_GPWE_WIDTH                    16u
#define PORT_GPCHR_GPWE(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GPCHR_GPWE_SHIFT))&PORT_GPCHR_GPWE_MASK)

/*******************************************************************************
 * GICLR (Global Interrupt Control Low Register) Bit Definitions
 ******************************************************************************/

/* Global Interrupt Write Enable (GIWE) */
#define PORT_GICLR_GIWE_MASK                     0xFFFFu
#define PORT_GICLR_GIWE_SHIFT                    0u
#define PORT_GICLR_GIWE_WIDTH                    16u
#define PORT_GICLR_GIWE(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GICLR_GIWE_SHIFT))&PORT_GICLR_GIWE_MASK)

/* Global Interrupt Write Data (GIWD) */
#define PORT_GICLR_GIWD_MASK                     0xFFFF0000u
#define PORT_GICLR_GIWD_SHIFT                    16u
#define PORT_GICLR_GIWD_WIDTH                    16u
#define PORT_GICLR_GIWD(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GICLR_GIWD_SHIFT))&PORT_GICLR_GIWD_MASK)

/*******************************************************************************
 * GICHR (Global Interrupt Control High Register) Bit Definitions
 ******************************************************************************/

/* Global Interrupt Write Enable (GIWE) */
#define PORT_GICHR_GIWE_MASK                     0xFFFFu
#define PORT_GICHR_GIWE_SHIFT                    0u
#define PORT_GICHR_GIWE_WIDTH                    16u
#define PORT_GICHR_GIWE(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GICHR_GIWE_SHIFT))&PORT_GICHR_GIWE_MASK)

/* Global Interrupt Write Data (GIWD) */
#define PORT_GICHR_GIWD_MASK                     0xFFFF0000u
#define PORT_GICHR_GIWD_SHIFT                    16u
#define PORT_GICHR_GIWD_WIDTH                    16u
#define PORT_GICHR_GIWD(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GICHR_GIWD_SHIFT))&PORT_GICHR_GIWD_MASK)

/*******************************************************************************
 * ISFR (Interrupt Status Flag Register) Bit Definitions
 ******************************************************************************/

/* Interrupt Status Flag (ISF) */
#define PORT_ISFR_ISF_MASK                       0xFFFFFFFFu
#define PORT_ISFR_ISF_SHIFT                      0u
#define PORT_ISFR_ISF_WIDTH                      32u
#define PORT_ISFR_ISF(x)                         (((uint32_t)(((uint32_t)(x))<<PORT_ISFR_ISF_SHIFT))&PORT_ISFR_ISF_MASK)

/*******************************************************************************
 * DFER (Digital Filter Enable Register) Bit Definitions
 ******************************************************************************/

/* Digital Filter Enable (DFE) */
#define PORT_DFER_DFE_MASK                       0xFFFFFFFFu
#define PORT_DFER_DFE_SHIFT                      0u
#define PORT_DFER_DFE_WIDTH                      32u
#define PORT_DFER_DFE(x)                         (((uint32_t)(((uint32_t)(x))<<PORT_DFER_DFE_SHIFT))&PORT_DFER_DFE_MASK)

/*******************************************************************************
 * DFCR (Digital Filter Clock Register) Bit Definitions
 ******************************************************************************/

/* Clock Source (CS) */
#define PORT_DFCR_CS_MASK                        0x1u
#define PORT_DFCR_CS_SHIFT                       0u
#define PORT_DFCR_CS_WIDTH                       1u
#define PORT_DFCR_CS(x)                          (((uint32_t)(((uint32_t)(x))<<PORT_DFCR_CS_SHIFT))&PORT_DFCR_CS_MASK)

/*******************************************************************************
 * DFWR (Digital Filter Width Register) Bit Definitions
 ******************************************************************************/

/* Filter Length (FILT) */
#define PORT_DFWR_FILT_MASK                      0x1Fu
#define PORT_DFWR_FILT_SHIFT                     0u
#define PORT_DFWR_FILT_WIDTH                     5u
#define PORT_DFWR_FILT(x)                        (((uint32_t)(((uint32_t)(x))<<PORT_DFWR_FILT_SHIFT))&PORT_DFWR_FILT_MASK)


/*******************************************************************************
 * PORT Helper Macros
 ******************************************************************************/

/**
 * @brief Enable PORT clock
 * @param pccIndex PCC index for PORT (PCC_PORTx_INDEX)
 */
#define PORT_ENABLE_CLOCK(pccIndex)     (PCC->PCCn[pccIndex] |= PCC_CGC_MASK)

/**
 * @brief Disable PORT clock
 * @param pccIndex PCC index for PORT (PCC_PORTx_INDEX)
 */
#define PORT_DISABLE_CLOCK(pccIndex)    (PCC->PCCn[pccIndex] &= ~PCC_CGC_MASK)

/**
 * @brief Check if PORT clock is enabled
 * @param pccIndex PCC index for PORT (PCC_PORTx_INDEX)
 * @return 1 if enabled, 0 if disabled
 */
#define PORT_IS_CLOCK_ENABLED(pccIndex) ((PCC->PCCn[pccIndex] & PCC_CGC_MASK) != 0U)

/**
 * @brief Set pin MUX
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 * @param mux MUX value (PORT_MUX_xxx)
 */
#define PORT_SET_MUX(base, pin, mux) \
    do { \
        (base)->PCR[pin] = ((base)->PCR[pin] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(mux); \
    } while(0)

/**
 * @brief Get pin MUX
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 * @return Current MUX value
 */
#define PORT_GET_MUX(base, pin) \
    (((base)->PCR[pin] & PORT_PCR_MUX_MASK) >> PORT_PCR_MUX_SHIFT)

/**
 * @brief Enable pull resistor
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 * @param pullup 1 for pull-up, 0 for pull-down
 */
#define PORT_ENABLE_PULL(base, pin, pullup) \
    do { \
        (base)->PCR[pin] = ((base)->PCR[pin] & ~PORT_PCR_PS_MASK) | \
                          PORT_PCR_PE(1) | PORT_PCR_PS(pullup); \
    } while(0)

/**
 * @brief Disable pull resistor
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 */
#define PORT_DISABLE_PULL(base, pin) \
    ((base)->PCR[pin] &= ~PORT_PCR_PE_MASK)

/**
 * @brief Set pin interrupt configuration
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 * @param irqc Interrupt configuration (PORT_IRQC_xxx)
 */
#define PORT_SET_IRQC(base, pin, irqc) \
    do { \
        (base)->PCR[pin] = ((base)->PCR[pin] & ~PORT_PCR_IRQC_MASK) | PORT_PCR_IRQC(irqc); \
    } while(0)

/**
 * @brief Clear pin interrupt flag
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 */
#define PORT_CLEAR_ISF(base, pin) \
    ((base)->PCR[pin] |= PORT_PCR_ISF_MASK)

/**
 * @brief Get pin interrupt flag status
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 * @return 1 if flag is set, 0 otherwise
 */
#define PORT_GET_ISF(base, pin) \
    (((base)->PCR[pin] & PORT_PCR_ISF_MASK) != 0U)

/**
 * @brief Get all interrupt status flags for port
 * @param base PORT base pointer
 * @return ISFR register value
 */
#define PORT_GET_ISFR(base) \
    ((base)->ISFR)

/**
 * @brief Clear all interrupt flags for port
 * @param base PORT base pointer
 * @param mask Pin mask
 */
#define PORT_CLEAR_ISFR(base, mask) \
    ((base)->ISFR = (mask))

/**
 * @brief Enable drive strength
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 */
#define PORT_ENABLE_HIGH_DRIVE(base, pin) \
    ((base)->PCR[pin] |= PORT_PCR_DSE_MASK)

/**
 * @brief Disable drive strength (set to low)
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 */
#define PORT_DISABLE_HIGH_DRIVE(base, pin) \
    ((base)->PCR[pin] &= ~PORT_PCR_DSE_MASK)

/**
 * @brief Enable passive filter
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 */
#define PORT_ENABLE_PASSIVE_FILTER(base, pin) \
    ((base)->PCR[pin] |= PORT_PCR_PFE_MASK)

/**
 * @brief Disable passive filter
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 */
#define PORT_DISABLE_PASSIVE_FILTER(base, pin) \
    ((base)->PCR[pin] &= ~PORT_PCR_PFE_MASK)

/**
 * @brief Enable digital filter for pin
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 */
#define PORT_ENABLE_DIGITAL_FILTER(base, pin) \
    ((base)->DFER |= (1U << (pin)))

/**
 * @brief Disable digital filter for pin
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 */
#define PORT_DISABLE_DIGITAL_FILTER(base, pin) \
    ((base)->DFER &= ~(1U << (pin)))

/**
 * @brief Complete pin configuration
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 * @param pcr Complete PCR register value
 */
#define PORT_SET_PCR(base, pin, pcr) \
    ((base)->PCR[pin] = (pcr))

/**
 * @brief Get pin configuration
 * @param base PORT base pointer
 * @param pin Pin number (0-31)
 * @return PCR register value
 */
#define PORT_GET_PCR(base, pin) \
    ((base)->PCR[pin])

#endif /* PORT_REG_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/