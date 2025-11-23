# I2C Driver Documentation

## Tổng Quan

Driver LPI2C (Low Power I2C) cho S32K144 cung cấp interface để giao tiếp I2C với các thiết bị ngoại vi. Driver hỗ trợ cả chế độ Master và Slave với các tính năng đầy đủ.

## Cấu Trúc Thư Mục

```
Core/Drivers/i2c/
├── i2c_reg.h       # Register-level definitions
├── i2c.h           # High-level API header
└── i2c.c           # Implementation
```

## Kiến Trúc Driver

### 1. Register Level (`i2c_reg.h`)

File này chứa các định nghĩa thanh ghi cấp thấp:

```c
typedef struct {
    __I  uint32_t VERID;        // Version ID Register
    __I  uint32_t PARAM;        // Parameter Register
    __IO uint32_t MCR;          // Master Control Register
    __IO uint32_t MSR;          // Master Status Register
    __IO uint32_t MIER;         // Master Interrupt Enable
    // ... other registers
    __IO uint32_t MTDR;         // Master Transmit Data
    __I  uint32_t MRDR;         // Master Receive Data
    // ... slave registers
} LPI2C_RegType;
```

**Base Addresses:**
- LPI2C0: `0x40066000`

**PCC Clock Index:**
- PCC_LPI2C0_INDEX: `102`

### 2. Driver API Level (`i2c.h`, `i2c.c`)

Cung cấp các hàm high-level để sử dụng I2C.

## Cấu Hình và Khởi Tạo

### 1. Cấu Trúc Cấu Hình Master

```c
typedef struct {
    uint32_t baudRate;          // Tốc độ (100000, 400000 Hz)
    uint8_t  prescaler;         // Prescaler (0-7)
    bool     enableMaster;      // Enable master mode
    bool     enableDebug;       // Enable debug mode
} I2C_MasterConfig_t;
```

### 2. Khởi Tạo I2C Master

```c
#include "i2c.h"
#include "port.h"

// Bước 1: Enable clock cho I2C và PORT
I2C_EnableClock(0);              // Enable LPI2C0 clock
PORT_EnableClock(PORT_A);        // Enable PORTA for pins

// Bước 2: Cấu hình pins (PTA2=SCL, PTA3=SDA)
PORT_SetPinMux(PORTA, 2, PORT_MUX_ALT3);  // I2C0_SCL
PORT_SetPinMux(PORTA, 3, PORT_MUX_ALT3);  // I2C0_SDA

// Bước 3: Cấu hình I2C Master
I2C_MasterConfig_t config = {
    .baudRate = 100000,          // 100 kHz (Standard mode)
    .prescaler = 0,              // No prescaler
    .enableMaster = true,
    .enableDebug = false
};

// Bước 4: Khởi tạo
uint32_t srcClock = 8000000;     // 8 MHz source clock
I2C_Status_t status = I2C_MasterInit(LPI2C0, &config, srcClock);

if (status != I2C_STATUS_SUCCESS) {
    // Xử lý lỗi
}
```

### 3. Baud Rate Phổ Biến

| Mode | Frequency | Typical Usage |
|------|-----------|---------------|
| Standard | 100 kHz | Sensors, EEPROMs |
| Fast | 400 kHz | High-speed peripherals |
| Fast Plus | 1 MHz | Advanced applications |

## Sử Dụng Master Mode

### 1. Ghi Dữ Liệu Đơn Giản

```c
/**
 * Ghi 1 byte vào slave
 */
void I2C_WriteByteToSlave(uint8_t slaveAddr, uint8_t data)
{
    I2C_Status_t status;
    
    // Start + Send address with WRITE
    status = I2C_MasterStart(LPI2C0, slaveAddr, I2C_WRITE);
    if (status != I2C_STATUS_SUCCESS) {
        return;
    }
    
    // Send data
    status = I2C_MasterSend(LPI2C0, &data, 1, true);  // true = send STOP
    
    if (status == I2C_STATUS_NACK) {
        // Slave không trả lời
    }
}
```

### 2. Ghi Nhiều Byte

```c
/**
 * Ghi buffer vào slave
 */
void I2C_WriteBuffer(uint8_t slaveAddr, const uint8_t *buffer, uint32_t length)
{
    // Start transfer
    I2C_MasterStart(LPI2C0, slaveAddr, I2C_WRITE);
    
    // Send all data with STOP at end
    I2C_MasterSend(LPI2C0, buffer, length, true);
}
```

