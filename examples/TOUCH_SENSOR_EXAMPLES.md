# Touch Sensor Examples

Complete collection of touch sensor examples for S32K144, from simple to advanced.

## 📚 Examples Overview

### 1. **Simple Example** (`touch_sensor_simple_example.c`)
**Best for: Beginners**

Minimalist single-button touch sensor with LED indicator.

**Features:**
- ✅ Single touch button
- ✅ Software polling (no interrupts)
- ✅ LED feedback
- ✅ Easy to understand

**Hardware:**
- Touch sensor: PTB13 (ADC0_SE12)
- LED: PTD15 (Red LED)

**Use case:** Learning touch sensor basics

---

### 2. **Complete Example** (`touch_sensor_complete_example.c`)
**Best for: Production applications**

Full-featured multi-channel touch sensor system with UART debugging.

**Features:**
- ✅ 4 touch buttons
- ✅ PDB hardware trigger @ 100Hz
- ✅ LED control per button
- ✅ UART debug output
- ✅ Auto baseline calibration
- ✅ Drift compensation
- ✅ System statistics

**Hardware:**
- Touch buttons: PTB0, PTB1, PTB13, PTB14
- LEDs: PTD15 (Red), PTD16 (Green), PTD0 (Blue)
- UART: PTC6/PTC7 @ 115200 baud

**Use case:** Production-ready touch interface

---

### 3. **Basic Polling Example** (`touch_sensor_example.c`)
**Best for: Understanding ADC-based detection**

Low-level ADC polling with manual threshold detection.

**Features:**
- ✅ Direct ADC control
- ✅ Manual baseline calibration
- ✅ Custom threshold logic
- ✅ Multiple detection methods

**Use case:** Learning ADC touch detection principles

---

## 🔧 Hardware Setup

### Basic Touch Pad Wiring

```
     Touch Plate
          |
          R₁ (1MΩ)
          |
          +-------- ADC Pin (PTBx)
          |
          R₂ (10kΩ)
          |
         GND
```

**Optional:** Add 100nF capacitor between ADC pin and GND for noise filtering.

### Component Requirements

**Per Touch Pad:**
- 1× Copper plate (2cm × 2cm minimum)
- 1× 1 MΩ resistor (high impedance)
- 1× 10 kΩ resistor (pull-down)
- 1× 100 nF capacitor (optional, for noise filtering)

**LEDs (for visual feedback):**
- Red LED: PTD15
- Green LED: PTD16
- Blue LED: PTD0

### Pin Assignments

| Touch Button | ADC Channel | Pin  | Description |
|-------------|-------------|------|-------------|
| Button 1    | ADC0_SE8    | PTB0 | Touch pad 1 |
| Button 2    | ADC0_SE9    | PTB1 | Touch pad 2 |
| Button 3    | ADC0_SE12   | PTB13| Touch pad 3 |
| Button 4    | ADC0_SE13   | PTB14| Touch pad 4 |

## 🚀 Quick Start

### 1. Simple Example (Recommended First)

```c
// Copy touch_sensor_simple_example.c to src/main.c

// Wire one touch pad to PTB13
// Compile and flash
// Touch the pad → LED turns ON
// Release → LED turns OFF
```

### 2. Complete Example (For Full System)

```c
// Use touch_sensor_complete_example.c

// Wire 4 touch pads to PTB0, PTB1, PTB13, PTB14
// Connect UART (PTC6/PTC7) for debug output
// Compile and flash
// Open serial terminal (115200 baud)
// Touch buttons to control LEDs
```

## 📋 Configuration Guide

### Touch Sensitivity Adjustment

**Threshold Value:**
```c
#define TOUCH_THRESHOLD  (200U)  // Delta from baseline
```

- **Too sensitive** (false triggers): Increase to 250-300
- **Not sensitive enough**: Decrease to 150-100
- **Typical range**: 150-300

### Scan Rate

**Software Polling:**
```c
Delay(10);  // 10ms = 100Hz
```

