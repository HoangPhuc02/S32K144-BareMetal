/**
 * @file    port_reg.h
 * @brief   PORT Register Definitions for S32K144
 */

#ifndef PORT_REG_H
#define PORT_REG_H

#include <stdint.h>

#define __I  volatile const /*!< Defines 'read only' permissions                 */
#define __O  volatile       /*!< Defines 'write only' permissions                */
#define __IO volatile       /*!< Defines 'read / write' permissions              */

/* Base addresses */
#define PORTA_BASE (0x40049000u) /** Peripheral PORTA base address */
#define PORTB_BASE (0x4004A000u) /** Peripheral PORTB base address */
#define PORTC_BASE (0x4004B000u) /** Peripheral PORTC base address */
#define PORTD_BASE (0x4004C000u) /** Peripheral PORTD base address */
#define PORTE_BASE (0x4004D000u) /** Peripheral PORTE base address */

/** Number of instances of the PORT module. */
#define PORT_INSTANCE_COUNT (5u)
/** PORT - Size of Registers Arrays */
#define PORT_PCR_COUNT 32u

/* Register structure */
typedef struct {
    __IO uint32_t PCR[PORT_PCR_COUNT]; /**< Pin Control Register n, array offset: 0x0, array step: 0x4 */
    __O uint32_t GPCLR;                /**< Global Pin Control Low Register, offset: 0x80 */
    __O uint32_t GPCHR;                /**< Global Pin Control High Register, offset: 0x84 */
    __O uint32_t GICLR;                /**< Global Interrupt Control Low Register, offset: 0x88 */
    __O uint32_t GICHR;                /**< Global Interrupt Control High Register, offset: 0x8C */
    uint8_t RESERVED_0[16];
    __IO uint32_t ISFR; /**< Interrupt Status Flag Register, offset: 0xA0 */
    uint8_t RESERVED_1[28];
    __IO uint32_t DFER; /**< Digital Filter Enable Register, offset: 0xC0 */
    __IO uint32_t DFCR; /**< Digital Filter Clock Register, offset: 0xC4 */
    __IO uint32_t DFWR; /**< Digital Filter Width Register, offset: 0xC8 */
} PORT_Type, *PORT_MemMapPtr;

/* Register pointers */
#define PORTA ((PORT_Type *)PORTA_BASE) /** Peripheral PORTA base pointer */
#define PORTB ((PORT_Type *)PORTB_BASE) /** Peripheral PORTB base pointer */
#define PORTC ((PORT_Type *)PORTC_BASE) /** Peripheral PORTC base pointer */
#define PORTD ((PORT_Type *)PORTD_BASE) /** Peripheral PORTD base pointer */
#define PORTE ((PORT_Type *)PORTE_BASE) /** Peripheral PORTE base pointer */

/** Array initializer of PORT peripheral base addresses */
#define PORT_BASE_ADDRS {PORTA_BASE, PORTB_BASE, PORTC_BASE, PORTD_BASE, PORTE_BASE}
/** Array initializer of PORT peripheral base pointers */
#define PORT_BASE_PTRS {PORTA, PORTB, PORTC, PORTD, PORTE}
/** Number of interrupt vector arrays for the PORT module. */
#define PORT_IRQS_ARR_COUNT (1u)
/** Number of interrupt channels for the PORT module. */
#define PORT_IRQS_CH_COUNT (1u)
/** Interrupt vectors for the PORT peripheral type */
#define PORT_IRQS {PORTA_IRQn, PORTB_IRQn, PORTC_IRQn, PORTD_IRQn, PORTE_IRQn}

/* ----------------------------------------------------------------------------
   -- PORT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PORT_Register_Masks PORT Register Masks
 * @{
 */

