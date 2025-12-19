/**
 * @file    lpit_reg.h
 * @brief   LPIT (Low Power Interrupt Timer) Register Definitions for S32K144
 * @details
 * Defines LPIT module registers and bitfields.
 * LPIT is a 32-bit low-power timer used for timing, PWM, and low-power applications.
 *
 * S32K144 LPIT features:
 * - 4 independent timer channels (Channel 0-3)
 * - 32-bit counter for each channel
 * - Modes: Periodic Counter, Dual 16-bit Periodic Counter
 * - Chain mode to form 64-bit timers
 * - Operates in low-power modes
 *
 * @author  PhucPH32
 * @date    28/11/2025
 * @version 1.0
 *
 * @note    Refer to S32K1xx Reference Manual Chapter 43 (LPIT)
 * @warning Must enable clock for LPIT before use
 *
 * @par Change Log:
 * - Version 1.0 (28/11/2025): Initialize LPIT driver
 */

#ifndef LPIT_REG_H
#define LPIT_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "def_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief LPIT0 module base address */
#define LPIT0_BASE          (0x40037000UL)

/** @brief Number of LPIT channels */
#define LPIT_CHANNEL_COUNT  (4U)

/*******************************************************************************
 * LPIT Register Structure
 ******************************************************************************/

/**
 * @brief LPIT Module Control Register (LPIT_MCR)
 * @details Common control register for LPIT module
 */
typedef struct {
    __IO uint32_t M_CEN      : 1;   /**< Bit 0: Module Clock Enable */
    __IO uint32_t SW_RST     : 1;   /**< Bit 1: Software Reset */
    __IO uint32_t DOZE_EN    : 1;   /**< Bit 2: Doze Mode Enable */
    __IO uint32_t DBG_EN     : 1;   /**< Bit 3: Debug Mode Enable */
    __I  uint32_t RESERVED   : 28;  /**< Bits 4-31: Reserved */
} LPIT_MCR_Type;

/**
 * @brief LPIT Module Status Register (LPIT_MSR)
 * @details Status register of timer channels
 */
typedef struct {
    __I  uint32_t TIF0       : 1;   /**< Bit 0: Channel 0 Timer Interrupt Flag */
    __I  uint32_t TIF1       : 1;   /**< Bit 1: Channel 1 Timer Interrupt Flag */
    __I  uint32_t TIF2       : 1;   /**< Bit 2: Channel 2 Timer Interrupt Flag */
    __I  uint32_t TIF3       : 1;   /**< Bit 3: Channel 3 Timer Interrupt Flag */
    __I  uint32_t RESERVED   : 28;  /**< Bits 4-31: Reserved */
} LPIT_MSR_Type;

/**
 * @brief LPIT Module Interrupt Enable Register (LPIT_MIER)
 * @details Interrupt enable register for channels
 */
typedef struct {
    __IO uint32_t TIE0       : 1;   /**< Bit 0: Channel 0 Timer Interrupt Enable */
    __IO uint32_t TIE1       : 1;   /**< Bit 1: Channel 1 Timer Interrupt Enable */
    __IO uint32_t TIE2       : 1;   /**< Bit 2: Channel 2 Timer Interrupt Enable */
    __IO uint32_t TIE3       : 1;   /**< Bit 3: Channel 3 Timer Interrupt Enable */
    __I  uint32_t RESERVED   : 28;  /**< Bits 4-31: Reserved */
} LPIT_MIER_Type;

/**
 * @brief LPIT Set Timer Interrupt Enable Register (LPIT_SETTEN)
 * @details Register to set interrupt enable (write-only)
 */
typedef struct {
    __O  uint32_t SET_T_EN_0 : 1;   /**< Bit 0: Set Timer 0 Interrupt Enable */
    __O  uint32_t SET_T_EN_1 : 1;   /**< Bit 1: Set Timer 1 Interrupt Enable */
    __O  uint32_t SET_T_EN_2 : 1;   /**< Bit 2: Set Timer 2 Interrupt Enable */
    __O  uint32_t SET_T_EN_3 : 1;   /**< Bit 3: Set Timer 3 Interrupt Enable */
    __I  uint32_t RESERVED   : 28;  /**< Bits 4-31: Reserved */
} LPIT_SETTEN_Type;

