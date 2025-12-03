/*
 * @file    pcc_reg.h
 * @brief   PCC Register Definitions for S32K144
 */

#ifndef PCC_REG_H_
#define PCC_REG_H_

#include <stdint.h>

/* IO definitions (access restrictions to peripheral registers) */
/**
*   IO Type Qualifiers are used
*   \li to specify the access to peripheral variables.
*   \li for automatic generation of peripheral register debug information.
*/
#ifndef __IO
	#define   __I     volatile const       /*!< Defines 'read only' permissions                 */
	#define     __O     volatile             /*!< Defines 'write only' permissions                */
	#define     __IO    volatile             /*!< Defines 'read / write' permissions              */
#endif


/* ----------------------------------------------------------------------------
   -- PCC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PCC_Peripheral_Access_Layer PCC Peripheral Access Layer
 * @{
 */

/** PCC - Size of Registers Arrays */
#define PCC_PCCn_COUNT                            116u

/** PCC - Register Layout Typedef */
typedef struct {
  __IO uint32_t PCCn[PCC_PCCn_COUNT];              /**< PCC FTFC Register..PCC CMP0 Register, array offset: 0x0, array step: 0x4 */
} PCC_Type, *PCC_MemMapPtr;

/** Number of instances of the PCC module. */
#define PCC_INSTANCE_COUNT                       (1u)

/* PCC - Peripheral instance base addresses */
/** Peripheral PCC base address */
#define PCC_BASE                              (0x40065000u)
/** Peripheral PCC base pointer */
#define PCC                                   ((PCC_Type *)PCC_BASE)
/** Array initializer of PCC peripheral base addresses */
#define PCC_BASE_ADDRS                        { PCC_BASE }
/** Array initializer of PCC peripheral base pointers */
#define PCC_BASE_PTRS                         { PCC }

/* ----------------------------------------------------------------------------
   -- PCC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PCC_Register_Masks PCC Register Masks
 * @{
 */

/*! @name PCCn - PCC FTFC Register..PCC CMP0 Register */
/*! @{ */
#define PCC_PCCn_PCS_MASK                        (0x7000000U)
#define PCC_PCCn_PCS_SHIFT                       (24U)
#define PCC_PCCn_PCS_WIDTH                       (3U)
#define PCC_PCCn_PCS(x)                          (((uint32_t)(((uint32_t)(x)) << PCC_PCCn_PCS_SHIFT)) & PCC_PCCn_PCS_MASK)

#define PCC_PCCn_CGC_MASK                        (0x40000000U)
#define PCC_PCCn_CGC_SHIFT                       (30U)
#define PCC_PCCn_CGC_WIDTH                       (1U)
#define PCC_PCCn_CGC(x)                          (((uint32_t)(((uint32_t)(x)) << PCC_PCCn_CGC_SHIFT)) & PCC_PCCn_CGC_MASK)
/*! @} */

/*!
 * @}
 */ /* end of group PCC_Register_Masks */

/*!
 * @}
 */ /* end of group PCC_Peripheral_Access_Layer */

#endif /* PCC_REG_H_ */
