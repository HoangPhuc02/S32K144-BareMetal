# CAN Examples - Build Instructions

## File Structure
```
example/
├── can_example1.c         - Original examples (updated với CAN driver mới)
└── can_button_test.c      - Example với nút nhấn ngắt
```

## Description

### can_button_test.c
Example mới với các tính năng:
- **Nút nhấn ngắt**: PTC12 (SW2) với PORT interrupt và debouncing
- **LED indicator**: PTD15 (Red LED) sáng khi transmit
- **CAN communication**: 500 kbps, gửi burst 10 messages khi nhấn nút
- **Message format**: Counter + pattern data
- **Interrupt-driven**: Sử dụng WFI (Wait For Interrupt) để tiết kiệm năng lượng

### can_example1.c (Updated)
Examples được cập nhật để sử dụng CAN driver mới:
- Example 1: Basic Send/Receive (Polling)
- Example 2: Interrupt Mode với callbacks
- Example 3: Extended 29-bit ID
- Example 4: Loopback Mode (Self-test)
- Example 5: Multiple Message Buffers
- Example 6: Error Monitoring
- Example 7: Remote Frame
- Example 8: CAN Gateway

## Hardware Setup

### CAN Button Test
```
PTC12 (SW2)  -----> Button input (internal pull-up, active low)
PTD15        -----> Red LED (active low)
PTE4         -----> CAN0_RX
PTE5         -----> CAN0_TX
```

### CAN Bus Connection
```
Board 1                Board 2
CAN_H  <-------------> CAN_H
CAN_L  <-------------> CAN_L
GND    <-------------> GND

Note: Cần 120Ω termination resistors ở 2 đầu CAN bus
```

## Build Configuration

### Include Paths
Thêm các đường dẫn sau vào project settings:
```
../Core/Drivers/can
../Core/Drivers/gpio
../Core/Drivers/port
../Core/Drivers/nvic
../Core/Drivers/pcc
../Core/Devices
../Core/Devices/S32K144
```

### Source Files
Thêm các driver files vào build:
```
Core/Drivers/can/can.c
Core/Drivers/gpio/gpio.c
Core/Drivers/port/port.c
Core/Drivers/nvic/nvic.c
```

### Linker Settings
- Memory: Flash hoặc RAM (tùy debug configuration)
- Heap size: Tối thiểu 0x400 (1KB)
- Stack size: Tối thiểu 0x400 (1KB)

## Build Commands

### Using S32DS IDE
1. Right-click project → Properties
2. C/C++ Build → Settings → Tool Settings
3. Standard S32DS GCC C Compiler → Includes
   - Add include paths listed above
4. Build project (Ctrl+B)

### Using Command Line (Make)
```bash
# Từ thư mục Debug_FLASH hoặc Debug_RAM
cd Debug_FLASH
make clean
make all
```

## Flash and Run

### Using S32DS Debugger
1. Select debug configuration:
   - `hello_world_s32k144_debug_flash_jlink.launch`
   - hoặc `hello_world_s32k144_debug_flash_pemicro.launch`
2. Click Debug button (F11)
3. Program sẽ được flash và dừng tại main()
4. Press Resume (F8) để chạy

### Using Command Line (OpenOCD/JLink)
```bash
# Using JLink
JLinkExe -device S32K144 -if SWD -speed 4000
> loadfile can_button_test.elf
> r
> g
> q

# Using OpenOCD
openocd -f interface/jlink.cfg -f target/s32k144.cfg
> program can_button_test.elf verify reset
```

## Usage Instructions

### CAN Button Test Example

1. **Flash program** vào S32K144 board
2. **Kết nối CAN bus** với board khác hoặc CAN analyzer
3. **Nhấn SW2** (PTC12) để trigger test
4. **Quan sát**:
   - Red LED (PTD15) sẽ nhấp nháy khi transmit
   - Console UART sẽ hiển thị messages sent
   - CAN analyzer sẽ thấy 10 messages với ID 0x123

5. **Message Format**:
   ```
   ID: 0x123 (Standard 11-bit)
   DLC: 8 bytes
   Data: [Counter_MSB] [Counter] [Counter] [Counter_LSB] [0xAA] [0x55] [0xDE] [0xAD]
   ```