/**
 * @brief LPIT Clear Timer Interrupt Enable Register (LPIT_CLRTEN)
 * @details Register to clear interrupt enable (write-only)
 */
typedef struct {
    __O  uint32_t CLR_T_EN_0 : 1;   /**< Bit 0: Clear Timer 0 Interrupt Enable */
    __O  uint32_t CLR_T_EN_1 : 1;   /**< Bit 1: Clear Timer 1 Interrupt Enable */
    __O  uint32_t CLR_T_EN_2 : 1;   /**< Bit 2: Clear Timer 2 Interrupt Enable */
    __O  uint32_t CLR_T_EN_3 : 1;   /**< Bit 3: Clear Timer 3 Interrupt Enable */
    __I  uint32_t RESERVED   : 28;  /**< Bits 4-31: Reserved */
} LPIT_CLRTEN_Type;

/**
 * @brief LPIT Timer Value Register (LPIT_TVALn)
 * @details Register holding timer value (32-bit)
 */
typedef struct {
    __IO uint32_t TMR_VAL;          /**< Timer Value - Reload value for timer */
} LPIT_TVAL_Type;

/**
 * @brief LPIT Current Timer Value Register (LPIT_CVALn)
 * @details Register holding current timer value (read-only)
 */
typedef struct {
    __I  uint32_t TMR_CUR_VAL;      /**< Current Timer Value - Current count value */
} LPIT_CVAL_Type;

/**
 * @brief LPIT Timer Control Register (LPIT_TCTRLn)
 * @details Control register for each timer channel
 */
typedef struct {
    __IO uint32_t T_EN       : 1;   /**< Bit 0: Timer Enable */
    __IO uint32_t CHAIN      : 1;   /**< Bit 1: Chain Channel - Link with previous channel */
    __IO uint32_t MODE       : 2;   /**< Bits 2-3: Timer Mode */
    __I  uint32_t RESERVED1  : 12;  /**< Bits 4-15: Reserved */
    __IO uint32_t TSOT       : 1;   /**< Bit 16: Timer Start On Trigger */
    __IO uint32_t TSOI       : 1;   /**< Bit 17: Timer Stop On Interrupt */
    __IO uint32_t TROT       : 1;   /**< Bit 18: Timer Reload On Trigger */
    __I  uint32_t RESERVED2  : 4;   /**< Bits 19-22: Reserved */
    __IO uint32_t TRG_SRC    : 1;   /**< Bit 23: Trigger Source */
    __IO uint32_t TRG_SEL    : 4;   /**< Bits 24-27: Trigger Select */
    __I  uint32_t RESERVED3  : 4;   /**< Bits 28-31: Reserved */
} LPIT_TCTRL_Type;

/**
 * @brief LPIT Channel Register Structure
 * @details Register structure for one LPIT channel
 */
typedef struct {
    __IO uint32_t TVAL;             /**< Timer Value Register */
    __I  uint32_t CVAL;             /**< Current Timer Value Register */
    __IO uint32_t TCTRL;            /**< Timer Control Register */
    __I  uint32_t RESERVED;         /**< Reserved */
} LPIT_CHANNEL_Type;

/**
 * @brief LPIT Module Structure
 * @details Complete structure of LPIT module
 */
typedef struct {
    __I  uint32_t VERID;            /**< 0x0000: Version ID Register */
    __I  uint32_t PARAM;            /**< 0x0004: Parameter Register */
    __IO uint32_t MCR;              /**< 0x0008: Module Control Register */
    __IO uint32_t MSR;              /**< 0x000C: Module Status Register */
    __IO uint32_t MIER;             /**< 0x0010: Module Interrupt Enable Register */
    __O  uint32_t SETTEN;           /**< 0x0014: Set Timer Enable Register */
    __O  uint32_t CLRTEN;           /**< 0x0018: Clear Timer Enable Register */
    __I  uint32_t RESERVED[1];      /**< 0x001C: Reserved */
    LPIT_CHANNEL_Type CHANNEL[4];   /**< 0x0020-0x005F: Timer Channels 0-3 */
} LPIT_Type;

