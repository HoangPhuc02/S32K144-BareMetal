# Examples for S32K144 Drivers

## Overview

This folder contains practical examples demonstrating how to use S32K144 bare-metal drivers.
Each example file is self-contained with multiple use cases.

## Example Files List

### ADC Example (`adc_example.c`)
- ADC initialization and configuration
- Single channel reading
- Multiple channel scanning
- Interrupt-driven conversion
- Continuous conversion mode

### CAN Examples (`can_example.c`)
- Basic CAN send/receive (polling)
- Interrupt mode with callbacks
- Extended ID (29-bit) support
- Loopback mode for testing
- Multiple Message Buffers usage
- Error monitoring and handling
- Remote frame transmission
- CAN gateway (dual interface)

### CAN Test (`can_test.c`)
- Additional CAN protocol tests
- Message filtering examples
- Timing configuration tests

### Clock Example (`clock_example.c`)
- System clock configuration
- PLL setup for 80MHz operation
- Clock source switching
- Peripheral clock management

### GPIO Example (`gpio_example.c`)
- GPIO initialization and configuration
- Digital input/output operations
- Port interrupt configuration
- LED control and button reading

### LPIT Example (`lpit_example.c`)
- Timer initialization
- Periodic timer interrupt
- Multi-channel timer usage
- Timer chaining

### SysTick Example (`systick_example.c`)
- System tick timer configuration
- Millisecond delay implementation
- System uptime counter

### Touch Sensor Example (`touch_sensor_example.c`)
- Capacitive touch sensor detection using ADC
- Basic polling mode touch detection
- Debouncing for noise immunity
- Adaptive threshold calibration
- Gesture detection (tap, hold, double-tap)
- UART debug output for calibration
- SysTick timer configuration
- Millisecond delay implementation
- Real-time counter
- System tick interrupt

### UART Example (`uart_example.c`)
- UART initialization
- Blocking send/receive
- Interrupt-driven communication
- Printf redirection

### I2C Example (`i2c_example.c`)
- I2C initialization and bus scan
- MPU6050 accelerometer/gyroscope reading
- AT24C256 EEPROM operations
- PCF8574 I/O expander control
- Error handling and recovery

## Hardware Requirements

### General
- S32K144EVB development board
- USB cable for programming and debugging
- Power supply (5V or 12V depending on board)

### Peripheral-Specific Requirements

**I2C Examples:**
- Pull-up resistors (4.7kΩ) on SCL/SDA lines
- I2C devices: MPU6050, AT24C256, PCF8574 (optional)
- Pin connections:
  ```
  PTA2 -> I2C0_SCL (with 4.7kΩ pull-up to VDD)
  PTA3 -> I2C0_SDA (with 4.7kΩ pull-up to VDD)
  ```

**UART Examples:**
- USB-to-Serial adapter or integrated ST-Link UART
- Terminal program (PuTTY, TeraTerm, Arduino Serial Monitor)
- Configuration: 115200 baud, 8N1
- Pin connections:
  ```
  PTC6 -> UART1_RX
  PTC7 -> UART1_TX
  ```

**CAN Examples:**
- CAN transceiver (TJA1050, MCP2551, or similar)
- CAN bus termination resistors (120Ω at each end)
- Another CAN node for testing (second S32K144 board or CAN analyzer)
- Pin connections:
  ```
  PTE4 -> CAN0_RX
  PTE5 -> CAN0_TX
  ```

**ADC Examples:**
- Potentiometer (10kΩ) for variable voltage input
- Voltage divider if needed for specific voltage ranges
- Pin connections:
  ```
  ADC0_SE12 (PTB13) -> Analog input (0-5V range)
  ```

**Touch Sensor Examples:**
- Capacitive touch plate (copper pad or aluminum foil)
- 1MΩ resistor for charging circuit
- 10kΩ pull-down resistor
- 100nF filter capacitor (recommended)
- LEDs for visual feedback
- Pin connections:
  ```
  PTB13 -> ADC0_SE12 (Touch sensor analog input)
  PTD0  -> Red LED (Touch detected indicator)
  PTD15 -> Green LED (Ready/status indicator)
  ```
- Touch sensor circuit:
  ```
  Touch Plate ---- 1MΩ ----+---- PTB13 (ADC0_SE12)
                            |
                           10kΩ (pull-down)
                            |
                           GND
  
  Optional: 100nF capacitor between PTB13 and GND for noise filtering
  ```

**GPIO/LPIT/SysTick Examples:**
- LEDs with current-limiting resistors (330Ω)
- Push buttons with pull-up/pull-down resistors
- Pin connections:
  ```
  PTD0 -> LED (Red)
  PTD15 -> LED (Green)
  PTD16 -> LED (Blue)
  PTC12 -> Button SW2
  PTC13 -> Button SW3
  ```

## How to Use Examples

1. **Select Example**: Choose the example you want to run from the list above

2. **Hardware Setup**: Connect required hardware according to the specifications

