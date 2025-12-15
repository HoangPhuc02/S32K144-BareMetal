# S32K144 BareMetal C Code Convention Rules

## 1. Naming Conventions

### 1.1 Module/Driver Names
- Use UPPER_CASE for module prefixes: `GPIO`, `UART`, `DMA`, `LPIT`, `I2C`, `SPI`
- Files follow lowercase with module name: `gpio.h`, `gpio.c`, `uart_reg.h`

### 1.2 Registers and Register Structures
- Use UPPER_CASE for register base names
- Structure-based access preferred over direct addressing
- Examples:
  ```c
  #define GPIO_BASE           0x400FF000UL
  #define LPIT0_BASE          0x40037000UL
  
  typedef struct {
      __IO uint32_t PDOR;    /**< Port Data Output Register */
      __I  uint32_t PDIR;    /**< Port Data Input Register */
      __IO uint32_t PDDR;    /**< Port Data Direction Register */
  } GPIO_Type;
  
  #define PTA                 ((GPIO_Type *)GPIOA_BASE)
  ```

### 1.3 Register Bit Fields
- Use `MODULE_REG_FIELD_MASK` format for bit masks
- Use `MODULE_REG_FIELD_SHIFT` format for bit positions
- Examples:
  ```c
  #define DMA_TCD_CSR_START_MASK      (0x0001U)
  #define DMA_TCD_CSR_INTMAJOR_MASK   (0x0002U)
  #define LPIT_MCR_M_CEN_MASK         (0x00000001UL)
  #define LPIT_MCR_M_CEN_SHIFT        (0U)
  ```

### 1.4 Functions
- **Module prefix + Action + Object** format
- Use PascalCase for public API functions
- Use lowercase snake_case with `static` for private functions
- Examples:
  ```c
  /* Public API - PascalCase with MODULE_ prefix */
  status_t GPIO_Init(gpio_port_t port, uint8_t pin, gpio_direction_t direction);
  void GPIO_SetPin(gpio_port_t port, uint8_t pin);
  void UART_SendByte(LPUART_Type *base, uint8_t data);
  status_t DMA_ConfigChannel(const dma_channel_config_t *config);
  status_t LPIT_StartChannel(uint8_t channel);
  
  /* Private functions - lowercase snake_case with static */
  static status_t uart_configure_baudrate(LPUART_Type *base, uint32_t baudrate);
  static void dma_reset_tcd(uint8_t channel);
  static bool lpit_is_valid_channel(uint8_t channel);
  ```

### 1.5 Variables
- **Static module variables**: `s_moduleName` (camelCase with `s_` prefix)
- **Global variables**: `g_variableName` (camelCase with `g_` prefix)
- **Local variables**: `variableName` (camelCase)
- **Constants**: `CONSTANT_NAME` (UPPER_CASE with module prefix)
- Examples:
  ```c
  /* Static module variables */
  static dma_callback_t s_dmaCallbacks[DMA_MAX_CHANNELS];
  static bool s_lpitInitialized = false;
  static uint32_t s_lpitClockFreq = 8000000U;
  
  /* Global variables (avoid when possible) */
  uint32_t g_systemCoreClock = 80000000U;
  
  /* Local variables */
  uint32_t channelConfig;
  bool isEnabled;
  
  /* Constants */
  #define GPIO_MAX_PINS       (32U)
  #define UART_TIMEOUT_VALUE  (1000U)
  ```

### 1.6 Data Types
- Use `typedef` for enums and structs with `_t` suffix
- Use lowercase with underscores for type names
- Module prefix for driver-specific types
- Examples:
  ```c
  /* Enums */
  typedef enum {
      GPIO_DIR_INPUT  = 0U,
      GPIO_DIR_OUTPUT = 1U
  } gpio_direction_t;
  
  typedef enum {
      DMA_TRANSFER_SIZE_1B = 0U,
      DMA_TRANSFER_SIZE_2B = 1U,
      DMA_TRANSFER_SIZE_4B = 2U
  } dma_transfer_size_t;
  
  /* Structures */
  typedef struct {
      uint8_t channel;
      uint32_t period;
      bool enableInterrupt;
  } lpit_channel_config_t;
  
  typedef struct {
      uint32_t baudRate;
      uart_parity_t parity;
      bool enableTx;
      bool enableRx;
  } uart_config_t;
  
  /* Function pointers */
  typedef void (*dma_callback_t)(uint8_t channel, void *userData);
  typedef void (*lpit_callback_t)(uint8_t channel, void *userData);
  ```

### 1.7 Status Return Types
- Use common `status_t` enum across all drivers
- Defined in `status.h`:
  ```c
  typedef enum {
      STATUS_SUCCESS = 0x00U,
      STATUS_ERROR   = 0x01U,
      STATUS_BUSY    = 0x02U,
      STATUS_TIMEOUT = 0x03U
  } status_t;
  ```

## 2. File Organization

### 2.1 Header Files (.h)
Structure for driver header files:
```c
/**
 * @file    gpio.h
 * @brief   GPIO Driver Header File for S32K144
 * @details This file contains the GPIO driver interface declarations,
 *          macros, and data structures for S32K144 microcontroller.
 * 
 * @author  xxx
 * @date    23/11/2025
 * @version 1.0
 * 
 * @note This driver provides register-level access to GPIO peripherals
 * @warning Ensure proper clock configuration before using GPIO functions
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 23, 2025): Initial version
 */

#ifndef GPIO_H
#define GPIO_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "gpio_reg.h"
#include "status.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup GPIO_Definitions GPIO Definitions
 * @{
 */

/** @brief Maximum number of pins per port */
#define GPIO_MAX_PINS       (32U)

/**
 * @brief GPIO port enumeration
 */
typedef enum {
    GPIO_PORT_A = 0U,    /**< Port A */
    GPIO_PORT_B = 1U,    /**< Port B */
    GPIO_PORT_C = 2U,    /**< Port C */
    GPIO_PORT_D = 3U,    /**< Port D */
    GPIO_PORT_E = 4U     /**< Port E */
} gpio_port_t;

/**
 * @brief GPIO pin direction enumeration
 */
typedef enum {
    GPIO_DIR_INPUT  = 0U,    /**< Pin configured as input */
    GPIO_DIR_OUTPUT = 1U     /**< Pin configured as output */
} gpio_direction_t;

/** @} */ /* End of GPIO_Definitions */

/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @defgroup GPIO_Functions GPIO Functions
 * @{
 */

/**
 * @brief Initialize GPIO pin
 * @details Configures the specified pin as GPIO with the given direction
 * 
 * @param[in] port GPIO port (GPIO_PORT_A to GPIO_PORT_E)
 * @param[in] pin Pin number (0-31)
 * @param[in] direction Pin direction (GPIO_DIR_INPUT or GPIO_DIR_OUTPUT)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Success
 *         - STATUS_ERROR: Invalid parameters
 * 
 * @note Clock for the port must be enabled before calling this function
 * 
 * @code
 * // Initialize PTD15 as output (Red LED)
 * GPIO_Init(GPIO_PORT_D, 15, GPIO_DIR_OUTPUT);
 * @endcode
 */
status_t GPIO_Init(gpio_port_t port, uint8_t pin, gpio_direction_t direction);

/**
 * @brief Set GPIO pin to high level
 * 
 * @param[in] port GPIO port
 * @param[in] pin Pin number (0-31)
 * 
 * @return None
 */
void GPIO_SetPin(gpio_port_t port, uint8_t pin);

/** @} */ /* End of GPIO_Functions */

#endif /* GPIO_H */
```

