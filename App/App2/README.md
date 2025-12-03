# App v2: Two-Board CAN/UART Communication System

## 📋 Project Overview

**Application:** Real-time ADC sampling with remote control via CAN bus and PC monitoring via UART

**Boards:**
- **Board 1:** ADC Sampler (controlled remotely via CAN)
- **Board 2:** Control Hub (button interface + CAN/UART bridge)

**Communication:**
- CAN Bus: 500 kbps (Board 1 ↔ Board 2)
- UART: 9600 bps (Board 2 → PC)

---

## 🎯 System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         BOARD 2 (Control Hub)                    │
│  ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐  │
│  │ Button 1 │───→│   CAN    │───→│  Board 1 │    │   UART   │  │
│  │ (Start)  │    │  Handler │    │          │    │ Handler  │──┼→ PC
│  │ Button 2 │    │          │←───│          │    │          │  │
│  │ (Stop)   │    └──────────┘    └──────────┘    └──────────┘  │
│  └──────────┘                                                    │
└─────────────────────────────────────────────────────────────────┘
                              │ CAN Bus
                              │ 500 kbps
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                         BOARD 1 (ADC Sampler)                    │
│  ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐  │
│  │   CAN    │───→│   ADC    │    │   LPIT   │    │   ADC    │  │
│  │  Handler │    │   Task   │←───│  Timer   │    │  Sensor  │  │
│  │          │←───│          │    │  (1 Hz)  │    │          │  │
│  └──────────┘    └──────────┘    └──────────┘    └──────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔧 Hardware Configuration

### Board 1 (ADC Sampler)

| Peripheral | Pin       | Function          | Description              |
|------------|-----------|-------------------|--------------------------|
| ADC0_SE12  | PTB13     | Analog Input      | ADC sensor input         |
| CAN0_TX    | PTE5      | CAN Transmit      | CAN bus TX               |
| CAN0_RX    | PTE4      | CAN Receive       | CAN bus RX               |
| LED        | PTD15     | GPIO Output       | Debug LED (green)        |

**ADC Configuration:**
- Channel: 12 (PTB13)
- Resolution: 12-bit (0-4095)
- Reference: 3.3V
- Sampling: 1 Hz (LPIT timer)

**LPIT Configuration:**
- Channel: 0
- Frequency: 1 Hz (1 second period)
- Clock: SIRC 8 MHz

### Board 2 (Control Hub)

| Peripheral | Pin       | Function          | Description              |
|------------|-----------|-------------------|--------------------------|
| Button 1   | PTC12     | GPIO Input        | Start ADC command        |
| Button 2   | PTC13     | GPIO Input        | Stop ADC command         |
| LPUART1_TX | PTC7      | UART Transmit     | Data to PC               |
| LPUART1_RX | PTC6      | UART Receive      | (Not used)               |
| CAN0_TX    | PTE5      | CAN Transmit      | CAN bus TX               |
| CAN0_RX    | PTE4      | CAN Receive       | CAN bus RX               |
| LED        | PTD15     | GPIO Output       | Debug LED (green)        |

**Button Configuration:**
- Pull-up enabled
- Debounce: 50ms
- Trigger: Falling edge interrupt

**UART Configuration:**
- Baudrate: 9600 bps
- Format: 8N1 (8 data bits, no parity, 1 stop bit)
- Mode: TX only (blocking)

---

## 📡 CAN Protocol Specification

### Message IDs

| Message ID | Direction       | Description                  |
|------------|-----------------|------------------------------|
| 0x100      | Board2 → Board1 | Command: Start ADC           |
| 0x101      | Board2 → Board1 | Command: Stop ADC            |
| 0x200      | Board1 → Board2 | Data: ADC value              |
| 0x300      | Board1 → Board2 | Acknowledge: Start success   |
| 0x301      | Board1 → Board2 | Acknowledge: Stop success    |

### Frame Format

**Command Frame (Board2 → Board1):**
```
ID: 0x100 (Start) or 0x101 (Stop)
Length: 8 bytes
Data: [CMD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
```

