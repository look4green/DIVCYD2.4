#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <SD.h>

#include "Touchscreen.h"
#include "utils.h"
#include "shared.h"

// === Screen & Touch ===
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320
#define TOUCH_CS      33
#define TOUCH_IRQ     25
#define TFT_BL        32
#define SD_CS         5

// === Touch Calibration ===
#define TS_MINX 100
#define TS_MAXX 3900
#define TS_MINY 100
#define TS_MAXY 3900

// === Colors ===
#define ORANGE         0xFDA0
#define TFTWHITE       0xFFFF
#define TFT_DARKBLUE   0x001F
#define SELECTED_ICON_COLOR ORANGE
#define TFT_GRAY       0x8410
#define BACKLIGHT_TIMEOUT 30000

// === Objects ===
TFT_eSPI tft = TFT_eSPI();

// === Battery ===
#define BATTERY_PIN 34
float currentBatteryVoltage = 0;


// === Menu Items ===
const int NUM_MENU_ITEMS = 8;
const char *menu_items[NUM_MENU_ITEMS] = {
  "WiFi", "Bluetooth", "2.4GHz", "SubGHz",
  "IR Remote", "Tools", "Setting", "About"
};

const int NUM_SUBMENU_ITEMS = 7;
const char *submenu_items[NUM_SUBMENU_ITEMS] = {
  "Packet Monitor", "Beacon Spammer", "WiFi Deauther",
  "Deauth Detector", "WiFi Scanner", "Captive Portal",
  "Back to Main Menu"
};

const int bluetooth_NUM_SUBMENU_ITEMS = 6;
const char *bluetooth_submenu_items[bluetooth_NUM_SUBMENU_ITEMS] = {
  "BLE Jammer", "BLE Spoofer", "Sour Apple",
  "Sniffer", "BLE Scanner", "Back to Main Menu"
};

const int nrf_NUM_SUBMENU_ITEMS = 5;
const char *nrf_submenu_items[nrf_NUM_SUBMENU_ITEMS] = {
  "Scanner", "Analyzer [Coming soon]",
  "WLAN Jammer [Coming soon]", "Proto Kill",
  "Back to Main Menu"
};

const int subghz_NUM_SUBMENU_ITEMS = 5;
const char *subghz_submenu_items[subghz_NUM_SUBMENU_ITEMS] = {
  "Replay Attack", "Bruteforce [Coming soon]",
  "SubGHz Jammer", "Saved Profile",
  "Back to Main Menu"
};

const int tools_NUM_SUBMENU_ITEMS = 3;
const char *tools_submenu_items[tools_NUM_SUBMENU_ITEMS] = {
  "Serial Monitor", "Update Firmware",
  "Back to Main Menu"
};

const int ir_NUM_SUBMENU_ITEMS = 3;
const char *ir_submenu_items[ir_NUM_SUBMENU_ITEMS] = {
  "Record [Coming soon]", "Saved Profile [Coming soon]",
  "Back to Main Menu"
};

const int about_NUM_SUBMENU_ITEMS = 1;
const char *about_submenu_items[about_NUM_SUBMENU_ITEMS] = {
  "Back to Main Menu"
};

// === Menu State ===

bool menu_initialized = false;

unsigned long last_interaction_time = 0;

int current_menu_index = 0;
int current_submenu_index = 0;
int last_menu_index = -1;
int last_submenu_index = -1;


const char **active_submenu_items = nullptr;
int active_submenu_size = 0;

// === Layout ===
const int COLUMN_WIDTH    = 120;
const int X_OFFSET_LEFT   = 10;
const int X_OFFSET_RIGHT  = X_OFFSET_LEFT + COLUMN_WIDTH;
const int Y_START         = 30;
const int Y_SPACING       = 75;

const uint16_t icon_colors[NUM_MENU_ITEMS] = {
  TFTWHITE, TFTWHITE, TFTWHITE, TFTWHITE,
  TFTWHITE, TFTWHITE, TFT_GRAY, TFTWHITE
};

const unsigned char *bitmap_icons[NUM_MENU_ITEMS] = {
  nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr
};