### 2.2 Register Definition Files (*_reg.h)
Separate register definitions from API:
```c
/**
 * @file    gpio_reg.h
 * @brief   GPIO Register Definitions for S32K144
 * @details Register addresses, bit fields, and structures for GPIO/PORT
 * 
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 */

#ifndef GPIO_REG_H
#define GPIO_REG_H

#include "def_reg.h"

/*******************************************************************************
 * Register Base Addresses
 ******************************************************************************/

/** @brief GPIO base addresses */
#define GPIOA_BASE          (0x400FF000UL)
#define GPIOB_BASE          (0x400FF040UL)
#define GPIOC_BASE          (0x400FF080UL)
#define GPIOD_BASE          (0x400FF0C0UL)
#define GPIOE_BASE          (0x400FF100UL)

/*******************************************************************************
 * Register Structure Definitions
 ******************************************************************************/

/**
 * @brief GPIO Register Structure
 */
typedef struct {
    __IO uint32_t PDOR;    /**< Port Data Output Register, offset: 0x00 */
    __I  uint32_t PDIR;    /**< Port Data Input Register, offset: 0x04 */
    __IO uint32_t PDDR;    /**< Port Data Direction Register, offset: 0x08 */
    __IO uint32_t PIDR;    /**< Port Input Disable Register, offset: 0x0C */
} GPIO_Type;

/*******************************************************************************
 * Register Access Macros
 ******************************************************************************/

/** @brief GPIO port pointers */
#define PTA                 ((GPIO_Type *)GPIOA_BASE)
#define PTB                 ((GPIO_Type *)GPIOB_BASE)
#define PTC                 ((GPIO_Type *)GPIOC_BASE)
#define PTD                 ((GPIO_Type *)GPIOD_BASE)
#define PTE                 ((GPIO_Type *)GPIOE_BASE)

#endif /* GPIO_REG_H */
```

### 2.3 Source Files (.c)
Implementation file structure:
```c
/**
 * @file    gpio.c
 * @brief   GPIO Driver Implementation for S32K144
 * @details Implementation of GPIO control functions
 * 
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "gpio.h"
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Private constant for validation */
#define GPIO_PIN_MAX        (31U)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Static initialization flag */
static bool s_gpioInitialized[5] = {false};

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Validate GPIO port and pin
 * @param port GPIO port
 * @param pin Pin number
 * @return true if valid, false otherwise
 */
static bool gpio_is_valid_pin(gpio_port_t port, uint8_t pin);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static bool gpio_is_valid_pin(gpio_port_t port, uint8_t pin)
{
    return ((port <= GPIO_PORT_E) && (pin <= GPIO_PIN_MAX));
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

status_t GPIO_Init(gpio_port_t port, uint8_t pin, gpio_direction_t direction)
{
    GPIO_Type *base;
    
    /* Validate parameters */
    if (!gpio_is_valid_pin(port, pin)) {
        return STATUS_ERROR;
    }
    
    /* Get port base address */
    base = gpio_get_base(port);
    
    /* Configure direction */
    if (direction == GPIO_DIR_OUTPUT) {
        base->PDDR |= (1U << pin);
    } else {
        base->PDDR &= ~(1U << pin);
    }
    
    s_gpioInitialized[port] = true;
    
    return STATUS_SUCCESS;
}

void GPIO_SetPin(gpio_port_t port, uint8_t pin)
{
    GPIO_Type *base = gpio_get_base(port);
    base->PDOR |= (1U << pin);
}
```

### 2.4 Common Definitions (def_reg.h)
Shared register access macros and definitions:
```c
/**
 * @file    def_reg.h
 * @brief   Common register helper macros and definitions
 * @details Low-level register access macros for all drivers
 */

#ifndef DEF_REG_H
#define DEF_REG_H

#include <stdint.h>

/*******************************************************************************
 * IO Type Qualifiers
 ******************************************************************************/

#ifndef __IO
#ifdef __cplusplus
  #define   __I     volatile             /**< Read only */
#else
  #define   __I     volatile const       /**< Read only */
#endif
#define     __O     volatile             /**< Write only */
#define     __IO    volatile             /**< Read/Write */
#endif

/*******************************************************************************
 * Register Access Macros
 ******************************************************************************/

/** @brief 32-bit register read */
#define REG_READ32(addr)           (*((volatile uint32_t *)(addr)))

/** @brief 32-bit register write */
#define REG_WRITE32(addr, value)   (*((volatile uint32_t *)(addr)) = (value))

/** @brief Set bits in 32-bit register */
#define REG_BIT_SET32(addr, mask)  (*((volatile uint32_t *)(addr)) |= (mask))

/** @brief Clear bits in 32-bit register */
#define REG_BIT_CLEAR32(addr, mask) (*((volatile uint32_t *)(addr)) &= ~(mask))

/** @brief Read-modify-write 32-bit register */
#define REG_RMW32(addr, mask, value) \
    (*((volatile uint32_t *)(addr)) = \
     ((*((volatile uint32_t *)(addr)) & ~(mask)) | ((value) & (mask))))

/*******************************************************************************
 * Common Status Type
 ******************************************************************************/

/**
 * @brief Common status type for all drivers
 */
typedef enum {
    STATUS_SUCCESS = 0x00U,    /**< Operation successful */
    STATUS_ERROR   = 0x01U,    /**< General error */
    STATUS_BUSY    = 0x02U,    /**< Resource busy */
    STATUS_TIMEOUT = 0x03U     /**< Operation timeout */
} status_t;

#endif /* DEF_REG_H */
```

