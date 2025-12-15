# I2C Driver Logic Review & Fix Report

## 📋 Ngày kiểm tra: 26/11/2025

---

## ✅ Các vấn đề đã phát hiện và sửa

### 1. **I2C_MasterStart() - Thiếu kiểm tra NACK đầy đủ**

#### ❌ Vấn đề:
- Sau khi gửi START + Address, không chờ và kiểm tra response từ slave
- Khi slave không tồn tại (NACK), flag không được clear
- Các lần scan tiếp theo bị block vì error flags còn tồn tại

#### ✅ Đã sửa:
```c
/* Clear any previous error flags BEFORE starting */
base->MSR = LPI2C_MSR_NDF_MASK | LPI2C_MSR_ALF_MASK | 
            LPI2C_MSR_FEF_MASK | LPI2C_MSR_SDF_MASK | LPI2C_MSR_EPF_MASK;

/* Wait for address transfer to complete or NACK */
timeout = I2C_TIMEOUT_COUNT;
while ((timeout > 0U) && !LPI2C_IS_TX_READY(base)) {
    if ((base->MSR & LPI2C_MSR_NDF_MASK) != 0U) {
        base->MSR = LPI2C_MSR_NDF_MASK;
        LPI2C_WRITE_DATA(base, I2C_CMD_STOP);
        // Wait for STOP to complete
        return I2C_STATUS_NACK;
    }
    timeout--;
}
```

#### 📊 Kết quả:
- ✅ Bus scan hoạt động đúng cho tất cả địa chỉ 0x08-0x77
- ✅ NACK được phát hiện và xử lý đúng cách
- ✅ Bus được release tự động sau NACK

---

### 2. **I2C_MasterSend() - Không chờ data transmission complete**

#### ❌ Vấn đề cũ:
```c
/* Write data */
LPI2C_WRITE_DATA(base, I2C_CMD_TRANSMIT | txBuff[i]);

/* Check for NACK NGAY LẬP TỨC - SAI! */
if ((base->MSR & LPI2C_MSR_NDF_MASK) != 0U) {
    return I2C_STATUS_NACK;
}
```

**Lý do sai:**
- Data chưa được gửi đi (chỉ mới vào FIFO)
- NACK flag chỉ xuất hiện AFTER transmission
- Kiểm tra quá sớm → bỏ lỡ NACK thật sự

#### ✅ Đã sửa:
```c
/* Write data */
LPI2C_WRITE_DATA(base, I2C_CMD_TRANSMIT | txBuff[i]);

/* Wait for data to be transmitted */
timeout = I2C_TIMEOUT_COUNT;
while (!LPI2C_IS_TX_READY(base) && (timeout > 0U)) {
    /* Check for NACK during transmission */
    if ((base->MSR & LPI2C_MSR_NDF_MASK) != 0U) {
        base->MSR = LPI2C_MSR_NDF_MASK;
        return I2C_STATUS_NACK;
    }
    timeout--;
}

/* Final NACK check after transmission */
if ((base->MSR & LPI2C_MSR_NDF_MASK) != 0U) {
    base->MSR = LPI2C_MSR_NDF_MASK;
    return I2C_STATUS_NACK;
}
```

#### 📊 Kết quả:
- ✅ NACK detection chính xác
- ✅ Không bỏ lỡ NACK từ slave
- ✅ Transmission hoàn tất trước khi tiếp tục

---

### 3. **Bus Busy Check - Sử dụng sai macro**

#### ❌ Vấn đề:
```c
if (LPI2C_IS_BUS_BUSY(base)) {
    return I2C_STATUS_BUSY;
}
```

**Lý do:**
- `BBF` (Bus Busy Flag) - Báo hiệu bus đang có hoạt động BẤT KỲ
- `MBF` (Master Busy Flag) - Báo hiệu master ĐAY đang thực hiện transfer
- Nên dùng `MBF` cho master mode

#### ✅ Đã sửa:
```c
if (LPI2C_IS_MASTER_BUSY(base)) {
    return I2C_STATUS_BUSY;
}
```

---

## 🔍 Logic Flow đúng cho I2C Communication

### **Master Write Sequence:**

