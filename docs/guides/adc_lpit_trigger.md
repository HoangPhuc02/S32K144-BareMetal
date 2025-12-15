# ADC Hardware Trigger với LPIT - Hướng dẫn sử dụng

## Tổng quan

Module này cho phép sử dụng LPIT (Low Power Interrupt Timer) để tự động trigger ADC conversion theo chu kỳ cố định. Điều này rất hữu ích cho:

- **Periodic sampling**: Lấy mẫu analog signal theo tần số cố định
- **Data acquisition**: Thu thập dữ liệu tự động không cần CPU
- **Power efficiency**: CPU có thể ngủ trong khi LPIT trigger ADC
- **Timing accuracy**: LPIT đảm bảo timing chính xác cho sampling

## Nguyên lý hoạt động

```
┌──────┐      Trigger      ┌──────┐    Conversion    ┌──────────┐
│ LPIT │ ────────────────> │ ADC  │ ──────────────> │ Interrupt│
└──────┘    (Periodic)      └──────┘    Complete     └──────────┘
    │                           │                           │
    │                           │                           │
    ▼                           ▼                           ▼
1 kHz Timer              Convert Channel           Callback
```

### Workflow:
1. **LPIT**: Tạo periodic trigger (ví dụ: 1 kHz)
2. **ADC**: Nhận trigger, start conversion tự động
3. **Interrupt**: Khi conversion complete, gọi callback
4. **Processing**: Xử lý result trong callback hoặc main loop

## Cấu hình cơ bản

### 1. Khởi tạo ADC với Hardware Trigger

```c
#include "adc.h"
#include "lpit.h"

// Configure ADC
ADC_Config_t adcConfig = {
    .clockSource = ADC_CLK_ALT1,            // FIRC clock
    .resolution = ADC_RESOLUTION_12BIT,     // 12-bit
    .clockDivider = ADC_CLK_DIV_4,
    .voltageRef = ADC_VREF_VREFH_VREFL,
    .triggerSource = ADC_TRIGGER_HARDWARE,  // ← Hardware trigger!
    .continuousMode = false,                // Single shot per trigger
    .dmaEnable = false,
    .interruptEnable = true                 // Enable interrupt
};

ADC_Init(ADC_INSTANCE_0, &adcConfig);
ADC_Calibrate(ADC_INSTANCE_0);
```

### 2. Cấu hình Hardware Trigger

```c
// Configure ADC for LPIT trigger on channel 0
ADC_ConfigHardwareTrigger(ADC_INSTANCE_0, ADC_CHANNEL_AD0, true);

// Enable interrupt
NVIC_SetPriority(ADC0_IRQn, 3);
NVIC_EnableIRQ(ADC0_IRQn);
```

### 3. Khởi tạo LPIT

```c
// Initialize LPIT with FIRC (48 MHz)
LPIT_Init(LPIT_CLK_SRC_FIRC);

// Configure LPIT channel 0 for 1 kHz trigger
lpit_channel_config_t lpitConfig = {
    .channel = 0,
    .mode = LPIT_MODE_32BIT_PERIODIC,
    .period = 48000,                    // 48MHz / 48000 = 1 kHz
    .enableInterrupt = false,           // No LPIT interrupt needed
    .chainChannel = false,
    .startOnTrigger = false,
    .stopOnInterrupt = false,
    .reloadOnTrigger = false
};

LPIT_ConfigChannel(&lpitConfig);
LPIT_StartChannel(0);  // Start triggering!
```

### 4. Xử lý ADC Result

```c
void ADC_ConversionCallback(ADC_Instance_t instance, uint16_t result)
{
    // This is called automatically on each conversion
    uint32_t voltage = ADC_ConvertToVoltage(result, ADC_RESOLUTION_12BIT, 3300);
    
    // Process result
    printf("ADC: %u, Voltage: %lu mV\n", result, voltage);
}

// Register callback
ADC_RegisterCallback(ADC_INSTANCE_0, ADC_ConversionCallback);
```

---

## API Functions

### `ADC_ConfigHardwareTrigger()`