## 3. Register Access Patterns

### 3.1 Structure-Based Access (Preferred)
Use structure pointers for register access:
```c
/* Definition in *_reg.h */
typedef struct {
    __IO uint32_t TVAL;     /**< Timer Value Register */
    __I  uint32_t CVAL;     /**< Current Timer Value Register */
    __IO uint32_t TCTRL;    /**< Timer Control Register */
} LPIT_CHANNEL_Type;

typedef struct {
    __IO uint32_t MCR;              /**< Module Control Register */
    __IO uint32_t MSR;              /**< Module Status Register */
    __IO uint32_t MIER;             /**< Module Interrupt Enable Register */
    LPIT_CHANNEL_Type CHANNEL[4];  /**< Timer Channels 0-3 */
} LPIT_Type;

#define LPIT0    ((LPIT_Type *)LPIT0_BASE)

/* Usage in .c file */
LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;           /* Enable module clock */
LPIT0->CHANNEL[0].TVAL = 8000 - 1;          /* Set timer value */
uint32_t currentValue = LPIT0->CHANNEL[0].CVAL;  /* Read current value */
```

### 3.2 Macro-Based Access (Alternative)
Use macros when structure access is not suitable:
```c
/* Register access macros */
#define REG_READ32(addr)           (*((volatile uint32_t *)(addr)))
#define REG_WRITE32(addr, value)   (*((volatile uint32_t *)(addr)) = (value))
#define REG_BIT_SET32(addr, mask)  (*((volatile uint32_t *)(addr)) |= (mask))
#define REG_BIT_CLEAR32(addr, mask) (*((volatile uint32_t *)(addr)) &= ~(mask))

/* Usage example */
REG_BIT_SET32(DMA_BASE + 0x0000, DMA_CR_EDBG_MASK);
REG_WRITE32(LPIT0_BASE + 0x0008, LPIT_MCR_M_CEN_MASK);
```

### 3.3 Bit Manipulation Patterns
Standard patterns for bit operations:
```c
/* Set specific bits (OR operation) */
LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;
DMA->TCD[channel].CSR |= DMA_TCD_CSR_START_MASK;

/* Clear specific bits (AND with complement) */
LPIT0->MCR &= ~LPIT_MCR_DOZE_EN_MASK;
GPIO->PDDR &= ~(1U << pin);

/* Toggle bits (XOR operation) */
GPIO->PDOR ^= (1U << pin);

/* Read and test bits */
if ((LPIT0->MSR & LPIT_MSR_TIF0_MASK) != 0U) {
    /* Timer interrupt flag is set */
}

/* Read-Modify-Write with mask */
uint32_t temp = LPIT0->CHANNEL[0].TCTRL;
temp &= ~LPIT_TCTRL_MODE_MASK;                    /* Clear mode bits */
temp |= (mode << LPIT_TCTRL_MODE_SHIFT) & LPIT_TCTRL_MODE_MASK;  /* Set new mode */
LPIT0->CHANNEL[0].TCTRL = temp;

/* Using macro for RMW */
#define REG_RMW32(addr, mask, value) \
    (*((volatile uint32_t *)(addr)) = \
     ((*((volatile uint32_t *)(addr)) & ~(mask)) | ((value) & (mask))))
```

### 3.4 Multi-Bit Field Access
Handle multi-bit fields properly:
```c
/* Define bit field positions and masks */
#define DMA_TCD_ATTR_SSIZE_SHIFT    (8U)
#define DMA_TCD_ATTR_SSIZE_MASK     (0x0700U)
#define DMA_TCD_ATTR_DSIZE_SHIFT    (0U)
#define DMA_TCD_ATTR_DSIZE_MASK     (0x0007U)

/* Set multi-bit field value */
uint16_t attr = 0U;
attr |= ((uint16_t)transferSize << DMA_TCD_ATTR_SSIZE_SHIFT) & DMA_TCD_ATTR_SSIZE_MASK;
attr |= ((uint16_t)transferSize << DMA_TCD_ATTR_DSIZE_SHIFT) & DMA_TCD_ATTR_DSIZE_MASK;
DMA->TCD[channel].ATTR = attr;

/* Extract multi-bit field value */
uint8_t mode = (LPIT0->CHANNEL[ch].TCTRL & LPIT_TCTRL_MODE_MASK) >> LPIT_TCTRL_MODE_SHIFT;
```

## 4. Code Formatting

### 4.1 Indentation
- Use **4 spaces** for indentation (not tabs)
- Align opening braces on **same line** for functions and structures
- Keep braces on **same line** for control structures
- Example:
  ```c
  status_t GPIO_Init(gpio_port_t port, uint8_t pin, gpio_direction_t direction)
  {
      GPIO_Type *base;
      
      if (!gpio_is_valid_pin(port, pin)) {
          return STATUS_ERROR;
      }
      
      for (uint8_t i = 0; i < MAX_ITERATIONS; i++) {
          /* Process iteration */
      }
      
      return STATUS_SUCCESS;
  }
  ```

### 4.2 Line Length
- Maximum **120 characters** per line
- Break long lines at logical points (parameters, operators)
- Indent continuation lines by 4 spaces
- Examples:
  ```c
  /* Long function call - break at parameters */
  status_t result = DMA_ConfigChannel(channelNum, sourceAddr, destAddr,
      transferSize, interruptEnable, chainMode);
  
  /* Long condition - break at logical operators */
  if ((LPIT0->MSR & LPIT_MSR_TIF0_MASK) != 0U &&
      (s_lpitCallbacks[channel] != NULL)) {
      /* Handle interrupt */
  }
  
  /* Long structure initialization */
  lpit_channel_config_t config = {
      .channel = 0,
      .mode = LPIT_MODE_32BIT_PERIODIC,
      .period = 8000 - 1,
      .enableInterrupt = true,
      .chainChannel = false
  };
  ```

### 4.3 Comments and Documentation

#### Doxygen Function Documentation
Use Doxygen format for all public API functions:
```c
/**
 * @brief Start a LPIT timer channel
 * @details Enables the timer channel to begin counting down from TVAL
 * 
 * @param[in] channel LPIT channel number (0-3)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Channel started successfully
 *         - STATUS_ERROR: Invalid channel number
 * 
 * @note Timer must be configured before calling this function
 * @warning Do not modify TVAL while timer is running
 * 
 * @code
 * // Start LPIT channel 0
 * LPIT_StartChannel(0);
 * @endcode
 */
status_t LPIT_StartChannel(uint8_t channel);
```

