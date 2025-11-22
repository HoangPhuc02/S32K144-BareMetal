# SK32144 C Code Convention Rules

## 1. Naming Conventions

### 1.1 Registers
- Use UPPER_CASE with `REG_` prefix
- Examples: `REG_CTRL`, `REG_STATUS`, `REG_DATA`

### 1.2 Register Bit Fields
- Use UPPER_CASE with register name + bit name
- Examples: `REG_CTRL_ENABLE`, `REG_STATUS_READY`, `REG_CTRL_MODE_MASK`

### 1.3 Register Addresses
- Use hexadecimal notation with `0x` prefix
- Define as macros with descriptive names
- Examples: 
  ```c
  #define SK32144_CTRL_REG_ADDR    0x00
  #define SK32144_STATUS_REG_ADDR  0x04
  #define SK32144_DATA_REG_ADDR    0x08
  ```

### 1.4 Functions
- **Initialization**: `SK32144_Init()`
- **Read operations**: `SK32144_ReadReg()`, `SK32144_GetStatus()`
- **Write operations**: `SK32144_WriteReg()`, `SK32144_SetConfig()`
- **Bit manipulation**: `SK32144_SetBit()`, `SK32144_ClearBit()`, `SK32144_ToggleBit()`
- Use PascalCase with `SK32144_` prefix for all public functions
- Use snake_case with `_` prefix for private/static functions

### 1.5 Variables
- **Global variables**: `g_variableName` (camelCase with `g_` prefix)
- **Local variables**: `variableName` (camelCase)
- **Pointer variables**: `p_variableName` (camelCase with `p_` prefix)
- **Constants**: `CONSTANT_NAME` (UPPER_CASE)

### 1.6 Data Types
- Use `typedef` for custom types with `_t` suffix
- Examples: `SK32144_Status_t`, `SK32144_Config_t`

## 2. File Organization

### 2.1 Header Files (.h)
```c
#ifndef SK32144_H
#define SK32144_H

/* Includes */
#include <stdint.h>
#include <stdbool.h>

/* Register Addresses */
#define SK32144_BASE_ADDR  0x40000000

/* Register Bit Definitions */
#define REG_CTRL_ENABLE    (1 << 0)
#define REG_CTRL_RESET     (1 << 1)

/* Type Definitions */
typedef enum {
    SK32144_OK = 0,
    SK32144_ERROR = 1
} SK32144_Status_t;

/* Function Prototypes */
SK32144_Status_t SK32144_Init(void);
void SK32144_WriteReg(uint32_t addr, uint32_t value);
uint32_t SK32144_ReadReg(uint32_t addr);

#endif /* SK32144_H */
```

### 2.2 Source Files (.c)
```c
/* Includes */
#include "sk32144.h"

/* Private Macros */
#define _TIMEOUT_VALUE  1000

/* Private Variables */
static uint32_t g_initFlag = 0;

/* Private Function Prototypes */
static void _SK32144_InternalInit(void);

/* Public Functions */
SK32144_Status_t SK32144_Init(void) {
    // Implementation
}

/* Private Functions */
static void _SK32144_InternalInit(void) {
    // Implementation
}
```

## 3. Register Access Patterns

### 3.1 Direct Register Access
```c
#define REG_WRITE(addr, value)  (*((volatile uint32_t *)(addr)) = (value))
#define REG_READ(addr)          (*((volatile uint32_t *)(addr)))
```

### 3.2 Bit Manipulation
```c
// Set bit
#define SET_BIT(reg, bit)       ((reg) |= (bit))

// Clear bit
#define CLEAR_BIT(reg, bit)     ((reg) &= ~(bit))

// Toggle bit
#define TOGGLE_BIT(reg, bit)    ((reg) ^= (bit))

// Read bit
#define READ_BIT(reg, bit)      ((reg) & (bit))

// Modify bits with mask
#define MODIFY_REG(reg, mask, value)  ((reg) = ((reg) & ~(mask)) | ((value) & (mask)))
```

### 3.3 Register Read-Modify-Write
```c
void SK32144_SetBit(uint32_t regAddr, uint32_t bitMask) {
    uint32_t regValue = REG_READ(regAddr);
    regValue |= bitMask;
    REG_WRITE(regAddr, regValue);
}
```

## 4. Code Formatting

### 4.1 Indentation
- Use 4 spaces for indentation (not tabs)
- Align braces on new lines for functions
- Keep braces on same line for control structures

### 4.2 Line Length
- Maximum 100 characters per line
- Break long lines at logical points

### 4.3 Comments
```c
/**
 * @brief Initialize SK32144 peripheral
 * 
 * @param config Pointer to configuration structure
 * @return SK32144_Status_t Operation status
 */
SK32144_Status_t SK32144_Init(SK32144_Config_t *config);

/* Single line comment for brief explanations */

// TODO: Add error handling
// FIXME: Bug in timeout calculation
```