3. **Include in Project**: 
   ```c
   // In your main.c
   #include "example/gpio_example.c"
   
   int main(void) {
       GPIO_Example1_BasicIO();  // Run specific example
       return 0;
   }
   ```

4. **Build and Flash**: Compile and upload to S32K144 board

5. **Monitor Output**: Use terminal or debugger to observe results

## Example Naming Convention

All examples follow this naming pattern:
```
<peripheral>_Example<N>_<Description>()
```

Example:
- `UART_Example1_BasicInit()`
- `CAN_Example3_ExtendedID()`
- `I2C_Example4_EEPROM()`

## Tips and Best Practices

1. **Clock Configuration**: Most examples assume 80MHz system clock. Adjust if different.

2. **Pin Multiplexing**: Ensure pins are not conflicting with other peripherals

3. **Interrupt Priorities**: Examples use default priorities. Adjust for your application.

4. **Debugging**: Use breakpoints and watch variables to understand flow

5. **Resource Cleanup**: Call deinit functions when switching between examples

## Common Issues and Solutions

| Issue | Solution |
|-------|----------|
| No output on UART | Check baud rate, pin connections, terminal settings |
| I2C timeout | Verify pull-up resistors, check device address |
| CAN errors | Check termination resistors, verify baudrate matches |
| ADC wrong values | Check voltage range, calibrate if needed |
| GPIO not working | Verify pin mux configuration, check PORT clock |

## Additional Resources

- [Coding Convention](../coding_convention_short.md)
- [Driver Documentation](../Core/Drivers/)
- [S32K144 Reference Manual](https://www.nxp.com/docs/en/reference-manual/S32K1XXRM.pdf)
- [Application Notes](../Doc/)

## Contributing

When adding new examples:
1. Follow the existing naming convention
2. Add comprehensive comments
3. Update this README with example description
4. Test thoroughly on hardware
5. Document hardware requirements

---

**Note**: Some examples are provided in Vietnamese for local documentation.
The code and critical comments are in English for broader accessibility.

## Detailed Example Descriptions

### I2C Examples (`i2c_example.c`)

**Example 1**: Basic I2C Initialization
- Initialize I2C with default configuration
- Configure pins and clock

**Example 2**: I2C Bus Scan
- Scan I2C bus to find connected devices
- Display addresses of all devices

**Example 3**: MPU6050 Accelerometer/Gyroscope
- Initialize MPU6050 sensor
- Read acceleration and gyroscope data
- Display real-time data

**Example 4**: AT24C256 EEPROM
- Single byte write/read
- Page write/read operations
- Data integrity verification

**Example 5**: PCF8574 I/O Expander
- Control 8-bit I/O
- LED chaser pattern
- Read input status

**Example 6**: Error Handling
- Handle NACK conditions
- Check bus status
- Error recovery procedures

### UART Examples (`uart_example.c`)

**Example 1**: Basic UART Initialization
- Initialize UART with default configuration
- Configure pins

**Example 2**: Send String
- Send character strings
- Blocking transmission

**Example 3**: Printf Implementation
- UART printf function
- Format strings with multiple data types

**Example 4**: Receive Data
- Receive single byte with timeout
- Receive line (until newline)

**Example 5**: Echo Application
- Simple character echo
- Exit with ESC key

**Example 6**: Command Line Interface
- Full-featured CLI
- Multiple commands (help, status, led, etc.)
- Backspace support

**Example 7**: Data Logging
- CSV format logging
- Simulated sensor data

**Example 8**: Error Handling
- Detect parity, frame, noise errors
- Clear error flags

**Example 9**: Loopback Test
- Self-test with TX->RX connection
- Verify data integrity

## Terminal Setup for UART Examples

Configure your terminal program with:
- **Baud rate**: 115200
- **Data bits**: 8
- **Parity**: None
- **Stop bits**: 1
- **Flow control**: None

## Troubleshooting

### I2C Issues

**Problem**: Device không phản hồi (NACK)
- Check pull-up resistors (4.7kΩ)
- Verify địa chỉ slave đúng
- Check connections
- Verify VDD và GND

**Problem**: Bus stuck
- Reset I2C peripheral
- Check SCL/SDA không bị short
- Verify pull-ups

### UART Issues

**Problem**: Garbage characters
- Check baud rate match
- Verify clock configuration
- Check pin connections

**Problem**: Missing characters
- Enable interrupts hoặc DMA
- Increase buffer size
- Check overrun errors

## Mở Rộng

Bạn có thể mở rộng examples với:

1. **DMA support** - Cho high-speed transfers
2. **Interrupt mode** - Non-blocking operations
3. **FreeRTOS tasks** - Multi-threaded applications
4. **More sensors** - BME280, BMP180, DS3231, etc.
5. **Protocols** - Modbus, AT commands, custom protocols

## Tài Liệu Tham Khảo

- [I2C Driver Documentation](../Doc/i2c.md)
- [UART Driver Documentation](../Doc/uart.md)
- S32K144 Reference Manual
- Application Notes

---

**Author**: PhucPH32  
**Date**: November 24, 2025  
**Version**: 1.0
