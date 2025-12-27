# Complete Example Index - S32K144 Bare-Metal Drivers

## Quick Reference Table

| Example File | Functions | Peripherals | Difficulty | Hardware Required |
|--------------|-----------|-------------|------------|-------------------|
| `adc_example.c` | 5+ | ADC0 | Beginner | Potentiometer |
| `can_example.c` | 8 | CAN0 | Intermediate | CAN transceiver, 2nd node |
| `can_test.c` | Various | CAN0 | Advanced | CAN bus setup |
| `clock_example.c` | 4+ | SCG, PCC | Beginner | None |
| `gpio_example.c` | 6+ | GPIO, PORT | Beginner | LEDs, buttons |
| `i2c_example.c` | 6 | I2C0 | Intermediate | I2C devices, pull-ups |
| `lpit_example.c` | 4+ | LPIT | Beginner | None |
| `systick_example.c` | 3+ | SysTick | Beginner | None |
| `touch_sensor_example.c` | 5 | ADC0, GPIO | Intermediate | Touch plate, resistors |
| `uart_example.c` | 9 | LPUART1 | Beginner | USB-Serial adapter |
| `uart_dma_loopback_test.c` | 6 | LPUART1, eDMA | Intermediate | Wire PTC7->PTC6, PTD16 LED |

## ADC Examples

### File: `adc_example.c`

1. **Single Channel Read**
   - Initialize ADC0 channel 12
   - Read voltage from potentiometer
   - Convert to millivolts

2. **Multi-Channel Scan**
   - Configure multiple ADC channels
   - Sequential scanning
   - Average multiple samples

3. **Interrupt Mode**
   - ADC conversion complete interrupt
   - Callback function usage
   - Non-blocking operation

4. **Continuous Conversion**
   - Continuous scan mode
   - DMA integration (if available)
   - High-speed sampling

5. **Calibration**
   - ADC calibration routine
   - Improve accuracy
   - Temperature compensation

**Pin Mapping:**
```
PTB13 -> ADC0_SE12 (Analog Input)
```

---

## CAN Examples

### File: `can_example.c`

1. **Basic Send/Receive (Polling)**
   - CAN0 initialization at 500 Kbps
   - Standard ID (11-bit) message transmission
   - Polling for received messages
   - Message Buffer 8 for TX, MB16 for RX

2. **Interrupt Mode with Callbacks**
   - TX complete callback
   - RX message callback
   - Non-blocking operation
   - Efficient interrupt handling

3. **Extended ID (29-bit)**
   - Configure for extended identifiers
   - 29-bit ID message format
   - Backward compatibility with Standard ID

4. **Loopback Mode**
   - Self-test configuration
   - Internal TX->RX loopback
   - No external CAN node required
   - Verify CAN controller functionality

5. **Multiple Message Buffers**
   - Use MB8-15 for different TX priorities
   - Use MB16-31 for different RX filters
   - Message prioritization
   - Efficient bus utilization

6. **Error Monitoring**
   - Track TX/RX error counters
   - Detect error states (Active/Passive/Bus-Off)
   - Error callback implementation
   - Recovery procedures

7. **Remote Frame**
   - Send remote frame requests
   - Respond to remote frames
   - Data frame vs remote frame handling

8. **CAN Gateway (Dual Interface)**
   - CAN0 to CAN1 message forwarding
   - ID translation
   - Message filtering
   - Protocol bridging

**Pin Mapping:**
```
PTE4 -> CAN0_RX
PTE5 -> CAN0_TX
(Requires CAN transceiver: TJA1050, MCP2551, etc.)
```

**Hardware Setup:**
```
S32K144 -> CAN Transceiver -> CAN Bus
                                 |
                            120Ω termination
                                 |
                         Other CAN node(s)
```

---

## CAN Test Examples

### File: `can_test.c`

Additional CAN protocol tests:
- Message filtering tests
- Timing parameter validation
- Bus load testing
- Error injection tests
- Recovery mechanism tests

---

## Clock Examples

### File: `clock_example.c`

1. **FIRC (Fast Internal RC) Clock**
   - Default 48 MHz internal oscillator
   - No external components required
   - Quick startup time

2. **SOSC (System Oscillator) Configuration**
   - External 8 MHz crystal
   - High accuracy
   - Stable frequency reference

3. **SPLL (System PLL) Setup**
   - Multiply SOSC to 80 MHz
   - Maximum system performance
   - Configure dividers for peripherals

4. **Clock Source Switching**
   - Runtime clock switching
   - Transition between sources
   - Power optimization

5. **Peripheral Clock Gating**
   - Enable/disable peripheral clocks
   - Power consumption reduction
   - PCC register management

**Clock Tree:**
```
SOSC (8 MHz) -> SPLL (×20/2) -> 80 MHz System Clock
                                    |
                                    +-> Core: 80 MHz
                                    +-> Bus: 40 MHz (÷2)
                                    +-> Flash: 20 MHz (÷4)
```

