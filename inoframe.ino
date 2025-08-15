#include <arduino.h>
#include <tft_espi.h>
#include <wire.h>
#include <pcf8574.h>
#include "Touchscreen.h"
#include "wificonfig.h"
#include "bleconfig.h"
#include "subconfig.h"
#include "utils.h"
#include "shared.h"
#include "icon.h"

// TFT Display Setup
TFT_eSPI tft = TFT_eSPI();
Touchscreen ts;  // Touchscreen instance

// Touchscreen Calibration (adjust these values based on your screen)
uint16_t calData[5] = {275, 3620, 264, 3532, 1};  // Example calibration values

// Button Configuration
#define pcf_ADDR 0x20
PCF8574 pcf(pcf_ADDR);

#define BTN_UP     6
#define BTN_DOWN   3
#define BTN_LEFT   4
#define BTN_RIGHT  5
#define BTN_SELECT 7

// Menu System Variables
bool feature_exit_requested = false;
int current_menu_index = 0;
int current_submenu_index = 0;
bool in_sub_menu = false;
bool is_main_menu = false;
bool feature_active = false;

// Touch Screen Mapping (adjust based on your screen orientation)
#define TS_MINX 150
#define TS_MAXX 3800
#define TS_MINY 120
#define TS_MAXY 3750

void setupTouchscreen() {
  tft.setTouch(calData);
  Serial.println("Touchscreen initialized");
}

// Menu and Submenu Items (unchanged)
// [Keep all your existing menu/submenu item definitions here]

void updateActiveSubmenu() {
  // [Keep your existing updateActiveSubmenu implementation]
}

// Helper Functions
bool isButtonPressed(int buttonPin) {
  return !pcf.digitalRead(buttonPin);
}

// Display Functions
void displayMenu() {
  // [Keep your existing displayMenu implementation]
}

void displaySubmenu() {
  // [Keep your existing displaySubmenu implementation]
}

// Touch Handlers
void handleTouch() {
  static unsigned long lastTouchTime = 0;
  const unsigned long debounceDelay = 200; // Touch debounce in ms
  
  if (millis() - lastTouchTime &lt; debounceDelay) return;
  
  if (ts.touched() &amp;&amp; !feature_active) {
    TS_Point p = ts.getPoint();
    lastTouchTime = millis();
    
    // Map touch coordinates to screen
    int x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    int y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    
    Serial.printf("Touch detected at x=%d, y=%d\n", x, y);
    
    if (in_sub_menu) {
      // Handle submenu touch
      for (int i = 0; i &lt; active_submenu_size; i++) {
        int yPos = 30 + i * 30;
        if (i == active_submenu_size - 1) yPos += 10;
        
        if (x &gt;= 10 &amp;&amp; x &lt;= 110 &amp;&amp; 
            y &gt;= yPos &amp;&amp; y &lt;= yPos + (i == active_submenu_size - 1 ? 40 : 30)) {
          current_submenu_index = i;
          last_interaction_time = millis();
          
          // Visual feedback for touch
          tft.fillCircle(x, y, 5, TFT_WHITE);
          delay(100);
          tft.fillCircle(x, y, 5, TFT_BLACK);
          
          // Handle selection
          if (isButtonPressed(BTN_SELECT)) {
            // [Handle submenu selection as in your existing code]
          }
          break;
        }
      }
    } else {
      // Handle main menu touch
      for (int i = 0; i &lt; NUM_MENU_ITEMS; i++) {
        int column = i / 4;
        int row = i % 4;
        int x_position = (column == 0) ? 10 : 130;
        int y_position = 30 + row * 75;
        
        if (x &gt;= x_position &amp;&amp; x &lt;= x_position + 100 &amp;&amp; 
            y &gt;= y_position &amp;&amp; y &lt;= y_position + 60) {
          current_menu_index = i;
          last_interaction_time = millis();
          
          // Visual feedback for touch
          tft.fillCircle(x, y, 10, TFT_WHITE);
          delay(100);
          tft.fillCircle(x, y, 10, TFT_BLACK);
          
          updateActiveSubmenu();
          if (active_submenu_items &amp;&amp; active_submenu_size &gt; 0) {
            current_submenu_index = 0;
            in_sub_menu = true;
            submenu_initialized = false;
            displaySubmenu();
          }
          break;
        }
      }
    }
  }
}

// Button Handlers (unchanged)
// [Keep all your existing button handler functions]
// Just add handleTouch() calls where appropriate

void setup() {
  Serial.begin(115200);
  
  // Initialize TFT
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  // Initialize Touchscreen
  setupTouchscreen();
  
  // Initialize buttons
  pcf.begin();
  pcf.pinMode(BTN_UP, INPUT_PULLUP);
  pcf.pinMode(BTN_DOWN, INPUT_PULLUP);
  pcf.pinMode(BTN_LEFT, INPUT_PULLUP);
  pcf.pinMode(BTN_RIGHT, INPUT_PULLUP);
  pcf.pinMode(BTN_SELECT, INPUT_PULLUP);

  // Show logo and initialize menu
  displayLogo(TFT_WHITE, 2000);
  displayMenu();
  drawStatusBar(readBatteryVoltage(), false);
  last_interaction_time = millis();
}

void loop() {
  handleButtons();
  handleTouch();  // Add this to handle touch events
  updateStatusBar();
  
  // Optional: Add touch debugging
  if (Serial.available()) {
    if (Serial.read()) {
      TS_Point p = ts.getPoint();
      Serial.printf("Raw Touch: X=%d, Y=%d, Z=%d\n", p.x, p.y, p.z);
      Serial.printf("Mapped: X=%d, Y=%d\n", 
                   map(p.x, TS_MINX, TS_MAXX, 0, tft.width()),
                   map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
    }
  }
}