/* PCR Bit Fields */
#define PORT_PCR_PS_MASK    0x1u
#define PORT_PCR_PS_SHIFT   0u
#define PORT_PCR_PS_WIDTH   1u
#define PORT_PCR_PS(x)      (((uint32_t)(((uint32_t)(x)) << PORT_PCR_PS_SHIFT)) & PORT_PCR_PS_MASK)
#define PORT_PCR_PE_MASK    0x2u
#define PORT_PCR_PE_SHIFT   1u
#define PORT_PCR_PE_WIDTH   1u
#define PORT_PCR_PE(x)      (((uint32_t)(((uint32_t)(x)) << PORT_PCR_PE_SHIFT)) & PORT_PCR_PE_MASK)
#define PORT_PCR_PFE_MASK   0x10u
#define PORT_PCR_PFE_SHIFT  4u
#define PORT_PCR_PFE_WIDTH  1u
#define PORT_PCR_PFE(x)     (((uint32_t)(((uint32_t)(x)) << PORT_PCR_PFE_SHIFT)) & PORT_PCR_PFE_MASK)
#define PORT_PCR_DSE_MASK   0x40u
#define PORT_PCR_DSE_SHIFT  6u
#define PORT_PCR_DSE_WIDTH  1u
#define PORT_PCR_DSE(x)     (((uint32_t)(((uint32_t)(x)) << PORT_PCR_DSE_SHIFT)) & PORT_PCR_DSE_MASK)
#define PORT_PCR_MUX_MASK   0x700u
#define PORT_PCR_MUX_SHIFT  8u
#define PORT_PCR_MUX_WIDTH  3u
#define PORT_PCR_MUX(x)     (((uint32_t)(((uint32_t)(x)) << PORT_PCR_MUX_SHIFT)) & PORT_PCR_MUX_MASK)
#define PORT_PCR_LK_MASK    0x8000u
#define PORT_PCR_LK_SHIFT   15u
#define PORT_PCR_LK_WIDTH   1u
#define PORT_PCR_LK(x)      (((uint32_t)(((uint32_t)(x)) << PORT_PCR_LK_SHIFT)) & PORT_PCR_LK_MASK)
#define PORT_PCR_IRQC_MASK  0xF0000u
#define PORT_PCR_IRQC_SHIFT 16u
#define PORT_PCR_IRQC_WIDTH 4u
#define PORT_PCR_IRQC(x)    (((uint32_t)(((uint32_t)(x)) << PORT_PCR_IRQC_SHIFT)) & PORT_PCR_IRQC_MASK)
#define PORT_PCR_ISF_MASK   0x1000000u
#define PORT_PCR_ISF_SHIFT  24u
#define PORT_PCR_ISF_WIDTH  1u
#define PORT_PCR_ISF(x)     (((uint32_t)(((uint32_t)(x)) << PORT_PCR_ISF_SHIFT)) & PORT_PCR_ISF_MASK)
/* GPCLR Bit Fields */
#define PORT_GPCLR_GPWD_MASK  0xFFFFu
#define PORT_GPCLR_GPWD_SHIFT 0u
#define PORT_GPCLR_GPWD_WIDTH 16u
#define PORT_GPCLR_GPWD(x)    (((uint32_t)(((uint32_t)(x)) << PORT_GPCLR_GPWD_SHIFT)) & PORT_GPCLR_GPWD_MASK)
#define PORT_GPCLR_GPWE_MASK  0xFFFF0000u
#define PORT_GPCLR_GPWE_SHIFT 16u
#define PORT_GPCLR_GPWE_WIDTH 16u
#define PORT_GPCLR_GPWE(x)    (((uint32_t)(((uint32_t)(x)) << PORT_GPCLR_GPWE_SHIFT)) & PORT_GPCLR_GPWE_MASK)
/* GPCHR Bit Fields */
#define PORT_GPCHR_GPWD_MASK  0xFFFFu
#define PORT_GPCHR_GPWD_SHIFT 0u
#define PORT_GPCHR_GPWD_WIDTH 16u
#define PORT_GPCHR_GPWD(x)    (((uint32_t)(((uint32_t)(x)) << PORT_GPCHR_GPWD_SHIFT)) & PORT_GPCHR_GPWD_MASK)
#define PORT_GPCHR_GPWE_MASK  0xFFFF0000u
#define PORT_GPCHR_GPWE_SHIFT 16u
#define PORT_GPCHR_GPWE_WIDTH 16u
#define PORT_GPCHR_GPWE(x)    (((uint32_t)(((uint32_t)(x)) << PORT_GPCHR_GPWE_SHIFT)) & PORT_GPCHR_GPWE_MASK)
/* GICLR Bit Fields */
#define PORT_GICLR_GIWE_MASK  0xFFFFu
#define PORT_GICLR_GIWE_SHIFT 0u
#define PORT_GICLR_GIWE_WIDTH 16u
#define PORT_GICLR_GIWE(x)    (((uint32_t)(((uint32_t)(x)) << PORT_GICLR_GIWE_SHIFT)) & PORT_GICLR_GIWE_MASK)
#define PORT_GICLR_GIWD_MASK  0xFFFF0000u
#define PORT_GICLR_GIWD_SHIFT 16u
#define PORT_GICLR_GIWD_WIDTH 16u
#define PORT_GICLR_GIWD(x)    (((uint32_t)(((uint32_t)(x)) << PORT_GICLR_GIWD_SHIFT)) & PORT_GICLR_GIWD_MASK)
/* GICHR Bit Fields */
#define PORT_GICHR_GIWE_MASK  0xFFFFu
#define PORT_GICHR_GIWE_SHIFT 0u
#define PORT_GICHR_GIWE_WIDTH 16u
#define PORT_GICHR_GIWE(x)    (((uint32_t)(((uint32_t)(x)) << PORT_GICHR_GIWE_SHIFT)) & PORT_GICHR_GIWE_MASK)
#define PORT_GICHR_GIWD_MASK  0xFFFF0000u
#define PORT_GICHR_GIWD_SHIFT 16u
#define PORT_GICHR_GIWD_WIDTH 16u
#define PORT_GICHR_GIWD(x)    (((uint32_t)(((uint32_t)(x)) << PORT_GICHR_GIWD_SHIFT)) & PORT_GICHR_GIWD_MASK)
/* ISFR Bit Fields */
#define PORT_ISFR_ISF_MASK  0xFFFFFFFFu
#define PORT_ISFR_ISF_SHIFT 0u
#define PORT_ISFR_ISF_WIDTH 32u
#define PORT_ISFR_ISF(x)    (((uint32_t)(((uint32_t)(x)) << PORT_ISFR_ISF_SHIFT)) & PORT_ISFR_ISF_MASK)
/* DFER Bit Fields */
#define PORT_DFER_DFE_MASK  0xFFFFFFFFu
#define PORT_DFER_DFE_SHIFT 0u
#define PORT_DFER_DFE_WIDTH 32u
#define PORT_DFER_DFE(x)    (((uint32_t)(((uint32_t)(x)) << PORT_DFER_DFE_SHIFT)) & PORT_DFER_DFE_MASK)
/* DFCR Bit Fields */
#define PORT_DFCR_CS_MASK  0x1u
#define PORT_DFCR_CS_SHIFT 0u
#define PORT_DFCR_CS_WIDTH 1u
#define PORT_DFCR_CS(x)    (((uint32_t)(((uint32_t)(x)) << PORT_DFCR_CS_SHIFT)) & PORT_DFCR_CS_MASK)
/* DFWR Bit Fields */
#define PORT_DFWR_FILT_MASK  0x1Fu
#define PORT_DFWR_FILT_SHIFT 0u
#define PORT_DFWR_FILT_WIDTH 5u
#define PORT_DFWR_FILT(x)    (((uint32_t)(((uint32_t)(x)) << PORT_DFWR_FILT_SHIFT)) & PORT_DFWR_FILT_MASK)

/*!
 * @}
 */ /* end of group PORT_Register_Masks */

#endif /* PORT_REG_H */