// === Submenu Activation ===
void updateActiveSubmenu() {
  switch (current_menu_index) {
    case 0:
      active_submenu_items = submenu_items;
      active_submenu_size = NUM_SUBMENU_ITEMS;
      break;
    case 1:
      active_submenu_items = bluetooth_submenu_items;
      active_submenu_size = bluetooth_NUM_SUBMENU_ITEMS;
      break;
    case 2:
      active_submenu_items = nrf_submenu_items;
      active_submenu_size = nrf_NUM_SUBMENU_ITEMS;
      break;
    case 3:
      active_submenu_items = subghz_submenu_items;
      active_submenu_size = subghz_NUM_SUBMENU_ITEMS;
      break;
    case 4:
      active_submenu_items = ir_submenu_items;
      active_submenu_size = ir_NUM_SUBMENU_ITEMS;
      break;
    case 5:
      active_submenu_items = tools_submenu_items;
      active_submenu_size = tools_NUM_SUBMENU_ITEMS;
      break;
    case 6:
      active_submenu_items = nullptr;
      active_submenu_size = 0;
      break;
    case 7:
      active_submenu_items = about_submenu_items;
      active_submenu_size = about_NUM_SUBMENU_ITEMS;
      break;
    default:
      active_submenu_items = nullptr;
      active_submenu_size = 0;
      break;
  }
}

// === Touch Zone Detection ===
bool touchZonePressed(String zone) {
  if (!ts.touched()) return false;
  TS_Point p = ts.getPoint();
  int x = map(p.x, TS_MINX, TS_MAXX, 0, SCREEN_WIDTH - 1);
  int y = map(p.y, TS_MAXY, TS_MINY, 0, SCREEN_HEIGHT - 1);

  if (zone == "UP") return (y < 80);
  if (zone == "DOWN") return (y > 240);
  if (zone == "SELECT") return (y >= 80 && y <= 240);
  return false;
}


// === Display Main Menu ===
void displayMenu() {
  manageBacklight();
  submenu_initialized = false;
  last_submenu_index = -1;
  tft.setTextFont(2);

  if (!menu_initialized) {
    tft.fillScreen(0x20e4); // Background color

    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
      int column = i / 4;
      int row = i % 4;
      int x = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
      int y = Y_START + row * Y_SPACING;

      tft.fillRoundRect(x, y, 100, 60, 5, TFT_DARKBLUE);
      tft.drawRoundRect(x, y, 100, 60, 5, TFT_GRAY);

      if (bitmap_icons[i]) {
        tft.drawBitmap(x + 42, y + 10, bitmap_icons[i], 16, 16, icon_colors[i]);
      }

      tft.setTextColor(TFTWHITE, TFT_DARKBLUE);
      int textWidth = 6 * strlen(menu_items[i]);
      int textX = x + (100 - textWidth) / 2;
      int textY = y + 30;
      tft.setCursor(textX, textY);
      tft.print(menu_items[i]);
    }

    menu_initialized = true;
    last_menu_index = -1;
  }

  if (last_menu_index != current_menu_index) {
    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
      int column = i / 4;
      int row = i % 4;
      int x = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
      int y = Y_START + row * Y_SPACING;

      if (i == last_menu_index) {
        tft.fillRoundRect(x, y, 100, 60, 5, TFT_DARKBLUE);
        tft.drawRoundRect(x, y, 100, 60, 5, TFT_GRAY);
        tft.setTextColor(TFTWHITE, TFT_DARKBLUE);
        if (bitmap_icons[i]) {
          tft.drawBitmap(x + 42, y + 10, bitmap_icons[i], 16, 16, icon_colors[i]);
        }
        int textWidth = 6 * strlen(menu_items[i]);
        int textX = x + (100 - textWidth) / 2;
        int textY = y + 30;
        tft.setCursor(textX, textY);
        tft.print(menu_items[i]);
      }
    }

    int column = current_menu_index / 4;
    int row = current_menu_index % 4;
    int x = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
    int y = Y_START + row * Y_SPACING;

    tft.fillRoundRect(x, y, 100, 60, 5, TFT_DARKBLUE);
    tft.drawRoundRect(x, y, 100, 60, 5, ORANGE);
    tft.setTextColor(ORANGE, TFT_DARKBLUE);
    if (bitmap_icons[current_menu_index]) {
      tft.drawBitmap(x + 42, y + 10, bitmap_icons[current_menu_index], 16, 16, SELECTED_ICON_COLOR);
    }
    int textWidth = 6 * strlen(menu_items[current_menu_index]);
    int textX = x + (100 - textWidth) / 2;
    int textY = y + 30;
    tft.setCursor(textX, textY);
    tft.print(menu_items[current_menu_index]);

    last_menu_index = current_menu_index;
  }

  drawStatusBar(currentBatteryVoltage, true);
}

