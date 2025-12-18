# SCG (System Clock Generator) Driver

Driver hoàn chỉnh cho SCG peripheral trên S32K144.

## 📋 Tổng Quan

SCG (System Clock Generator) là module điều khiển đồng hồ hệ thống trên S32K144, cung cấp:
- **4 nguồn clock**: SIRC, FIRC, SOSC, SPLL
- **3 chế độ hoạt động**: RUN, VLPR, HSRUN
- **Clock dividers**: Core, Bus, Slow clocks
- **Quản lý clock linh hoạt** cho các chế độ tiết kiệm điện

## 🔧 Tính Năng

### Nguồn Clock Hỗ Trợ

| Nguồn Clock | Tần Số | Mô Tả |
|------------|--------|-------|
| **SIRC** | 2 MHz / 8 MHz | Slow Internal RC oscillator |
| **FIRC** | 48/52/56/60 MHz | Fast Internal RC oscillator |
| **SOSC** | External | System Oscillator (cần crystal) |
| **SPLL** | Configurable | System PLL (dùng SOSC làm input) |

### Clock Dividers

Mỗi clock có thể chia cho Core, Bus, và Slow clocks:
- **DIVCORE**: Chia cho core clock (CPU)
- **DIVBUS**: Chia cho bus clock (peripherals)
- **DIVSLOW**: Chia cho slow clock (flash, low-speed peripherals)
- **Giá trị chia**: 1-16

## 📁 Cấu Trúc File

```
lib/hal/scg/
├── scg_reg.h    # Register definitions
├── scg.h        # Driver API header
└── scg.c        # Driver implementation
```

## 🚀 Cách Sử Dụng

### 1. Khởi Tạo FIRC (48 MHz)

```c
#include "scg.h"

/* Configure FIRC */
scg_firc_config_t fircConfig = {
    .range = SCG_FIRC_RANGE_48M,
    .enableInStop = false,
    .enableInLowPower = false,
    .regulator = true,
    .div1 = SCG_CLOCK_DIV_BY_1,  /* FIRCDIV1 = 48 MHz */
    .div2 = SCG_CLOCK_DIV_BY_2   /* FIRCDIV2 = 24 MHz */
};

if (SCG_InitFIRC(&fircConfig)) {
    /* FIRC initialized successfully */
}
```

### 2. Khởi Tạo SOSC (8 MHz External Crystal)

```c
/* Configure SOSC with 8 MHz crystal */
scg_sosc_config_t soscConfig = {
    .freq = 8000000U,              /* 8 MHz crystal */
    .range = SCG_SOSC_RANGE_MID,   /* 1-8 MHz range */
    .enableInStop = false,
    .enableInLowPower = false,
    .useExternalRef = false,       /* Crystal mode */
    .highGainOsc = false
};

if (SCG_InitSOSC(&soscConfig)) {
    /* SOSC initialized successfully */
}
```

### 3. Khởi Tạo SPLL (160 MHz từ 8 MHz SOSC)

```c
/* Configure SPLL: (8 MHz / 1) * 40 = 160 MHz */
scg_spll_config_t spllConfig = {
    .prediv = 0,                   /* Divide by (0+1) = 1 */
    .mult = 24,                    /* Multiply by (24+16) = 40 */
    .enableInStop = false,
    .div1 = SCG_CLOCK_DIV_BY_2,    /* SPLLDIV1 = 80 MHz */
    .div2 = SCG_CLOCK_DIV_BY_2     /* SPLLDIV2 = 80 MHz */
};

if (SCG_InitSPLL(&spllConfig)) {
    /* SPLL locked successfully */
    /* Output: 8MHz / 1 * 40 = 320 MHz VCO / 2 = 160 MHz */
}
```

### 4. Cấu Hình System Clock (80 MHz từ SPLL)

```c
/* Configure system clock: SPLL 160MHz -> Core 80MHz */
scg_system_clock_config_t sysClkConfig = {
    .source = SCG_CLOCK_SRC_SPLL,
    .divCore = SCG_CLOCK_DIV_BY_2,  /* 160/2 = 80 MHz */
    .divBus  = SCG_CLOCK_DIV_BY_2,  /* 160/2 = 80 MHz */
    .divSlow = SCG_CLOCK_DIV_BY_3   /* 160/3 = 53 MHz */
};

if (SCG_SetSystemClockConfig(SCG_SYSTEM_CLOCK_MODE_RUN, &sysClkConfig)) {
    /* System clock switched to SPLL successfully */
}
```

### 5. Đọc Tần Số Clock