### 3. Đọc Dữ Liệu Từ Slave

```c
/**
 * Đọc nhiều byte từ slave
 */
void I2C_ReadBuffer(uint8_t slaveAddr, uint8_t *buffer, uint32_t length)
{
    // Start + Send address with READ
    I2C_MasterStart(LPI2C0, slaveAddr, I2C_READ);
    
    // Receive data with STOP at end
    I2C_MasterReceive(LPI2C0, buffer, length, true);
}
```

### 4. Ghi Rồi Đọc (Write-Read)

```c
/**
 * Ghi register address, sau đó đọc dữ liệu
 * (Thường dùng cho sensors, EEPROM)
 */
void I2C_WriteReadTransaction(uint8_t slaveAddr, uint8_t regAddr, 
                               uint8_t *rxBuffer, uint32_t rxLen)
{
    // Phase 1: Write register address (no STOP)
    I2C_MasterStart(LPI2C0, slaveAddr, I2C_WRITE);
    I2C_MasterSend(LPI2C0, &regAddr, 1, false);  // false = no STOP
    
    // Phase 2: Repeated START + Read data
    I2C_MasterStart(LPI2C0, slaveAddr, I2C_READ);
    I2C_MasterReceive(LPI2C0, rxBuffer, rxLen, true);  // true = STOP
}
```

## Ví Dụ Thực Tế

### 1. Đọc Cảm Biến MPU6050 (Accelerometer/Gyro)

```c
#define MPU6050_ADDR        0x68
#define MPU6050_REG_ACCEL_X 0x3B

void MPU6050_ReadAccel(int16_t *accelX, int16_t *accelY, int16_t *accelZ)
{
    uint8_t buffer[6];
    uint8_t regAddr = MPU6050_REG_ACCEL_X;
    
    // Write register address
    I2C_MasterStart(LPI2C0, MPU6050_ADDR, I2C_WRITE);
    I2C_MasterSend(LPI2C0, &regAddr, 1, false);
    
    // Read 6 bytes (X, Y, Z - 2 bytes each)
    I2C_MasterStart(LPI2C0, MPU6050_ADDR, I2C_READ);
    I2C_MasterReceive(LPI2C0, buffer, 6, true);
    
    // Combine bytes (big-endian)
    *accelX = (int16_t)((buffer[0] << 8) | buffer[1]);
    *accelY = (int16_t)((buffer[2] << 8) | buffer[3]);
    *accelZ = (int16_t)((buffer[4] << 8) | buffer[5]);
}
```

### 2. Ghi/Đọc EEPROM AT24C256

```c
#define EEPROM_ADDR     0x50

/**
 * Ghi 1 byte vào EEPROM
 */
void EEPROM_WriteByte(uint16_t memAddr, uint8_t data)
{
    uint8_t buffer[3];
    
    // Address (2 bytes) + Data (1 byte)
    buffer[0] = (uint8_t)(memAddr >> 8);    // Address high
    buffer[1] = (uint8_t)(memAddr & 0xFF);  // Address low
    buffer[2] = data;
    
    I2C_MasterStart(LPI2C0, EEPROM_ADDR, I2C_WRITE);
    I2C_MasterSend(LPI2C0, buffer, 3, true);
    
    // Wait for EEPROM write cycle (5ms typical)
    for (volatile uint32_t i = 0; i < 50000; i++);
}

/**
 * Đọc 1 byte từ EEPROM
 */
uint8_t EEPROM_ReadByte(uint16_t memAddr)
{
    uint8_t addrBuf[2];
    uint8_t data;
    
    // Send address
    addrBuf[0] = (uint8_t)(memAddr >> 8);
    addrBuf[1] = (uint8_t)(memAddr & 0xFF);
    
    I2C_MasterStart(LPI2C0, EEPROM_ADDR, I2C_WRITE);
    I2C_MasterSend(LPI2C0, addrBuf, 2, false);
    
    // Read data
    I2C_MasterStart(LPI2C0, EEPROM_ADDR, I2C_READ);
    I2C_MasterReceive(LPI2C0, &data, 1, true);
    
    return data;
}
```

