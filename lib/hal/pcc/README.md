# PCC (Peripheral Clock Control) Driver

Driver hoàn chỉnh cho PCC peripheral trên S32K144.

## 📋 Tổng Quan

PCC (Peripheral Clock Control) là module điều khiển clock cho các peripheral trên S32K144, cung cấp:
- **Clock gating**: Bật/tắt clock cho từng peripheral
- **Clock source selection**: Chọn nguồn clock cho peripheral
- **Clock divider**: Chia clock cho peripheral
- **Quản lý năng lượng**: Tiết kiệm điện khi không dùng peripheral

## 🔧 Tính Năng

### Nguồn Clock Cho Peripherals

| Clock Source | Tần Số Điển Hình | Mô Tả |
|--------------|-------------------|-------|
| **SOSC_DIV2** | 4 MHz | System Oscillator / 2 |
| **SIRC_DIV2** | 4 MHz | Slow IRC / 2 |
| **FIRC_DIV2** | 24 MHz | Fast IRC / 2 |
| **SPLL_DIV2** | 80 MHz | System PLL / 2 |

### Peripherals Được Hỗ Trợ

- **Communication**: UART, I2C, SPI, CAN, FlexIO
- **Timers**: FTM, LPTMR, PDB, RTC
- **Analog**: ADC, CMP
- **GPIO**: PORT A-E
- **Others**: DMA, DMAMUX, EWM, CRC, QSPI, ENET

## 📁 Cấu Trúc File

```
lib/hal/pcc/
├── pcc_reg.h    # Register definitions
├── pcc.h        # Driver API header
└── pcc.c        # Driver implementation
```

## 🚀 Cách Sử Dụng

### 1. Bật Clock Đơn Giản

```c
#include "pcc.h"

/* Enable clock for PORTC */
PCC_EnablePeripheralClock(PCC_PORTC_INDEX);

/* Enable clock for ADC0 */
PCC_EnablePeripheralClock(PCC_ADC0_INDEX);

/* Enable clock for LPUART1 */
PCC_EnablePeripheralClock(PCC_LPUART1_INDEX);
```

### 2. Cấu Hình Clock Chi Tiết

```c
/* Configure ADC0: FIRC_DIV2 (24 MHz), no divider */
pcc_config_t adcConfig = {
    .clockSource = PCC_CLK_SRC_FIRC_DIV2,
    .enableClock = true,
    .divider = 0,                /* Divide by 1 */
    .fractionalDivider = false
};

PCC_SetPeripheralClockConfig(PCC_ADC0_INDEX, &adcConfig);
```

### 3. Đổi Clock Source

```c
/* Disable clock first */
PCC_DisablePeripheralClock(PCC_ADC0_INDEX);

/* Change clock source to SPLL_DIV2 */
PCC_SetPeripheralClockSource(PCC_ADC0_INDEX, PCC_CLK_SRC_SPLL_DIV2);

/* Re-enable clock */
PCC_EnablePeripheralClock(PCC_ADC0_INDEX);
```

### 4. Kiểm Tra Trạng Thái

```c
/* Check if clock is enabled */
if (PCC_IsPeripheralClockEnabled(PCC_LPUART1_INDEX)) {
    /* UART1 clock is running */
}

/* Check if peripheral exists on this chip variant */
if (PCC_IsPeripheralPresent(PCC_FLEXCAN2_INDEX)) {
    /* CAN2 is available */
}

/* Get current clock source */
pcc_clock_source_t source = PCC_GetPeripheralClockSource(PCC_ADC0_INDEX);
```

### 5. Đọc Tần Số Clock

```c
/* Get peripheral clock frequency */
uint32_t adcFreq = PCC_GetPeripheralClockFreq(PCC_ADC0_INDEX);
printf("ADC0 Clock: %lu Hz\n", adcFreq);
```

## 📊 Ví Dụ Hoàn Chỉnh

### Khởi Tạo Communication Peripherals