**Hardware Trigger (PDB):**
```c
.scan_period_us = 10000,  // 10ms = 100Hz
```

- **Faster response**: 5ms (200Hz)
- **Lower power**: 20ms (50Hz)
- **Recommended**: 10ms (100Hz)

### Debounce Settings

```c
.debounce_count = 3,  // 3 consecutive samples
```

- **More stable**: 5 samples (50ms delay @ 100Hz)
- **Faster response**: 2 samples (20ms delay)
- **Recommended**: 3 samples (30ms delay)

## 🔍 Debugging

### Check Baseline Values

**Simple method - LED blinks:**
```c
// During calibration, LED blinks show progress
// If calibration fails, LED blinks rapidly
```

**Complete method - UART output:**
```
Calibrating baselines (do not touch)...
Button1: Baseline = 1850
Button2: Baseline = 1920
Button3: Baseline = 1780
Button4: Baseline = 1805
```

### Check Touch Detection

**UART output shows touch events:**
```
[TOUCH] Button1 pressed (delta=345, count=1)
[RELEASE] Button1 released
```

### System Status

**Press Button 4 in complete example:**
```
=== Touch Sensor Status ===
Total touches: 12

Button1:
  Press count: 4
  Raw value: 1852
  Baseline: 1850
  Delta: 2
  State: IDLE
  LED: OFF
===========================
```

## ⚠️ Troubleshooting

### Problem: LED doesn't respond to touch

**Solutions:**
1. Increase touch plate size (minimum 2cm × 2cm)
2. Decrease threshold value (try 150 or 100)
3. Check wiring - verify 1MΩ resistor connection
4. Ensure finger touches the bare copper plate

### Problem: False triggers (LED flickers)

**Solutions:**
1. Add 100nF capacitor between ADC pin and GND
2. Increase debounce count to 5 samples
3. Increase threshold to 250-300
4. Keep wires short, avoid running near power lines
5. Enable drift compensation

### Problem: Calibration fails

**Solutions:**
1. Don't touch pads during calibration (first 2 seconds)
2. Check ADC peripheral is enabled
3. Verify pin configuration (PORT mux set to ANALOG)
4. Ensure stable power supply

### Problem: Touch sensitivity degrades over time

**Solutions:**
1. Enable drift compensation:
   ```c
   .enable_drift_compensation = true
   ```
2. Periodic re-calibration (every few hours)
3. Check for environmental changes (temperature, humidity)

### Problem: Different buttons have different sensitivity

**Solutions:**
1. Use individual threshold per channel
2. Verify all pads are same size and material
3. Check for equal wire lengths
4. Calibrate each channel separately

## 📊 Performance Metrics

### Simple Example
- **Scan rate:** 100 Hz (software polling)
- **Response time:** 30-50 ms (with debouncing)
- **CPU usage:** ~10% (polling overhead)
- **Channels:** 1

### Complete Example
- **Scan rate:** 100 Hz (PDB hardware trigger)
- **Response time:** 30-50 ms (with debouncing)
- **CPU usage:** <1% (interrupt-driven)
- **Channels:** Up to 8

### Typical Touch Values
- **Baseline (no touch):** 1500-2000 ADC counts
- **Delta (touched):** 200-800 ADC counts
- **Noise level:** ±5-10 ADC counts

## 📖 API Reference

### Initialization
```c
touch_status_t TOUCH_Init(const touch_system_config_t *config);
touch_status_t TOUCH_ConfigChannel(uint8_t channel_id, const touch_channel_config_t *config);
```

### Calibration
```c
touch_status_t TOUCH_CalibrateAll(uint16_t num_samples);
touch_status_t TOUCH_CalibrateChannel(uint8_t channel_id, uint16_t num_samples);
```

### Operation
```c
touch_status_t TOUCH_StartScan(void);
touch_status_t TOUCH_StopScan(void);
touch_status_t TOUCH_Process(void);  // For SW trigger mode
```

