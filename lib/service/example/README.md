# Service Layer Examples

Thư mục này chứa các example code để demonstrate cách sử dụng các service layer.

## 📋 Danh sách Examples

### 0. GPIO Service (`gpio_srv_example.c`) 🆕
**Chức năng:** Digital I/O với external interrupt support
- ✅ Basic LED blinking
- ✅ Button interrupt handling
- ✅ Callback function registration
- ✅ Rising/Falling/Both edges detection
- ✅ Polled vs Interrupt comparison
- ✅ Debouncing techniques

**Hardware Requirements:**
- Button SW2 trên PTC12 (with pull-up)
- Button SW3 trên PTC13 (with pull-up)
- LED Blue trên PTD0
- LED Red trên PTD15
- LED Green trên PTD16

**Example Functions:**
- `GPIO_BasicExample()` - Simple LED blink
- `GPIO_ButtonInterruptExample()` - Button interrupt với callbacks (RECOMMENDED)
- `GPIO_ButtonPolledExample()` - Button polling (no interrupt)
- `GPIO_BothEdgesExample()` - Both edges interrupt detection

**Important Notes:**
- ⚠️ **Phải enable NVIC interrupt** trong startup.c hoặc main.c
- ⚠️ **Phải implement IRQ handlers:** `PORTC_IRQHandler()`, `PORTD_IRQHandler()`
- ⚠️ Buttons on EVB thường active-low (pressed = 0)
- ⚠️ Cần pull-up resistor cho buttons

---

### 1. LPIT Service (`lpit_srv_example.c`)
**Chức năng:** Timer với callback functions
- ✅ LED blinking với multiple timers
- ✅ Callback function registration
- ✅ Start/Stop timer dynamically
- ✅ Period configuration in microseconds

**Hardware Requirements:**
- LED trên PTD0 (Blue LED)
- LED trên PTD15 (Red LED)

**Example Functions:**
- `LPIT_Example()` - Basic timer with LED blinking
- `LPIT_AdvancedExample()` - Dynamic start/stop

---

### 2. ADC Service (`adc_srv_example.c`)
**Chức năng:** Analog-to-Digital conversion
- ✅ Single channel reading
- ✅ Multi-channel reading
- ✅ Voltage calculation (mV)
- ✅ Threshold monitoring
- ✅ ADC calibration

**Hardware Requirements:**
- Potentiometer/Analog sensor nối vào PTB3 (ADC0_SE12)
- UART cho debug output

**Example Functions:**
- `ADC_BasicExample()` - Single channel với UART output
- `ADC_MultiChannelExample()` - Read multiple channels
- `ADC_ThresholdExample()` - Voltage monitoring với threshold

---

### 3. UART Service (`uart_srv_example.c`)
**Chức năng:** Serial communication
- ✅ Send strings
- ✅ Send individual bytes
- ✅ Different baudrates
- ✅ Formatted output (printf style)
- ✅ Debug logging

**Hardware Requirements:**
- USB-UART converter nối vào:
  - TX: PTC7
  - RX: PTC6
- Terminal program (PuTTY, Tera Term, etc.)

**Example Functions:**
- `UART_BasicExample()` - Basic send string và counter
- `UART_BaudrateExample()` - Test different baudrates
- `UART_MenuExample()` - Menu system demo
- `UART_DebugExample()` - Logging system

---

### 4. CAN Service (`can_srv_example.c`)
**Chức năng:** CAN bus communication
- ✅ Send/Receive CAN messages
- ✅ Standard ID (11-bit)
- ✅ Extended ID (29-bit)
- ✅ Message filtering
- ✅ Different baudrates

**Hardware Requirements:**
- CAN transceiver (MCP2551, TJA1050)
- CAN bus connection
- Pins:
  - CAN0_TX: PTE5
  - CAN0_RX: PTE4

**Example Functions:**
- `CAN_BasicExample()` - Send/receive với UART debug
- `CAN_SensorDataExample()` - Periodic sensor data transmission
- `CAN_ExtendedIDExample()` - 29-bit ID demo

---

### 5. Clock Service (`clock_srv_example.c`)
**Chức năng:** System clock configuration
- ✅ Multiple clock modes (80MHz, 48MHz, 112MHz, 4MHz)
- ✅ V1: Pre-defined presets
- ✅ V2: Custom configuration
- ✅ Dynamic clock switching
- ✅ Frequency calculation
- ✅ SPLL calculator

**Hardware Requirements:**
- External 8MHz crystal (optional, FIRC mode không cần)
- LED on PTD15 (for visual demo)

**Example Functions:**

**V1 (Simple):**
- `CLOCK_V1_PresetExample()` - Test tất cả preset modes

**V2 (Advanced):**
- `CLOCK_V2_CustomExample()` - Custom 100MHz configuration
- `CLOCK_V2_DynamicSwitchExample()` - Runtime clock switching
- `CLOCK_V2_CalculatorExample()` - SPLL frequency calculator

---

## 🚀 Cách sử dụng

### 1. Include example file vào project
```c
#include "service/example/gpio_srv_example.c"
#include "service/example/lpit_srv_example.c"
```