```c
#include "pcc.h"

void Peripheral_Clock_Init(void)
{
    /* Configure LPUART1: SOSCDIV2 (4 MHz) for stable baud rate */
    pcc_config_t uartConfig = {
        .clockSource = PCC_CLK_SRC_SOSC_DIV2,
        .enableClock = true,
        .divider = 0,
        .fractionalDivider = false
    };
    PCC_SetPeripheralClockConfig(PCC_LPUART1_INDEX, &uartConfig);
    
    /* Configure LPI2C0: FIRCDIV2 (24 MHz) */
    pcc_config_t i2cConfig = {
        .clockSource = PCC_CLK_SRC_FIRC_DIV2,
        .enableClock = true,
        .divider = 0,
        .fractionalDivider = false
    };
    PCC_SetPeripheralClockConfig(PCC_LPI2C0_INDEX, &i2cConfig);
    
    /* Enable PORT clocks for GPIO */
    PCC_EnablePeripheralClock(PCC_PORTA_INDEX);
    PCC_EnablePeripheralClock(PCC_PORTB_INDEX);
    PCC_EnablePeripheralClock(PCC_PORTC_INDEX);
    PCC_EnablePeripheralClock(PCC_PORTD_INDEX);
    PCC_EnablePeripheralClock(PCC_PORTE_INDEX);
}
```

### Cấu Hình ADC và Timer

```c
void ADC_Timer_Clock_Init(void)
{
    /* Configure ADC0: SPLLDIV2 (80 MHz) with divider */
    pcc_config_t adcConfig = {
        .clockSource = PCC_CLK_SRC_SPLL_DIV2,
        .enableClock = true,
        .divider = 1,                /* Divide by 2 -> 40 MHz */
        .fractionalDivider = false
    };
    PCC_SetPeripheralClockConfig(PCC_ADC0_INDEX, &adcConfig);
    
    /* Configure PDB0: SPLLDIV2 (80 MHz) */
    pcc_config_t pdbConfig = {
        .clockSource = PCC_CLK_SRC_SPLL_DIV2,
        .enableClock = true,
        .divider = 0,
        .fractionalDivider = false
    };
    PCC_SetPeripheralClockConfig(PCC_PDB0_INDEX, &pdbConfig);
    
    /* Configure FTM0: SPLLDIV2 (80 MHz) */
    pcc_config_t ftmConfig = {
        .clockSource = PCC_CLK_SRC_SPLL_DIV2,
        .enableClock = true,
        .divider = 0,
        .fractionalDivider = false
    };
    PCC_SetPeripheralClockConfig(PCC_FTM0_INDEX, &ftmConfig);
}
```

### Tiết Kiệm Điện Năng

```c
void Peripheral_LowPower_Config(void)
{
    /* Disable unused peripherals to save power */
    PCC_DisablePeripheralClock(PCC_ADC1_INDEX);
    PCC_DisablePeripheralClock(PCC_FLEXCAN1_INDEX);
    PCC_DisablePeripheralClock(PCC_FLEXCAN2_INDEX);
    PCC_DisablePeripheralClock(PCC_FTM2_INDEX);
    PCC_DisablePeripheralClock(PCC_FTM3_INDEX);
    PCC_DisablePeripheralClock(PCC_SAI0_INDEX);
    PCC_DisablePeripheralClock(PCC_SAI1_INDEX);
}
```

## 🔍 API Reference

### Cấu Hình Clock

```c
bool PCC_SetPeripheralClockConfig(uint8_t peripheral, const pcc_config_t *config);
bool PCC_GetPeripheralClockConfig(uint8_t peripheral, pcc_config_t *config);
```

### Clock Enable/Disable

```c
bool PCC_EnablePeripheralClock(uint8_t peripheral);
bool PCC_DisablePeripheralClock(uint8_t peripheral);
```

### Clock Source

```c
bool PCC_SetPeripheralClockSource(uint8_t peripheral, pcc_clock_source_t source);
pcc_clock_source_t PCC_GetPeripheralClockSource(uint8_t peripheral);
```

