# LPIT (Low Power Interrupt Timer) Driver

## 📋 Mục lục
- [Giới thiệu](#giới-thiệu)
- [Tính năng](#tính-năng)
- [Kiến trúc LPIT](#kiến-trúc-lpit)
- [Cấu trúc Driver](#cấu-trúc-driver)
- [Hướng dẫn sử dụng](#hướng-dẫn-sử-dụng)
- [Ví dụ](#ví-dụ)
- [API Reference](#api-reference)

## 🎯 Giới thiệu

**LPIT (Low Power Interrupt Timer)** là timer module 32-bit được thiết kế cho các ứng dụng low-power trên S32K144. LPIT cung cấp:
- ⏱️ **4 kênh timer độc lập** với độ chính xác cao
- 🔋 **Hoạt động trong low-power modes** (VLPR, STOP)
- ⛓️ **Chain mode** để tạo timer 64-bit
- 🎯 **Periodic interrupts** cho task scheduling
- 🔄 **Flexible triggering** options

## ✨ Tính năng

### Hardware Features
- **4 kênh timer 32-bit** độc lập (Channel 0-3)
- **Counter đếm xuống** từ TVAL về 0
- **Auto-reload** sau khi expire
- **Multiple timer modes**:
  - 32-bit Periodic Counter
  - Dual 16-bit Periodic Counter
  - Trigger Accumulator
  - Input Capture

### Operating Modes
- ⚡ **Normal mode**: Hoạt động bình thường
- 💤 **Doze mode**: Option để dừng trong Doze
- 🔍 **Debug mode**: Hoạt động khi CPU debug
- 🔗 **Chain mode**: Nối 2 kênh thành 64-bit timer

### Clock Sources
LPIT có thể chọn nhiều clock sources:
- **SIRC** (Slow IRC): 8 MHz - Tốt cho low-power
- **FIRC** (Fast IRC): 48 MHz - Độ chính xác cao
- **SOSC** (System Osc): 8 MHz - External crystal
- **SPLL** (System PLL): 80 MHz - Performance cao

## 🏗️ Kiến trúc LPIT

### Block Diagram
```
┌─────────────────────────────────────────────────────┐
│                   LPIT Module                       │
├─────────────────────────────────────────────────────┤
│                                                     │
│  Clock Source (SIRC/FIRC/SOSC/SPLL)               │
│         ↓                                          │
│  ┌──────────────────────────────────────────┐     │
│  │  Module Control (MCR)                    │     │
│  │  - M_CEN: Module Clock Enable            │     │
│  │  - SW_RST: Software Reset                │     │
│  │  - DOZE_EN: Doze Mode Enable             │     │
│  │  - DBG_EN: Debug Mode Enable             │     │
│  └──────────────────────────────────────────┘     │
│         ↓                                          │
│  ┌─────────────────┐  ┌─────────────────┐        │
│  │   Channel 0     │  │   Channel 1     │        │
│  │  ┌───────────┐  │  │  ┌───────────┐  │        │
│  │  │ TVAL (32) │  │  │  │ TVAL (32) │  │        │
│  │  │ CVAL (32) │  │  │  │ CVAL (32) │  │        │
│  │  │   TCTRL   │  │  │  │   TCTRL   │  │        │
│  │  └───────────┘  │  │  └───────────┘  │        │
│  │       ↓         │  │       ↓         │        │
│  │  Interrupt 0    │  │  Interrupt 1    │        │
│  └─────────────────┘  └─────────────────┘        │
│                                                     │
│  ┌─────────────────┐  ┌─────────────────┐        │
│  │   Channel 2     │  │   Channel 3     │        │
│  │  ┌───────────┐  │  │  ┌───────────┐  │        │
│  │  │ TVAL (32) │  │  │  │ TVAL (32) │  │        │
│  │  │ CVAL (32) │  │  │  │ CVAL (32) │  │        │
│  │  │   TCTRL   │  │  │  │   TCTRL   │  │        │
│  │  └───────────┘  │  │  └───────────┘  │        │
│  │       ↓         │  │       ↓         │        │
│  │  Interrupt 2    │  │  Interrupt 3    │        │
│  └─────────────────┘  └─────────────────┘        │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### Timer Operation
```
Counter Operation (Đếm xuống):

TVAL Register = Reload Value (vd: 8000)
     ↓
   Start
     ↓
CVAL = TVAL (8000)
     ↓
CVAL-- (7999, 7998, 7997, ...)
     ↓
CVAL = 0
     ↓
Interrupt Flag Set
     ↓
CVAL = TVAL (Reload)
     ↓
Repeat...
```

### Chain Mode
```
Channel 0 (Lower 32-bit)      Channel 1 (Upper 32-bit)
┌──────────────────┐          ┌──────────────────┐
│  TVAL = 0xFFFFFFFF │    ──>  │  TVAL = 0xFFFFFFFF │
│  Count down...   │          │  Count down...   │
│  When CH0 = 0    │  ───────>│  CH1 counts      │
│  Reload & repeat │          │  Every CH0 wrap  │
└──────────────────┘          └──────────────────┘
         32-bit                      32-bit
         └────────────┬────────────┘
                   64-bit Timer
```

## 📂 Cấu trúc Driver

### Files
```
lpit/
├── lpit_reg.h      # Register definitions, bit masks
├── lpit.h          # API declarations, data structures
└── lpit.c          # Implementation
```

### Key Data Structures

#### `lpit_channel_config_t`
```c
typedef struct {
    uint8_t channel;                // Số kênh (0-3)
    lpit_mode_t mode;               // Timer mode
    uint32_t period;                // Period value (TVAL)
    bool enableInterrupt;           // Enable interrupt
    bool chainChannel;              // Chain với kênh trước
    bool startOnTrigger;            // Start on trigger
    bool stopOnInterrupt;           // Stop on interrupt
    bool reloadOnTrigger;           // Reload on trigger
} lpit_channel_config_t;
```

## 🚀 Hướng dẫn sử dụng

### Bước 1: Khởi tạo LPIT
```c
// Khởi tạo với SIRC clock (8 MHz)
LPIT_Init(LPIT_CLK_SRC_SIRC);
```

### Bước 2: Cấu hình kênh
```c
lpit_channel_config_t config;

config.channel = 0;
config.mode = LPIT_MODE_32BIT_PERIODIC;
config.period = 7999;  // 8MHz / 8000 = 1ms
config.enableInterrupt = true;
config.chainChannel = false;
config.startOnTrigger = false;
config.stopOnInterrupt = false;
config.reloadOnTrigger = false;

LPIT_ConfigChannel(&config);
```

### Bước 3: Đăng ký Callback
```c
void myCallback(uint8_t channel, void *userData) {
    // Xử lý khi timer expire
    GPIO_TogglePin(LED_PORT, LED_PIN);
}

LPIT_InstallCallback(0, myCallback, NULL);
```

### Bước 4: Enable Interrupt & Start
```c
// Enable NVIC interrupt
NVIC_EnableIRQ(LPIT0_Ch0_IRQn);

// Start timer
LPIT_StartChannel(0);
```

### Bước 5: Implement ISR
```c
void LPIT0_Ch0_IRQHandler(void) {
    LPIT_IRQHandler(0);  // Tự động clear flag & gọi callback
}
```

## 📝 Ví dụ

### Ví dụ 1: Basic Timer - 1ms Interrupt
```c
#include "lpit.h"
#include "gpio.h"

void timerCallback(uint8_t channel, void *userData) {
    static uint32_t counter = 0;
    counter++;
    
    // Blink LED mỗi 500ms (500 interrupts)
    if (counter % 500 == 0) {
        GPIO_TogglePin(PORTD, 15);
    }
}

int main(void) {
    lpit_channel_config_t config;
    
    // Khởi tạo GPIO
    GPIO_Init(PORTD, 15, GPIO_OUTPUT);
    
    // Khởi tạo LPIT với SIRC (8 MHz)
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    // Cấu hình 1ms timer
    config.channel = 0;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = LPIT_CalculatePeriod(8000000, 1000);  // 1kHz = 1ms
    config.enableInterrupt = true;
    config.chainChannel = false;
    config.startOnTrigger = false;
    config.stopOnInterrupt = false;
    config.reloadOnTrigger = false;
    
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(0, timerCallback, NULL);
    
    // Enable interrupt và start
    NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
    LPIT_StartChannel(0);
    
    while (1) {
        __asm("WFI");  // Wait for interrupt
    }
}

void LPIT0_Ch0_IRQHandler(void) {
    LPIT_IRQHandler(0);
}
```

### Ví dụ 2: Multiple Timers
```c
void redLedCallback(uint8_t channel, void *userData) {
    GPIO_TogglePin(PORTD, 15);  // Red LED - 1 Hz
}

void greenLedCallback(uint8_t channel, void *userData) {
    GPIO_TogglePin(PORTD, 16);  // Green LED - 2 Hz
}

void blueLedCallback(uint8_t channel, void *userData) {
    GPIO_TogglePin(PORTD, 0);   // Blue LED - 4 Hz
}

int main(void) {
    lpit_channel_config_t config;
    
    // Init GPIO
    GPIO_Init(PORTD, 15, GPIO_OUTPUT);
    GPIO_Init(PORTD, 16, GPIO_OUTPUT);
    GPIO_Init(PORTD, 0, GPIO_OUTPUT);
    
    // Init LPIT
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    // Channel 0: Red LED - Toggle every 500ms (1 Hz blink)
    config.channel = 0;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = LPIT_CalculatePeriod(8000000, 2);
    config.enableInterrupt = true;
    config.chainChannel = false;
    config.startOnTrigger = false;
    config.stopOnInterrupt = false;
    config.reloadOnTrigger = false;
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(0, redLedCallback, NULL);
    
    // Channel 1: Green LED - Toggle every 250ms (2 Hz blink)
    config.channel = 1;
    config.period = LPIT_CalculatePeriod(8000000, 4);
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(1, greenLedCallback, NULL);
    
    // Channel 2: Blue LED - Toggle every 125ms (4 Hz blink)
    config.channel = 2;
    config.period = LPIT_CalculatePeriod(8000000, 8);
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(2, blueLedCallback, NULL);
    
    // Enable interrupts
    NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
    NVIC_EnableIRQ(LPIT0_Ch1_IRQn);
    NVIC_EnableIRQ(LPIT0_Ch2_IRQn);
    
    // Start all timers
    LPIT_StartChannel(0);
    LPIT_StartChannel(1);
    LPIT_StartChannel(2);
    
    while (1) {
        __asm("WFI");
    }
}

void LPIT0_Ch0_IRQHandler(void) { LPIT_IRQHandler(0); }
void LPIT0_Ch1_IRQHandler(void) { LPIT_IRQHandler(1); }
void LPIT0_Ch2_IRQHandler(void) { LPIT_IRQHandler(2); }
```

### Ví dụ 3: Blocking Delay
```c
int main(void) {
    GPIO_Init(PORTD, 15, GPIO_OUTPUT);
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    while (1) {
        GPIO_SetPin(PORTD, 15);
        LPIT_DelayUs(0, 1000000);  // 1 second ON
        
        GPIO_ClearPin(PORTD, 15);
        LPIT_DelayUs(0, 1000000);  // 1 second OFF
    }
}
```

### Ví dụ 4: Chain Mode (64-bit Timer)
```c
int main(void) {
    lpit_channel_config_t config;
    
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    // Channel 0: Lower 32-bit
    config.channel = 0;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = 0xFFFFFFFFU;  // Max value
    config.enableInterrupt = false;
    config.chainChannel = false;  // Base timer
    config.startOnTrigger = false;
    config.stopOnInterrupt = false;
    config.reloadOnTrigger = false;
    LPIT_ConfigChannel(&config);
    
    // Channel 1: Upper 32-bit (Chained)
    config.channel = 1;
    config.chainChannel = true;   // Chain to channel 0
    config.enableInterrupt = true; // Interrupt on 64-bit overflow
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(1, myCallback, NULL);
    
    NVIC_EnableIRQ(LPIT0_Ch1_IRQn);
    
    // Start both channels
    LPIT_StartChannel(0);
    LPIT_StartChannel(1);
    
    while (1) {
        // Very long period timer now running!
    }
}
```

### Ví dụ 5: Task Scheduling
```c
void task1_10ms(uint8_t ch, void *data) {
    // Task chạy mỗi 10ms
    // Đọc sensor, filter signal...
}

void task2_100ms(uint8_t ch, void *data) {
    // Task chạy mỗi 100ms
    // Update display, compute...
}

void task3_1000ms(uint8_t ch, void *data) {
    // Task chạy mỗi 1 second
    // Send data, log, heartbeat...
}

int main(void) {
    lpit_channel_config_t config;
    
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    // Task 1: 10ms (100 Hz)
    config.channel = 0;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = LPIT_CalculatePeriod(8000000, 100);
    config.enableInterrupt = true;
    config.chainChannel = false;
    config.startOnTrigger = false;
    config.stopOnInterrupt = false;
    config.reloadOnTrigger = false;
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(0, task1_10ms, NULL);
    
    // Task 2: 100ms (10 Hz)
    config.channel = 1;
    config.period = LPIT_CalculatePeriod(8000000, 10);
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(1, task2_100ms, NULL);
    
    // Task 3: 1000ms (1 Hz)
    config.channel = 2;
    config.period = LPIT_CalculatePeriod(8000000, 1);
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(2, task3_1000ms, NULL);
    
    NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
    NVIC_EnableIRQ(LPIT0_Ch1_IRQn);
    NVIC_EnableIRQ(LPIT0_Ch2_IRQn);
    
    LPIT_StartChannel(0);
    LPIT_StartChannel(1);
    LPIT_StartChannel(2);
    
    while (1) {
        __asm("WFI");
    }
}
```

### Ví dụ 6: Timing Measurement
```c
int main(void) {
    lpit_channel_config_t config;
    uint32_t startVal, endVal, elapsedTicks;
    float elapsedMs;
    
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    // Cấu hình free-running timer
    config.channel = 0;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = 0xFFFFFFFFU;
    config.enableInterrupt = false;
    config.chainChannel = false;
    config.startOnTrigger = false;
    config.stopOnInterrupt = false;
    config.reloadOnTrigger = false;
    LPIT_ConfigChannel(&config);
    LPIT_StartChannel(0);
    
    // Measure execution time
    LPIT_GetCurrentValue(0, &startVal);
    
    // Code to measure
    for (volatile int i = 0; i < 10000; i++) {
        __asm("NOP");
    }
    
    LPIT_GetCurrentValue(0, &endVal);
    
    // Calculate (timer counts down)
    elapsedTicks = startVal - endVal;
    elapsedMs = (float)elapsedTicks / 8000.0f;  // 8MHz = 8000 ticks/ms
    
    printf("Elapsed: %lu ticks = %.3f ms\n", elapsedTicks, elapsedMs);
}
```

## 📖 API Reference

### Initialization

#### `LPIT_Init(lpit_clock_source_t clockSource)`
Khởi tạo LPIT module với clock source.
- **clockSource**: `LPIT_CLK_SRC_SIRC` (8MHz), `LPIT_CLK_SRC_FIRC` (48MHz), etc.
- **Return**: `STATUS_SUCCESS`

#### `LPIT_Deinit(void)`
Deinitialize LPIT module.

### Configuration

#### `LPIT_ConfigChannel(const lpit_channel_config_t *config)`
Cấu hình một kênh timer.
- **config**: Cấu trúc cấu hình
- **Return**: `STATUS_SUCCESS` hoặc `STATUS_ERROR`

#### `LPIT_SetPeriod(uint8_t channel, uint32_t period)`
Thay đổi period của timer.
- **channel**: 0-3
- **period**: Giá trị TVAL (0 to 0xFFFFFFFF)

### Control

#### `LPIT_StartChannel(uint8_t channel)`
Start timer channel.

#### `LPIT_StopChannel(uint8_t channel)`
Stop timer channel.

### Status

#### `LPIT_GetCurrentValue(uint8_t channel, uint32_t *value)`
Đọc giá trị hiện tại của counter.

#### `LPIT_GetInterruptFlag(uint8_t channel)`
Kiểm tra interrupt flag.

#### `LPIT_ClearInterruptFlag(uint8_t channel)`
Clear interrupt flag.

### Interrupt

#### `LPIT_EnableInterrupt(uint8_t channel)`
Enable interrupt cho kênh.

#### `LPIT_DisableInterrupt(uint8_t channel)`
Disable interrupt cho kênh.

#### `LPIT_InstallCallback(uint8_t channel, lpit_callback_t callback, void *userData)`
Đăng ký callback function.

#### `LPIT_IRQHandler(uint8_t channel)`
Xử lý interrupt (gọi trong ISR).

### Utility

#### `LPIT_DelayUs(uint8_t channel, uint32_t delayUs)`
Tạo delay chính xác (blocking).

#### `LPIT_CalculatePeriod(uint32_t clockFreq, uint32_t desiredFreq)`
Tính period value từ frequencies.

## ⚙️ Cấu hình Clock

### PCC Configuration
```c
// LPIT clock được cấu hình qua PCC (Peripheral Clock Control)
PCC_LPIT0 = PCC_CGC_MASK |           // Enable clock gate
            (PCS_SIRC << PCC_PCS_SHIFT);  // Select SIRC

// Clock sources:
// - PCS_NONE (0): Clock disabled
// - PCS_SOSC (1): System Oscillator (8 MHz)
// - PCS_SIRC (2): Slow IRC (8 MHz) - LOW POWER
// - PCS_FIRC (3): Fast IRC (48 MHz) - HIGH PRECISION
// - PCS_SPLL (6): System PLL (80 MHz) - HIGH PERFORMANCE
```

## ⚠️ Lưu ý quan trọng

### 1. Counter Direction
LPIT đếm **XUỐNG** từ TVAL về 0 (không phải đếm lên như SysTick):
```
TVAL = 8000
CVAL = 8000 → 7999 → 7998 → ... → 1 → 0 → Interrupt → Reload
```

### 2. Period Calculation
```
Period (TVAL) = (Clock Frequency / Desired Frequency) - 1

Ví dụ: Muốn 1ms với SIRC 8MHz:
TVAL = (8,000,000 Hz / 1,000 Hz) - 1 = 7,999
```

### 3. Chain Mode
- Chỉ có thể chain với kênh **trước đó**: CH1→CH0, CH2→CH1, CH3→CH2
- Không thể chain ngược hoặc skip channels

### 4. Interrupt Priority
LPIT interrupts có IRQ numbers riêng:
- `LPIT0_Ch0_IRQn` = IRQ 48
- `LPIT0_Ch1_IRQn` = IRQ 49
- `LPIT0_Ch2_IRQn` = IRQ 50
- `LPIT0_Ch3_IRQn` = IRQ 51

### 5. Low Power Considerations
- **SIRC** (8 MHz) tốt nhất cho low-power apps
- Enable `DOZE_EN` nếu muốn LPIT dừng trong Doze mode
- LPIT có thể wake-up từ STOP mode nếu enabled

### 6. Clock Gating
Phải enable clock trong PCC trước khi dùng:
```c
PCC_LPIT0 |= PCC_CGC_MASK;  // Enable clock gate
```

## 🔧 Troubleshooting

### Timer không chạy?
1. ✅ Kiểm tra clock source đã enable chưa
2. ✅ Kiểm tra M_CEN bit (Module Clock Enable)
3. ✅ Kiểm tra T_EN bit (Timer Enable)
4. ✅ Kiểm tra TVAL có giá trị hợp lệ không

### Interrupt không trigger?
1. ✅ Enable interrupt trong MIER register
2. ✅ Enable trong NVIC
3. ✅ Implement ISR đúng tên
4. ✅ Kiểm tra interrupt priority

### Period không chính xác?
1. ✅ Kiểm tra clock frequency đúng chưa
2. ✅ Tính toán TVAL có đúng không
3. ✅ Clock source có stable không

### Chain mode không hoạt động?
1. ✅ Kênh có được chain đúng thứ tự không
2. ✅ Start kênh base (lower) trước
3. ✅ Kiểm tra CHAIN bit

## 📚 Tài liệu tham khảo

- **S32K1xx Reference Manual** - Chapter 43: LPIT
- **S32K144 Data Sheet** - Section 3.4.3: LPIT
- **PCC (Peripheral Clock Control)** - Chapter 28

---

**Author:** PhucPH32  
**Date:** 28/11/2025  
**Version:** 1.0  
**Status:** ✅ Complete