---

## GPIO Examples

### File: `gpio_example.c`

1. **LED Control (Digital Output)**
   - Configure GPIO as output
   - Turn LED on/off
   - Toggle LED state

2. **Button Reading (Digital Input)**
   - Configure GPIO as input with pull-up
   - Read button state
   - Debouncing techniques

3. **Port Interrupt**
   - Configure rising/falling edge interrupt
   - Button interrupt handler
   - ISR best practices

4. **LED Blink Pattern**
   - Multiple LED control
   - Timing with delays
   - Pattern generation

5. **Switch Debounce**
   - Software debouncing algorithm
   - Timer-based debouncing
   - Eliminate false triggers

6. **LED Matrix Control**
   - Row/column scanning
   - Multiplexing technique
   - Display patterns

**Pin Mapping:**
```
PTD0  -> Red LED
PTD15 -> Green LED  
PTD16 -> Blue LED
PTC12 -> Button SW2
PTC13 -> Button SW3
```

---

## I2C Examples

### File: `i2c_example.c`

1. **Basic I2C Initialization**
   - Configure I2C0 at 100 kHz
   - Setup SCL/SDA pins
   - Enable peripheral clock

2. **I2C Bus Scan**
   - Scan addresses 0x08-0x77
   - Detect connected devices
   - Display found addresses

3. **MPU6050 (Accelerometer/Gyroscope)**
   - Initialize sensor (WHO_AM_I check)
   - Read 3-axis acceleration
   - Read 3-axis gyroscope
   - Temperature reading

4. **AT24C256 EEPROM**
   - Single byte write/read
   - Page write (up to 64 bytes)
   - Sequential read
   - Data verification

5. **PCF8574 (I/O Expander)**
   - 8-bit parallel I/O control
   - LED chaser pattern
   - Read input pins
   - Interrupt handling

6. **Error Handling**
   - NACK detection and handling
   - Timeout management
   - Bus error recovery
   - Repeated start conditions

**Pin Mapping:**
```
PTA2 -> I2C0_SCL (with 4.7kΩ pull-up)
PTA3 -> I2C0_SDA (with 4.7kΩ pull-up)
```

**I2C Device Addresses:**
```
MPU6050:  0x68 (AD0=0) or 0x69 (AD0=1)
AT24C256: 0x50-0x57 (A0-A2 pins)
PCF8574:  0x20-0x27 (A0-A2 pins)
```

---

## LPIT Examples

### File: `lpit_example.c`

1. **Basic Periodic Timer**
   - Configure LPIT channel 0
   - 1 second interrupt
   - LED toggle in ISR

2. **Multi-Channel Timers**
   - Use channels 0-3
   - Different periods for each
   - Independent interrupts

3. **Timer Chaining**
   - Chain channels for longer periods
   - 32-bit to 64-bit extension
   - Precise long delays

4. **PWM Generation**
   - Use timer for software PWM
   - Variable duty cycle
   - LED brightness control

**Features:**
- 4 independent channels
- 32-bit timer resolution
- Clock sources: SIRC, FIRC, SPLL
- Up to 1 MHz counting frequency

---

## SysTick Examples

### File: `systick_example.c`

1. **Millisecond Delay**
   - Configure SysTick for 1ms tick
   - Implement delay_ms() function
   - Non-blocking delay option

2. **System Uptime Counter**
   - Track milliseconds since boot
   - 32-bit or 64-bit counter
   - Rollover handling

3. **Task Scheduler**
   - Simple cooperative scheduler
   - Time-based task execution
   - Priority handling

**Configuration:**
```c
// For 80 MHz system clock
SysTick_Config(80000);  // 1ms tick
```

---

## Touch Sensor Examples

### File: `touch_sensor_example.c`

1. **Basic Touch Detection with Polling**
   - Initialize ADC for capacitive touch sensing
   - Continuous polling of touch sensor
   - Simple threshold-based detection
   - LED feedback for touch state
   - Baseline calibration at startup

2. **Touch Detection with Debouncing**
   - Software debounce filtering
   - Requires multiple consecutive samples
   - Prevents false triggers from noise
   - Hysteresis for stable detection
   - Better noise immunity

3. **Adaptive Threshold Calibration**
   - Automatic baseline adjustment
   - Compensates for environmental changes
   - Temperature/humidity drift compensation
   - Periodic recalibration during idle
   - Long-term stability

4. **Multi-Touch Gesture Detection**
   - Single tap detection (<500ms touch)
   - Hold detection (>1000ms touch)
   - Double-tap detection (two taps within 500ms)
   - Timing-based gesture classification
   - LED pattern feedback for each gesture