### Status Query
```c
touch_status_t TOUCH_GetChannelState(uint8_t channel_id, touch_state_t *state);
touch_status_t TOUCH_GetChannelData(uint8_t channel_id, touch_channel_t **data);
```

## 🎯 Usage Examples

### Example 1: Simple Touch Button

```c
// Initialize with software trigger
touch_system_config_t config = {
    .trigger_mode = TOUCH_TRIGGER_SW,
    .num_channels = 1,
    .callback = MyCallback
};
TOUCH_Init(&config);

// Configure channel
touch_channel_config_t ch = {
    .adc_channel = 12,
    .threshold = 200,
    .debounce_count = 3,
    .enable_drift_compensation = true
};
TOUCH_ConfigChannel(0, &ch);

// Calibrate and start
TOUCH_CalibrateAll(32);
TOUCH_StartScan();

// Main loop
while (1) {
    TOUCH_Process();  // Call every 10ms
    Delay(10);
}
```

### Example 2: Hardware Triggered Multi-Channel

```c
// Initialize with PDB trigger @ 100Hz
touch_system_config_t config = {
    .trigger_mode = TOUCH_TRIGGER_HW_PDB,
    .scan_period_us = 10000,  // 10ms
    .num_channels = 4,
    .callback = MyCallback
};
TOUCH_Init(&config);

// Configure 4 channels
for (uint8_t i = 0; i < 4; i++) {
    touch_channel_config_t ch = {
        .adc_channel = channel_map[i],
        .threshold = 200,
        .debounce_count = 3,
        .enable_drift_compensation = true
    };
    TOUCH_ConfigChannel(i, &ch);
}

// Calibrate and start
TOUCH_CalibrateAll(32);
TOUCH_StartScan();

// Main loop - touch handled in interrupt
while (1) {
    __asm("WFI");  // Low power mode
}
```

### Example 3: Custom Callback Handler

```c
void MyTouchCallback(uint8_t channel, touch_state_t state, int16_t delta)
{
    switch (state) {
        case TOUCH_STATE_TOUCHED:
            printf("Button %d pressed (delta=%d)\n", channel, delta);
            // Perform action
            HandleButtonPress(channel);
            break;
            
        case TOUCH_STATE_RELEASED:
            printf("Button %d released\n", channel);
            // Perform action
            HandleButtonRelease(channel);
            break;
            
        default:
            break;
    }
}
```

## 📁 File Structure

```
examples/
├── touch_sensor_simple_example.c       # Beginner-friendly single button
├── touch_sensor_complete_example.c     # Full-featured multi-channel
├── touch_sensor_example.c              # Low-level ADC polling
└── TOUCH_SENSOR_EXAMPLES.md            # This file

lib/middleware/TouchSensor/
├── touch_sensor.h                      # API header
├── touch_sensor.c                      # Implementation
├── README.md                           # Library documentation
└── TOUCH_SENSOR_MIGRATION.md          # Migration guide
```

## 🔗 Related Documentation

- **Library Documentation:** `lib/middleware/TouchSensor/README.md`
- **Migration Guide:** `lib/middleware/TouchSensor/TOUCH_SENSOR_MIGRATION.md`
- **Implementation Guide:** `docs/guides/touch_sensor_guide.md`
- **ADC Driver:** `lib/hal/adc/ADC.md`
- **PDB Driver:** `lib/hal/pdb/README.md`

## 📝 License

Copyright (c) 2025 PhucPH32

## 🤝 Contributing

Contributions welcome! Please ensure:
- Code follows NXP S32K144 coding standards
- Examples are well-documented
- Hardware setup is clearly described
- Troubleshooting tips are included

## 📧 Support

For questions or issues:
1. Check troubleshooting section above
2. Review library documentation
3. Examine example code comments
4. Test with simple example first

---

**Last Updated:** December 16, 2025  
**Version:** 1.0  
**Author:** PhucPH32
