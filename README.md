# CYD2.4 ESP32-2432S024 Project

## 🧠 Overview
This project runs on the **CYD 2.4-inch ESP32-2432S024** development board, featuring a rich hardware stack including a TFT display, capacitive touch, microSD card slot, RGB LED, speaker, and light sensor. The firmware is built on **Arduino** and **ESP-IDF** frameworks.

---

## 🔧 Board Features
- **ESP32 Dual-core processor**
- **240x320 ILI9341 TFT display** (SPI)
- **CST820 Capacitive touch screen** (I2C)
- **MicroSD card slot**
- **RGB LED** (R/G/B)
- **Speaker output**
- **Light sensor (CDS)**

---

## 🧰 Configuration
### 📺 TFT Display
- MISO: GPIO 12  
- MOSI: GPIO 13  
- SCLK: GPIO 14  
- CS:   GPIO 15  
- DC:   GPIO 2  
- BL:   GPIO 27

### 🖱️ Touchscreen (CST820)
- SDA: GPIO 33  
- SCL: GPIO 32  
- IRQ: GPIO 21  
- RST: GPIO 25

### 💾 MicroSD Card
- MISO: GPIO 19  
- MOSI: GPIO 23  
- SCLK: GPIO 18  
- CS:   GPIO 5

### 🌈 RGB LED
- Red:   GPIO 4  
- Green: GPIO 16  
- Blue:  GPIO 17

### 🔊 Speaker
- Out: GPIO 26

### 🌞 Light Sensor
- CDS: GPIO 34

---

## 🚀 Build Flags
Configured for **high performance and compatibility** with the onboard peripherals. Example flags:
```bash
-DILI9341_DRIVER=1
-DTOUCH_DRIVER=0x0820
-DSPI_FREQUENCY=65000000
-DI2C_TOUCH_FREQUENCY=400000