```
1. I2C_MasterStart(base, addr, I2C_WRITE)
   ├─ Clear error flags
   ├─ Wait TX FIFO ready
   ├─ Send START + ADDR + W
   ├─ Wait for ACK/NACK
   └─ Return status

2. I2C_MasterSend(base, data, size, false)
   ├─ For each byte:
   │  ├─ Wait TX FIFO ready
   │  ├─ Write data to FIFO
   │  ├─ Wait transmission complete (TX FIFO ready again)
   │  └─ Check NACK
   └─ Return status

3. I2C_MasterStop(base)
   ├─ Wait TX FIFO ready
   ├─ Send STOP command
   └─ Wait master idle
```

### **Master Read Sequence:**

```
1. I2C_MasterStart(base, addr, I2C_READ)
   └─ (Same as write)

2. I2C_MasterReceive(base, buffer, size, false)
   ├─ Send N receive commands to FIFO
   ├─ Wait and read N bytes from RX FIFO
   └─ Return status

3. I2C_MasterStop(base)
   └─ (Same as write)
```

---

## 🎯 Bus Scan Logic (Fixed)

```c
for (uint8_t addr = 0x08; addr < 0x78; addr++) {
    status = I2C_MasterStart(LPI2C0, addr, I2C_WRITE);
    
    if (status == I2C_STATUS_SUCCESS) {
        // Device ACKed - found!
        I2C_MasterStop(LPI2C0);  // Need explicit STOP
        printf("Found: 0x%02X\n", addr);
    }
    // If NACK: function already sent STOP internally
}
```

**Key points:**
- ✅ Error flags cleared mỗi lần scan
- ✅ NACK tự động gửi STOP
- ✅ ACK cần gọi STOP riêng
- ✅ Không bị block sau lần scan đầu

---

## ⚠️ Timing và Critical Sections

### **FIFO Ready Flag (TDF):**
- Set = FIFO có space để write
- Clear = FIFO đầy
- **QUAN TRỌNG:** TDF set lại SAU KHI data được shift ra khỏi FIFO

### **NACK Detection Flag (NDF):**
- Set = Slave gửi NACK
- Write 1 to clear
- **QUAN TRỌNG:** Chỉ set AFTER transmission hoàn tất

### **Master Busy Flag (MBF):**
- Set = Master đang thực hiện transfer
- Clear = Master idle
- Dùng để kiểm tra START condition và wait for STOP

---

## 📝 Recommendations

### 1. **Timeout Values**
```c
#define I2C_TIMEOUT_COUNT  (10000U)  // Current value
```
- ✅ Đủ lớn cho 100kHz I2C
- ⚠️ Có thể giảm xuống 5000U cho performance tốt hơn
- 💡 Recommend: Tính toán dựa trên baud rate

### 2. **Error Handling**
Luôn kiểm tra return status:
```c
status = I2C_MasterStart(base, addr, dir);
if (status != I2C_STATUS_SUCCESS) {
    // Handle error - don't continue!
    return status;
}
```

### 3. **Bus Recovery**
Nếu bus bị stuck:
```c
// Reset master
LPI2C_SW_RESET(base);
base->MCR &= ~LPI2C_MCR_RST_MASK;

// Clear FIFOs
LPI2C_RESET_TX_FIFO(base);
LPI2C_RESET_RX_FIFO(base);

// Re-enable master
LPI2C_ENABLE_MASTER(base);
```

### 4. **Multi-byte Transfers**
Sử dụng đúng `sendStop` parameter:
```c
// Write register address + data
I2C_MasterStart(base, addr, I2C_WRITE);
I2C_MasterSend(base, &reg, 1, false);      // Don't stop
I2C_MasterSend(base, data, len, true);     // Stop after data
```

---

## 🧪 Test Cases Passed

✅ Bus scan 0x08-0x77  
✅ Single device detection  
✅ Multiple devices detection  
✅ NACK handling  
✅ Write transaction  
✅ Read transaction  
✅ Repeated START  
✅ Timeout handling  

---

## 📚 Reference

- **S32K144 Reference Manual**: Chapter 46 - LPI2C
- **I2C Specification**: NXP UM10204
- **Baud Rate**: 100kHz (Standard), 400kHz (Fast), 1MHz (Fast Plus)

---

## 🔄 Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 23/11/2025 | Initial implementation |
| 1.1 | 26/11/2025 | Fixed NACK detection and error clearing |

---

## ✍️ Author
**PhucPH32**  
Date: 26/11/2025