### Trạng Thái

```c
bool PCC_IsPeripheralClockEnabled(uint8_t peripheral);
bool PCC_IsPeripheralPresent(uint8_t peripheral);
```

### Clock Divider

```c
bool PCC_SetPeripheralClockDivider(uint8_t peripheral, uint8_t divider, bool fractional);
```

### Tần Số

```c
uint32_t PCC_GetPeripheralClockFreq(uint8_t peripheral);
```

## 📋 Peripheral Index

### Communication Peripherals

```c
PCC_LPUART0_INDEX    // UART 0
PCC_LPUART1_INDEX    // UART 1
PCC_LPUART2_INDEX    // UART 2
PCC_LPI2C0_INDEX     // I2C 0
PCC_LPI2C1_INDEX     // I2C 1
PCC_LPSPI0_INDEX     // SPI 0
PCC_LPSPI1_INDEX     // SPI 1
PCC_LPSPI2_INDEX     // SPI 2
PCC_FLEXCAN0_INDEX   // CAN 0
PCC_FLEXCAN1_INDEX   // CAN 1
PCC_FLEXCAN2_INDEX   // CAN 2
```

### Timer Peripherals

```c
PCC_FTM0_INDEX       // FlexTimer 0
PCC_FTM1_INDEX       // FlexTimer 1
PCC_FTM2_INDEX       // FlexTimer 2
PCC_FTM3_INDEX       // FlexTimer 3
PCC_LPTMR0_INDEX     // Low Power Timer
PCC_PDB0_INDEX       // Programmable Delay Block 0
PCC_PDB1_INDEX       // Programmable Delay Block 1
PCC_RTC_INDEX        // Real-Time Clock
```

### Analog Peripherals

```c
PCC_ADC0_INDEX       // ADC 0
PCC_ADC1_INDEX       // ADC 1
PCC_CMP0_INDEX       // Comparator 0
```

### GPIO Peripherals

```c
PCC_PORTA_INDEX      // PORT A
PCC_PORTB_INDEX      // PORT B
PCC_PORTC_INDEX      // PORT C
PCC_PORTD_INDEX      // PORT D
PCC_PORTE_INDEX      // PORT E
```

### Other Peripherals

```c
PCC_DMA_INDEX        // DMA Controller
PCC_DMAMUX_INDEX     // DMA Multiplexer
PCC_EWM_INDEX        // External Watchdog Monitor
PCC_CRC_INDEX        // CRC Module
PCC_FLEXIO_INDEX     // Flexible I/O
PCC_QSPI_INDEX       // Quad SPI (if available)
PCC_ENET_INDEX       // Ethernet (if available)
```

## ⚠️ Lưu Ý Quan Trọng

1. **Clock source change**: Phải disable clock trước khi đổi clock source
2. **Peripheral presence**: Không phải tất cả peripherals đều có trên mọi variant của S32K144
3. **Clock dependencies**: Một số peripherals cần clock nguồn đặc biệt (ví dụ: RTC cần LPO)
4. **Reset default**: Sau reset, tất cả peripheral clocks đều bị tắt

## 📈 Clock Source Recommendations

| Peripheral | Recommended Source | Lý Do |
|------------|-------------------|-------|
| **UART** | SOSC_DIV2 | Stable baud rate |
| **I2C** | FIRC_DIV2 | Fast, stable |
| **SPI** | SPLL_DIV2 | High speed |
| **ADC** | FIRC_DIV2 / SPLL_DIV2 | Stable sampling |
| **FTM** | SPLL_DIV2 | PWM resolution |
| **PDB** | SPLL_DIV2 | Precise timing |
| **CAN** | SOSC_DIV2 | CAN spec compliance |

## 🔗 Liên Quan

- **SCG Driver**: Cung cấp clock sources cho PCC
- **Reference Manual**: S32K1XXRM Rev. 12.1, Chapter 29

---

**Tác giả:** PhucPH32  
**Ngày:** 18/12/2025  
**Version:** 1.0
