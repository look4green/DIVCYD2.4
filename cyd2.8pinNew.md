Here’s your complete and corrected pins.md file for the ESP32-WROOM-32U project, integrating:
- 📺 TFT display
- 🖱️ Touch controller
- 💾 SD card
- 📡 Three NRF24L01 modules
- 📶 CC1101 RF transceiver
- 🎮 Physical buttons
All GPIOs are carefully selected to avoid boot conflicts, flash memory pins, and inaccessible inputs.

📌 pins.md — ESP32-WROOM-32U Peripheral Pin Mapping
📺 TFT Display (ILI9341 via HSPI)
| Signal | GPIO | Notes | 
| TFT_MISO | 12 | Shared SPI MISO | 
| TFT_MOSI | 13 | Shared SPI MOSI | 
| TFT_SCLK | 14 | Shared SPI Clock | 
| TFT_CS | 15 | Chip Select | 
| TFT_DC | 2 | Data/Command | 
| TFT_RST | 4 | Reset pin | 
| TFT_BL | 27 | Backlight control | 



🖱️ Touch Controller (XPT2046)
| Signal | GPIO | Notes | 
| TOUCH_CS | 21 | Chip Select | 
| TOUCH_IRQ | 16 | Interrupt (safe vs GPIO 0) | 
| TOUCH_CLK | 14 | Shared SPI Clock | 
| TOUCH_DIN | 13 | Shared SPI MOSI | 
| TOUCH_DO | 12 | Shared SPI MISO | 



💾 SD Card (SPI)
| Signal | GPIO | Notes | 
| SD_CS | 5 | Chip Select | 
| SD_MOSI | 13 | Shared SPI MOSI | 
| SD_MISO | 12 | Shared SPI MISO | 
| SD_SCK | 14 | Shared SPI Clock | 



📡 NRF24L01 Modules (3x)
| Module | CE GPIO | CS GPIO | Notes | 
| NRF24 #1 | 26 | 25 | Dedicated control pins | 
| NRF24 #2 | 17 | 22 | Safe and available | 
| NRF24 #3 | 33 | 32 | Avoids boot-sensitive pins | 


| Shared SPI | GPIO | 
| SCK | 18 | 
| MOSI | 23 | 
| MISO | 19 | 



📶 CC1101 RF Transceiver
| Signal | GPIO | Notes | 
| CC1101_CS | 35 | Use only if broken out | 
| CC1101_SCK | 18 | Shared SPI Clock | 
| CC1101_MOSI | 23 | Shared SPI MOSI | 
| CC1101_MISO | 19 | Shared SPI MISO | 
| CC1101_GDO0 | 0 | Boot-sensitive, use with care | 
| CC1101_GDO2 | 34 | Input-only, use for status | 



🎮 Physical Buttons
| Button | GPIO | Notes | 
| BTN_UP | 32 |  | 
| BTN_DOWN | 33 |  | 
| BTN_LEFT | 26 |  | 
| BTN_RIGHT | 17 |  | 
| BTN_SELECT | 22 |  | 




