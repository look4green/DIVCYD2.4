#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Wire.h>

#include "Touchscreen.h"
#include "wificonfig.h"
#include "bleconfig.h"
#include "subconfig.h"
#include "utils.h"
#include "shared.h"
#include "icon.h"

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display

// Notification UI variables
bool notificationVisible = true;
int notifX, notifY, notifWidth, notifHeight;
int closeButtonX, closeButtonY, closeButtonSize = 15;
int okButtonX, okButtonY, okButtonWidth = 60, okButtonHeight = 20;

// Feature control flag
bool feature_exit_requested = false;

// Main menu setup
const int NUM_MENU_ITEMS = 8;
const char *menu_items[NUM_MENU_ITEMS] = {
    "WiFi",
    "Bluetooth",
    "2.4GHz",
    "SubGHz",
    "IR Remote",
    "Tools",
    "Setting",
    "About"
};

const unsigned char *bitmap_icons[NUM_MENU_ITEMS] = {
    bitmap_icon_wifi,
    bitmap_icon_spoofer,
    bitmap_icon_jammer,
    bitmap_icon_analyzer,
    bitmap_icon_led,
    bitmap_icon_stat,
    bitmap_icon_setting,
    bitmap_icon_question
};

int current_menu_index = 0;
bool is_main_menu = false;

// Submenu definitions
const int NUM_SUBMENU_ITEMS = 6;
const char *submenu_items[NUM_SUBMENU_ITEMS] = {
    "Packet Monitor",
    "Beacon Spammer",
    "WiFi Deauther [Coming soon]",
    "Deauth Detector",
    "WiFi Scanner",
    "Back to Main Menu"
};

const int bluetooth_NUM_SUBMENU_ITEMS = 6;
const char *bluetooth_submenu_items[bluetooth_NUM_SUBMENU_ITEMS] = {
    "BLE Jammer",
    "BLE Spoofer",
    "Sour Apple",
    "Analyzer [Coming soon]",
    "BLE Scanner",
    "Back to Main Menu"
};

const int nrf_NUM_SUBMENU_ITEMS = 5;
const char *nrf_submenu_items[nrf_NUM_SUBMENU_ITEMS] = {
    "Scanner",
    "Analyzer [Coming soon]",
    "WLAN Jammer [Coming soon]",
    "Proto Kill",
    "Back to Main Menu"
};

const int subghz_NUM_SUBMENU_ITEMS = 5;
const char *subghz_submenu_items[subghz_NUM_SUBMENU_ITEMS] = {
    "Replay Attack",
    "Bruteforce [Coming soon]",
    "SubGHz Jammer",
    "Saved Profile",
    "Back to Main Menu"
};

// Submenu state
int current_submenu_index = 0;
bool in_sub_menu = false;

const char **active_submenu_items = nullptr;
int active_submenu_size = 0;

// Define icon arrays for each submenu
const unsigned char *wifi_submenu_icons[NUM_SUBMENU_ITEMS] = {
    bitmap_icon_wifi,         // Packet Monitor
    bitmap_icon_antenna,      // Beacon Spammer
    bitmap_icon_wifi_jammer,  // WiFi Deauther
    bitmap_icon_eye2,         // Deauth Detector
    bitmap_icon_jammer,       // WiFi Scanner
    bitmap_icon_go_back       // Back to Main Menu
};

const unsigned char *bluetooth_submenu_icons[bluetooth_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_ble_jammer,   // BLE Jammer
    bitmap_icon_spoofer,      // BLE Spoofer
    bitmap_icon_apple,        // Sour Apple
    bitmap_icon_analyzer,     // Analyzer
    bitmap_icon_graph,        // BLE Scanner
    bitmap_icon_go_back       // Back to Main Menu
};

const unsigned char *nrf_submenu_icons[nrf_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_scanner,      // Scanner
    bitmap_icon_analyzer,     // Analyzer
    bitmap_icon_no_signal,    // WLAN Jammer
    bitmap_icon_kill,         // Proto Kill
    bitmap_icon_go_back       // Back to Main Menu
};

const unsigned char *subghz_submenu_icons[subghz_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_antenna,      // Replay Attack
    bitmap_icon_signals,      // Bruteforce
    bitmap_icon_no_signal,    // SubGHz Jammer
    bitmap_icon_list,         // Saved Profile
    bitmap_icon_go_back       // Back to Main Menu
};

// Array to hold the active icon set
const unsigned char **active_submenu_icons = nullptr;