### 4.4 Function Structure
```c
SK32144_Status_t SK32144_WriteReg(uint32_t addr, uint32_t value)
{
    /* Parameter validation */
    if (addr > SK32144_MAX_ADDR) {
        return SK32144_ERROR;
    }
    
    /* Write operation */
    REG_WRITE(addr, value);
    
    return SK32144_OK;
}
```

## 5. Register Definition Best Practices

### 5.1 Bit Position Definitions
```c
/* Define bit positions */
#define REG_CTRL_ENABLE_POS     0
#define REG_CTRL_MODE_POS       4
#define REG_CTRL_INT_EN_POS     8

/* Define bit masks using positions */
#define REG_CTRL_ENABLE_MASK    (0x1UL << REG_CTRL_ENABLE_POS)
#define REG_CTRL_MODE_MASK      (0xFUL << REG_CTRL_MODE_POS)
#define REG_CTRL_INT_EN_MASK    (0x1UL << REG_CTRL_INT_EN_POS)
```

### 5.2 Register Structure Definition
```c
typedef struct {
    volatile uint32_t CTRL;      /* Offset: 0x00 - Control Register */
    volatile uint32_t STATUS;    /* Offset: 0x04 - Status Register */
    volatile uint32_t DATA;      /* Offset: 0x08 - Data Register */
    uint32_t RESERVED[5];        /* Offset: 0x0C - Reserved */
} SK32144_Regs_t;

#define SK32144 ((SK32144_Regs_t *)SK32144_BASE_ADDR)
```

### 5.3 Usage Example
```c
/* Using structure-based access */
SK32144->CTRL |= REG_CTRL_ENABLE_MASK;
uint32_t status = SK32144->STATUS;

/* Using address-based access */
SK32144_WriteReg(SK32144_CTRL_REG_ADDR, REG_CTRL_ENABLE);
```

## 6. Error Handling

### 6.1 Return Status Codes
```c
typedef enum {
    SK32144_OK              = 0x00,
    SK32144_ERROR           = 0x01,
    SK32144_BUSY            = 0x02,
    SK32144_TIMEOUT         = 0x03,
    SK32144_INVALID_PARAM   = 0x04
} SK32144_Status_t;
```

### 6.2 Error Checking Pattern
```c
SK32144_Status_t status;

status = SK32144_Init(&config);
if (status != SK32144_OK) {
    /* Handle error */
    return status;
}

status = SK32144_Configure();
if (status != SK32144_OK) {
    /* Handle error */
    return status;
}
```

## 7. Documentation Requirements

### 7.1 File Header
```c
/**
 * @file    sk32144.h
 * @brief   SK32144 register definitions and API
 * @author  Your Name
 * @date    2024-11-22
 * @version 1.0
 */
```

### 7.2 Function Documentation
```c
/**
 * @brief Configure SK32144 operating mode
 * 
 * @param mode Operating mode (0-3)
 * @param flags Configuration flags
 * @return SK32144_Status_t 
 *         - SK32144_OK: Success
 *         - SK32144_INVALID_PARAM: Invalid mode value
 * 
 * @note This function must be called after SK32144_Init()
 */
SK32144_Status_t SK32144_SetMode(uint8_t mode, uint32_t flags);
```

## 8. Safety Guidelines

### 8.1 Volatile Keyword
- Always use `volatile` qualifier for hardware registers
- Example: `volatile uint32_t *reg_ptr`

### 8.2 Type Safety
- Use fixed-width integer types: `uint8_t`, `uint16_t`, `uint32_t`
- Avoid implicit type conversions

### 8.3 Magic Numbers
- Define all magic numbers as named constants
- Bad: `REG_WRITE(0x40000000, 0x1234)`
- Good: `REG_WRITE(SK32144_CTRL_REG_ADDR, CTRL_DEFAULT_VALUE)`

### 8.4 Critical Sections
```c
/* Disable interrupts for atomic operations */
__disable_irq();
SK32144->CTRL = new_value;
__enable_irq();
```

## 9. Example Complete Implementation

```c
/* sk32144.h */
#ifndef SK32144_H
#define SK32144_H

#include <stdint.h>

/* Base address */
#define SK32144_BASE_ADDR       0x40000000UL

/* Register offsets */
#define SK32144_CTRL_OFFSET     0x00
#define SK32144_STATUS_OFFSET   0x04
#define SK32144_DATA_OFFSET     0x08

/* Control register bits */
#define REG_CTRL_ENABLE         (1UL << 0)
#define REG_CTRL_RESET          (1UL << 1)
#define REG_CTRL_MODE_POS       4
#define REG_CTRL_MODE_MASK      (0xFUL << REG_CTRL_MODE_POS)

/* Status codes */
typedef enum {
    SK32144_OK = 0,
    SK32144_ERROR = 1
} SK32144_Status_t;

/* API Functions */
SK32144_Status_t SK32144_Init(void);
void SK32144_Enable(void);
void SK32144_Disable(void);
uint32_t SK32144_GetStatus(void);

#endif /* SK32144_H */
```

---

**Note**: Adapt these conventions based on your project requirements and team standards.