**ADC Data Frame (Board1 → Board2):**
```
ID: 0x200
Length: 8 bytes
Data: [D7, D6, D5, D4, D3, D2, D1, D0]  (ASCII digits)

Encoding: ADC value → 8 ASCII digits (big-endian)
Example 1: ADC = 1
  Data: [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01]
  
Example 2: ADC = 456
  Data: [0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x05, 0x06]
  
Example 3: ADC = 4095 (max 12-bit)
  Data: [0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x09, 0x05]
```

**Acknowledge Frame (Board1 → Board2):**
```
ID: 0x300 (Start ACK) or 0x301 (Stop ACK)
Length: 8 bytes
Data: [ACK, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
```

---

## 🔄 Operation Flow

### 1. System Initialization

```
Board 1:
  1. Initialize clocks (80MHz)
  2. Initialize CAN (500kbps)
  3. Initialize ADC + calibration
  4. Initialize LPIT (stopped)
  5. Enter IDLE state

Board 2:
  1. Initialize clocks (80MHz)
  2. Initialize UART (9600bps)
  3. Initialize CAN (500kbps)
  4. Initialize buttons (with interrupt)
  5. Enter IDLE state
  6. Send welcome message to PC
```

### 2. Start ADC Sampling (Button 1 Pressed)

```
Board 2:
  1. Detect button press (debounced)
  2. Send CAN command (ID: 0x100, CMD_START_ADC)
  3. Print "Starting ADC..." to UART
  
Board 1:
  1. Receive CAN command (ID: 0x100)
  2. Start LPIT timer (1Hz)
  3. Enable LPIT interrupt
  4. Send ACK (ID: 0x300, ACK_START_SUCCESS)
  
Board 2:
  1. Receive ACK (ID: 0x300)
  2. Print "ADC sampling started" to UART
```

### 3. ADC Sampling (Every 1 Second)

```
Board 1:
  1. LPIT interrupt triggers (1Hz)
  2. Read ADC value (blocking)
  3. Pack ADC value into CAN frame (ASCII format)
  4. Send CAN data (ID: 0x200)
  5. Toggle debug LED
  
Board 2:
  1. Receive CAN data (ID: 0x200)
  2. Unpack ADC value (ASCII → integer)
  3. Format: "ADC: 1234\r\n"
  4. Send to PC via UART
```

### 4. Stop ADC Sampling (Button 2 Pressed)

```
Board 2:
  1. Detect button press (debounced)
  2. Send CAN command (ID: 0x101, CMD_STOP_ADC)
  3. Print "Stopping ADC..." to UART
  
Board 1:
  1. Receive CAN command (ID: 0x101)
  2. Stop LPIT timer
  3. Disable LPIT interrupt
  4. Send ACK (ID: 0x301, ACK_STOP_SUCCESS)
  
Board 2:
  1. Receive ACK (ID: 0x301)
  2. Print "ADC sampling stopped" to UART
```

---

## 📂 Project Structure

```
App2/
├── common/
│   ├── app_config.h        # System configuration (pins, baudrates, timeouts)
│   ├── app_types.h         # Common types and enums
│   ├── protocol.h          # CAN protocol definitions
│   └── protocol.c          # Protocol pack/unpack functions
│
├── board1/                 # Board 1: ADC Sampler
│   ├── board1_main.c       # Main application
│   ├── board1_can_handler.h
│   ├── board1_can_handler.c  # CAN communication (RX commands, TX data)
│   ├── board1_adc_task.h
│   └── board1_adc_task.c   # ADC sampling with LPIT timer
│
└── board2/                 # Board 2: Control Hub
    ├── board2_main.c       # Main application
    ├── board2_button_handler.h
    ├── board2_button_handler.c  # Button debouncing & events
    ├── board2_can_handler.h
    ├── board2_can_handler.c     # CAN communication (TX commands, RX data)
    ├── board2_uart_handler.h
    └── board2_uart_handler.c    # UART to PC
```

---

## 🚀 Build Instructions

### Prerequisites

