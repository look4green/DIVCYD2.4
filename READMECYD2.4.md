# CYD2.4 ESP32-2432S024 Project

## 🧠 Overview
This project runs on the **CYD 2.4-inch ESP32-2432S024R** development board, featuring a rich hardware stack including a TFT display, capacitive touch, microSD card slot, RGB LED. The firmware is built on **Arduino** and **ESP-IDF** frameworks.

---

## 🔧 Board Features
- **ESP32 Dual-core processor**
- **240x320 ILI9341 TFT display** (SPI)
- **CST820 Capacitive touch screen** (I2C)
- **MicroSD card slot**
- **RGB LED** (R/G/B)

#define USER_SETUP_INFO "CYD-2432S024R Setup"

// Display Driver
#define ILI9341_2_DRIVER

// Screen Dimensions
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// Backlight Control
#define TFT_BL   27
#define TFT_BACKLIGHT_ON HIGH

// SPI Pins for TFT (HSPI)
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1  // Use -1 if connected to board RST

// Touchscreen (XPT2046) SPI Pins
#define TOUCH_CS 33
#define TOUCH_IRQ 36
#define TOUCH_MOSI 32
#define TOUCH_MISO 39
#define TOUCH_CLK 25

// Optional RGB LED Pins (active LOW)
#define RED_LED_PIN    4
#define GREEN_LED_PIN 16
#define BLUE_LED_PIN  17

// Enable DMA for faster rendering
#define ESP32_DMA

// Fonts (optional)
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

💾 SD Card SPI Pin Map (VSPI)
| Signal | GPIO | Description | 
| MISO | 19 | Master In Slave Out | 
| MOSI | 23 | Master Out Slave In | 
| SCK | 18 | SPI Clock | 
| CS | 5 | Chip Select | 





