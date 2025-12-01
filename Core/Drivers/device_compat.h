/**
 * @file    device_compat.h
 * @brief   Device Compatibility Layer for S32K144
 * @details Provides compatibility macros and definitions for SDK components
 * 
 * @author  PhucPH32
 * @date    December 2, 2025
 * @version 1.0
 */

#ifndef DEVICE_COMPAT_H
#define DEVICE_COMPAT_H

/*******************************************************************************
 * Debug Macros
 ******************************************************************************/

/**
 * @brief Breakpoint assembly instruction for debugging
 */
#ifndef BKPT_ASM
    #if defined(__GNUC__) || defined(__ARMCC_VERSION)
        #define BKPT_ASM    __asm volatile ("BKPT #0")
    #elif defined(__ICCARM__)
        #define BKPT_ASM    __asm("BKPT #0")
    #elif defined(__CC_ARM)
        #define BKPT_ASM    __breakpoint(0)
    #else
        #define BKPT_ASM    ((void)0)
    #endif
#endif

/**
 * @brief Development assertion macro
 */
#ifndef DEV_ASSERT
    #ifdef DEBUG
        #define DEV_ASSERT(x)   do { if(!(x)) { BKPT_ASM; while(1) {} } } while(0)
    #else
        #define DEV_ASSERT(x)   ((void)0)
    #endif
#endif

#endif /* DEVICE_COMPAT_H */