```c
/* Get all clock frequencies */
scg_clock_frequencies_t freqs;
if (SCG_GetClockFrequencies(&freqs)) {
    printf("Core Clock: %lu Hz\n", freqs.coreClk);
    printf("Bus Clock:  %lu Hz\n", freqs.busClk);
    printf("Slow Clock: %lu Hz\n", freqs.slowClk);
}

/* Or get individual frequencies */
uint32_t coreFreq = SCG_GetCoreClockFreq();
uint32_t busFreq  = SCG_GetBusClockFreq();
```

## 📊 Ví Dụ Hoàn Chỉnh

### Cấu Hình RUN Mode 80 MHz

```c
#include "scg.h"

void Clock_Init_80MHz(void)
{
    /* 1. Initialize SOSC (8 MHz external crystal) */
    scg_sosc_config_t soscConfig = {
        .freq = 8000000U,
        .range = SCG_SOSC_RANGE_MID,
        .enableInStop = false,
        .enableInLowPower = false,
        .useExternalRef = false,
        .highGainOsc = false
    };
    SCG_InitSOSC(&soscConfig);
    
    /* 2. Initialize SPLL (160 MHz) */
    scg_spll_config_t spllConfig = {
        .prediv = 0,                /* Divide by 1 */
        .mult = 24,                 /* Multiply by 40 */
        .enableInStop = false,
        .div1 = SCG_CLOCK_DIV_BY_2,
        .div2 = SCG_CLOCK_DIV_BY_2
    };
    SCG_InitSPLL(&spllConfig);
    
    /* 3. Switch system clock to SPLL (80 MHz core) */
    scg_system_clock_config_t sysConfig = {
        .source = SCG_CLOCK_SRC_SPLL,
        .divCore = SCG_CLOCK_DIV_BY_2,  /* 80 MHz */
        .divBus  = SCG_CLOCK_DIV_BY_2,  /* 80 MHz */
        .divSlow = SCG_CLOCK_DIV_BY_3   /* 53 MHz */
    };
    SCG_SetSystemClockConfig(SCG_SYSTEM_CLOCK_MODE_RUN, &sysConfig);
}
```

## 🔍 API Reference

### Khởi Tạo Clock Sources

```c
bool SCG_InitSIRC(const scg_sirc_config_t *config);
bool SCG_InitFIRC(const scg_firc_config_t *config);
bool SCG_InitSOSC(const scg_sosc_config_t *config);
bool SCG_InitSPLL(const scg_spll_config_t *config);
```

### Cấu Hình System Clock

```c
bool SCG_SetSystemClockConfig(scg_system_clock_mode_t mode, 
                               const scg_system_clock_config_t *config);
bool SCG_GetSystemClockConfig(scg_system_clock_mode_t mode, 
                               scg_system_clock_config_t *config);
```

### Quản Lý Clock

```c
bool SCG_EnableClock(scg_clock_source_t source);
bool SCG_DisableClock(scg_clock_source_t source);
bool SCG_IsClockValid(scg_clock_source_t source);
scg_clock_source_t SCG_GetSystemClockSource(void);
```

### Đọc Tần Số

```c
bool SCG_GetClockFrequencies(scg_clock_frequencies_t *freqs);
uint32_t SCG_GetCoreClockFreq(void);
uint32_t SCG_GetBusClockFreq(void);
uint32_t SCG_GetSlowClockFreq(void);
```

## ⚠️ Lưu Ý Quan Trọng

1. **SPLL yêu cầu SOSC**: Phải khởi tạo SOSC trước khi sử dụng SPLL
2. **Clock switching**: Không thể tắt clock source đang được sử dụng
3. **FIRC default**: Sau reset, hệ thống dùng FIRC làm clock mặc định
4. **Timeout**: Các hàm có timeout để tránh treo nếu clock không valid

## 📈 Các Cấu Hình Clock Phổ Biến

| Cấu Hình | Core | Bus | Slow | Nguồn |
|----------|------|-----|------|-------|
| **Default (Reset)** | 48 MHz | 48 MHz | 24 MHz | FIRC |
| **RUN 80MHz** | 80 MHz | 40 MHz | 26 MHz | SPLL |
| **RUN 112MHz** | 112 MHz | 56 MHz | 28 MHz | SPLL |
| **HSRUN 120MHz** | 120 MHz | 60 MHz | 30 MHz | SPLL |
| **VLPR Low Power** | 8 MHz | 8 MHz | 4 MHz | SIRC |

## 🔗 Liên Quan

- **PCC Driver**: Sử dụng SCG để cung cấp clock cho peripherals
- **Reference Manual**: S32K1XXRM Rev. 12.1, Chapter 28

---

**Tác giả:** PhucPH32  
**Ngày:** 18/12/2025  
**Version:** 1.0