### 3. Quét Bus I2C (Scan)

```c
/**
 * Quét tất cả địa chỉ I2C để tìm devices
 */
void I2C_ScanBus(void)
{
    printf("Scanning I2C bus...\n");
    
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        // Try to start communication
        I2C_Status_t status = I2C_MasterStart(LPI2C0, addr, I2C_WRITE);
        
        if (status == I2C_STATUS_SUCCESS) {
            printf("Device found at 0x%02X\n", addr);
            I2C_MasterStop(LPI2C0);
        }
        
        // Small delay between probes
        for (volatile uint32_t i = 0; i < 1000; i++);
    }
    
    printf("Scan complete.\n");
}
```

## Slave Mode

### Khởi Tạo Slave

```c
I2C_SlaveConfig_t slaveConfig = {
    .slaveAddress = 0x50,           // 7-bit address
    .enable10BitAddress = false,
    .enableGeneralCall = false,
    .enableSlave = true
};

I2C_SlaveInit(LPI2C0, &slaveConfig);
```

### Xử Lý Slave Communication

```c
/**
 * Slave transmit example
 */
void I2C_SlaveTransmitHandler(void)
{
    uint8_t txData[] = {0xAA, 0xBB, 0xCC, 0xDD};
    
    I2C_SlaveSend(LPI2C0, txData, sizeof(txData));
}

/**
 * Slave receive example
 */
void I2C_SlaveReceiveHandler(void)
{
    uint8_t rxBuffer[16];
    
    I2C_Status_t status = I2C_SlaveReceive(LPI2C0, rxBuffer, 16);
    
    if (status == I2C_STATUS_SUCCESS) {
        // Process received data
    }
}
```

## Xử Lý Lỗi

### Các Mã Lỗi

```c
typedef enum {
    I2C_STATUS_SUCCESS = 0,         // Thành công
    I2C_STATUS_BUSY,                // Bus đang bận
    I2C_STATUS_NACK,                // Nhận NACK từ slave
    I2C_STATUS_ARBITRATION_LOST,    // Mất quyền master
    I2C_STATUS_TIMEOUT,             // Timeout
    I2C_STATUS_ERROR                // Lỗi chung
} I2C_Status_t;
```

### Kiểm Tra Trạng Thái

```c
void I2C_CheckAndClearErrors(void)
{
    uint32_t status = I2C_MasterGetStatus(LPI2C0);
    
    // Check for NACK
    if (status & LPI2C_MSR_NDF_MASK) {
        printf("NACK detected!\n");
        I2C_MasterClearStatus(LPI2C0, LPI2C_MSR_NDF_MASK);
    }
    
    // Check for arbitration lost
    if (status & LPI2C_MSR_ALF_MASK) {
        printf("Arbitration lost!\n");
        I2C_MasterClearStatus(LPI2C0, LPI2C_MSR_ALF_MASK);
    }
    
    // Check for FIFO error
    if (status & LPI2C_MSR_FEF_MASK) {
        printf("FIFO error!\n");
        I2C_MasterClearStatus(LPI2C0, LPI2C_MSR_FEF_MASK);
    }
    
    // Check if bus busy
    if (I2C_IsBusBusy(LPI2C0)) {
        printf("Bus is busy\n");
    }
}
```

## Helper Macros

### Register Level Macros

```c
// Enable/Disable Master
LPI2C_ENABLE_MASTER(LPI2C0);
LPI2C_DISABLE_MASTER(LPI2C0);

// Reset
LPI2C_SW_RESET(LPI2C0);
LPI2C_RESET_TX_FIFO(LPI2C0);
LPI2C_RESET_RX_FIFO(LPI2C0);

// Status Checks
if (LPI2C_IS_TX_READY(LPI2C0)) { }
if (LPI2C_IS_RX_READY(LPI2C0)) { }
if (LPI2C_IS_BUS_BUSY(LPI2C0)) { }
if (LPI2C_IS_MASTER_BUSY(LPI2C0)) { }

// Data Transfer
LPI2C_WRITE_DATA(LPI2C0, data);
data = LPI2C_READ_DATA(LPI2C0);

// Clear status
LPI2C_CLEAR_STATUS(LPI2C0);
```

## Pin Configuration

