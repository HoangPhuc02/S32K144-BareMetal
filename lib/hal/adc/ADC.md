# ADC Driver for S32K144 - Hướng Dẫn Thiết Lập và Sử Dụng

## Mục Lục
1. [Giới Thiệu](#giới-thiệu)
2. [Đặc Điểm Phần Cứng](#đặc-điểm-phần-cứng)
3. [Kiến Trúc Driver](#kiến-trúc-driver)
4. [Thiết Lập Cơ Bản](#thiết-lập-cơ-bản)
5. [Các Chế Độ Hoạt Động](#các-chế-độ-hoạt-động)
6. [API Reference](#api-reference)
7. [Ví Dụ Sử Dụng](#ví-dụ-sử-dụng)
8. [Lưu Ý và Khuyến Nghị](#lưu-ý-và-khuyến-nghị)

---

## Giới Thiệu

ADC (Analog-to-Digital Converter) driver cho S32K144 cung cấp interface đầy đủ để chuyển đổi tín hiệu analog sang digital với độ phân giải cao và nhiều tính năng nâng cao.

### Tính Năng Chính
- ✅ Hỗ trợ ADC0 và ADC1 (2 module ADC độc lập)
- ✅ Độ phân giải: 8-bit, 10-bit, 12-bit
- ✅ 16 kênh đầu vào analog mỗi module
- ✅ Chế độ single conversion và continuous conversion
- ✅ Hardware averaging (4, 8, 16, 32 samples)
- ✅ Software trigger và hardware trigger
- ✅ Interrupt và DMA support
- ✅ Calibration tự động
- ✅ Multiple voltage reference options

---

## Đặc Điểm Phần Cứng

### 1. Khối ADC của S32K144

```
┌─────────────────────────────────────────┐
│         ADC Module (ADC0/ADC1)          │
├─────────────────────────────────────────┤
│                                         │
│  ┌──────────┐    ┌──────────────┐     │
│  │ 16 Input │───▶│  12-bit SAR  │     │
│  │ Channels │    │     ADC      │     │
│  └──────────┘    └──────────────┘     │
│                         │               │
│  ┌──────────┐          ▼               │
│  │  Clock   │    ┌──────────────┐     │
│  │ Divider  │───▶│   Control    │     │
│  └──────────┘    │    Logic     │     │
│                   └──────────────┘     │
│  ┌──────────┐          │               │
│  │ Hardware │          ▼               │
│  │Averaging │    ┌──────────────┐     │
│  └──────────┘    │ Result Regs  │     │
│                   │  (16 x 12b)  │     │
│                   └──────────────┘     │
│                         │               │
│                         ▼               │
│                  ┌──────────────┐      │
│                  │  Interrupt   │      │
│                  │     DMA      │      │
│                  └──────────────┘      │
└─────────────────────────────────────────┘
```

### 2. ADC Channels và Pin Mapping

| Channel | ADC0 Pin | ADC1 Pin | Chức Năng                    |
|---------|----------|----------|------------------------------|
| AD0     | PTA0     | PTB0     | External analog input 0      |
| AD1     | PTA1     | PTB1     | External analog input 1      |
| AD2     | PTA6     | PTB13    | External analog input 2      |
| AD3     | PTA7     | PTB14    | External analog input 3      |
| AD4     | PTB0     | PTB2     | External analog input 4      |
| AD5     | PTB1     | PTB3     | External analog input 5      |
| AD6     | PTB13    | PTB4     | External analog input 6      |
| AD7     | PTB14    | PTB5     | External analog input 7      |
| AD8     | PTC0     | PTC1     | External analog input 8      |
| AD9     | PTC1     | PTC0     | External analog input 9      |
| AD10    | PTC2     | PTC2     | External analog input 10     |
| AD11    | PTC3     | PTC3     | External analog input 11     |
| AD12    | PTC14    | PTC14    | External analog input 12     |
| AD13    | PTC15    | PTC15    | External analog input 13     |
| AD14    | PTC16    | PTC16    | External analog input 14     |
| AD15    | PTC17    | PTC17    | External analog input 15     |

### 3. Timing Specifications

| Parameter                  | Min   | Typ   | Max   | Unit |
|---------------------------|-------|-------|-------|------|
| Conversion Time (12-bit)  | -     | 2.4   | -     | µs   |
| Conversion Time (10-bit)  | -     | 2.0   | -     | µs   |
| Conversion Time (8-bit)   | -     | 1.6   | -     | µs   |
| ADC Clock Frequency       | 0.4   | 2     | 8     | MHz  |
| Sampling Rate (12-bit)    | -     | 400   | -     | kSPS |
| Input Voltage Range       | 0     | -     | VREFH | V    |

---

## Kiến Trúc Driver

### 1. Cấu Trúc File

```
Core/Drivers/adc/
├── adc_reg.h       # Register definitions và bit masks
├── adc.h           # API interface và data structures  
└── adc.c           # Implementation code

example/
└── adc_example.c   # Usage examples
```

### 2. Các Layer của Driver

```
┌────────────────────────────────────────┐
│        Application Layer               │
│   (adc_example.c)                      │
├────────────────────────────────────────┤
│        API Layer                       │
│   ADC_Init(), ADC_ReadBlocking()       │
│   ADC_Calibrate(), etc.                │
├────────────────────────────────────────┤
│        HAL Layer                       │
│   ADC configuration, control           │
│   (adc.c)                              │
├────────────────────────────────────────┤
│        Register Layer                  │
│   Register definitions, macros         │
│   (adc_reg.h)                          │
├────────────────────────────────────────┤
│        Hardware                        │
│   ADC0, ADC1 peripherals               │
└────────────────────────────────────────┘
```

---

## Thiết Lập Cơ Bản

### 1. Quy Trình Khởi Tạo ADC

```c
// Bước 1: Cấu hình PORT (nếu sử dụng pin external)
PORT_SetPinMux(PORT_INSTANCE_B, 0, PORT_MUX_ALT0);  // PTB0 as ADC0_SE4

// Bước 2: Enable clock cho ADC
ADC_EnableClock(ADC_INSTANCE_0);

// Bước 3: Cấu hình ADC
ADC_Config_t adcConfig = {
    .clockSource = ADC_CLK_ALT1,              // Clock source
    .resolution = ADC_RESOLUTION_12BIT,       // 12-bit resolution
    .clockDivider = ADC_CLK_DIV_4,            // Clock divider
    .voltageRef = ADC_VREF_VREFH_VREFL,       // Voltage reference
    .triggerSource = ADC_TRIGGER_SOFTWARE,    // Trigger mode
    .continuousMode = false,                  // Single conversion
    .dmaEnable = false,                       // No DMA
    .interruptEnable = false                  // No interrupt
};

ADC_Init(ADC_INSTANCE_0, &adcConfig);

// Bước 4: Calibrate ADC (khuyến nghị)
if (ADC_Calibrate(ADC_INSTANCE_0) != ADC_STATUS_SUCCESS) {
    // Xử lý lỗi calibration
}

// Bước 5: Thực hiện conversion
uint16_t adcValue;
ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &adcValue);

// Bước 6: Chuyển đổi sang điện áp (optional)
uint32_t voltage = ADC_ConvertToVoltage(adcValue, ADC_RESOLUTION_12BIT, 3300);
```

### 2. Clock Configuration

ADC clock có thể được cấu hình từ nhiều nguồn:

```c
typedef enum {
    ADC_CLK_ALT1 = 0U,  // Alternate clock 1 (thường là SIRCDIV2)
    ADC_CLK_ALT2 = 1U,  // Alternate clock 2 (thường là FIRCDIV2)
    ADC_CLK_ALT3 = 2U,  // Alternate clock 3 (thường là SOSCDIV2)
    ADC_CLK_ALT4 = 3U   // Alternate clock 4 (thường là SPLLDIV2)
} ADC_ClockSource_t;
```

**Lưu ý:** ADC clock frequency phải trong khoảng 0.4 - 8 MHz để đảm bảo độ chính xác.

### 3. Voltage Reference Selection

```c
typedef enum {
    ADC_VREF_VREFH_VREFL = 0U,  // VREFH/VREFL pins (default)
    ADC_VREF_VALTH = 1U         // VALTH (alternate reference)
} ADC_VoltageRef_t;
```

**Khuyến nghị:** Sử dụng VREFH/VREFL với reference voltage ổn định (3.3V).

---

## Các Chế Độ Hoạt Động

### 1. Single Conversion Mode

Thực hiện một lần conversion khi được trigger:

```c
ADC_Config_t config = {
    .continuousMode = false,  // Single conversion
    // ... other settings
};

ADC_Init(ADC_INSTANCE_0, &config);

// Thực hiện conversion
uint16_t result;
ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &result);
```

**Ưu điểm:**
- Tiết kiệm năng lượng
- Kiểm soát chính xác thời điểm conversion

**Nhược điểm:**
- Phải trigger lại cho mỗi conversion

### 2. Continuous Conversion Mode

ADC tự động thực hiện conversion liên tục:

```c
ADC_Config_t config = {
    .continuousMode = true,  // Continuous conversion
    // ... other settings
};

ADC_Init(ADC_INSTANCE_0, &config);

// Start continuous conversion
ADC_StartConversion(ADC_INSTANCE_0, ADC_CHANNEL_AD4);

// Read results whenever needed
while (running) {
    if (ADC_IsConversionComplete(ADC_INSTANCE_0)) {
        uint16_t result = ADC_GetResult(ADC_INSTANCE_0);
        // Process result
    }
}

// Stop when done
ADC_StopConversion(ADC_INSTANCE_0);
```

**Ưu điểm:**
- Sampling rate cao
- Phù hợp với real-time monitoring

**Nhược điểm:**
- Tiêu thụ năng lượng cao hơn

### 3. Hardware Averaging Mode

Tự động tính trung bình nhiều sample để giảm noise:

```c
ADC_AverageConfig_t avgConfig = {
    .enable = true,
    .averageMode = ADC_AVERAGE_16  // Average 16 samples
};

ADC_ConfigureAveraging(ADC_INSTANCE_0, &avgConfig);

// Mỗi conversion sẽ là trung bình của 16 samples
ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &result);
```

**Hiệu Quả Giảm Noise:**

| Averaging | Samples | Noise Reduction | Conversion Time |
|-----------|---------|-----------------|-----------------|
| OFF       | 1       | 1x              | ~2.4 µs         |
| 4         | 4       | 2x              | ~9.6 µs         |
| 8         | 8       | 2.8x            | ~19.2 µs        |
| 16        | 16      | 4x              | ~38.4 µs        |
| 32        | 32      | 5.7x            | ~76.8 µs        |

### 4. Interrupt Mode

Sử dụng interrupt để xử lý conversion result:

```c
void ADC_ConversionCallback(ADC_Instance_t instance, uint16_t result) {
    // Xử lý result trong callback
    g_adcResult = result;
    g_conversionDone = true;
}

ADC_Config_t config = {
    .interruptEnable = true,  // Enable interrupt
    // ... other settings
};

ADC_Init(ADC_INSTANCE_0, &config);
ADC_RegisterCallback(ADC_INSTANCE_0, ADC_ConversionCallback);

// Enable interrupt trong NVIC
NVIC_EnableIRQ(ADC0_IRQn);

// Start conversion (non-blocking)
ADC_StartConversion(ADC_INSTANCE_0, ADC_CHANNEL_AD4);

// Callback sẽ được gọi khi conversion hoàn thành
```

---

## API Reference

### Initialization Functions

#### ADC_Init()
```c
ADC_Status_t ADC_Init(ADC_Instance_t instance, const ADC_Config_t *config);
```
**Mô tả:** Khởi tạo ADC với cấu hình được chỉ định.

**Parameters:**
- `instance`: ADC instance (ADC_INSTANCE_0 hoặc ADC_INSTANCE_1)
- `config`: Pointer đến cấu trúc cấu hình

**Return:** Status code

**Lưu ý:** Clock phải được enable trước khi gọi hàm này.

---

#### ADC_Calibrate()
```c
ADC_Status_t ADC_Calibrate(ADC_Instance_t instance);
```
**Mô tả:** Thực hiện calibration sequence cho ADC.

**Khi nào cần calibrate:**
- Sau khi khởi tạo ADC
- Khi nhiệt độ thay đổi đáng kể (>10°C)
- Khi voltage thay đổi
- Sau khi thay đổi clock configuration

**Thời gian:** Khoảng 1-2 ms

---

### Conversion Functions

#### ADC_ReadBlocking()
```c
ADC_Status_t ADC_ReadBlocking(ADC_Instance_t instance, 
                              ADC_Channel_t channel, 
                              uint16_t *result);
```
**Mô tả:** Thực hiện conversion và chờ kết quả (blocking call).

**Quy trình:**
1. Start conversion trên channel
2. Wait cho conversion complete
3. Read và return result

**Thời gian:** ~2.4 µs (12-bit) + overhead

---

#### ADC_StartConversion()
```c
ADC_Status_t ADC_StartConversion(ADC_Instance_t instance, 
                                 ADC_Channel_t channel);
```
**Mô tả:** Bắt đầu conversion (non-blocking).

**Sử dụng với:**
- Continuous mode
- Interrupt mode
- Manual polling

---

#### ADC_IsConversionComplete()
```c
bool ADC_IsConversionComplete(ADC_Instance_t instance);
```
**Mô tả:** Kiểm tra xem conversion đã hoàn thành chưa.

**Return:** 
- `true`: Conversion complete
- `false`: Conversion in progress

---

#### ADC_GetResult()
```c
uint16_t ADC_GetResult(ADC_Instance_t instance);
```
**Mô tả:** Đọc kết quả conversion gần nhất.

**Lưu ý:** Nên kiểm tra conversion complete trước khi gọi.

---

### Utility Functions

#### ADC_ConvertToVoltage()
```c
uint32_t ADC_ConvertToVoltage(uint16_t adcValue, 
                               ADC_Resolution_t resolution, 
                               uint32_t vrefMillivolts);
```
**Mô tả:** Chuyển đổi giá trị ADC sang điện áp (mV).

**Công thức:**
```
Voltage (mV) = (adcValue × vrefMillivolts) / maxValue
```

Trong đó:
- `maxValue` = 255 (8-bit), 1023 (10-bit), 4095 (12-bit)
- `vrefMillivolts` = Điện áp reference (thường 3300 mV)

**Ví dụ:**
```c
uint16_t adc = 2048;  // Giá trị ADC
uint32_t voltage = ADC_ConvertToVoltage(adc, ADC_RESOLUTION_12BIT, 3300);
// voltage = (2048 × 3300) / 4095 = 1650 mV
```

---

## Ví Dụ Sử Dụng

### Example 1: Đọc Điện Áp Đơn Giản

```c
void ReadVoltage_Simple(void)
{
    uint16_t adcValue;
    uint32_t voltage;
    
    // Khởi tạo ADC
    ADC_Config_t config = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = false,
        .dmaEnable = false,
        .interruptEnable = false
    };
    
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &config);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    // Đọc giá trị
    ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &adcValue);
    
    // Chuyển sang điện áp
    voltage = ADC_ConvertToVoltage(adcValue, ADC_RESOLUTION_12BIT, 3300);
    
    printf("ADC: %d, Voltage: %d mV\n", adcValue, voltage);
}
```

### Example 2: Đọc Potentiometer với Averaging

```c
void ReadPotentiometer(void)
{
    uint16_t adcValue;
    uint32_t percentage;
    
    // Cấu hình với hardware averaging
    ADC_Config_t config = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = false,
        .dmaEnable = false,
        .interruptEnable = false
    };
    
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &config);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    // Enable averaging
    ADC_AverageConfig_t avgConfig = {
        .enable = true,
        .averageMode = ADC_AVERAGE_16
    };
    ADC_ConfigureAveraging(ADC_INSTANCE_0, &avgConfig);
    
    // Đọc liên tục
    while (1) {
        ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &adcValue);
        
        // Tính phần trăm (0-100%)
        percentage = (adcValue * 100) / 4095;
        
        printf("Position: %d%%\n", percentage);
        
        SYSTICK_DelayMs(100);  // Delay 100ms
    }
}
```

### Example 3: Đọc Nhiều Kênh

```c
void ReadMultipleChannels(void)
{
    uint16_t channels_adc[4];
    uint32_t channels_voltage[4];
    
    ADC_Channel_t channels[] = {
        ADC_CHANNEL_AD4,
        ADC_CHANNEL_AD5,
        ADC_CHANNEL_AD6,
        ADC_CHANNEL_AD7
    };
    
    // Khởi tạo ADC
    ADC_Config_t config = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = false,
        .dmaEnable = false,
        .interruptEnable = false
    };
    
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &config);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    // Đọc tất cả channels
    for (uint32_t i = 0; i < 4; i++) {
        ADC_ReadBlocking(ADC_INSTANCE_0, channels[i], &channels_adc[i]);
        channels_voltage[i] = ADC_ConvertToVoltage(
            channels_adc[i], 
            ADC_RESOLUTION_12BIT, 
            3300
        );
        
        printf("CH%d: %d (ADC) = %d mV\n", 
               i, channels_adc[i], channels_voltage[i]);
    }
}
```

### Example 4: Continuous Mode với Interrupt

```c
volatile uint16_t g_latestResult = 0;
volatile bool g_newData = false;

void ADC_MyCallback(ADC_Instance_t instance, uint16_t result)
{
    g_latestResult = result;
    g_newData = true;
}

void ContinuousMonitoring(void)
{
    // Cấu hình với continuous + interrupt
    ADC_Config_t config = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = true,      // Continuous mode
        .dmaEnable = false,
        .interruptEnable = true      // Interrupt enabled
    };
    
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &config);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    // Register callback
    ADC_RegisterCallback(ADC_INSTANCE_0, ADC_MyCallback);
    
    // Enable interrupt trong NVIC
    NVIC_EnableIRQ(ADC0_IRQn);
    
    // Start continuous conversion
    ADC_StartConversion(ADC_INSTANCE_0, ADC_CHANNEL_AD4);
    
    // Main loop
    while (1) {
        if (g_newData) {
            g_newData = false;
            
            uint32_t voltage = ADC_ConvertToVoltage(
                g_latestResult, 
                ADC_RESOLUTION_12BIT, 
                3300
            );
            
            printf("New data: %d mV\n", voltage);
        }
        
        // Làm công việc khác
    }
}
```

---

## Lưu Ý và Khuyến Nghị

### 1. Clock Configuration

⚠️ **Quan Trọng:** ADC clock frequency phải trong khoảng **0.4 - 8 MHz**.

```c
// Ví dụ: Bus clock = 48 MHz
// Cần divider = 8 để có ADC clock = 6 MHz (OK)
.clockDivider = ADC_CLK_DIV_8
```

**Bảng tính ADC clock:**

| Bus Clock | Divider | ADC Clock | Valid?     |
|-----------|---------|-----------|------------|
| 48 MHz    | /4      | 12 MHz    | ❌ Quá cao |
| 48 MHz    | /8      | 6 MHz     | ✅ OK      |
| 24 MHz    | /4      | 6 MHz     | ✅ OK      |
| 8 MHz     | /1      | 8 MHz     | ✅ OK      |

### 2. Calibration

**Khi nào cần calibrate:**
- ✅ Sau khi khởi tạo ADC (bắt buộc)
- ✅ Khi nhiệt độ thay đổi >10°C
- ✅ Sau khi thay đổi voltage hoặc clock
- ✅ Định kỳ trong ứng dụng high-precision

**Không cần calibrate:**
- ❌ Trước mỗi conversion
- ❌ Khi switch channel (không cần)

### 3. Hardware vs Software Averaging

**Hardware Averaging:**
- ✅ Tự động bởi phần cứng
- ✅ Giảm CPU overhead
- ✅ Timing chính xác
- ❌ Tăng conversion time

**Software Averaging:**
- ✅ Linh hoạt hơn (custom algorithm)
- ✅ Có thể filter outliers
- ❌ CPU overhead cao hơn
- ❌ Timing không chính xác bằng

**Khuyến nghị:** Sử dụng hardware averaging khi có thể.

### 4. Input Impedance

ADC có input impedance khoảng **10 kΩ**.

**Source impedance khuyến nghị:**
- ✅ < 1 kΩ: Tốt nhất
- ⚠️ 1-10 kΩ: OK, có thể cần tăng sampling time
- ❌ > 10 kΩ: Cần buffer hoặc tăng sampling time

**Ví dụ:**
```c
// Nếu source impedance cao
.clockDivider = ADC_CLK_DIV_8,  // Chậm hơn = sampling time dài hơn

// Hoặc dùng op-amp buffer
// Vin ──[High Z]──> OpAmp ──[Low Z]──> ADC
```

### 5. Voltage Reference

**VREFH/VREFL pins:**
- ✅ Độ chính xác cao nhất
- ✅ External reference có thể
- ⚠️ Cần decoupling capacitor (0.1µF)

**VALTH (alternate):**
- ⚠️ Độ chính xác thấp hơn
- ✅ Không cần external connection

**Khuyến nghị:** Sử dụng VREFH/VREFL với 3.3V stable reference.

### 6. Noise Reduction Tips

**Hardware:**
- ✅ Decoupling capacitors (0.1µF + 10µF)
- ✅ Separate analog và digital ground
- ✅ Twisted pair cho analog signals
- ✅ Shielding cho dây dài

**Software:**
- ✅ Hardware averaging (16-32 samples)
- ✅ Digital filtering (moving average, median)
- ✅ Oversampling and decimation
- ✅ Discard outliers

**Circuit example:**
```
     3.3V
      │
      ├── 10µF ──┐
      ├── 0.1µF ─┼─ VREFH
      │          │
Analog Signal ────┼─ ADCx_SEy
      │          │
      └─────────┼─ VREFL
                │
               GND
```

### 7. Performance Optimization

**Maximize sampling rate:**
```c
config.resolution = ADC_RESOLUTION_8BIT;   // Fastest
config.clockDivider = ADC_CLK_DIV_1;       // Highest clock
avgConfig.enable = false;                   // No averaging
```

**Maximize accuracy:**
```c
config.resolution = ADC_RESOLUTION_12BIT;  // Highest resolution
config.clockDivider = ADC_CLK_DIV_8;       // Slower, more stable
avgConfig.enable = true;
avgConfig.averageMode = ADC_AVERAGE_32;    // Most averaging
```

**Balance (recommended):**
```c
config.resolution = ADC_RESOLUTION_12BIT;
config.clockDivider = ADC_CLK_DIV_4;
avgConfig.enable = true;
avgConfig.averageMode = ADC_AVERAGE_16;
```

### 8. Common Mistakes

❌ **Không calibrate sau init:**
```c
ADC_Init(ADC_INSTANCE_0, &config);
// Thiếu: ADC_Calibrate(ADC_INSTANCE_0);
ADC_ReadBlocking(...);  // Kết quả không chính xác!
```

✅ **Đúng:**
```c
ADC_Init(ADC_INSTANCE_0, &config);
ADC_Calibrate(ADC_INSTANCE_0);  // Bắt buộc!
ADC_ReadBlocking(...);
```

---

❌ **Clock quá nhanh:**
```c
config.clockDivider = ADC_CLK_DIV_1;  // Bus 48MHz → ADC 48MHz (WRONG!)
```

✅ **Đúng:**
```c
config.clockDivider = ADC_CLK_DIV_8;  // Bus 48MHz → ADC 6MHz (OK)
```

---

❌ **Đọc result trước khi conversion complete:**
```c
ADC_StartConversion(...);
uint16_t result = ADC_GetResult(...);  // Chưa complete!
```

✅ **Đúng:**
```c
ADC_StartConversion(...);
ADC_WaitForConversion(...);
uint16_t result = ADC_GetResult(...);  // OK
```

---

## Tài Liệu Tham Khảo

1. **S32K144 Reference Manual** - Chapter 37: ADC
2. **S32K144 Datasheet** - ADC Electrical Characteristics
3. **AN5142** - Tips and Tricks for ADC Configuration
4. **AN4342** - ADC Calibration Procedure

---

## Phụ Lục

### A. Conversion Time Calculation

Công thức tính conversion time:

```
T_conv = T_sample + T_SAR

Trong đó:
- T_sample = Sample time (configure via CFG2[SMPLTS])
- T_SAR = SAR conversion time (phụ thuộc resolution)
```

**SAR conversion times:**
- 8-bit: 20 ADC clocks
- 10-bit: 24 ADC clocks  
- 12-bit: 28 ADC clocks

**Ví dụ:**
```
ADC Clock = 6 MHz → T_adc = 1/6MHz = 0.167 µs
Resolution = 12-bit → T_SAR = 28 × 0.167 = 4.67 µs
Sample Time = 0xFF → T_sample = 255 × 0.167 = 42.5 µs

Total = 4.67 + 42.5 = 47.17 µs
```

### B. Resolution vs Accuracy

| Resolution | Steps | LSB @ 3.3V | Accuracy (typical) |
|------------|-------|------------|-------------------|
| 8-bit      | 256   | 12.9 mV    | ±1 LSB            |
| 10-bit     | 1024  | 3.22 mV    | ±1 LSB            |
| 12-bit     | 4096  | 0.806 mV   | ±2 LSB            |

**Lưu ý:** Accuracy thực tế phụ thuộc vào nhiều yếu tố (noise, reference voltage, temperature, etc.)

---

**Copyright © 2025 PhucPH32. All rights reserved.**
