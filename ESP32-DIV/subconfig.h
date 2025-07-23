#ifndef SUBCONFIG_H
#define SUBCONFIG_H

#include "utils.h"
#include "arduinoFFT.h"
#include <TFT_eSPI.h> 
#include <PCF8574.h>
#include <ELECHOUSE_CC1101_ESP32DIV.h>

#include <RCSwitch.h>
#include <EEPROM.h>
#include <stdio.h>
#include <string>
#include <cstddef>
#include <Wire.h>

extern TFT_eSPI tft;
extern PCF8574 pcf;


// Define touchscreen pins (updated with your values)
#define XPT2046_CS    33  // Chip Select
#define XPT2046_IRQ   27  // Interrupt (input-only, ensure compatibility)
#define XPT2046_MOSI  22  // SPI MOSI
#define XPT2046_MISO  19  // SPI MISO
#define XPT2046_CLK   18  // SPI Clock



namespace replayat {
  void ReplayAttackSetup();
  void ReplayAttackLoop();
}

namespace SavedProfile {
  void saveSetup();
  void saveLoop();
}

namespace subjammer {
  void subjammerSetup();
  void subjammerLoop();
}



#endif // SUBCONFIG_H