Cấu hình ADC để sử dụng hardware trigger từ LPIT.

```c
ADC_Status_t ADC_ConfigHardwareTrigger(ADC_Instance_t instance, 
                                       ADC_Channel_t channel, 
                                       bool enableInterrupt);
```

**Parameters:**
- `instance`: ADC instance (ADC_INSTANCE_0 hoặc ADC_INSTANCE_1)
- `channel`: ADC channel để convert (ADC_CHANNEL_AD0 - ADC_CHANNEL_AD15)
- `enableInterrupt`: Enable interrupt khi conversion complete

**Returns:**
- `ADC_STATUS_SUCCESS`: Cấu hình thành công
- `ADC_STATUS_ERROR`: Lỗi tham số

**Example:**
```c
// Enable hardware trigger với interrupt
ADC_ConfigHardwareTrigger(ADC_INSTANCE_0, ADC_CHANNEL_AD0, true);
```

---

### `ADC_StartHardwareTrigger()`

Start ADC ở chế độ hardware trigger. Đây là convenience function kết hợp config và enable.

```c
ADC_Status_t ADC_StartHardwareTrigger(ADC_Instance_t instance, 
                                      ADC_Channel_t channel);
```

**Parameters:**
- `instance`: ADC instance
- `channel`: ADC channel

**Returns:**
- `ADC_STATUS_SUCCESS`: Start thành công

**Example:**
```c
// Start ADC waiting for LPIT triggers
ADC_StartHardwareTrigger(ADC_INSTANCE_0, ADC_CHANNEL_AD0);
```

---

### `ADC_StopHardwareTrigger()`

Dừng ADC hardware trigger mode và disable channel.

```c
ADC_Status_t ADC_StopHardwareTrigger(ADC_Instance_t instance);
```

**Parameters:**
- `instance`: ADC instance

**Returns:**
- `ADC_STATUS_SUCCESS`: Stop thành công

**Example:**
```c
// Stop ADC hardware trigger
ADC_StopHardwareTrigger(ADC_INSTANCE_0);
```

---

### `ADC_IsHardwareTriggerEnabled()`

Kiểm tra xem ADC có đang ở chế độ hardware trigger không.

```c
bool ADC_IsHardwareTriggerEnabled(ADC_Instance_t instance);
```

**Parameters:**
- `instance`: ADC instance

**Returns:**
- `true`: ADC đang ở hardware trigger mode
- `false`: ADC đang ở software trigger mode

**Example:**
```c
if (ADC_IsHardwareTriggerEnabled(ADC_INSTANCE_0)) {
    printf("ADC is waiting for LPIT triggers\n");
}
```

---

## Complete Examples

### Example 1: Basic Periodic Sampling

```c
#include "adc.h"
#include "lpit.h"

#define SAMPLE_RATE_HZ  1000    // 1 kHz
#define LPIT_CLOCK      48000000 // 48 MHz

void Setup_PeriodicSampling(void)
{
    // 1. Configure ADC
    ADC_Config_t adcConfig = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .triggerSource = ADC_TRIGGER_HARDWARE,  // Key!
        .interruptEnable = true
    };
    
    ADC_Init(ADC_INSTANCE_0, &adcConfig);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    // 2. Register callback
    ADC_RegisterCallback(ADC_INSTANCE_0, MyADCCallback);
    
    // 3. Configure for hardware trigger
    ADC_ConfigHardwareTrigger(ADC_INSTANCE_0, ADC_CHANNEL_AD0, true);
    NVIC_EnableIRQ(ADC0_IRQn);
    
    // 4. Configure LPIT
    LPIT_Init(LPIT_CLK_SRC_FIRC);
    
    lpit_channel_config_t lpitConfig = {
        .channel = 0,
        .period = LPIT_CLOCK / SAMPLE_RATE_HZ,
        .enableInterrupt = false
    };
    
    LPIT_ConfigChannel(&lpitConfig);
    LPIT_StartChannel(0);
    
    printf("Sampling at %d Hz\n", SAMPLE_RATE_HZ);
}

void MyADCCallback(ADC_Instance_t instance, uint16_t result)
{
    // Called automatically at 1 kHz
    uint32_t voltage = ADC_ConvertToVoltage(result, ADC_RESOLUTION_12BIT, 3300);
    
    // Process voltage
    if (voltage > 1650) {  // > 1.65V
        LED_On();
    } else {
        LED_Off();
    }
}
```

