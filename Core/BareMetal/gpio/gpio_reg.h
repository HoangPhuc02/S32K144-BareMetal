/**
 * @file    gpio_reg.h
 * @brief   GPIO Register Definitions for S32K144
 */

#ifndef GPIO_REG_H
#define GPIO_REG_H

#include <stdint.h>

#define __I  volatile const /*!< Defines 'read only' permissions                 */
#define __O  volatile       /*!< Defines 'write only' permissions                */
#define __IO volatile       /*!< Defines 'read / write' permissions              */

/* Base addresses */
#define PTA_BASE (0x400FF000u) /** Peripheral PTA base address */
#define PTB_BASE (0x400FF040u) /** Peripheral PTB base address */
#define PTC_BASE (0x400FF080u) /** Peripheral PTC base address */
#define PTD_BASE (0x400FF0C0u) /** Peripheral PTD base address */
#define PTE_BASE (0x400FF100u) /** Peripheral PTE base address */

/* Register structure */
typedef struct {
    __IO uint32_t PDOR; /**< Port Data Output Register, offset: 0x0 */
    __O uint32_t PSOR;  /**< Port Set Output Register, offset: 0x4 */
    __O uint32_t PCOR;  /**< Port Clear Output Register, offset: 0x8 */
    __O uint32_t PTOR;  /**< Port Toggle Output Register, offset: 0xC */
    __I uint32_t PDIR;  /**< Port Data Input Register, offset: 0x10 */
    __IO uint32_t PDDR; /**< Port Data Direction Register, offset: 0x14 */
    __IO uint32_t PIDR; /**< Port Input Disable Register, offset: 0x18 */
} GPIO_Type, *GPIO_MemMapPtr;

/** Number of instances of the GPIO module. */
#define GPIO_INSTANCE_COUNT                      (5u)

/* Register pointers */
#define PTA ((GPIO_Type *)PTA_BASE) /** Peripheral PTA base pointer */
#define PTB ((GPIO_Type *)PTB_BASE) /** Peripheral PTB base pointer */
#define PTC ((GPIO_Type *)PTC_BASE) /** Peripheral PTC base pointer */
#define PTD ((GPIO_Type *)PTD_BASE) /** Peripheral PTD base pointer */
#define PTE ((GPIO_Type *)PTE_BASE) /** Peripheral PTE base pointer */

/** Array initializer of GPIO peripheral base addresses */
#define GPIO_BASE_ADDRS {PTA_BASE, PTB_BASE, PTC_BASE, PTD_BASE, PTE_BASE}
/** Array initializer of GPIO peripheral base pointers */
#define GPIO_BASE_PTRS {PTA, PTB, PTC, PTD, PTE}

/* ----------------------------------------------------------------------------
   -- GPIO Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Register_Masks GPIO Register Masks
 * @{
 */

/* PDOR Bit Fields */
#define GPIO_PDOR_PDO_MASK  0xFFFFFFFFu
#define GPIO_PDOR_PDO_SHIFT 0u
#define GPIO_PDOR_PDO_WIDTH 32u
#define GPIO_PDOR_PDO(x)    (((uint32_t)(((uint32_t)(x)) << GPIO_PDOR_PDO_SHIFT)) & GPIO_PDOR_PDO_MASK)
/* PSOR Bit Fields */
#define GPIO_PSOR_PTSO_MASK  0xFFFFFFFFu
#define GPIO_PSOR_PTSO_SHIFT 0u
#define GPIO_PSOR_PTSO_WIDTH 32u
#define GPIO_PSOR_PTSO(x)    (((uint32_t)(((uint32_t)(x)) << GPIO_PSOR_PTSO_SHIFT)) & GPIO_PSOR_PTSO_MASK)
/* PCOR Bit Fields */
#define GPIO_PCOR_PTCO_MASK  0xFFFFFFFFu
#define GPIO_PCOR_PTCO_SHIFT 0u
#define GPIO_PCOR_PTCO_WIDTH 32u
#define GPIO_PCOR_PTCO(x)    (((uint32_t)(((uint32_t)(x)) << GPIO_PCOR_PTCO_SHIFT)) & GPIO_PCOR_PTCO_MASK)
/* PTOR Bit Fields */
#define GPIO_PTOR_PTTO_MASK  0xFFFFFFFFu
#define GPIO_PTOR_PTTO_SHIFT 0u
#define GPIO_PTOR_PTTO_WIDTH 32u
#define GPIO_PTOR_PTTO(x)    (((uint32_t)(((uint32_t)(x)) << GPIO_PTOR_PTTO_SHIFT)) & GPIO_PTOR_PTTO_MASK)
/* PDIR Bit Fields */
#define GPIO_PDIR_PDI_MASK  0xFFFFFFFFu
#define GPIO_PDIR_PDI_SHIFT 0u
#define GPIO_PDIR_PDI_WIDTH 32u
#define GPIO_PDIR_PDI(x)    (((uint32_t)(((uint32_t)(x)) << GPIO_PDIR_PDI_SHIFT)) & GPIO_PDIR_PDI_MASK)
/* PDDR Bit Fields */
#define GPIO_PDDR_PDD_MASK  0xFFFFFFFFu
#define GPIO_PDDR_PDD_SHIFT 0u
#define GPIO_PDDR_PDD_WIDTH 32u
#define GPIO_PDDR_PDD(x)    (((uint32_t)(((uint32_t)(x)) << GPIO_PDDR_PDD_SHIFT)) & GPIO_PDDR_PDD_MASK)
/* PIDR Bit Fields */
#define GPIO_PIDR_PID_MASK  0xFFFFFFFFu
#define GPIO_PIDR_PID_SHIFT 0u
#define GPIO_PIDR_PID_WIDTH 32u
#define GPIO_PIDR_PID(x)    (((uint32_t)(((uint32_t)(x)) << GPIO_PIDR_PID_SHIFT)) & GPIO_PIDR_PID_MASK)

/*!
 * @}
 */ /* end of group GPIO_Register_Masks */

#endif /* GPIO_REG_H */