// Update submenu and icons based on current menu selection
void updateActiveSubmenu() {
    switch (current_menu_index) {
        case 0: // WiFi
            active_submenu_items = submenu_items;
            active_submenu_size = NUM_SUBMENU_ITEMS;
            active_submenu_icons = wifi_submenu_icons;
            break;
        case 1: // Bluetooth
            active_submenu_items = bluetooth_submenu_items;
            active_submenu_size = bluetooth_NUM_SUBMENU_ITEMS;
            active_submenu_icons = bluetooth_submenu_icons;
            break;
        case 2: // 2.4GHz (NRF)
            active_submenu_items = nrf_submenu_items;
            active_submenu_size = nrf_NUM_SUBMENU_ITEMS;
            active_submenu_icons = nrf_submenu_icons;
            break;
        case 3: // SubGHz
            active_submenu_items = subghz_submenu_items;
            active_submenu_size = subghz_NUM_SUBMENU_ITEMS;
            active_submenu_icons = subghz_submenu_icons;
            break;
        default:
            active_submenu_items = nullptr;
            active_submenu_size = 0;
            active_submenu_icons = nullptr;
            break;
    }
}

// Battery voltage (assuming readBatteryVoltage() is defined elsewhere)
float currentBatteryVoltage = readBatteryVoltage();

//==========================================================================
// Backlight management based on user interaction

unsigned long last_interaction_time = 0;
const unsigned long BACKLIGHT_TIMEOUT = 100000;  // 100 seconds

void manageBacklight() {
  // If you connect a backlight control pin, uncomment below and define BACKLIGHT_PIN
  /*
  if (millis() - last_interaction_time > BACKLIGHT_TIMEOUT) {
    digitalWrite(BACKLIGHT_PIN, LOW);  // Turn off backlight
  } else {
    digitalWrite(BACKLIGHT_PIN, HIGH); // Keep backlight on
  }
  */
}
//==========================================================================

int last_submenu_index = -1;
bool submenu_initialized = false;
int last_menu_index = -1;
bool menu_initialized = false;

void displaySubmenu() {
    menu_initialized = false;
    last_menu_index = -1;

    tft.setTextFont(2);
    tft.setTextSize(1);

    if (!submenu_initialized) {
        tft.fillScreen(TFT_BLACK);

        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30;
            if (i == active_submenu_size - 1) yPos += 10;

            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawBitmap(10, yPos, active_submenu_icons[i], 16, 16, TFT_WHITE);

            tft.setCursor(30, yPos);
            if (i < active_submenu_size - 1) {
                tft.print("| ");
            }
            tft.print(active_submenu_items[i]);
        }

        submenu_initialized = true;
        last_submenu_index = -1;
    }

    if (last_submenu_index != current_submenu_index) {
        if (last_submenu_index >= 0) {
            int prev_yPos = 30 + last_submenu_index * 30;
            if (last_submenu_index == active_submenu_size - 1) prev_yPos += 10;

            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawBitmap(10, prev_yPos, active_submenu_icons[last_submenu_index], 16, 16, TFT_WHITE);
            tft.setCursor(30, prev_yPos);
            if (last_submenu_index < active_submenu_size - 1) {
                tft.print("| ");
            }
            tft.print(active_submenu_items[last_submenu_index]);
        }

        int new_yPos = 30 + current_submenu_index * 30;
        if (current_submenu_index == active_submenu_size - 1) new_yPos += 10;

        tft.setTextColor(ORANGE, TFT_BLACK);
        tft.drawBitmap(10, new_yPos, active_submenu_icons[current_submenu_index], 16, 16, ORANGE);
        tft.setCursor(30, new_yPos);
        if (current_submenu_index < active_submenu_size - 1) {
            tft.print("| ");
        }
        tft.print(active_submenu_items[current_submenu_index]);

        last_submenu_index = current_submenu_index;
    }

    drawStatusBar(currentBatteryVoltage, true);
}
//---------------------------------------------------------------------
const int COLUMN_WIDTH = 120;
const int X_OFFSET_LEFT = 10;
const int X_OFFSET_RIGHT = X_OFFSET_LEFT + COLUMN_WIDTH;
const int Y_START = 30;
const int Y_SPACING = 75;