---

### Example 2: Multi-Channel Scanning

```c
#define NUM_CHANNELS  4
ADC_Channel_t channels[NUM_CHANNELS] = {
    ADC_CHANNEL_AD0,
    ADC_CHANNEL_AD1, 
    ADC_CHANNEL_AD2,
    ADC_CHANNEL_AD3
};

uint16_t channelResults[NUM_CHANNELS];
uint8_t currentChannel = 0;

void MultiChannel_Callback(ADC_Instance_t instance, uint16_t result)
{
    // Store result
    channelResults[currentChannel] = result;
    
    // Move to next channel
    currentChannel = (currentChannel + 1) % NUM_CHANNELS;
    
    // Reconfigure for next channel
    ADC_ConfigHardwareTrigger(ADC_INSTANCE_0, 
                             channels[currentChannel], 
                             true);
    
    // If completed all channels
    if (currentChannel == 0) {
        ProcessAllChannels(channelResults);
    }
}
```

---

### Example 3: Data Acquisition with Averaging

```c
#define BUFFER_SIZE  100

uint16_t adcBuffer[BUFFER_SIZE];
uint32_t bufferIndex = 0;
bool bufferFull = false;

void DataAcquisition_Callback(ADC_Instance_t instance, uint16_t result)
{
    // Fill buffer
    adcBuffer[bufferIndex++] = result;
    
    // Check if buffer full
    if (bufferIndex >= BUFFER_SIZE) {
        bufferIndex = 0;
        bufferFull = true;
    }
}

void ProcessBuffer(void)
{
    if (!bufferFull) return;
    
    // Calculate average
    uint32_t sum = 0;
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        sum += adcBuffer[i];
    }
    uint16_t average = sum / BUFFER_SIZE;
    
    // Convert to voltage
    uint32_t avgVoltage = ADC_ConvertToVoltage(average, 
                                               ADC_RESOLUTION_12BIT, 
                                               3300);
    
    printf("Average: %u mV\n", avgVoltage);
    bufferFull = false;
}
```

---

### Example 4: Dynamic Sampling Rate

```c
void ChangeSamplingRate(uint32_t newRateHz)
{
    // Stop current operation
    LPIT_StopChannel(0);
    ADC_StopHardwareTrigger(ADC_INSTANCE_0);
    
    // Calculate new period
    uint32_t newPeriod = 48000000 / newRateHz;
    
    // Reconfigure LPIT
    lpit_channel_config_t config = {
        .channel = 0,
        .period = newPeriod,
        .enableInterrupt = false
    };
    
    LPIT_ConfigChannel(&config);
    
    // Restart
    ADC_StartHardwareTrigger(ADC_INSTANCE_0, ADC_CHANNEL_AD0);
    LPIT_StartChannel(0);
    
    printf("New rate: %lu Hz\n", newRateHz);
}

// Usage
ChangeSamplingRate(500);   // 500 Hz
ChangeSamplingRate(2000);  // 2 kHz
ChangeSamplingRate(10000); // 10 kHz
```

---

## Sampling Rate Guidelines

| Application | Sampling Rate | LPIT Period (48 MHz) | Notes |
|-------------|---------------|----------------------|-------|
| Temperature | 1-10 Hz | 4,800,000 - 48,000,000 | Slow changing signals |
| Audio | 8-44.1 kHz | 1,088 - 6,000 | Standard audio rates |
| Vibration | 10-50 kHz | 960 - 4,800 | High-speed monitoring |
| Control loop | 100-1000 Hz | 48,000 - 480,000 | Motor control, etc. |

**Formula:**
```
LPIT_Period = LPIT_Clock_Frequency / Desired_Sample_Rate
```