### S32K144 I2C Pin Options

| Instance | SCL Pin | SDA Pin | Alternative Function |
|----------|---------|---------|----------------------|
| LPI2C0   | PTA2    | PTA3    | ALT3 |
| LPI2C0   | PTD8    | PTD9    | ALT2 |

### Cấu Hình Open-Drain

I2C yêu cầu open-drain output:

```c
// Enable open-drain for I2C pins
PORT_SetOpenDrain(PORTA, 2, true);   // SCL
PORT_SetOpenDrain(PORTA, 3, true);   // SDA

// Enable pull-up (nếu không có external pull-up)
PORT_SetPullUp(PORTA, 2);
PORT_SetPullUp(PORTA, 3);
```

## Clock Configuration

### Tính Toán Clock

I2C clock được tính theo công thức:

```
SCL frequency = Source Clock / (2^PRESCALE × (CLKLO + CLKHI + 2))
```

Ví dụ:
- Source clock: 8 MHz
- Target: 100 kHz
- PRESCALE = 0
- CLKLO = CLKHI = 38

```
100 kHz = 8 MHz / (1 × (38 + 38 + 2)) = 8 MHz / 78
```

Driver tự động tính toán các tham số này trong `I2C_MasterInit()`.

## Best Practices

### 1. Pull-up Resistors

- **Bắt buộc**: I2C yêu cầu pull-up trên SCL và SDA
- **Giá trị**: 
  - 100 kHz: 4.7kΩ - 10kΩ
  - 400 kHz: 2.2kΩ - 4.7kΩ
  - 1 MHz: 1kΩ - 2.2kΩ

### 2. Bus Capacitance

- Tối đa 400pF cho Fast mode
- Dây ngắn hơn = tần số cao hơn
- Nhiều devices = cần pull-up mạnh hơn

### 3. Address Conflicts

- Tránh trùng địa chỉ giữa các slaves
- Reserved addresses: 0x00-0x07, 0x78-0x7F

### 4. Timeout Protection

- Luôn kiểm tra timeout
- Implement software timeout cho blocking operations
- Driver có sẵn timeout counter

### 5. Error Recovery

```c
void I2C_RecoverFromError(void)
{
    // Disable master
    LPI2C_DISABLE_MASTER(LPI2C0);
    
    // Reset FIFOs
    LPI2C_RESET_TX_FIFO(LPI2C0);
    LPI2C_RESET_RX_FIFO(LPI2C0);
    
    // Clear all status
    LPI2C_CLEAR_STATUS(LPI2C0);
    
    // Re-enable master
    LPI2C_ENABLE_MASTER(LPI2C0);
    
    // Small delay
    for (volatile uint32_t i = 0; i < 10000; i++);
}
```

## Debugging

### 1. Oscilloscope Check

Kiểm tra:
- SCL square wave có đúng tần số không
- SDA có START/STOP conditions đúng không
- ACK/NACK pulses
- Voltage levels (0V và VDD)

### 2. Logic Analyzer

Capture toàn bộ transaction:
- Address byte
- Data bytes
- ACK/NACK bits
- Timing violations

### 3. Common Issues

| Vấn đề | Nguyên nhân | Giải pháp |
|--------|-------------|-----------|
| Bus stuck high | Thiếu pull-up | Thêm resistors |
| Bus stuck low | Device holding SDA | Reset device, check wiring |
| NACK every time | Sai địa chỉ | Scan bus, check address |
| Random errors | Noise, capacitance | Shorter wires, better layout |
| Clock stretching | Slow slave | Enable timeout |

## Tài Liệu Tham Khảo

1. **S32K1xx Reference Manual** - Chapter: LPI2C
2. **I2C Specification** - NXP/Philips I2C-bus specification
3. **Application Notes**:
   - AN4471: Understanding I2C Bus
   - AN5243: LPI2C Driver Example

## Migration Notes

Nếu migrate từ driver cũ:

```c
// Old style (direct register access)
LPI2C0->MCR |= (1 << 0);

// New style (driver API)
LPI2C_ENABLE_MASTER(LPI2C0);
// or
I2C_MasterInit(LPI2C0, &config, srcClock);
```

---

**Phiên bản**: 1.0  
**Ngày cập nhật**: 24/11/2025  
**Tác giả**: PhucPH32