void displayMenu() {
  #define TFT_DARKBLUE  0x3166
  #define TFT_LIGHTBLUE 0x051F
  #define TFTWHITE      0xFFFF
  #define TFT_GRAY      0x8410
  #define SELECTED_ICON_COLOR 0xfbe4

  const uint16_t icon_colors[NUM_MENU_ITEMS] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
  };

  submenu_initialized = false;
  last_submenu_index = -1;

  tft.setTextFont(2);

  if (!menu_initialized) {
    tft.fillScreen(0x20e4);

    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
      int column = i / 4;
      int row = i % 4;
      int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
      int y_position = Y_START + row * Y_SPACING;

      tft.fillRoundRect(x_position, y_position, 100, 60, 5, TFT_DARKBLUE);
      tft.drawRoundRect(x_position, y_position, 100, 60, 5, TFT_GRAY);

      tft.drawBitmap(x_position + 42, y_position + 10, bitmap_icons[i], 16, 16, icon_colors[i]);

      tft.setTextColor(TFTWHITE, TFT_DARKBLUE);
      int textWidth = 6 * strlen(menu_items[i]);
      int textX = x_position + (100 - textWidth) / 2;
      int textY = y_position + 30;
      tft.setCursor(textX, textY);
      tft.print(menu_items[i]);
    }

    menu_initialized = true;
    last_menu_index = -1;
  }

  if (last_menu_index != current_menu_index) {
    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
      if (i == last_menu_index) {
        int column = i / 4;
        int row = i % 4;
        int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
        int y_position = Y_START + row * Y_SPACING;

        tft.fillRoundRect(x_position, y_position, 100, 60, 5, TFT_DARKBLUE);
        tft.drawRoundRect(x_position, y_position, 100, 60, 5, TFT_GRAY);
        tft.setTextColor(TFTWHITE, TFT_DARKBLUE);
        tft.drawBitmap(x_position + 42, y_position + 10, bitmap_icons[i], 16, 16, icon_colors[i]);
        int textWidth = 6 * strlen(menu_items[i]);
        int textX = x_position + (100 - textWidth) / 2;
        int textY = y_position + 30;
        tft.setCursor(textX, textY);
        tft.print(menu_items[i]);
      }
    }

    int column = current_menu_index / 4;
    int row = current_menu_index % 4;
    int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
    int y_position = Y_START + row * Y_SPACING;

    tft.fillRoundRect(x_position, y_position, 100, 60, 5, TFT_DARKBLUE);
    tft.drawRoundRect(x_position, y_position, 100, 60, 5, ORANGE);

    tft.setTextColor(ORANGE, TFT_DARKBLUE);
    tft.drawBitmap(x_position + 42, y_position + 10, bitmap_icons[current_menu_index], 16, 16, SELECTED_ICON_COLOR);
    int textWidth = 6 * strlen(menu_items[current_menu_index]);
    int textX = x_position + (100 - textWidth) / 2;
    int textY = y_position + 30;
    tft.setCursor(textX, textY);
    tft.print(menu_items[current_menu_index]);

    last_menu_index = current_menu_index;
  }

  drawStatusBar(currentBatteryVoltage, true);
}
//-----------------------------------------------
void handleWiFiSubmenuTouch() {
  int touchedIndex = getTouchedSubmenuIndex();
  if (touchedIndex == -1) return;

  current_submenu_index = touchedIndex;
  last_interaction_time = millis();
  displaySubmenu();
  delay(200);

  if (current_submenu_index == 5) {
    in_sub_menu = false;
    feature_active = false;
    feature_exit_requested = false;
    displayMenu();
    is_main_menu = false;
    return;
  }

  in_sub_menu = true;
  feature_active = true;
  feature_exit_requested = false;

  switch (current_submenu_index) {
    case 0:
      PacketMonitor::ptmSetup();
      while (!feature_exit_requested) {
        PacketMonitor::ptmLoop();
        if (getTouchedSubmenuIndex() == 0) break;
      }
      break;

    case 1:
      BeaconSpammer::beaconSpamSetup();
      while (!feature_exit_requested) {
        BeaconSpammer::beaconSpamLoop();
        if (getTouchedSubmenuIndex() == 1) break;
      }
      break;

    case 2:
      // Placeholder for future feature
      while (!feature_exit_requested) {
        if (getTouchedSubmenuIndex() == 2) break;
      }
      break;

    case 3:
      DeauthDetect::deauthdetectSetup();
      while (!feature_exit_requested) {
        DeauthDetect::deauthdetectLoop();
        if (getTouchedSubmenuIndex() == 3) break;
      }
      break;

    case 4:
      WifiScan::wifiscanSetup();
      while (!feature_exit_requested) {
        WifiScan::wifiscanLoop();
        if (getTouchedSubmenuIndex() == 4) break;
      }
      break;
  }

  // Return to submenu after feature ends
  in_sub_menu = true;
  is_main_menu = false;
  submenu_initialized = false;
  feature_active = false;
  feature_exit_requested = false;
  displaySubmenu();
  delay(200);
}
//-------------------------------------------------------
void handleBluetoothSubmenuTouch() {
  if (!ts.touched() || feature_active) return;

  TS_Point p = ts.getPoint();
  delay(10);  // Debounce

  int x = map(p.x, TS_MINX, TS_MAXX, 0, 239);
  int y = map(p.y, TS_MAXY, TS_MINY, 0, 319);

  for (int i = 0; i < active_submenu_size; i++) {
    int yPos = 30 + i * 30;
    if (i == active_submenu_size - 1) yPos += 10;

    int button_x1 = 10;
    int button_y1 = yPos;
    int button_x2 = 110;
    int button_y2 = yPos + ((i == active_submenu_size - 1) ? 40 : 30);

    if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
      current_submenu_index = i;
      last_interaction_time = millis();
      displaySubmenu();
      delay(200);

      if (i == 5) {
        // Exit to main menu
        in_sub_menu = false;
        feature_active = false;
        feature_exit_requested = false;
        displayMenu();
        is_main_menu = false;
        return;
      }

      // Setup and loop function dispatch table
      struct Feature {
        void (*setup)();
        void (*loop)();
      };

      Feature features[] = {
        {BleJammer::blejamSetup, BleJammer::blejamLoop},
        {BleSpoofer::spooferSetup, BleSpoofer::spooferLoop},
        {SourApple::sourappleSetup, SourApple::sourappleLoop},
        {nullptr, nullptr}, // Placeholder for index 3
        {BleScan::bleScanSetup, BleScan::bleScanLoop}
      };

      if (i >= 0 && i < 5) {
        in_sub_menu = true;
        feature_active = true;
        feature_exit_requested = false;

        Feature& f = features[i];
        if (f.setup) f.setup();

        while (!feature_exit_requested) {
          if (f.loop) f.loop();

          if (ts.touched()) {
            TS_Point p2 = ts.getPoint();
            delay(10);
            int tx = map(p2.x, TS_MINX, TS_MAXX, 0, 239);
            int ty = map(p2.y, TS_MAXY, TS_MINY, 0, 319);

            // Define a "Back" touch zone (e.g., bottom-right corner)
            if (tx > 180 && ty > 280) {
              in_sub_menu = true;
              is_main_menu = false;
              submenu_initialized = false;
              feature_active = false;
              feature_exit_requested = false;
              displaySubmenu();
              delay(200);
              break;
            }
          }
        }

        if (feature_exit_requested) {
          in_sub_menu = true;
          is_main_menu = false;
          submenu_initialized = false;
          feature_active = false;
          feature_exit_requested = false;
          displaySubmenu();
          delay(200);
        }
      }

      break;
    }
  }
}
//-------------------------------------------
void handleSubGHzSubmenuTouch() {
  if (!ts.touched() || feature_active) return;

  TS_Point p = ts.getPoint();
  delay(10);  // Debounce

  int x = map(p.x, TS_MINX, TS_MAXX, 0, 239);
  int y = map(p.y, TS_MAXY, TS_MINY, 0, 319);

  for (int i = 0; i < active_submenu_size; i++) {
    int yPos = 30 + i * 30;
    if (i == active_submenu_size - 1) yPos += 10;

    int button_x1 = 10;
    int button_y1 = yPos;
    int button_x2 = 110;
    int button_y2 = yPos + ((i == active_submenu_size - 1) ? 40 : 30);

    if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
      current_submenu_index = i;
      last_interaction_time = millis();
      displaySubmenu();
      delay(200);

      if (i == 4) {
        // Exit to main menu
        in_sub_menu = false;
        feature_active = false;
        feature_exit_requested = false;
        displayMenu();
        handleButtons();
        is_main_menu = false;
        return;
      }

      // Setup and loop dispatch
      struct Feature {
        void (*setup)();
        void (*loop)();
        bool requiresPins;
      };

      Feature features[] = {
        {replayat::ReplayAttackSetup, replayat::ReplayAttackLoop, true},
        {nullptr, nullptr, false}, // index 1 unused
        {subjammer::subjammerSetup, subjammer::subjammerLoop, true},
        {SavedProfile::saveSetup, SavedProfile::saveLoop, true}
      };

      if (i >= 0 && i < 4) {
        Feature& f = features[i];

        if (f.requiresPins) {
          pinMode(26, INPUT);
          pinMode(16, INPUT);
        }

        in_sub_menu = true;
        feature_active = true;
        feature_exit_requested = false;

        if (f.setup) f.setup();

        while (!feature_exit_requested) {
          if (f.loop) f.loop();

          if (ts.touched()) {
            TS_Point p2 = ts.getPoint();
            delay(10);
            int tx = map(p2.x, TS_MINX, TS_MAXX, 0, 239);
            int ty = map(p2.y, TS_MAXY, TS_MINY, 0, 319);

            // Define a "Back" touch zone (e.g., bottom-right corner)
            if (tx > 180 && ty > 280) {
              in_sub_menu = true;
              is_main_menu = false;
              submenu_initialized = false;
              feature_active = false;
              feature_exit_requested = false;
              displaySubmenu();
              delay(200);
              break;
            }
          }
        }

        if (feature_exit_requested) {
          in_sub_menu = true;
          is_main_menu = false;
          submenu_initialized = false;
          feature_active = false;
          feature_exit_requested = false;
          displaySubmenu();
          delay(200);
        }
      }

      break;
    }
  }
}
//-----------------------------------------
void handleButtons() {
  if (in_sub_menu) {
    handleSubmenuInput();
    return;
  }

  handleTouchNavigation();
}