#### Block Comments
Use C-style comments for block descriptions:
```c
/* 
 * Configure DMA Transfer Control Descriptor (TCD)
 * - Set source and destination addresses
 * - Configure transfer size and offsets
 * - Enable interrupts if needed
 */
```

#### Single-Line Comments
Use C-style for brief inline comments:
```c
uint32_t timeout = 1000U;  /* Timeout in milliseconds */
base->PDDR |= (1U << pin); /* Set pin as output */
```

#### TODO/FIXME/NOTE Tags
```c
/* TODO: Add DMA error handling */
/* FIXME: Clock frequency calculation incorrect for SPLL */
/* NOTE: This function assumes clock is already enabled */
```

### 4.4 Function Structure
Standard function implementation pattern:
```c
status_t DMA_ConfigChannel(const dma_channel_config_t *config)
{
    uint32_t tcdControl;
    uint8_t channel;
    
    /* Validate parameters */
    if (config == NULL) {
        return STATUS_ERROR;
    }
    
    channel = config->channel;
    
    if (!DMA_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Disable channel before configuration */
    DMA_StopChannel(channel);
    
    /* Configure source address */
    DMA->TCD[channel].SADDR = config->sourceAddr;
    
    /* Configure destination address */
    DMA->TCD[channel].DADDR = config->destAddr;
    
    /* Configure transfer attributes */
    tcdControl = 0U;
    if (config->enableInterrupt) {
        tcdControl |= DMA_TCD_CSR_INTMAJOR_MASK;
    }
    DMA->TCD[channel].CSR = tcdControl;
    
    return STATUS_SUCCESS;
}
```

### 4.5 Spacing and Alignment
- One space after keywords: `if (`, `while (`, `for (`
- No space between function name and parenthesis: `GPIO_Init(`
- Space around binary operators: `a + b`, `x == y`
- No space around unary operators: `!flag`, `~mask`, `*ptr`
- Space after comma: `func(a, b, c)`
- Examples:
  ```c
  /* Correct spacing */
  if (value > MAX_VALUE) {
      result = calculate(a, b, c);
  }
  
  for (uint8_t i = 0; i < count; i++) {
      sum += array[i];
  }
  
  bool isValid = !error && (status == STATUS_SUCCESS);
  ```

## 5. Type Definitions and Constants

### 5.1 Fixed-Width Integer Types
Always use fixed-width types from `<stdint.h>`:
```c
/* Correct - Fixed-width types */
uint8_t counter;
uint16_t portValue;
uint32_t registerData;
int32_t signedValue;

/* Incorrect - Implementation-defined types */
unsigned char counter;      /* Don't use */
unsigned short portValue;   /* Don't use */
unsigned int registerData;  /* Don't use */
```

### 5.2 Boolean Type
Use `<stdbool.h>` for boolean values:
```c
#include <stdbool.h>

bool isEnabled = true;
bool hasError = false;

/* In function parameters */
status_t LPIT_ConfigChannel(uint8_t channel, bool enableInterrupt);
```

### 5.3 Enumeration Definitions
- Use `typedef enum` with `_t` suffix
- Explicit value assignment for hardware-related enums
- Use `U` suffix for unsigned values
- Examples:
  ```c
  /**
   * @brief DMA transfer size options
   */
  typedef enum {
      DMA_TRANSFER_SIZE_1B  = 0U,
      DMA_TRANSFER_SIZE_2B  = 1U,
      DMA_TRANSFER_SIZE_4B  = 2U,
      DMA_TRANSFER_SIZE_16B = 4U,
      DMA_TRANSFER_SIZE_32B = 5U
  } dma_transfer_size_t;
  
  /**
   * @brief GPIO pin direction
   */
  typedef enum {
      GPIO_DIR_INPUT  = 0U,
      GPIO_DIR_OUTPUT = 1U
  } gpio_direction_t;
  ```

### 5.4 Structure Definitions
- Use `typedef struct` with `_t` suffix
- Document each member
- Initialize with designated initializers
- Examples:
  ```c
  /**
   * @brief LPIT channel configuration structure
   */
  typedef struct {
      uint8_t channel;            /**< Channel number (0-3) */
      lpit_mode_t mode;           /**< Timer mode */
      uint32_t period;            /**< Period value (TVAL) */
      bool enableInterrupt;       /**< Enable interrupt on expire */
      bool chainChannel;          /**< Chain with previous channel */
      bool startOnTrigger;        /**< Start on external trigger */
      bool stopOnInterrupt;       /**< Stop when interrupt occurs */
      bool reloadOnTrigger;       /**< Reload on external trigger */
  } lpit_channel_config_t;
  
  /* Usage with designated initializers */
  lpit_channel_config_t config = {
      .channel = 0,
      .mode = LPIT_MODE_32BIT_PERIODIC,
      .period = 7999,
      .enableInterrupt = true,
      .chainChannel = false,
      .startOnTrigger = false,
      .stopOnInterrupt = false,
      .reloadOnTrigger = false
  };
  ```

### 5.5 Function Pointer Types
Use `typedef` for callback function pointers:
```c
/**
 * @brief DMA callback function type
 * @param channel DMA channel number
 * @param userData Pointer to user data
 */
typedef void (*dma_callback_t)(uint8_t channel, void *userData);

/**
 * @brief LPIT callback function type
 * @param channel LPIT channel number
 * @param userData Pointer to user data
 */
typedef void (*lpit_callback_t)(uint8_t channel, void *userData);

/* Usage */
static dma_callback_t s_dmaCallbacks[DMA_MAX_CHANNELS];

status_t DMA_InstallCallback(uint8_t channel, dma_callback_t callback, void *userData);
```

### 5.6 Constant Definitions
- Use `#define` for compile-time constants
- Use `const` for runtime constants
- Always use type suffix (`U`, `UL`, etc.)
- Examples:
  ```c
  /* Compile-time constants */
  #define GPIO_MAX_PINS       (32U)
  #define DMA_MAX_CHANNELS    (16U)
  #define LPIT_MAX_CHANNELS   (4U)
  #define UART_TIMEOUT_MS     (1000U)
  
  /* Runtime constants */
  static const uint32_t CLOCK_FREQUENCIES[] = {
      8000000U,   /* SIRC: 8 MHz */
      48000000U,  /* FIRC: 48 MHz */
      80000000U   /* SPLL: 80 MHz */
  };
  
  /* Peripheral base addresses */
  #define GPIOA_BASE          (0x400FF000UL)
  #define LPIT0_BASE          (0x40037000UL)
  #define DMA_BASE            (0x40008000UL)
  ```

