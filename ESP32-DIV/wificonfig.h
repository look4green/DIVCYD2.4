#ifndef WIFICONFIG_H
#define WIFICONFIG_H

#include "arduinoFFT.h"
#include "utils.h"

#include <WiFi.h>
#include <TFT_eSPI.h> 
#include <PCF8574.h>
#include <XPT2046_Touchscreen.h>

extern TFT_eSPI tft;
extern PCF8574 pcf;


// Define touchscreen pins (updated with your values)
#define XPT2046_CS    33  // Chip Select
#define XPT2046_IRQ   27  // Interrupt (input-only, ensure compatibility)
#define XPT2046_MOSI  22  // SPI MOSI
#define XPT2046_MISO  19  // SPI MISO
#define XPT2046_CLK   18  // SPI Clock


#include "WiFi.h"
#include <esp_wifi.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <string>
#include <cstddef>
#include <Wire.h>
#include <Preferences.h>
using namespace std;


namespace PacketMonitor {
  void ptmSetup();
  void ptmLoop();
}

namespace BeaconSpammer {
  void beaconSpamSetup();
  void beaconSpamLoop();
}

namespace DeauthDetect {
  void deauthdetectSetup();
  void deauthdetectLoop();
}

namespace WifiScan {
  void wifiscanSetup();
  void wifiscanLoop();
}



#endif // WIFICONFIG_H
