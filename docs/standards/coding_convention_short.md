# Coding Convention 
## 1. Naming Conventions

### 1.1 Module Prefixes
Mỗi module sử dụng tiền tố riêng cho tất cả symbols:
- **GPIO**: `GPIO_`, `gpio_`
- **UART**: `UART_`, `uart_`
- **DMA**: `DMA_`, `dma_`
- **LPIT**: `LPIT_`, `lpit_`
- **ADC**: `ADC_`, `adc_`

### 1.2 Register Names
Thanh ghi viết HOA, theo datasheet:
```c
/* Register structure */
typedef struct {
    __IO uint32_t CTRL;     /* Control Register */
    __I  uint32_t STATUS;   /* Status Register */
    __IO uint32_t DATA;     /* Data Register */
} PERIPHERAL_Type;

/* Base pointers viết HOA */
#define PTA         ((GPIO_Type *)GPIOA_BASE)
#define LPIT0       ((LPIT_Type *)LPIT0_BASE)
#define DMA         ((DMA_Type *)DMA_BASE)
```

### 1.3 Function Names
`MODULE_Action()` - Uppercase module prefix, PascalCase action:
```c
status_t GPIO_Init(gpio_port_t port, uint8_t pin);
status_t LPIT_ConfigChannel(uint8_t channel, const lpit_channel_config_t *config);
void DMA_StartChannel(uint8_t channel);
uint32_t UART_Read(void);
```

### 1.4 Type Definitions
Suffix `_t` cho tất cả typedef, lowercase với module prefix:
```c
typedef enum {
    GPIO_DIR_INPUT  = 0U,
    GPIO_DIR_OUTPUT = 1U
} gpio_direction_t;

typedef struct {
    uint8_t channel;
    uint32_t period;
    bool enableInterrupt;
} lpit_channel_config_t;

typedef void (*dma_callback_t)(uint8_t channel, void *userData);
```

### 1.5 Define Constants
`MODULE_REGISTER_FIELD_SUFFIX` - All UPPERCASE:
```c
/* Bit masks và shifts */
#define LPIT_MCR_M_CEN_MASK         (0x00000001UL)
#define LPIT_MCR_M_CEN_SHIFT        (0U)
#define DMA_TCD_ATTR_SSIZE_MASK     (0x0700U)
#define DMA_TCD_ATTR_SSIZE_SHIFT    (8U)

/* Constants */
#define GPIO_MAX_PINS               (32U)
#define DMA_MAX_CHANNELS            (16U)
#define LPIT_MAX_CHANNELS           (4U)

/* Base addresses */
#define GPIOA_BASE                  (0x400FF000UL)
#define LPIT0_BASE                  (0x40037000UL)
```

### 1.6 Variables

#### Local Variables
camelCase, bắt đầu bằng chữ thường:
```c
uint32_t regValue;
uint8_t channelIndex;
bool isEnabled;
```

#### Static Variables
Prefix `s_`, camelCase:
```c
static dma_callback_t s_dmaCallbacks[DMA_MAX_CHANNELS];
static uint32_t s_lpitClockFreq;
static bool s_isInitialized = false;
```

#### Global Variables
Prefix `g_`, camelCase:
```c
volatile uint32_t g_systemTicks;
uint8_t g_dataBuffer[256];
```

#### Pointer Variables
Suffix `Ptr`:
```c
GPIO_Type *basePtr;
const lpit_channel_config_t *configPtr;
uint8_t *dataPtr;
```

---

## 2. Register Access Patterns

### 2.1 Structure-Based Access (Preferred)
```c
/* Register structure definition */
typedef struct {
    __IO uint32_t PDOR;    /* Port Data Output Register */
    __I  uint32_t PDIR;    /* Port Data Input Register */
    __IO uint32_t PDDR;    /* Port Data Direction Register */
} GPIO_Type;

#define PTA         ((GPIO_Type *)0x400FF000UL)

/* Usage */
PTA->PDOR = 0x12345678UL;
uint32_t value = PTA->PDIR;
```