### 5.7 Bit Mask and Shift Definitions
Standard pattern for bit field definitions:
```c
/* Pattern: MODULE_REG_FIELD_MASK and MODULE_REG_FIELD_SHIFT */

/* Single bit flags */
#define LPIT_MCR_M_CEN_MASK         (0x00000001UL)
#define LPIT_MCR_M_CEN_SHIFT        (0U)
#define LPIT_MCR_SW_RST_MASK        (0x00000002UL)
#define LPIT_MCR_SW_RST_SHIFT       (1U)

/* Multi-bit fields */
#define DMA_TCD_ATTR_SSIZE_MASK     (0x0700U)
#define DMA_TCD_ATTR_SSIZE_SHIFT    (8U)
#define DMA_TCD_ATTR_DSIZE_MASK     (0x0007U)
#define DMA_TCD_ATTR_DSIZE_SHIFT    (0U)

/* Usage */
LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;  /* Set bit */
LPIT0->MCR &= ~LPIT_MCR_SW_RST_MASK;  /* Clear bit */

/* Multi-bit field access */
uint32_t value = (reg & DMA_TCD_ATTR_SSIZE_MASK) >> DMA_TCD_ATTR_SSIZE_SHIFT;
reg = (reg & ~DMA_TCD_ATTR_SSIZE_MASK) | ((newVal << DMA_TCD_ATTR_SSIZE_SHIFT) & DMA_TCD_ATTR_SSIZE_MASK);
```


## 6. Error Handling and Status Codes

### 6.1 Standard Status Type
All driver functions should return `status_t`:
```c
/**
 * @brief Common status type for all drivers
 */
typedef enum {
    STATUS_SUCCESS          = 0x00U,    /**< Operation successful */
    STATUS_ERROR            = 0x01U,    /**< Generic error */
    STATUS_BUSY             = 0x02U,    /**< Resource is busy */
    STATUS_TIMEOUT          = 0x03U,    /**< Operation timed out */
    STATUS_INVALID_PARAM    = 0x04U,    /**< Invalid parameter */
    STATUS_UNSUPPORTED      = 0x05U,    /**< Operation not supported */
    STATUS_NOT_INITIALIZED  = 0x06U     /**< Module not initialized */
} status_t;
```

### 6.2 Parameter Validation
Always validate input parameters:
```c
status_t DMA_ConfigChannel(uint8_t channel, const dma_channel_config_t *config)
{
    /* Validate channel number */
    if (channel >= DMA_MAX_CHANNELS) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Validate configuration pointer */
    if (config == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Validate transfer size */
    if (config->sourceTransferSize > DMA_TRANSFER_SIZE_32B) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Configuration logic here */
    return STATUS_SUCCESS;
}
```

### 6.3 Status Checking
Caller should always check return status:
```c
/* Good practice */
status_t status;

status = LPIT_Init(LPIT_CLK_SRC_SIRC_8MHZ);
if (status != STATUS_SUCCESS) {
    /* Handle error */
    return status;
}

status = LPIT_ConfigChannel(0, &config);
if (status != STATUS_SUCCESS) {
    /* Handle error */
    return status;
}

/* Alternative compact form */
if (GPIO_Init(GPIOD, 15, GPIO_DIR_OUTPUT) != STATUS_SUCCESS) {
    /* Handle error */
}
```

### 6.4 NULL Pointer Checking
Always check pointer validity before dereferencing:
```c
status_t DMA_InstallCallback(uint8_t channel, dma_callback_t callback, void *userData)
{
    if (channel >= DMA_MAX_CHANNELS) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Allow NULL callback for uninstalling */
    s_dmaCallbacks[channel] = callback;
    s_dmaUserData[channel] = userData;
    
    return STATUS_SUCCESS;
}
```

### 6.5 Range Checking
Validate numerical ranges:
```c
status_t LPIT_ConfigChannel(uint8_t channel, const lpit_channel_config_t *config)
{
    /* Channel range check */
    if (channel >= LPIT_MAX_CHANNELS) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Mode validation */
    if (config->mode > LPIT_MODE_32BIT_ONESHOT) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Period validation (must be > 0) */
    if (config->period == 0U) {
        return STATUS_INVALID_PARAM;
    }
    
    return STATUS_SUCCESS;
}
```

### 6.6 Assert for Debug Builds
Use assertions for conditions that should never occur:
```c
#include "devassert.h"

void DMA_IRQHandler(void)
{
    uint32_t intStatus = DMA->INT;
    
    for (uint8_t channel = 0; channel < DMA_MAX_CHANNELS; channel++) {
        if (intStatus & (1UL << channel)) {
            /* This should always be true if hardware is working correctly */
            DEV_ASSERT(DMA->TCD[channel].CSR & DMA_TCD_CSR_DONE_MASK);
            
            /* Clear interrupt flag */
            DMA->INT = (1UL << channel);
            
            /* Call user callback */
            if (s_dmaCallbacks[channel] != NULL) {
                s_dmaCallbacks[channel](channel, s_dmaUserData[channel]);
            }
        }
    }
}
```

### 6.7 Error Recovery Patterns
Implement cleanup on error:
```c
status_t DMA_MemCopy(const void *src, void *dest, uint32_t size, dma_transfer_size_t transferSize)
{
    status_t status;
    uint8_t channel;
    
    /* Find free channel */
    status = DMA_FindFreeChannel(&channel);
    if (status != STATUS_SUCCESS) {
        return STATUS_BUSY;
    }
    
    /* Configure channel */
    dma_channel_config_t config = {
        /* ... configuration ... */
    };
    
    status = DMA_ConfigChannel(channel, &config);
    if (status != STATUS_SUCCESS) {
        /* Release channel on error */
        DMA_ReleaseChannel(channel);
        return status;
    }
    
    /* Start transfer */
    DMA_StartChannel(channel);
    return STATUS_SUCCESS;
}
```


## 7. Documentation Requirements

### 7.1 File Header Documentation
Every source file must have a comprehensive header:
```c
/**
 * @file    dma.c
 * @brief   DMA Driver Implementation for S32K144
 * @details Implementation of DMA controller driver with eDMA support
 *          - 16 independent DMA channels
 *          - Transfer Control Descriptor (TCD) configuration
 *          - Interrupt and callback support
 *          - Memory-to-memory and peripheral transfers
 * 
 * @author  PhucPH32
 * @date    27/11/2024
 * @version 1.0
 */
```

### 7.2 Module Documentation
Use `@defgroup` and `@addtogroup` for grouping:
```c
/**
 * @defgroup LPIT_Driver LPIT Timer Driver
 * @brief Low Power Interrupt Timer driver for S32K144
 * @{
 */

/* Function declarations */

/** @} */ /* End of LPIT_Driver */
```