Example: 1 kHz @ 48 MHz
```
Period = 48,000,000 / 1,000 = 48,000 ticks
```

---

## Timing Accuracy

### Factors affecting accuracy:
1. **LPIT clock source**: FIRC (48 MHz) có độ chính xác ±1%
2. **ADC conversion time**: ~12-15 clock cycles
3. **Interrupt latency**: 1-10 µs tùy CPU load

### Jitter measurement:
```c
volatile uint32_t lastTimestamp = 0;
volatile uint32_t maxJitter = 0;

void MeasureJitter_Callback(ADC_Instance_t instance, uint16_t result)
{
    uint32_t currentTime = LPIT_GetCurrentTimerValue(0);
    uint32_t delta = lastTimestamp - currentTime;
    
    if (delta > maxJitter) {
        maxJitter = delta;
    }
    
    lastTimestamp = currentTime;
}
```

---

## Best Practices

### 1. ✅ DO: Use appropriate sampling rate
```c
// Good: Nyquist theorem - sample at 2x signal bandwidth
// For 100 Hz signal, sample at 200+ Hz
#define SIGNAL_BW_HZ    100
#define SAMPLE_RATE_HZ  (SIGNAL_BW_HZ * 2.5)  // 250 Hz with margin
```

### 2. ✅ DO: Keep callback short
```c
void FastCallback(ADC_Instance_t instance, uint16_t result)
{
    // Fast: Store result
    buffer[index++] = result;
    
    // Don't do heavy processing here!
    // Process in main loop instead
}
```

### 3. ❌ DON'T: Process in callback
```c
void SlowCallback(ADC_Instance_t instance, uint16_t result)
{
    // Bad: Heavy processing in callback
    float filtered = ApplyFIRFilter(result);  // Slow!
    printf("Result: %f\n", filtered);         // Very slow!
}
```

### 4. ✅ DO: Handle buffer overflow
```c
void SafeCallback(ADC_Instance_t instance, uint16_t result)
{
    if (bufferIndex < BUFFER_SIZE) {
        buffer[bufferIndex++] = result;
    } else {
        overflowCount++;  // Track overflow
    }
}
```

---

## Troubleshooting

### Problem: No ADC conversions happening

**Solution:**
1. Check LPIT is running: `LPIT_IsChannelRunning(0)`
2. Verify hardware trigger enabled: `ADC_IsHardwareTriggerEnabled()`
3. Check ADC clock is enabled
4. Verify LPIT clock source is configured

```c
// Debug code
if (!LPIT_IsChannelRunning(0)) {
    printf("LPIT not running!\n");
}

if (!ADC_IsHardwareTriggerEnabled(ADC_INSTANCE_0)) {
    printf("ADC not in hardware trigger mode!\n");
}
```

---

### Problem: Sampling rate is wrong

**Solution:**
Check LPIT period calculation:

```c
uint32_t desiredRate = 1000;  // 1 kHz
uint32_t lpitClock = 48000000; // 48 MHz
uint32_t period = lpitClock / desiredRate;

printf("Desired rate: %lu Hz\n", desiredRate);
printf("LPIT period: %lu ticks\n", period);
printf("Actual rate: %lu Hz\n", lpitClock / period);
```

---

### Problem: Missed conversions

**Solution:**
1. Reduce sampling rate
2. Use DMA instead of interrupts
3. Optimize callback code
4. Check CPU utilization

```c
// Monitor missed conversions
volatile uint32_t expectedCount = 0;
volatile uint32_t actualCount = 0;

void MonitorCallback(ADC_Instance_t instance, uint16_t result)
{
    actualCount++;
    
    if (actualCount < expectedCount) {
        missedCount = expectedCount - actualCount;
    }
}

// In main loop
expectedCount = (current_time_ms / 1000) * SAMPLE_RATE_HZ;
```

---

## See Also

- `adc.h` - ADC driver API
- `lpit.h` - LPIT driver API  
- `adc_lpit_trigger_example.c` - Complete working example
- S32K144 Reference Manual - Chapter 42 (ADC), Chapter 39 (LPIT)