- S32 Design Studio IDE
- S32K144 SDK installed
- Two S32K144EVB boards
- CAN transceiver (e.g., TJA1050)
- USB-to-UART adapter (for Board 2 → PC)

### Build Steps

1. **Open Project:**
   ```
   S32DS → File → Import → Existing Projects
   Select: S32KK144_BareMetal
   ```

2. **Configure Build for Board 1:**
   ```
   Project Properties → C/C++ Build → Settings
   Add to Include Paths:
     - App/App2/common
     - App/App2/board1
     - Core/BareMetal/adc
     - Core/BareMetal/can
     - Core/BareMetal/lpit
     - Core/BareMetal/pcc
     - Core/BareMetal/scg
     - Core/BareMetal/gpio
     - Core/BareMetal/port
   
   Add to Source Files:
     - App/App2/board1/board1_main.c
     - App/App2/board1/board1_can_handler.c
     - App/App2/board1/board1_adc_task.c
     - App/App2/common/protocol.c
   ```

3. **Configure Build for Board 2:**
   ```
   Similar to Board 1, but include:
     - App/App2/board2/board2_main.c
     - App/App2/board2/board2_button_handler.c
     - App/App2/board2/board2_can_handler.c
     - App/App2/board2/board2_uart_handler.c
     - App/App2/common/protocol.c
   ```

4. **Build:**
   ```
   Project → Build Project
   ```

5. **Flash:**
   ```
   Run → Debug Configurations → GDB SEGGER J-Link → Flash
   ```

---

## 🧪 Testing Procedure

### 1. Hardware Setup

```
Board 1 Setup:
  - Connect ADC sensor to PTB13 (ADC0_SE12)
  - Connect CAN_H, CAN_L, GND to CAN bus
  - Connect J-Link debugger
  - Power via USB

Board 2 Setup:
  - Connect Button 1 to PTC12 (active low with pull-up)
  - Connect Button 2 to PTC13 (active low with pull-up)
  - Connect CAN_H, CAN_L, GND to CAN bus
  - Connect UART TX (PTC7) to USB-to-UART adapter RX
  - Connect UART GND to adapter GND
  - Connect J-Link debugger
  - Power via USB

CAN Bus:
  - Connect CAN_H of Board1 to CAN_H of Board2
  - Connect CAN_L of Board1 to CAN_L of Board2
  - Connect GND of both boards
  - Add 120Ω termination resistors at both ends
```

### 2. Software Test

```
Step 1: Flash both boards
  Board 1 → board1_main.c
  Board 2 → board2_main.c

Step 2: Open serial terminal (TeraTerm, PuTTY)
  Port: COMx (Board 2 UART)
  Baudrate: 9600
  Data bits: 8
  Parity: None
  Stop bits: 1

Step 3: Power on boards
  Expected output:
    === Board 2 Ready ===
    Press Button 1 to start ADC
    Press Button 2 to stop ADC
    
    System initialized

Step 4: Press Button 1
  Expected output:
    >>> Button 1 pressed: Starting ADC...
    ADC sampling started
    ADC: 1234
    ADC: 1235
    ADC: 1236
    ... (every 1 second)

Step 5: Press Button 2
  Expected output:
    >>> Button 2 pressed: Stopping ADC...
    ADC sampling stopped
```

### 3. Debug Checklist

```
□ Both boards powered
□ CAN bus wired correctly (H-H, L-L, GND-GND)
□ 120Ω termination resistors installed
□ UART TX/RX not swapped
□ Button pull-ups enabled
□ ADC sensor connected
□ Clock frequency verified (80MHz)
□ CAN baudrate verified (500kbps)
□ UART baudrate verified (9600bps)
```

---

## 📊 Expected Results

### Normal Operation

```
UART Output (Board 2 → PC):

=== Board 2 Ready ===
Press Button 1 to start ADC
Press Button 2 to stop ADC

System initialized

>>> Button 1 pressed: Starting ADC...
ADC sampling started
ADC: 0123
ADC: 0124
ADC: 0125
ADC: 0126
... (continues every 1 second)

>>> Button 2 pressed: Stopping ADC...
ADC sampling stopped

>>> Button 1 pressed: Starting ADC...
ADC sampling started
ADC: 0127
ADC: 0128
...
```