5. **Touch Sensor with UART Debug**
   - Real-time data output over UART
   - CSV format for easy analysis
   - ADC value, baseline, state logging
   - Useful for threshold calibration
   - Performance monitoring

**Pin Mapping:**
```
PTB13 -> ADC0_SE12 (Touch sensor analog input)
PTD0  -> Red LED (Touch detected indicator)
PTD15 -> Green LED (Ready/status indicator)
```

**Hardware Setup:**
```
Touch Plate
     |
     +---- 1MΩ ----+---- PTB13 (ADC0_SE12)
                   |
                  10kΩ (pull-down)
                   |
                  GND

Optional: 100nF capacitor between PTB13 and GND for filtering
```

**Operation Principle:**
- Human body adds ~100-200pF capacitance when touching
- RC circuit charging time changes with capacitance
- ADC measures voltage level during charging
- Threshold comparison determines touch state
- Hardware averaging reduces noise

**Typical ADC Values (12-bit):**
- No touch (baseline): 1800-2000
- Touch detected: 2500-3000
- Threshold press: 2500
- Threshold release: 2000 (hysteresis)

**Touch Sensor Design Tips:**
1. Use copper PCB pad (minimum 1cm x 1cm) or aluminum foil
2. Add ground plane around touch pad for better sensitivity
3. Keep traces short to minimize interference
4. Use hardware averaging (16 samples) for noise reduction
5. Add 100nF capacitor for additional filtering
6. Calibrate baseline in actual operating environment
7. Consider proximity detection by lowering threshold
8. Shield sensitive traces if in noisy environment

---

## UART Examples

### File: `uart_example.c`

1. **Basic UART Initialization**
   - Configure LPUART1 at 115200 baud
   - 8N1 format (8 data, no parity, 1 stop)
   - Enable TX/RX

2. **Send String**
   - Blocking string transmission
   - Character-by-character sending
   - Wait for TX complete

3. **Printf Implementation**
   - Redirect printf() to UART
   - Formatted output support
   - Integer, float, string formatting

4. **Receive Data**
   - Single character receive with timeout
   - Receive line (until \n or \r)
   - Buffer management

5. **Echo Application**
   - Simple echo server
   - Character-by-character echo
   - Exit on ESC key (0x1B)

6. **Command Line Interface**
   - Full-featured CLI
   - Commands: help, status, led, echo, reset
   - Backspace/delete support
   - Command history (optional)

7. **Data Logging**
   - CSV format output
   - Timestamp, sensor data
   - Real-time data streaming

8. **Error Handling**
   - Parity error detection
   - Frame error detection
   - Noise error detection
   - Error flag clearing

9. **Loopback Test**
   - Connect TX to RX externally
   - Send test patterns
   - Verify received data
   - Self-test functionality

**Pin Mapping:**
```
PTC6 -> LPUART1_RX
PTC7 -> LPUART1_TX
```

**Terminal Configuration:**
```
Baud rate:    115200
Data bits:    8
Parity:       None
Stop bits:    1
Flow control: None
```

### File: `uart_dma_loopback_test.c`

1. **Clock and Peripheral Init**
   - Configures FIRC/SPLL clocks and PCC gates for DMA0, DMAMUX, LPUART1, PORTC/PTD16
   - Initializes GPIO for the blue status LED (PTD16)

2. **DMA Channel Pairing**
   - DMAMUX channel 0 -> UART1 TX, channel 1 -> UART1 RX
   - Enables request source 2:1 ratio, continuous link disabled for clarity

3. **Loopback Execution**
   - Transmits `LOOPBACK_TEST_BYTES` bytes via DMA using a static test pattern
   - Concurrent RX DMA fills a buffer and triggers completion callbacks

4. **Data Verification & Feedback**
   - Compares received payload against expected data in the DMA callbacks
   - Turns PTD16 LED on for pass, off for fail, and prints human-readable status to UART

5. **Error Handling**
   - Polls status flags for transfer halt/timeouts
   - Provides simple retry path by re-arming DMA descriptors if verification fails

**Pin Mapping:**
```
PTC7 -> LPUART1_TX (jumper to PTC6)
PTC6 -> LPUART1_RX (jumper to PTC7)
PTD16 -> Blue LED (status)
```

**Hardware Setup:**
```
1. Install jumper wire between PTC7 (TX) and PTC6 (RX)
2. Connect board to PC via OpenSDA for console output
3. Observe PTD16 LED (ON = pass, OFF = fail)
```

---

## Building and Running Examples

### Step 1: Include Example File

In your `main.c`:
```c
// Include the example file
#include "example/gpio_example.c"

int main(void) {
    // Initialize system
    SystemInit();
    
    // Run specific example
    GPIO_Example1_LEDControl();
    
    while (1) {
        // Main loop
    }
    
    return 0;
}
```

### Step 2: Build Project