/*******************************************************************************
 * Register Access Macros
 ******************************************************************************/

/** @brief LPIT0 module pointer */
#define LPIT0               ((LPIT_Type *)LPIT0_BASE)

/*******************************************************************************
 * Module Control Register (MCR) Bit Definitions
 ******************************************************************************/

/** @brief Module Clock Enable - Enable clock for LPIT module */
#define LPIT_MCR_M_CEN_MASK     (0x00000001UL)
#define LPIT_MCR_M_CEN_SHIFT    (0U)

/** @brief Software Reset - Reset LPIT module */
#define LPIT_MCR_SW_RST_MASK    (0x00000002UL)
#define LPIT_MCR_SW_RST_SHIFT   (1U)

/** @brief Doze Mode Enable - LPIT operates in Doze mode */
#define LPIT_MCR_DOZE_EN_MASK   (0x00000004UL)
#define LPIT_MCR_DOZE_EN_SHIFT  (2U)

/** @brief Debug Mode Enable - LPIT operates in Debug mode */
#define LPIT_MCR_DBG_EN_MASK    (0x00000008UL)
#define LPIT_MCR_DBG_EN_SHIFT   (3U)

/*******************************************************************************
 * Module Status Register (MSR) Bit Definitions
 ******************************************************************************/

/** @brief Timer Interrupt Flags */
#define LPIT_MSR_TIF0_MASK      (0x00000001UL)
#define LPIT_MSR_TIF1_MASK      (0x00000002UL)
#define LPIT_MSR_TIF2_MASK      (0x00000004UL)
#define LPIT_MSR_TIF3_MASK      (0x00000008UL)

/*******************************************************************************
 * Module Interrupt Enable Register (MIER) Bit Definitions
 ******************************************************************************/

/** @brief Timer Interrupt Enable bits */
#define LPIT_MIER_TIE0_MASK     (0x00000001UL)
#define LPIT_MIER_TIE1_MASK     (0x00000002UL)
#define LPIT_MIER_TIE2_MASK     (0x00000004UL)
#define LPIT_MIER_TIE3_MASK     (0x00000008UL)

/*******************************************************************************
 * Timer Control Register (TCTRLn) Bit Definitions
 ******************************************************************************/

/** @brief Timer Enable - Enable/disable timer channel */
#define LPIT_TCTRL_T_EN_MASK    (0x00000001UL)
#define LPIT_TCTRL_T_EN_SHIFT   (0U)

/** @brief Chain Channel - Link with previous channel */
#define LPIT_TCTRL_CHAIN_MASK   (0x00000002UL)
#define LPIT_TCTRL_CHAIN_SHIFT  (1U)

/** @brief Timer Mode */
#define LPIT_TCTRL_MODE_MASK    (0x0000000CUL)
#define LPIT_TCTRL_MODE_SHIFT   (2U)

/** @brief Timer Start On Trigger */
#define LPIT_TCTRL_TSOT_MASK    (0x00010000UL)
#define LPIT_TCTRL_TSOT_SHIFT   (16U)

/** @brief Timer Stop On Interrupt */
#define LPIT_TCTRL_TSOI_MASK    (0x00020000UL)
#define LPIT_TCTRL_TSOI_SHIFT   (17U)

/** @brief Timer Reload On Trigger */
#define LPIT_TCTRL_TROT_MASK    (0x00040000UL)
#define LPIT_TCTRL_TROT_SHIFT   (18U)

/** @brief Trigger Source */
#define LPIT_TCTRL_TRG_SRC_MASK (0x00800000UL)
#define LPIT_TCTRL_TRG_SRC_SHIFT (23U)

/** @brief Trigger Select */
#define LPIT_TCTRL_TRG_SEL_MASK (0x0F000000UL)
#define LPIT_TCTRL_TRG_SEL_SHIFT (24U)



#endif /* LPIT_REG_H */
