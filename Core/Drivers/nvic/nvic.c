/**
 * @file    nvic.c
 * @brief   NVIC Driver Implementation for S32K144
 * @details Implementation of NVIC driver functions
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "nvic.h"

/*******************************************************************************
 * Private Definitions
 ******************************************************************************/

/** @brief Number of implemented priority bits (4 bits = 16 levels) */
#define NVIC_PRIO_BITS          (4U)

/** @brief Vector table alignment requirement */
#define NVIC_VTOR_ALIGNMENT     (128U)

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Enable interrupt
 */
status_t NVIC_EnableIRQ(IRQn_Type IRQn)
{
    /* Validate parameter */
    if ((int32_t)IRQn < 0 || (int32_t)IRQn >= NVIC_NUM_INTERRUPTS) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Enable interrupt */
    NVIC->ISER[((uint32_t)IRQn >> 5U)] = (1UL << ((uint32_t)IRQn & 0x1FU));
    
    return STATUS_SUCCESS;
}

/**
 * @brief Disable interrupt
 */
status_t NVIC_DisableIRQ(IRQn_Type IRQn)
{
    /* Validate parameter */
    if ((int32_t)IRQn < 0 || (int32_t)IRQn >= NVIC_NUM_INTERRUPTS) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Disable interrupt */
    NVIC->ICER[((uint32_t)IRQn >> 5U)] = (1UL << ((uint32_t)IRQn & 0x1FU));
    
    /* Ensure write completes before returning */
    __asm volatile ("dsb");
    __asm volatile ("isb");
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get interrupt enable status
 */
status_t NVIC_GetEnableIRQ(IRQn_Type IRQn, bool *isEnabled)
{
    /* Validate parameters */
    if ((int32_t)IRQn < 0 || (int32_t)IRQn >= NVIC_NUM_INTERRUPTS || isEnabled == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Read enable status */
    *isEnabled = ((NVIC->ISER[((uint32_t)IRQn >> 5U)] & 
                   (1UL << ((uint32_t)IRQn & 0x1FU))) != 0U);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Set interrupt pending
 */
status_t NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
    /* Validate parameter */
    if ((int32_t)IRQn < 0 || (int32_t)IRQn >= NVIC_NUM_INTERRUPTS) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Set pending bit */
    NVIC->ISPR[((uint32_t)IRQn >> 5U)] = (1UL << ((uint32_t)IRQn & 0x1FU));
    
    return STATUS_SUCCESS;
}

/**
 * @brief Clear interrupt pending
 */
status_t NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
    /* Validate parameter */
    if ((int32_t)IRQn < 0 || (int32_t)IRQn >= NVIC_NUM_INTERRUPTS) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Clear pending bit */
    NVIC->ICPR[((uint32_t)IRQn >> 5U)] = (1UL << ((uint32_t)IRQn & 0x1FU));
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get interrupt pending status
 */
status_t NVIC_GetPendingIRQ(IRQn_Type IRQn, bool *isPending)
{
    /* Validate parameters */
    if ((int32_t)IRQn < 0 || (int32_t)IRQn >= NVIC_NUM_INTERRUPTS || isPending == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Read pending status */
    *isPending = ((NVIC->ISPR[((uint32_t)IRQn >> 5U)] & 
                   (1UL << ((uint32_t)IRQn & 0x1FU))) != 0U);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get interrupt active status
 */
status_t NVIC_GetActiveIRQ(IRQn_Type IRQn, bool *isActive)
{
    /* Validate parameters */
    if ((int32_t)IRQn < 0 || (int32_t)IRQn >= NVIC_NUM_INTERRUPTS || isActive == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Read active status */
    *isActive = ((NVIC->IABR[((uint32_t)IRQn >> 5U)] & 
                  (1UL << ((uint32_t)IRQn & 0x1FU))) != 0U);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Set interrupt priority
 */
status_t NVIC_SetPriority(IRQn_Type IRQn, uint8_t priority)
{
    /* Validate parameters */
    if ((int32_t)IRQn < 0 || (int32_t)IRQn >= NVIC_NUM_INTERRUPTS) {
        return STATUS_INVALID_PARAM;
    }
    
    if (priority > NVIC_PRIORITY_MIN) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Set priority - shift left because only upper 4 bits are used */
    NVIC->IP[(uint32_t)IRQn] = (priority << (8U - NVIC_PRIO_BITS));
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get interrupt priority
 */
status_t NVIC_GetPriority(IRQn_Type IRQn, uint8_t *priority)
{
    /* Validate parameters */
    if ((int32_t)IRQn < 0 || (int32_t)IRQn >= NVIC_NUM_INTERRUPTS || priority == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Get priority - shift right because only upper 4 bits are used */
    *priority = (NVIC->IP[(uint32_t)IRQn] >> (8U - NVIC_PRIO_BITS));
    
    return STATUS_SUCCESS;
}

/**
 * @brief Set priority grouping
 */
status_t NVIC_SetPriorityGrouping(nvic_priority_group_t priorityGroup)
{
    uint32_t regValue;
    
    /* Validate parameter */
    if (priorityGroup > NVIC_PRIORITYGROUP_0) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Read-Modify-Write AIRCR register */
    regValue = SCB->AIRCR;
    regValue &= ~(SCB_AIRCR_VECTKEY_MASK | SCB_AIRCR_PRIGROUP_MASK);
    regValue |= (SCB_AIRCR_VECTKEY | 
                ((uint32_t)priorityGroup << SCB_AIRCR_PRIGROUP_SHIFT));
    SCB->AIRCR = regValue;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get priority grouping
 */
status_t NVIC_GetPriorityGrouping(nvic_priority_group_t *priorityGroup)
{
    /* Validate parameter */
    if (priorityGroup == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Read priority grouping from AIRCR */
    *priorityGroup = (nvic_priority_group_t)((SCB->AIRCR & SCB_AIRCR_PRIGROUP_MASK) >> 
                                             SCB_AIRCR_PRIGROUP_SHIFT);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Encode priority
 */
status_t NVIC_EncodePriority(nvic_priority_group_t priorityGroup,
                              uint8_t preemptPriority,
                              uint8_t subPriority,
                              uint8_t *priority)
{
    uint32_t preemptBits;
    uint32_t subBits;
    
    /* Validate parameters */
    if (priority == NULL || priorityGroup > NVIC_PRIORITYGROUP_0) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Calculate number of bits for preempt and sub priority */
    preemptBits = 7U - (uint32_t)priorityGroup;
    subBits = (uint32_t)priorityGroup - (7U - NVIC_PRIO_BITS);
    
    /* Validate preempt and sub priority values */
    if (preemptPriority >= (1UL << preemptBits) || 
        subPriority >= (1UL << subBits)) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Encode priority */
    *priority = (uint8_t)(((preemptPriority & ((1UL << preemptBits) - 1UL)) << subBits) |
                         (subPriority & ((1UL << subBits) - 1UL)));
    
    return STATUS_SUCCESS;
}

/**
 * @brief Decode priority
 */
status_t NVIC_DecodePriority(nvic_priority_group_t priorityGroup,
                              uint8_t priority,
                              uint8_t *preemptPriority,
                              uint8_t *subPriority)
{
    uint32_t preemptBits;
    uint32_t subBits;
    
    /* Validate parameters */
    if (preemptPriority == NULL || subPriority == NULL || 
        priorityGroup > NVIC_PRIORITYGROUP_0) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Calculate number of bits */
    preemptBits = 7U - (uint32_t)priorityGroup;
    subBits = (uint32_t)priorityGroup - (7U - NVIC_PRIO_BITS);
    
    /* Decode priority */
    *preemptPriority = (priority >> subBits) & ((1UL << preemptBits) - 1UL);
    *subPriority = priority & ((1UL << subBits) - 1UL);
    
    return STATUS_SUCCESS;
}

/**
 * @brief System reset
 */
void NVIC_SystemReset(void)
{
    /* Ensure all memory accesses are complete */
    __asm volatile ("dsb");
    
    /* Request system reset */
    SCB->AIRCR = (SCB_AIRCR_VECTKEY | 
                 (SCB->AIRCR & SCB_AIRCR_PRIGROUP_MASK) |
                 (1UL << 2U));  /* SYSRESETREQ bit */
    
    /* Ensure write completes */
    __asm volatile ("dsb");
    
    /* Wait for reset */
    while (1) {
        __asm volatile ("nop");
    }
}

/**
 * @brief Disable all interrupts (global)
 */
uint32_t NVIC_DisableGlobalIRQ(void)
{
    uint32_t primask;
    
    /* Read current PRIMASK value */
    __asm volatile ("mrs %0, primask" : "=r" (primask));
    
    /* Disable interrupts */
    __asm volatile ("cpsid i" ::: "memory");
    
    return primask;
}

/**
 * @brief Enable all interrupts (global)
 */
void NVIC_EnableGlobalIRQ(uint32_t primask)
{
    /* Restore PRIMASK value */
    __asm volatile ("msr primask, %0" :: "r" (primask) : "memory");
}

/**
 * @brief Set vector table offset
 */
status_t NVIC_SetVectorTable(uint32_t offset)
{
    /* Check alignment */
    if ((offset & (NVIC_VTOR_ALIGNMENT - 1U)) != 0U) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Set vector table offset */
    SCB->VTOR = offset;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get vector table offset
 */
status_t NVIC_GetVectorTable(uint32_t *offset)
{
    /* Validate parameter */
    if (offset == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Read vector table offset */
    *offset = SCB->VTOR;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Wait for interrupt
 */
void NVIC_WaitForInterrupt(void)
{
    __asm volatile ("wfi");
}

/**
 * @brief Wait for event
 */
void NVIC_WaitForEvent(void)
{
    __asm volatile ("wfe");
}

/**
 * @brief Send event
 */
void NVIC_SendEvent(void)
{
    __asm volatile ("sev");
}

/**
 * @brief Enable fault exceptions
 */
status_t NVIC_EnableFaultHandlers(bool enableUsageFault,
                                   bool enableBusFault,
                                   bool enableMemManageFault)
{
    uint32_t shcsr = SCB->SHCSR;
    
    if (enableUsageFault) {
        shcsr |= (1UL << 18U);  /* USGFAULTENA */
    }
    
    if (enableBusFault) {
        shcsr |= (1UL << 17U);  /* BUSFAULTENA */
    }
    
    if (enableMemManageFault) {
        shcsr |= (1UL << 16U);  /* MEMFAULTENA */
    }
    
    SCB->SHCSR = shcsr;
    
    return STATUS_SUCCESS;
}
