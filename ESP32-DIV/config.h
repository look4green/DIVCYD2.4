#ifndef CONFIG_H
#define CONFIG_H

// 📺 TFT Display (ILI9341)
#define TFT_CS     15
#define TFT_DC     2
#define TFT_RST    -1   // Use EN pin
#define TFT_BL     21   // Backlight

// 🖱️ Touch Controller (XPT2046)
#define TOUCH_CS   33
#define TOUCH_IRQ  36
#define TOUCH_CLK  25
#define TOUCH_DIN  32
#define TOUCH_DOUT 39

// 💾 SD Card (SPI)
#define SD_CS      5
#define SD_MOSI    23
#define SD_MISO    19
#define SD_CLK     18

// 📡 CC1101 Sub-GHz
#define CC1101_CS   27
#define CC1101_GDO0 34

// 📶 NRF24L01+
#define NRF_CE   16
#define NRF_CSN  4
#define NRF_IRQ  17

#endif
