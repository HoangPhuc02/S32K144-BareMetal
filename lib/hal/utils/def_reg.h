/**
 * @file    def_reg.h
 * @brief   Common register helper macros and interrupt vector definitions
 * @details
 * This header provides low-level register access macros (read/write/bit
 * operations) and the IRQn_Type enumeration for the S32K144 device. It is
 * intended for use by peripheral register headers and low-level drivers.
 *
 * Key contents:
 * - IO qualifiers (__I, __O, __IO)
 * - 32-bit register access macros (REG_READ32, REG_WRITE32, REG_BIT_SET32,
 *   REG_BIT_CLEAR32, REG_RMW32)
 * - Interrupt vector numbers (IRQn_Type) for S32K144
 *
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 *
 * @note    This file contains device-specific interrupt numbers. Keep in sync
 * with the S32K144 reference manual.
 *
 * @warning These macros perform direct memory access. Use caution and ensure
 * correct addresses are provided.
 *
 * @par     Change Log:
 * - v1.0 (2025-11-23) : Added Doxygen header and clarifying notes.
 */

#ifndef DEF_REG_H
#define DEF_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 * Generic macros
 *****************************************************************************/

/* IO definitions (access restrictions to peripheral registers) */
/**
*   IO Type Qualifiers are used
*   \li to specify the access to peripheral variables.
*   \li for automatic generation of peripheral register debug information.
*/
#ifndef __IO
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions                 */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions                */
#define     __IO    volatile             /*!< Defines 'read / write' permissions              */
#endif


/**
* @brief 32 bits memory read macro.
*/
#if !defined(REG_READ32)
  #define REG_READ32(address)               (*(volatile uint32_t*)(address))
#endif

/**
* @brief 32 bits memory write macro.
*/
#if !defined(REG_WRITE32)
  #define REG_WRITE32(address, value)       ((*(volatile uint32_t*)(address))= (uint32_t)(value))
#endif

/**
* @brief 32 bits bits setting macro.
*/
#if !defined(REG_BIT_SET32)
  #define REG_BIT_SET32(address, mask)      ((*(volatile uint32_t*)(address))|= (uint32_t)(mask))
#endif

/**
* @brief 32 bits bits clearing macro.
*/
#if !defined(REG_BIT_CLEAR32)
  #define REG_BIT_CLEAR32(address, mask)    ((*(volatile uint32_t*)(address))&= ((uint32_t)~((uint32_t)(mask))))
#endif

/**
* @brief 32 bit clear bits and set with new value
* @note It is user's responsability to make sure that value has only "mask" bits set - (value&~mask)==0
*/
#if !defined(REG_RMW32)
  #define REG_RMW32(address, mask, value)   (REG_WRITE32((address), ((REG_READ32(address)& ((uint32_t)~((uint32_t)(mask))))| ((uint32_t)(value)))))
#endif

#define NULL (void*)0

/*!
 * @}
 */ /* end of group Interrupt_vector_numbers_S32K144 */

 /**
  * 
  */

  #define STD_ON            1
  #define STD_OFF           0


#endif /* DEF_REG_H */