Using S32 Design Studio:
1. Right-click on project
2. Select "Build Project"
3. Wait for compilation to complete

### Step 3: Flash to Board

1. Connect S32K144 board via USB
2. Right-click on project
3. Select "Debug As" -> "S32 Debugger"
4. Program will be flashed and started

### Step 4: Monitor Output

For UART examples:
1. Open terminal (PuTTY, TeraTerm, etc.)
2. Select correct COM port
3. Configure: 115200, 8N1
4. Observe output

---

## Troubleshooting Guide

### Example Doesn't Run

**Symptom**: No output or unexpected behavior

**Solutions**:
1. Verify system clock is configured (80 MHz expected)
2. Check peripheral clock is enabled
3. Ensure pins are configured correctly
4. Verify hardware connections

### UART No Output

**Symptom**: Terminal shows no data

**Solutions**:
1. Check baud rate matches (115200)
2. Verify TX/RX pins are correct
3. Ensure terminal COM port is correct
4. Check USB-Serial adapter is working

### I2C Timeout

**Symptom**: I2C operations hang or timeout

**Solutions**:
1. Verify pull-up resistors (4.7kΩ) are present
2. Check device address is correct
3. Ensure device is powered
4. Verify SCL/SDA connections

### CAN Errors

**Symptom**: CAN messages not sent/received

**Solutions**:
1. Check termination resistors (120Ω at both ends)
2. Verify baudrate matches all nodes
3. Ensure CAN transceiver is powered
4. Check TX/RX pin connections

### ADC Wrong Values

**Symptom**: ADC readings are incorrect

**Solutions**:
1. Check voltage is within 0-5V range
2. Verify VREFH connection
3. Perform ADC calibration
4. Check for noise on analog input

### Touch Sensor Not Working

**Symptom**: Touch sensor doesn't detect touches or gives erratic readings

**Solutions**:
1. Verify 1MΩ charging resistor is connected
2. Check 10kΩ pull-down resistor is present
3. Ensure touch plate size is adequate (minimum 1cm x 1cm)
4. Perform baseline calibration with no touch
5. Adjust TOUCH_THRESHOLD_PRESS value based on your readings
6. Add 100nF filter capacitor if noise is present
7. Check PTB13 pin configuration (must be analog mode)
8. Verify ADC hardware averaging is enabled
9. Keep finger grounded (touch board GND first) for better sensitivity
10. Try larger touch plate area if sensitivity is low

**Typical Issues:**
- Too sensitive: Increase threshold value
- Not sensitive enough: Decrease threshold, increase touch plate size
- Erratic behavior: Add filtering capacitor, improve grounding
- Baseline drift: Enable adaptive calibration (Example 3)

---

## Best Practices

1. **Always Initialize Clocks First**
   ```c
   SystemInit();  // Or custom clock config
   ```

2. **Enable Peripheral Clocks Before Use**
   ```c
   PCC->PCCn[PCC_PORTA_INDEX] |= PCC_CGC_MASK;
   ```

3. **Configure Pins After Clock Enable**
   ```c
   PORT_SetMux(PORTA, 2, PORT_MUX_ALT3);  // I2C SCL
   ```

4. **Use Interrupts for Real-Time Operation**
   - Callbacks for asynchronous events
   - Keep ISR short and fast
   - Set appropriate priorities

5. **Implement Error Handling**
   - Check return status
   - Implement recovery procedures
   - Log errors for debugging

---

## Example Complexity Levels

### Beginner (⭐)
- `gpio_example.c` - Digital I/O basics
- `systick_example.c` - Simple timing
- `uart_example.c` (Examples 1-3) - Basic serial communication
- `clock_example.c` - Clock configuration
- `adc_example.c` (Example 1) - Simple ADC read

### Intermediate (⭐⭐)
- `lpit_example.c` - Timer interrupts
- `i2c_example.c` - I2C protocol and devices
- `can_example.c` (Examples 1-4) - Basic CAN
- `touch_sensor_example.c` (Examples 1-3) - Capacitive touch sensing
- `uart_example.c` (Examples 4-7) - Advanced UART
### Advanced (⭐⭐⭐)
- `can_example.c` (Examples 5-8) - Advanced CAN features
- `can_test.c` - Protocol testing
- `touch_sensor_example.c` (Examples 4-5) - Gesture detection and debug
- Multi-peripheral integration projects

---

## Additional Resources

- **Driver Documentation**: See `Core/Drivers/*/` for detailed API docs
- **Hardware Manual**: S32K1xx Reference Manual
- **Coding Standards**: `coding_convention_short.md`
- **Application README**: `App/README.md` for complete application examples
- **Touch Sensor Design**: See touch_sensor_example.c for detailed hardware setup

---

**Last Updated**: December 1, 2025  
**Maintainer**: PhucPH32  
**Version**: 1.1
**Version**: 1.0
