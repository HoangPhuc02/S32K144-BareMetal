# Examples for S32K144 Drivers

## Tổng Quan

Thư mục này chứa các ví dụ thực tế về cách sử dụng các driver cho S32K144.

## Danh Sách Examples

### 1. I2C Examples (`i2c_example.c`)

Các ví dụ về giao tiếp I2C:

- **Example 1**: Basic I2C Initialization
  - Khởi tạo I2C cơ bản
  - Cấu hình pins và clock
  
- **Example 2**: I2C Bus Scan
  - Quét bus I2C để tìm devices
  - Hiển thị địa chỉ của tất cả devices
  
- **Example 3**: MPU6050 Accelerometer/Gyroscope
  - Khởi tạo sensor MPU6050
  - Đọc dữ liệu gia tốc và con quay
  - Hiển thị real-time data
  
- **Example 4**: AT24C256 EEPROM
  - Ghi/đọc byte đơn
  - Ghi/đọc page
  - Verify data integrity
  
- **Example 5**: PCF8574 I/O Expander
  - Điều khiển 8-bit I/O
  - LED chaser pattern
  - Đọc trạng thái input
  
- **Example 6**: Error Handling
  - Xử lý NACK
  - Kiểm tra bus status
  - Error recovery

**Hardware cần thiết:**
- S32K144 board
- Pull-up resistors (4.7kΩ) trên SCL/SDA

**Pin connections:**
```
PTA2 -> I2C0_SCL (với 4.7kΩ pull-up)
PTA3 -> I2C0_SDA (với 4.7kΩ pull-up)
```

### 2. UART Examples (`uart_example.c`)

Các ví dụ về giao tiếp UART:

- **Example 1**: Basic UART Initialization
  - Khởi tạo UART với default config
  - Cấu hình pins
  
- **Example 2**: Send String
  - Gửi chuỗi ký tự
  - Blocking transmission
  
- **Example 3**: Printf Implementation
  - UART printf function
  - Format string với multiple data types
  
- **Example 4**: Receive Data
  - Nhận single byte với timeout
  - Nhận line (cho đến newline)
  
- **Example 5**: Echo Application
  - Simple echo (phản hồi ký tự)
  - Exit với ESC key
  
- **Example 6**: Command Line Interface
  - Full-featured CLI
  - Multiple commands (help, status, led, etc.)
  - Backspace support
  
- **Example 7**: Data Logging
  - CSV format logging
  - Simulated sensor data
  
- **Example 8**: Error Handling
  - Detect parity, frame, noise errors
  - Clear error flags
  
- **Example 9**: Loopback Test
  - Self-test với TX->RX connection
  - Verify data integrity

**Hardware cần thiết:**
- S32K144 board
- USB-to-Serial adapter hoặc ST-Link
- Terminal program (PuTTY, TeraTerm, etc.)

**Pin connections:**
```
PTB0 -> LPUART0_RX
PTB1 -> LPUART0_TX
```

## Cách Sử Dụng

### 1. Build và Flash

```bash
# Thêm file example vào project
# Build project
# Flash to board
```

### 2. Chạy Examples

#### I2C Examples:
```c
int main(void)
{
    /* System initialization */
    SystemInit();
    
    /* Run all I2C examples */
    I2C_ExamplesMain();
    
    while(1) {
        /* Main loop */
    }
}
```

#### UART Examples:
```c
int main(void)
{
    /* System initialization */
    SystemInit();
    
    /* Run all UART examples */
    UART_ExamplesMain();
    
    while(1) {
        /* Main loop */
    }
}
```

### 3. Chạy Example Riêng Lẻ

Bạn có thể chỉ chạy một example cụ thể:

```c
int main(void)
{
    /* System init */
    SystemInit();
    
    /* Run specific example */
    Example1_BasicInit();
    Example2_ScanBus();
    
    while(1) {
        /* Main loop */
    }
}
```

## Terminal Setup

Để xem output từ UART examples:

1. **Baud rate**: 115200
2. **Data bits**: 8
3. **Parity**: None
4. **Stop bits**: 1
5. **Flow control**: None

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
