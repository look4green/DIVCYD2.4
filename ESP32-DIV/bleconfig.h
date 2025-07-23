#ifndef BLECONFIG_H
#define BLECONFIG_H

#include "utils.h"

#include <TFT_eSPI.h> 
#include <PCF8574.h>
#include <XPT2046_Touchscreen.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "esp_bt.h"
#include "esp_wifi.h"
#include <Wire.h>


// Define touchscreen pins (updated with your values)
#define XPT2046_CS    33  // Chip Select
#define XPT2046_IRQ   27  // Interrupt (input-only, ensure compatibility)
#define XPT2046_MOSI  22  // SPI MOSI
#define XPT2046_MISO  19  // SPI MISO
#define XPT2046_CLK   18  // SPI Clock

extern TFT_eSPI tft;
extern PCF8574 pcf;


namespace BleJammer {
void blejamSetup();
void blejamLoop();
}

namespace BleSpoofer {
  void spooferSetup();
  void spooferLoop();
}

namespace SourApple {
  void sourappleSetup();
  void sourappleLoop();
}

namespace BleScan {
  void bleScanSetup();
  void bleScanLoop();
}

namespace Scanner {
  void scannerSetup();
  void scannerLoop();  
}

namespace ProtoKill {
  void prokillLoop();
  void prokillSetup();
}

#endif // CONFIG_H