### 7.3 Function Documentation
Complete Doxygen documentation for all public functions:
```c
/**
 * @brief Configure and initialize a LPIT channel
 * @details Sets up the timer channel with specified configuration including:
 *          - Timer mode (periodic/one-shot)
 *          - Period value
 *          - Interrupt enable
 *          - Chain mode
 * 
 * @param[in] channel Channel number (0-3)
 * @param[in] config Pointer to channel configuration structure
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Channel configured successfully
 *         - STATUS_INVALID_PARAM: Invalid channel number or NULL config
 *         - STATUS_ERROR: Configuration failed
 * 
 * @pre LPIT module must be initialized with LPIT_Init()
 * @post Timer is configured but not started
 * 
 * @note Chain mode requires previous channel to be configured
 * @warning Do not modify configuration while timer is running
 * 
 * @code
 * // Example: Configure channel 0 for 1ms periodic timer
 * lpit_channel_config_t config = {
 *     .channel = 0,
 *     .mode = LPIT_MODE_32BIT_PERIODIC,
 *     .period = 7999,  // 1ms at 8MHz
 *     .enableInterrupt = true,
 *     .chainChannel = false
 * };
 * 
 * status_t status = LPIT_ConfigChannel(0, &config);
 * if (status == STATUS_SUCCESS) {
 *     LPIT_StartChannel(0);
 * }
 * @endcode
 * 
 * @see LPIT_Init, LPIT_StartChannel, LPIT_StopChannel
 */
status_t LPIT_ConfigChannel(uint8_t channel, const lpit_channel_config_t *config);
```

### 7.4 Structure Documentation
Document each structure and its members:
```c
/**
 * @brief DMA channel configuration structure
 * @details Complete configuration for a single DMA channel including
 *          source/destination addresses, transfer sizes, and control options
 */
typedef struct {
    uint8_t channel;                        /**< DMA channel number (0-15) */
    uint32_t sourceAddr;                    /**< Source address */
    uint32_t destAddr;                      /**< Destination address */
    dma_transfer_size_t sourceTransferSize; /**< Source transfer size */
    dma_transfer_size_t destTransferSize;   /**< Destination transfer size */
    int16_t sourceOffset;                   /**< Source address offset after each transfer */
    int16_t destOffset;                     /**< Destination address offset after each transfer */
    uint32_t minorByteCount;                /**< Bytes to transfer per minor loop */
    uint32_t majorLoopCount;                /**< Number of minor loops */
    bool enableInterrupt;                   /**< Enable interrupt on major loop completion */
} dma_channel_config_t;
```

### 7.5 Enumeration Documentation
```c
/**
 * @brief LPIT timer operating modes
 */
typedef enum {
    LPIT_MODE_32BIT_PERIODIC = 0U,  /**< 32-bit periodic timer mode */
    LPIT_MODE_DUAL_16BIT = 1U,      /**< Dual 16-bit periodic timer mode */
    LPIT_MODE_32BIT_TRIGGER = 2U,   /**< 32-bit trigger accumulator mode */
    LPIT_MODE_32BIT_ONESHOT = 3U    /**< 32-bit one-shot timer mode */
} lpit_mode_t;
```

### 7.6 Register Documentation
Document register structures and bit fields:
```c
/**
 * @brief LPIT Channel Register Structure
 * @details Control and status registers for a single LPIT channel
 */
typedef struct {
    __IO uint32_t TVAL;     /**< Timer Value Register - Initial countdown value */
    __I  uint32_t CVAL;     /**< Current Timer Value - Current countdown value (read-only) */
    __IO uint32_t TCTRL;    /**< Timer Control Register - Channel control and configuration */
    uint32_t RESERVED;      /**< Reserved space */
} LPIT_CHANNEL_Type;

/* Bit field definitions */
#define LPIT_TCTRL_T_EN_MASK        (0x00000001UL)  /**< Timer Enable */
#define LPIT_TCTRL_CHAIN_MASK       (0x00000002UL)  /**< Chain Channel */
#define LPIT_TCTRL_MODE_MASK        (0x0000000CUL)  /**< Timer Mode */
#define LPIT_TCTRL_TIE_MASK         (0x00000040UL)  /**< Timer Interrupt Enable */
```

### 7.7 Example Code Documentation
Provide working examples in documentation:
```c
/**
 * @example lpit_example.c
 * @brief LPIT timer usage examples
 * 
 * This example demonstrates:
 * - Basic periodic timer setup
 * - Multiple timers with different periods
 * - Chain mode for 64-bit timing
 * - Delay function implementation
 * - Task scheduling with timers
 */
```

### 7.8 TODO and FIXME Comments
Document incomplete or problematic code:
```c
/* TODO: Implement DMA error handling for bus errors */
/* FIXME: Chain mode timing calculation incorrect for SPLL clock */
/* NOTE: This function assumes peripheral clock is already enabled */
/* WARNING: Function is not thread-safe */
```


## 8. Safety and Best Practices

### 8.1 Volatile Qualifier
Always use `volatile` for hardware register access:
```c
/* Correct - volatile ensures actual memory access */
typedef struct {
    __IO uint32_t CTRL;    /* __IO expands to volatile */
    __I  uint32_t STATUS;  /* __I expands to volatile const */
    __O  uint32_t DATA;    /* __O expands to volatile */
} PERIPHERAL_Type;

/* Direct register access */
volatile uint32_t *reg = (volatile uint32_t *)0x40000000UL;
*reg = 0x1234U;  /* Compiler won't optimize this away */

/* Incorrect - compiler may optimize away */
uint32_t *reg = (uint32_t *)0x40000000UL;
*reg = 0x1234U;  /* May be optimized out */
```

### 8.2 Type Safety and Fixed-Width Types
Use standard fixed-width types to ensure portability:
```c
/* Correct - explicit size */
uint8_t  byte_value;      /* Always 8 bits */
uint16_t word_value;      /* Always 16 bits */
uint32_t dword_value;     /* Always 32 bits */
int32_t  signed_value;    /* Always 32 bits signed */

/* Incorrect - platform dependent */
unsigned char byte;       /* May be > 8 bits on some platforms */
unsigned int word;        /* Size varies (16/32/64 bits) */
long large_value;         /* Size varies (32/64 bits) */
```