### 2.2 Bit Manipulation

> Nếu được thì tạo thành một các macro giành cho bit manipulation 
#### Set Bit
```c
/* Single bit */
LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;

/* Multiple bits */
LPIT0->CHANNEL[0].TCTRL |= (LPIT_TCTRL_T_EN_MASK | LPIT_TCTRL_TIE_MASK);
```

#### Clear Bit
```c
/* Single bit */
LPIT0->MCR &= ~LPIT_MCR_SW_RST_MASK;

/* Multiple bits */
DMA->TCD[channel].CSR &= ~(DMA_TCD_CSR_DONE_MASK | DMA_TCD_CSR_ACTIVE_MASK);
```

#### Toggle Bit
```c
PTA->PDOR ^= (1UL << pin);
```

#### Check Bit
```c
if (LPIT0->MSR & LPIT_MSR_TIF0_MASK) {
    /* Bit is set */
}

if ((DMA->TCD[channel].CSR & DMA_TCD_CSR_DONE_MASK) != 0U) {
    /* Transfer done */
}
```

#### Read Multi-Bit Field
```c
uint32_t value = (reg & DMA_TCD_ATTR_SSIZE_MASK) >> DMA_TCD_ATTR_SSIZE_SHIFT;
```

#### Write Multi-Bit Field
```c
/* Method 1: Clear then set */
reg &= ~DMA_TCD_ATTR_SSIZE_MASK;
reg |= ((newValue << DMA_TCD_ATTR_SSIZE_SHIFT) & DMA_TCD_ATTR_SSIZE_MASK);

/* Method 2: One line */
reg = (reg & ~DMA_TCD_ATTR_SSIZE_MASK) | 
      ((newValue << DMA_TCD_ATTR_SSIZE_SHIFT) & DMA_TCD_ATTR_SSIZE_MASK);
```

### 2.3 Bit Field Definitions Pattern
```c
/* Pattern: MODULE_REGISTER_FIELD_MASK/SHIFT */

/* Single bit flags */
#define LPIT_MCR_M_CEN_MASK         (0x00000001UL)
#define LPIT_MCR_M_CEN_SHIFT        (0U)
#define LPIT_MCR_SW_RST_MASK        (0x00000002UL)
#define LPIT_MCR_SW_RST_SHIFT       (1U)

/* Multi-bit fields */
#define LPIT_TCTRL_MODE_MASK        (0x0000000CUL)
#define LPIT_TCTRL_MODE_SHIFT       (2U)
#define DMA_TCD_ATTR_SSIZE_MASK     (0x0700U)
#define DMA_TCD_ATTR_SSIZE_SHIFT    (8U)
```

### 2.4 Volatile Keyword
Luôn dùng `volatile` cho hardware registers:
```c
typedef struct {
    __IO uint32_t CTRL;    /* __IO = volatile */
    __I  uint32_t STATUS;  /* __I = volatile const */
    __O  uint32_t DATA;    /* __O = volatile */
} PERIPHERAL_Type;

/* IO qualifiers */
#define __I     volatile const    /* Read only */
#define __O     volatile          /* Write only */
#define __IO    volatile          /* Read/Write */
```

---

## 3. File Structure

### 3.1 Register Definition File (*_reg.h)
```c
/**
 * @file    gpio_reg.h
 * @brief   GPIO Register Definitions for S32K144
 */

#ifndef GPIO_REG_H
#define GPIO_REG_H

#include "def_reg.h"

/* Base addresses */
#define GPIOA_BASE          (0x400FF000UL)
#define GPIOB_BASE          (0x400FF040UL)

/* Register structure */
typedef struct {
    __IO uint32_t PDOR;
    __I  uint32_t PDIR;
    __IO uint32_t PDDR;
} GPIO_Type;

/* Register pointers */
#define PTA                 ((GPIO_Type *)GPIOA_BASE)
#define PTB                 ((GPIO_Type *)GPIOB_BASE)

#endif /* GPIO_REG_H */
```