### Expected Output
```
===========================================
  CAN Button Test Example
===========================================
Hardware:
  - Button: PTC12 (SW2)
  - LED: PTD15 (Red)
  - CAN: PTE4/PTE5

Operation:
  1. Press SW2 button
  2. System sends 10 CAN messages
  3. LED blinks during transmission
  4. Messages displayed on console

Waiting for button press...
===========================================

CAN initialized: 500 kbps, ID=0x123

*** Button pressed! Starting CAN test ***
CAN TX [0]: 00 00 00 00 AA 55 DE AD 
CAN TX [1]: 00 00 00 01 AA 55 DE AD 
CAN TX [2]: 00 00 00 02 AA 55 DE AD 
...
CAN TX [9]: 00 00 00 09 AA 55 DE AD 
*** Test complete! ***
Total messages sent: 10
Waiting for next button press...
```

## Troubleshooting

### Problem: CAN messages không được gửi
**Solution:**
- Kiểm tra CAN bus termination (120Ω ở 2 đầu)
- Verify CAN_H và CAN_L không bị đảo ngược
- Check PCC clock enable cho CAN0
- Kiểm tra baudrate settings (phải match giữa 2 boards)

### Problem: Button không hoạt động
**Solution:**
- Verify PTC12 có pull-up resistor
- Check PORTC clock đã được enable
- Kiểm tra NVIC interrupt đã được enable
- Test GPIO read trực tiếp: `GPIO_ReadPin(GPIO_PORT_C, 12)`

### Problem: LED không sáng
**Solution:**
- LED trên EVB là active low (write 0 để sáng)
- Check PORTD clock enabled
- Verify GPIO direction set to output
- Test write trực tiếp: `GPIO_WritePin(GPIO_PORT_D, 15, 0)`

### Problem: Compile errors về undefined identifiers
**Solution:**
- Check tất cả include paths đã được thêm
- Verify các driver .c files đã được add vào build
- Clean and rebuild project
- Check dependencies trong makefile

### Problem: Interrupt không trigger
**Solution:**
- Verify NVIC priority đã được set
- Check NVIC_EnableIRQ() đã được call
- Ensure interrupt flag được clear trong ISR
- Verify SCB->VTOR pointing to correct vector table

## CAN Protocol Details

### Baudrate Calculation
```
Baudrate = CAN_Clock / (PRESDIV + 1) / (PROPSEG + PSEG1 + PSEG2 + 4)

For 500 kbps with 40 MHz clock:
500000 = 40000000 / (PRESDIV + 1) / (PROPSEG + PSEG1 + PSEG2 + 4)
PRESDIV = 4, PROPSEG = 7, PSEG1 = 7, PSEG2 = 2
```

### Message Buffer Allocation
```
MB0-7:   System/FIFO (if RX FIFO enabled)
MB8-15:  TX buffers (8 mailboxes)
MB16-31: RX buffers (16 mailboxes)
```

### Standard ID Format
```
Bit 28-18: Standard ID (11 bits)
Bit 17-0:  Reserved
```

### Data Frame Format
```
+--------+-----+-----+--------+----------+
|  SOF   | ID  | RTR |  DLC   |   DATA   |
| 1 bit  |11bit|1bit | 4 bits | 0-8 bytes|
+--------+-----+-----+--------+----------+
```

## Performance Metrics

### Timing Analysis
- Button debounce: 50ms (software)
- CAN transmit time: ~200µs @ 500kbps (8 bytes)
- Inter-message delay: 100ms
- Total burst time: ~1 second (10 messages)

### Resource Usage
- Flash: ~8KB (including drivers)
- RAM: ~1KB (including buffers)
- CPU: <1% (interrupt-driven)

## References

- S32K144 Reference Manual: Chapter 53 (FlexCAN)
- CAN Specification 2.0A/B
- ARM Cortex-M4 Technical Reference Manual
- Core/Drivers/can/CAN.md - Driver documentation

## Version History

### v1.0 (03/12/2025)
- Initial release
- Button interrupt example
- Updated CAN examples với driver mới
- Added comprehensive documentation

## License

Copyright (c) 2025 PhucPH32
Licensed under MIT License