### 8.3 Magic Numbers
Define all magic numbers as named constants:
```c
/* Bad - unclear magic numbers */
REG->CTRL = 0x12345678U;
delay = counter * 125;

/* Good - descriptive constants */
#define CTRL_ENABLE_ALL_FEATURES   (0x12345678UL)
#define US_TO_TICKS_MULTIPLIER     (125U)

REG->CTRL = CTRL_ENABLE_ALL_FEATURES;
delay = counter * US_TO_TICKS_MULTIPLIER;

/* Real examples from drivers */
#define LPIT_MAX_CHANNELS          (4U)
#define DMA_MAX_CHANNELS           (16U)
#define SIRC_FREQ_HZ               (8000000U)
#define FIRC_FREQ_HZ               (48000000U)
```

### 8.4 Critical Sections and Atomicity
Protect read-modify-write operations on shared resources:
```c
/* Hardware register access - usually atomic, but disable interrupts for safety */
void GPIO_TogglePin(gpio_port_t port, uint8_t pin)
{
    GPIO_Type *base = GPIO_GetBase(port);
    
    /* Disable interrupts for atomic operation */
    __disable_irq();
    base->PDOR ^= (1UL << pin);
    __enable_irq();
}

/* Multi-step hardware configuration */
void LPIT_ConfigAndStart(uint8_t channel, uint32_t period)
{
    __disable_irq();
    
    /* Multi-register configuration must be atomic */
    LPIT0->CHANNEL[channel].TVAL = period;
    LPIT0->CHANNEL[channel].TCTRL = LPIT_TCTRL_T_EN_MASK | LPIT_TCTRL_TIE_MASK;
    
    __enable_irq();
}

/* Shared data structures in ISR context */
static volatile uint32_t s_dmaTransferCount[DMA_MAX_CHANNELS];

void DMA_IRQHandler(void)
{
    /* ISR context - increment is atomic on Cortex-M4 for aligned 32-bit */
    s_dmaTransferCount[channel]++;
}

uint32_t DMA_GetTransferCount(uint8_t channel)
{
    uint32_t count;
    
    /* Reading shared data - disable interrupts */
    __disable_irq();
    count = s_dmaTransferCount[channel];
    __enable_irq();
    
    return count;
}
```

### 8.5 Pointer Safety
Validate pointers and avoid dangling references:
```c
/* Always validate before dereferencing */
status_t DMA_ConfigChannel(uint8_t channel, const dma_channel_config_t *config)
{
    if (config == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Safe to use config now */
    DMA->TCD[channel].SADDR = config->sourceAddr;
    return STATUS_SUCCESS;
}

/* Don't return pointers to local variables */
/* Bad */
uint32_t* GetBuffer(void)
{
    uint32_t buffer[10];  /* Stack variable */
    return buffer;        /* Dangling pointer! */
}

/* Good */
static uint32_t s_buffer[10];  /* Static lifetime */
uint32_t* GetBuffer(void)
{
    return s_buffer;  /* Safe */
}
```

### 8.6 Avoid Implicit Type Conversions
Use explicit casts for clarity and safety:
```c
/* Implicit conversion - unclear intent */
uint32_t value = -1;  /* Implicit conversion of signed to unsigned */

/* Explicit cast - clear intent */
uint32_t value = (uint32_t)(-1);  /* Explicitly cast to 0xFFFFFFFF */

/* Register access with proper casting */
uint8_t data = 0x12U;
uint32_t *regPtr = (uint32_t *)0x40000000UL;
*regPtr = (uint32_t)data;  /* Explicit width conversion */

/* Pointer type conversions */
void *genericPtr = &data;
uint8_t *dataPtr = (uint8_t *)genericPtr;  /* Explicit cast */
```

### 8.7 Buffer Overflow Protection
Always validate array indices and buffer sizes:
```c
status_t DMA_InstallCallback(uint8_t channel, dma_callback_t callback)
{
    /* Validate array index before access */
    if (channel >= DMA_MAX_CHANNELS) {
        return STATUS_INVALID_PARAM;
    }
    
    s_dmaCallbacks[channel] = callback;
    return STATUS_SUCCESS;
}

/* Validate buffer operations */
status_t UART_Write(const uint8_t *data, uint32_t size)
{
    if (data == NULL || size == 0U) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Check against maximum buffer size */
    if (size > UART_TX_BUFFER_SIZE) {
        return STATUS_INVALID_PARAM;
    }
    
    for (uint32_t i = 0U; i < size; i++) {
        /* Safe array access within validated bounds */
        UART_TransmitByte(data[i]);
    }
    
    return STATUS_SUCCESS;
}
```

### 8.8 Uninitialized Variables
Always initialize variables before use:
```c
/* Bad - uninitialized */
uint32_t counter;
counter++;  /* Undefined behavior */

/* Good - initialized */
uint32_t counter = 0U;
counter++;  /* Defined behavior */

/* Structure initialization */
lpit_channel_config_t config = {
    .channel = 0,
    .mode = LPIT_MODE_32BIT_PERIODIC,
    .period = 7999,
    .enableInterrupt = true,
    .chainChannel = false,
    .startOnTrigger = false,
    .stopOnInterrupt = false,
    .reloadOnTrigger = false
};  /* All members explicitly initialized */
```

### 8.9 Integer Overflow
Check for potential overflows in calculations:
```c
/* Period calculation with overflow protection */
uint32_t LPIT_CalculatePeriod(uint32_t timeUs, uint32_t clockFreqHz)
{
    uint64_t temp;
    
    /* Use 64-bit to prevent overflow */
    temp = ((uint64_t)clockFreqHz * timeUs) / 1000000ULL;
    
    /* Check if result fits in 32-bit */
    if (temp > 0xFFFFFFFFULL) {
        return 0xFFFFFFFFUL;  /* Saturate to max */
    }
    
    return (uint32_t)temp;
}

/* Addition with overflow check */
if (value > (UINT32_MAX - increment)) {
    /* Would overflow */
    return STATUS_ERROR;
}
value += increment;  /* Safe */
```


## 9. Complete Driver Example

This section demonstrates a simplified but complete GPIO driver following all conventions.