//---------------------------------------------------------------------

// 📲 Handles touchscreen input in main menu
void handleTouchNavigation() {
  static unsigned long lastTouchTime = 0;
  const unsigned long touchFeedbackDelay = 100;

  if (ts.touched() && !feature_active && (millis() - lastTouchTime >= touchFeedbackDelay)) {
    TS_Point p = ts.getPoint();
    delay(10);

    int x = map(p.x, TS_MINX, TS_MAXX, 0, 239);
    int y = map(p.y, TS_MAXY, TS_MINY, 0, 319);

    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
      int column = i / 4;
      int row = i % 4;
      int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
      int y_position = Y_START + row * Y_SPACING;

      int button_x1 = x_position;
      int button_y1 = y_position;
      int button_x2 = x_position + 100;
      int button_y2 = y_position + 60;

      if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
        current_menu_index = i;
        last_interaction_time = millis();
        displayMenu();

        unsigned long startTime = millis();
        while (ts.touched() && (millis() - startTime < touchFeedbackDelay)) {
          delay(10);
        }

        if (ts.touched()) {
          updateActiveSubmenu();

          if (active_submenu_items && active_submenu_size > 0) {
            current_submenu_index = 0;
            in_sub_menu = true;
            submenu_initialized = false;
            displaySubmenu();
          } else {
            is_main_menu = !is_main_menu;
            displayMenu();
          }
        }

        delay(200);
        break;
      }
    }
  }
}