### 2. Enable interrupts trong startup.c
```c
/* For GPIO button interrupts */
void PORTC_IRQHandler(void) {
    GPIO_SRV_PORTC_IRQHandler();
}

void PORTD_IRQHandler(void) {
    GPIO_SRV_PORTD_IRQHandler();
}

/* Enable in NVIC */
NVIC_EnableIRQ(PORTC_IRQn);
NVIC_EnableIRQ(PORTD_IRQn);
```

### 3. Call example function trong main()
```c
int main(void)
{
    /* Example: GPIO Button Interrupt */
    GPIO_ButtonInterruptExample();
    
    /* or */
    
    /* Example: LPIT Timer */
    LPIT_Example();
    
    while(1);
}
```

### 3. Common initialization pattern
Tất cả examples đều follow pattern này:
```c
void Service_Example(void)
{
    /* 1. Initialize clock system */
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    
    /* 2. Enable peripheral clocks */
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTX, ...);
    
    /* 3. Configure PORT/GPIO if needed */
    PORT_SRV_ConfigPin(...);
    
    /* 4. Initialize service */
    SERVICE_Init(...);
    
    /* 5. Main loop */
    while(1) {
        /* Use service APIs */
    }
}
```

---

## 📊 Clock Configuration Guide

### Recommended clock modes:

| Mode | Core Clock | Use Case |
|------|------------|----------|
| **RUN_48MHz** | 48 MHz | No external crystal needed, moderate performance |
| **RUN_80MHz** | 80 MHz | Standard high performance (recommended) |
| **HSRUN_112MHz** | 112 MHz | Maximum performance |
| **VLPR_4MHz** | 4 MHz | Low power, battery applications |

### Peripheral clock sources:

```c
/* Fast peripherals (UART, CAN, ADC) */
CLOCK_SRV_V2_EnablePeripheral(peripheral, CLOCK_SRV_V2_PCS_SOSCDIV2);

/* Medium speed (LPIT, SPI) */
CLOCK_SRV_V2_EnablePeripheral(peripheral, CLOCK_SRV_V2_PCS_FIRCDIV2);

/* GPIO/PORT (no clock source needed) */
CLOCK_SRV_V2_EnablePeripheral(peripheral, CLOCK_SRV_V2_PCS_NONE);
```

---

## 🔧 Troubleshooting

### Common Issues:

1. **Button interrupt không hoạt động:**
   - ❌ Check NVIC interrupt enabled: `NVIC_EnableIRQ(PORTC_IRQn)`
   - ❌ Check IRQ handler implemented: `PORTC_IRQHandler()`
   - ❌ Verify pull-up enabled: `PORT_PULL_UP`
   - ❌ Check PORT clock enabled
   - ❌ Ensure interrupt type correct: `GPIO_SRV_INT_FALLING_EDGE` for active-low buttons

2. **Button bouncing (multiple triggers):**
   - 💡 Add software debouncing in callback
   - 💡 Add delay after detecting edge
   - 💡 Use hardware debounce circuit (RC filter)
   - 💡 Check callback execution time (keep it short!)

3. **UART không output:**
   - Check baudrate settings
   - Verify PORT configuration (PTC6/PTC7)
   - Ensure clock source enabled (SOSCDIV2 hoặc FIRCDIV2)

3. **ADC read fails:**
   - Run calibration first: `ADC_SRV_Calibrate()`
   - Check pin configured as analog: `PORT_MUX_ANALOG`
   - Ensure ADC clock enabled

4. **CAN không hoạt động:**
   - Verify CAN transceiver connected
   - Check baudrate matches CAN bus
   - Ensure termination resistors (120Ω)

5. **Timer không chạy:**
   - Check LPIT clock source: `CLOCK_SRV_V2_PCS_FIRCDIV2`
   - Verify callback registered
   - Enable global interrupts if using callbacks

6. **Clock switching fails:**
   - Validate configuration first: `CLOCK_SRV_V2_ValidateConfig()`
   - Ensure SOSC enabled before SPLL
   - Check SMC mode restrictions (HSRUN, VLPR)

---

## 📝 Notes

- **All examples include UART debug output** (except UART example itself)
- **Hardware setup required** - Check pin connections
- **Modify pin definitions** if using different hardware
- **Examples are standalone** - Can copy directly to main.c

---

## 🎯 Quick Start

**Simplest example để test system (no external hardware needed):**

```c
#include "service/example/gpio_srv_example.c"

int main(void)
{
    GPIO_BasicExample();  // Just blinks LED!
    while(1);
}
```

**Button interrupt example (recommended for learning):**

```c
#include "service/example/gpio_srv_example.c"

/* Add to startup.c or before main() */
void PORTC_IRQHandler(void) {
    GPIO_SRV_PORTC_IRQHandler();
}

int main(void)
{
    /* Enable interrupt in NVIC */
    NVIC_EnableIRQ(PORTC_IRQn);
    
    /* Run example */
    GPIO_ButtonInterruptExample();
    while(1);
}
```

**UART debug example (needs USB-UART):**

```c
#include "service/example/uart_srv_example.c"

int main(void)
{
    UART_BasicExample();  // Sends "Hello World" + counter
    while(1);
}
```

---

## 📚 Related Documentation

- Service headers: `App/App2/service/inc/`
- Service implementation: `App/App2/service/src/`
- Driver documentation: `Core/Drivers/`

---

**Author:** PhucPH32  
**Date:** 05/12/2025  
**Version:** 1.0