### 9.1 Register Definition File (gpio_reg.h)
```c
/**
 * @file    gpio_reg.h
 * @brief   GPIO Register Definitions for S32K144
 * @author  PhucPH32
 * @date    29/11/2024
 * @version 1.0
 */

#ifndef GPIO_REG_H
#define GPIO_REG_H

#include "def_reg.h"

/*******************************************************************************
 * Register Base Addresses
 ******************************************************************************/

#define GPIOA_BASE          (0x400FF000UL)
#define GPIOB_BASE          (0x400FF040UL)
#define GPIOC_BASE          (0x400FF080UL)
#define GPIOD_BASE          (0x400FF0C0UL)

/*******************************************************************************
 * Register Structure Definitions
 ******************************************************************************/

/**
 * @brief GPIO Register Structure
 */
typedef struct {
    __IO uint32_t PDOR;    /**< Port Data Output Register */
    __I  uint32_t PDIR;    /**< Port Data Input Register */
    __IO uint32_t PDDR;    /**< Port Data Direction Register */
} GPIO_Type;

/*******************************************************************************
 * Register Access Macros
 ******************************************************************************/

#define PTA                 ((GPIO_Type *)GPIOA_BASE)
#define PTB                 ((GPIO_Type *)GPIOB_BASE)
#define PTC                 ((GPIO_Type *)GPIOC_BASE)
#define PTD                 ((GPIO_Type *)GPIOD_BASE)

#endif /* GPIO_REG_H */
```

### 9.2 API Header File (gpio.h)
```c
/**
 * @file    gpio.h
 * @brief   GPIO Driver API for S32K144
 * @author  PhucPH32
 * @date    29/11/2024
 * @version 1.0
 */

#ifndef GPIO_H
#define GPIO_H

#include "gpio_reg.h"
#include "status.h"
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define GPIO_MAX_PINS       (32U)

/**
 * @brief GPIO port selection
 */
typedef enum {
    GPIO_PORT_A = 0U,
    GPIO_PORT_B = 1U,
    GPIO_PORT_C = 2U,
    GPIO_PORT_D = 3U
} gpio_port_t;

/**
 * @brief GPIO pin direction
 */
typedef enum {
    GPIO_DIR_INPUT  = 0U,
    GPIO_DIR_OUTPUT = 1U
} gpio_direction_t;

/*******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * @brief Initialize a GPIO pin
 * 
 * @param[in] port GPIO port (A, B, C, D)
 * @param[in] pin Pin number (0-31)
 * @param[in] direction Pin direction (input/output)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Pin initialized successfully
 *         - STATUS_INVALID_PARAM: Invalid port or pin number
 */
status_t GPIO_Init(gpio_port_t port, uint8_t pin, gpio_direction_t direction);

/**
 * @brief Set GPIO pin to high level
 * 
 * @param[in] port GPIO port
 * @param[in] pin Pin number (0-31)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Pin set successfully
 *         - STATUS_INVALID_PARAM: Invalid parameters
 */
status_t GPIO_SetPin(gpio_port_t port, uint8_t pin);

/**
 * @brief Read GPIO pin state
 * 
 * @param[in] port GPIO port
 * @param[in] pin Pin number (0-31)
 * @param[out] value Pointer to store pin value (0 or 1)
 * 
 * @return status_t
 *         - STATUS_SUCCESS: Pin read successfully
 *         - STATUS_INVALID_PARAM: Invalid parameters or NULL pointer
 */
status_t GPIO_ReadPin(gpio_port_t port, uint8_t pin, bool *value);

#endif /* GPIO_H */
```

### 9.3 Implementation File (gpio.c)
```c
/**
 * @file    gpio.c
 * @brief   GPIO Driver Implementation for S32K144
 * @author  PhucPH32
 * @date    29/11/2024
 * @version 1.0
 */

#include "gpio.h"
#include <stddef.h>

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Get GPIO base pointer from port number
 * 
 * @param[in] port GPIO port
 * @return GPIO_Type* Pointer to GPIO base or NULL if invalid
 */
static GPIO_Type* GPIO_GetBase(gpio_port_t port)
{
    GPIO_Type *base;
    
    switch (port) {
        case GPIO_PORT_A:
            base = PTA;
            break;
        case GPIO_PORT_B:
            base = PTB;
            break;
        case GPIO_PORT_C:
            base = PTC;
            break;
        case GPIO_PORT_D:
            base = PTD;
            break;
        default:
            base = NULL;
            break;
    }
    
    return base;
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

status_t GPIO_Init(gpio_port_t port, uint8_t pin, gpio_direction_t direction)
{
    GPIO_Type *base;
    
    /* Validate parameters */
    if (pin >= GPIO_MAX_PINS) {
        return STATUS_INVALID_PARAM;
    }
    
    base = GPIO_GetBase(port);
    if (base == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Configure pin direction */
    if (direction == GPIO_DIR_OUTPUT) {
        base->PDDR |= (1UL << pin);
    } else {
        base->PDDR &= ~(1UL << pin);
    }
    
    return STATUS_SUCCESS;
}

status_t GPIO_SetPin(gpio_port_t port, uint8_t pin)
{
    GPIO_Type *base;
    
    /* Validate parameters */
    if (pin >= GPIO_MAX_PINS) {
        return STATUS_INVALID_PARAM;
    }
    
    base = GPIO_GetBase(port);
    if (base == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Set pin to high */
    base->PDOR |= (1UL << pin);
    
    return STATUS_SUCCESS;
}

status_t GPIO_ReadPin(gpio_port_t port, uint8_t pin, bool *value)
{
    GPIO_Type *base;
    
    /* Validate parameters */
    if (pin >= GPIO_MAX_PINS || value == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    base = GPIO_GetBase(port);
    if (base == NULL) {
        return STATUS_INVALID_PARAM;
    }
    
    /* Read pin state */
    *value = ((base->PDIR & (1UL << pin)) != 0U);
    
    return STATUS_SUCCESS;
}
```

### 9.4 Usage Example (main.c)
```c
/**
 * @file    main.c
 * @brief   GPIO driver usage example
 */

#include "gpio.h"

int main(void)
{
    status_t status;
    bool pinState;
    
    /* Initialize pin D15 as output (LED) */
    status = GPIO_Init(GPIO_PORT_D, 15, GPIO_DIR_OUTPUT);
    if (status != STATUS_SUCCESS) {
        /* Handle error */
        return -1;
    }
    
    /* Initialize pin C12 as input (button) */
    status = GPIO_Init(GPIO_PORT_C, 12, GPIO_DIR_INPUT);
    if (status != STATUS_SUCCESS) {
        /* Handle error */
        return -1;
    }
    
    while (1) {
        /* Read button state */
        status = GPIO_ReadPin(GPIO_PORT_C, 12, &pinState);
        if (status == STATUS_SUCCESS) {
            if (pinState) {
                /* Button pressed - turn on LED */
                GPIO_SetPin(GPIO_PORT_D, 15);
            }
        }
    }
    
    return 0;
}
```

---

**Note**: These conventions are based on actual drivers developed for S32K144 bare-metal projects. Adapt them according to your project requirements and team standards.