// 📂 Dispatches submenu input handling based on current menu
void handleSubmenuInput() {
  switch (current_menu_index) {
    case 0: handleWiFiSubmenuButtons(); break;
    case 1: handleBluetoothSubmenuButtons(); break;
    case 2: handleNRFSubmenuButtons(); break;
    case 3: handleSubGHzSubmenuButtons(); break;
    default: break;
  }
}
//-------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  setupTouchscreen();
  drawSplashScreen();

  displayMenu();
  drawStatusBar(currentBatteryVoltage, false);
  last_interaction_time = millis();
}
//---------------------------------------------
void drawSplashScreen() {
  int16_t screenWidth = tft.width();
  int16_t screenHeight = tft.height();
  int16_t logoX = (screenWidth - 150) / 2;
  int16_t logoY = (screenHeight - 150) / 2 - 20;

  tft.drawBitmap(logoX, logoY, bitmap_icon_cifer, 150, 150, TFT_WHITE);

  tft.setTextColor(TFT_WHITE);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setCursor(screenWidth / 3.5, logoY + 150 + 10);
  tft.print("ESP32-DIV");

  tft.setTextSize(1);
  tft.setCursor(screenWidth / 3.5, logoY + 180);
  tft.print("by CiferTech");

  tft.setCursor(screenWidth / 2.5, logoY + 230);
  tft.print("v1.0.0");

  delay(2000);
}