// === Display Submenu ===
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

      tft.setTextColor(TFTWHITE, TFT_BLACK);
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

      tft.setTextColor(TFTWHITE, TFT_BLACK);
      tft.setCursor(30, prev_yPos);
      if (last_submenu_index < active_submenu_size - 1) {
        tft.print("| ");
      }
      tft.print(active_submenu_items[last_submenu_index]);
    }

    int new_yPos = 30 + current_submenu_index * 30;
    if (current_submenu_index == active_submenu_size - 1) new_yPos += 10;

    tft.setTextColor(ORANGE, TFT_BLACK);
    tft.setCursor(30, new_yPos);
    if (current_submenu_index < active_submenu_size - 1) {
      tft.print("| ");
    }
    tft.print(active_submenu_items[current_submenu_index]);

    last_submenu_index = current_submenu_index;
  }

  drawStatusBar(currentBatteryVoltage, true);
}

// === SubGHz Submenu Touch Handler ===
void handleSubGHzSubmenuTouch() {
  if (touchZonePressed("UP")) {
    current_submenu_index = (current_submenu_index - 1 + active_submenu_size) % active_submenu_size;
    last_interaction_time = millis();
    displaySubmenu();
    delay(200);
  }

  if (touchZonePressed("DOWN")) {
    current_submenu_index = (current_submenu_index + 1) % active_submenu_size;
    last_interaction_time = millis();
    displaySubmenu();
    delay(200);
  }

  if (touchZonePressed("SELECT")) {
    last_interaction_time = millis();
    delay(200);

    switch (current_submenu_index) {
      case 0:
        launchReplayAttack();
        break;
      case 2:
        launchSubJammer();
        break;
      case 3:
        launchProfileSaver();
        break;
      case 4:
        in_sub_menu = false;
        feature_active = false;
        feature_exit_requested = false;
        is_main_menu = false;
        displayMenu();
        break;
    }
  }
}

// === Feature Launchers ===
void launchReplayAttack() {
  feature_active = true;
  feature_exit_requested = false;
  pinMode(17, INPUT); // NRF_CE
  pinMode(5, INPUT);  // NRF_CSN

  while (!feature_exit_requested) {
    // replayat::ReplayAttackLoop();
    if (ts.touched()) {
      while (ts.touched()) delay(10);
      break;
    }
  }

  feature_active = false;
  feature_exit_requested = false;
  submenu_initialized = false;
  displaySubmenu();
  delay(200);
}

void launchSubJammer() {
  feature_active = true;
  feature_exit_requested = false;
  pinMode(17, INPUT);
  pinMode(5, INPUT);

  while (!feature_exit_requested) {
    // subghz::SubJammerLoop();
    if (ts.touched()) {
      while (ts.touched()) delay(10);
      break;
    }
  }

  feature_active = false;
  feature_exit_requested = false;
  submenu_initialized = false;
  displaySubmenu();
  delay(200);
}

void launchProfileSaver() {
  feature_active = true;
  feature_exit_requested = false;

  while (!feature_exit_requested) {
    // subghz::ProfileSaverLoop();
    if (ts.touched()) {
      while (ts.touched()) delay(10);
      break;
    }
  }

  feature_active = false;
  feature_exit_requested = false;
  submenu_initialized = false;
  displaySubmenu();
  delay(200);
}

// === About Page ===
void handleAboutPage() {
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextFont(2);

  const char* title = "[About This Project]";
  tft.setCursor(10, 90);
  tft.println(title);

  int lineHeight = 30;
  int text_x = 10;
  int text_y = 130;

  const char* aboutLines[] = {
    "- BlackWireV1",
    "- Edited by: 21keju",
    "- Version: 1.1.0",
    "- Contact: realkeju@icloud.com",
    "- GitHub: github.com/BlackWire",
    "- Website: BlackWire.net"
  };

  for (int i = 0; i < 6; i++) {
    tft.setCursor(text_x, text_y);
    tft.println(aboutLines[i]);
    text_y += lineHeight;
  }

  delay(1000);
  while (!ts.touched()) delay(10);
  while (ts.touched()) delay(10);

  displayMenu();
}