### 3.2 API Header File (*.h)
```c
/**
 * @file    gpio.h
 * @brief   GPIO Driver API for S32K144
 */

#ifndef GPIO_H
#define GPIO_H

#include "gpio_reg.h"
#include "status.h"
#include <stdint.h>

/* Constants */
#define GPIO_MAX_PINS       (32U)

/* Types */
typedef enum {
    GPIO_DIR_INPUT  = 0U,
    GPIO_DIR_OUTPUT = 1U
} gpio_direction_t;

/* API Functions */
status_t GPIO_Init(gpio_port_t port, uint8_t pin, gpio_direction_t direction);
void GPIO_SetPin(gpio_port_t port, uint8_t pin);

#endif /* GPIO_H */
```

### 3.3 Implementation File (*.c)
```c
/**
 * @file    gpio.c
 * @brief   GPIO Driver Implementation
 */

#include "gpio.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static bool s_isInitialized = false;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static GPIO_Type* GPIO_GetBase(gpio_port_t port)
{
    /* Implementation */
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/
status_t GPIO_Init(gpio_port_t port, uint8_t pin, gpio_direction_t direction)
{
    /* Implementation */
}
```

---

## 4. Common Patterns

### 4.1 Status Return Type
```c
typedef enum {
    STATUS_SUCCESS          = 0x00U,
    STATUS_ERROR            = 0x01U,
    STATUS_BUSY             = 0x02U,
    STATUS_TIMEOUT          = 0x03U,
    STATUS_INVALID_PARAM    = 0x04U
} status_t;
```

### 4.2 Parameter Validation
```c
status_t DMA_ConfigChannel(uint8_t channel, const dma_channel_config_t *config)
{
    /* Validate range */
    if (channel >= DMA_MAX_CHANNELS) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Validate pointer */
    if (config == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Implementation */
    return STATUS_SUCCESS;
}
```

### 4.3 Fixed-Width Integer Types
```c
uint8_t  byte;       /* Always 8 bits */
uint16_t word;       /* Always 16 bits */
uint32_t dword;      /* Always 32 bits */
int32_t  signed;     /* Always 32 bits signed */
bool     flag;       /* true/false from stdbool.h */
```

### 4.4 Constants with Type Suffix
```c
#define VALUE_8BIT      (0x12U)         /* uint8_t */
#define VALUE_16BIT     (0x1234U)       /* uint16_t */
#define VALUE_32BIT     (0x12345678UL)  /* uint32_t */
#define VALUE_64BIT     (0x123456789ABCDEFULL)  /* uint64_t */
```

---

## Quick Examples

### Complete Register Access
```c
/* Definition */
#define LPIT_TCTRL_T_EN_MASK    (0x00000001UL)
#define LPIT_TCTRL_MODE_MASK    (0x0000000CUL)
#define LPIT_TCTRL_MODE_SHIFT   (2U)

/* Enable timer */
LPIT0->CHANNEL[0].TCTRL |= LPIT_TCTRL_T_EN_MASK;

/* Set mode to periodic (value = 0) */
LPIT0->CHANNEL[0].TCTRL &= ~LPIT_TCTRL_MODE_MASK;
LPIT0->CHANNEL[0].TCTRL |= ((0U << LPIT_TCTRL_MODE_SHIFT) & LPIT_TCTRL_MODE_MASK);

/* Read and check */
if (LPIT0->MSR & LPIT_MSR_TIF0_MASK) {
    LPIT0->MSR = LPIT_MSR_TIF0_MASK;  /* Clear flag by writing 1 */
}
```

### Complete Function
```c
status_t GPIO_SetPin(gpio_port_t port, uint8_t pin)
{
    GPIO_Type *basePtr;
    
    /* Validate */
    if (pin >= GPIO_MAX_PINS) {
        return STATUS_INVALID_PARAM;
    }
    
    basePtr = GPIO_GetBase(port);
    if (basePtr == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Set bit */
    basePtr->PDOR |= (1UL << pin);
    
    return STATUS_SUCCESS;
}
```