### Timing Analysis

```
Button press → CAN command: < 10ms
CAN command → ADC start: < 5ms
ADC sampling period: 1000ms ± 1ms (LPIT accuracy)
ADC read time: < 1ms (12-bit with 16x averaging)
CAN transmission: < 1ms (500kbps, 8 bytes)
UART transmission: ~10ms (9600bps, ~10 bytes)
Total latency: < 20ms (button → PC display)
```

---

## 🐛 Troubleshooting

### Issue 1: No UART Output

**Possible causes:**
- Wrong COM port selected
- Baudrate mismatch (check 9600)
- TX/RX pins swapped
- UART not initialized

**Solution:**
- Verify COM port in Device Manager
- Check baudrate settings
- Measure TX pin with oscilloscope (should show activity)

### Issue 2: CAN Communication Failure

**Possible causes:**
- Missing termination resistors
- CAN_H/CAN_L swapped or shorted
- Baudrate mismatch
- Different clock frequencies

**Solution:**
- Measure CAN bus voltage (should be ~2.5V idle)
- Check CAN_H and CAN_L with oscilloscope
- Verify both boards use same baudrate (500kbps)

### Issue 3: Button Not Responding

**Possible causes:**
- Debounce time too long
- Pull-up not enabled
- Interrupt not configured

**Solution:**
- Check button state with GPIO read
- Verify PORT interrupt flag in debugger
- Reduce debounce time for testing

### Issue 4: ADC Values Incorrect

**Possible causes:**
- ADC not calibrated
- Wrong reference voltage
- Sensor impedance too high

**Solution:**
- Verify ADC_Calibrate() called
- Check VREF setting (3.3V)
- Increase sample time for high impedance

---

## 📝 Code Comments Standard

All code follows these documentation standards:

```c
/**
 * @brief Short description (one line)
 * 
 * Longer description with details about:
 * - What the function does
 * - When to call it
 * - Important notes
 * 
 * @param[in] input Input parameter description
 * @param[out] output Output parameter description
 * 
 * @return Return value description
 * 
 * @note Important notes
 * @warning Warnings
 * 
 * Example:
 * @code
 * uint16_t adc_value;
 * Board2_CanHandler_GetLastAdcValue(&adc_value);
 * @endcode
 */
```

---

## 📚 API Reference

See individual header files for detailed API documentation:
- `common/app_config.h` - Configuration macros
- `common/app_types.h` - Common types and enums
- `common/protocol.h` - CAN protocol functions
- `board1/board1_adc_task.h` - ADC sampling API
- `board1/board1_can_handler.h` - Board 1 CAN API
- `board2/board2_button_handler.h` - Button handling API
- `board2/board2_can_handler.h` - Board 2 CAN API
- `board2/board2_uart_handler.h` - UART transmission API

---

## 🔧 Customization

### Change CAN Baudrate

Edit `app_config.h`:
```c
#define CAN_BAUDRATE_KBPS           (500U)  // Change to 250, 1000, etc.
```

### Change UART Baudrate

Edit `app_config.h`:
```c
#define UART_BAUDRATE               (9600U) // Change to 115200, etc.
```

### Change ADC Sampling Rate

Edit `app_config.h`:
```c
#define ADC_SAMPLING_INTERVAL_MS    (1000U) // Change to 500, 2000, etc.
#define LPIT_FREQUENCY_HZ           (1U)    // Recalculate: 1000/interval
```

### Change Button Pins

Edit `app_config.h`:
```c
#define BUTTON1_PIN                 (12U)   // Change to your pin
#define BUTTON2_PIN                 (13U)   // Change to your pin
```

---

**Created:** December 3, 2025  
**Version:** 1.0  
**Author:** GitHub Copilot  
**Status:** ✅ Ready for Testing
