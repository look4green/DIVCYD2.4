// BlackWireV1.ino
// This file contains the main Arduino setup() and loop() functions,
// as well as all global variable definitions and core UI logic.

#include <Arduino.h>
#include <TFT_eSPI.h> // TFT_eSPI includes its own touch driver
#include <Wire.h>
#include <SPI.h>      // SPI is needed for both TFT and Touch communication
#include <WiFi.h>     // Required for WiFi functions
#include <SD.h>       // Required for SD card functionality

// Your project's custom header files
// These include definitions for pins, constants, and function declarations.
#include "config.h"    // Hardware pin definitions, BACKLIGHT_TIMEOUT, BUILD_VERSION, SD_CS
#include "shared.h"    // Global menu item counts, custom colors, extern global variables
#include "wificonfig.h" // WiFi credentials and wificonfig namespace declarations, Terminal namespace
#include "bleconfig.h"  // Placeholder: defines Bluetooth feature functions
#include "nrfconfig.h"  // Placeholder: defines NRF (2.4GHz) feature functions
#include "subconfig.h"  // Placeholder: defines SubGHz feature functions

#include "utils.h"   // Utility functions (notifications, status bar, loading, logo, waitForTouchRelease)
#include "icon.h"    // Extern declarations for bitmap icons

// --- GLOBAL INSTANTIATIONS and DEFINITIONS ---
// The TFT_eSPI object is instantiated here once. Other files access it via 'extern TFT_eSPI tft;'
TFT_eSPI tft = TFT_eSPI();

// --- Global State Variables Definitions ---
// These variables are defined here as they are global to the entire application.
// They are declared 'extern' in shared.h for other files to access them.
bool feature_exit_requested = false; // Flag to request exit from an active feature or submenu "Back"
bool feature_active = false;         // Flag indicating if a feature (e.g., Terminal) is currently running
bool in_sub_menu = false;            // Flag indicating if the system is currently navigating a submenu
bool is_main_menu = false;           // Flag indicating if the main menu is currently displayed
bool submenu_initialized = false;    // Flag to track if the current submenu has been drawn
bool menu_initialized = false;       // Flag to track if the main menu has been drawn

int current_menu_index = 0;          // Index of the currently selected item in the main menu
int current_submenu_index = 0;       // Index of the currently selected item in a submenu

// Battery voltage and last interaction time (for backlight management)
unsigned long last_interaction_time = 0; // Timestamp of the last user interaction

// Variables to track last selected items for efficient redrawing
int last_submenu_index = -1;
int last_menu_index = -1;

// --- Menu Data Definitions (array definitions, sizes are from shared.h) ---
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

// This array of pointers to bitmaps is defined here.
// The individual bitmaps (e.g., bitmap_icon_wifi) are extern and defined in icon.cpp.
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

// --- Submenu Data Definitions (array definitions, sizes are from shared.h) ---
const char *submenu_items[NUM_SUBMENU_ITEMS] = {
    "Packet Monitor", "Beacon Spammer", "WiFi Deauther", "Deauth Detector",
    "WiFi Scanner", "Captive Portal", "Back to Main Menu"
};

const char *bluetooth_submenu_items[bluetooth_NUM_SUBMENU_ITEMS] = {
    "BLE Jammer", "BLE Spoofer", "Sour Apple", "Sniffer",
    "BLE Scanner", "Back to Main Menu"
};

const char *nrf_submenu_items[nrf_NUM_SUBMENU_ITEMS] = {
    "Scanner", "Analyzer [Coming soon]", "WLAN Jammer [Coming soon]",
    "Proto Kill", "Back to Main Menu"
};

const char *subghz_submenu_items[subghz_NUM_SUBMENU_ITEMS] = {
    "Replay Attack", "Bruteforce [Coming soon]", "SubGHz Jammer",
    "Saved Profile", "Back to Main Menu"
};

const char *tools_submenu_items[tools_NUM_SUBMENU_ITEMS] = {
    "Serial Monitor", "Update Firmware", "Back to Main Menu"
};

const char *ir_submenu_items[ir_NUM_SUBMENU_ITEMS] = {
    "Record [Coming soon]", "Saved Profile [Coming soon]", "Back to Main Menu"
};

const char *about_submenu_items[about_NUM_SUBMENU_ITEMS] = {
    "Back to Main Menu"
};

// Arrays of pointers to submenu icons (actual icons are extern and defined in icon.cpp)
const unsigned char *wifi_submenu_icons[NUM_SUBMENU_ITEMS] = {
    bitmap_icon_wifi,        // Packet Monitor (reusing main menu icon for simplicity)
    bitmap_icon_antenna,     // Beacon Spammer
    bitmap_icon_wifi_jammer, // WiFi Deauther
    bitmap_icon_eye2,        // Deauth Detector
    bitmap_icon_jammer,      // WiFi Scanner (reusing jammer icon for now)
    bitmap_icon_bash,        // Captive Portal
    bitmap_icon_go_back
};

const unsigned char *bluetooth_submenu_icons[bluetooth_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_ble_jammer, // BLE Jammer
    bitmap_icon_spoofer,    // BLE Spoofer (reusing spoofer icon)
    bitmap_icon_apple,      // Sour Apple
    bitmap_icon_analyzer,   // Analyzer (reusing analyzer icon)
    bitmap_icon_graph,      // BLE Scanner
    bitmap_icon_go_back
};

const unsigned char *nrf_submenu_icons[nrf_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_scanner,    // Scanner
    bitmap_icon_question,   // Analyzer [Coming soon]
    bitmap_icon_question,   // WLAN Jammer [Coming soon]
    bitmap_icon_kill,       // Proto Kill (reusing kill icon for now)
    bitmap_icon_go_back
};

const unsigned char *subghz_submenu_icons[subghz_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_antenna,   // Replay Attack
    bitmap_icon_question,  // Bruteforce [Coming soon]
    bitmap_icon_no_signal, // SubGHz Jammer
    bitmap_icon_list,      // Saved Profile
    bitmap_icon_go_back
};

const unsigned char *tools_submenu_icons[tools_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_bash,     // Serial Monitor
    bitmap_icon_follow,   // Update Firmware
    bitmap_icon_go_back
};

const unsigned char *ir_submenu_icons[ir_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_question, // Record [Coming soon]
    bitmap_icon_question, // Saved Profile [Coming soon]
    bitmap_icon_go_back
};

const unsigned char *about_submenu_icons[about_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_go_back
};

// --- DEFINITIONS for active submenu pointers ---
// These variables are declared extern in shared.h, and defined here once.
// They must be initialized with default values.
const char **active_submenu_items = nullptr;
int active_submenu_size = 0;
const unsigned char **active_submenu_icons = nullptr;

// --- Constants for Menu Display Layout ---
// Defined once here to avoid redefinition errors.
const int COLUMN_WIDTH = 120;
const int X_OFFSET_LEFT = 10;
const int X_OFFSET_RIGHT = X_OFFSET_LEFT + COLUMN_WIDTH;
const int Y_START = 30;
const int Y_SPACING = 75;


// --- Forward Declarations for Functions Defined in this .ino file ---
void setupTouchscreen();
bool isButtonPressed(int buttonPin); // This function will always return false now
bool touchZonePressed(String zone);
// waitForTouchRelease is now in utils.cpp/utils.h
void updateActiveSubmenu();
void displaySubmenu();
void displayMenu();
void handleIRSubmenuButtons();
void handleAboutPage();
void handleWiFiSubmenuButtons();
void handleBluetoothSubmenuButtons();
void handleNRFSubmenuButtons();
void handleSubGHzSubmenuButtons();
void handleToolsSubmenuButtons();
void handleButtonsAndTouch();
void setup(); // Arduino setup()
void loop();  // Arduino loop()

// --- Helper Function: setupTouchscreen ---
// Configures the touchscreen, relying on User_Setup.h for main driver config.
void setupTouchscreen() {
    Serial.println("setupTouchscreen: Initializing touchscreen (relying on User_Setup.h).");
    tft.setSwapBytes(true); // Ensure byte order is correct if images appear inverted

    // Crucial: Re-introducing the touch calibration/initialization call
    // This is needed for the touch controller to become active and respond.
    // These are placeholder values. Your actual User_Setup.h configuration or a full
    // touch calibration sketch will provide more accurate values.
    uint16_t calData[5] = { 300, 3600, 270, 3600, 7 }; // Example calibration data (minX, maxX, minY, maxY, orientation)
    tft.setTouch(calData); // Configure and apply touch calibration

    Serial.println("setupTouchscreen: Touchscreen initialization complete.");
}


// --- Utility Functions (Local to .ino if not in utils.cpp) ---

// This function now always returns false, as physical buttons are removed.
bool isButtonPressed(int buttonPin) {
    return false;
}

// Checks if a touch occurred within a predefined screen zone
bool touchZonePressed(String zone) {
    uint16_t x, y;
    if (!tft.getTouch(&x, &y)) return false;

    // Adjust these based on your screen rotation and specific UI layout
    if (zone == "UP") return (y < 80);
    if (zone == "DOWN") return (y > 240);
    if (zone == "LEFT") return (x < 80);
    if (zone == "RIGHT") return (x > 240);
    if (zone == "SELECT") return (y >= 80 && y <= 240);
    return false;
}


// --- Menu Management Functions ---

// Updates the pointers to the active submenu items and icons based on the main menu selection
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
        case 4: // IR
            active_submenu_items = ir_submenu_items;
            active_submenu_size = ir_NUM_SUBMENU_ITEMS;
            active_submenu_icons = ir_submenu_icons;
            break;
        case 5: // Tools
            active_submenu_items = tools_submenu_items;
            active_submenu_size = tools_NUM_SUBMENU_ITEMS;
            active_submenu_icons = tools_submenu_icons;
            break;
        case 6: // Setting - No submenu items defined, so direct action or nothing
            active_submenu_items = nullptr; // Explicitly set to nullptr as there's no submenu
            active_submenu_size = 0;
            active_submenu_icons = nullptr;
            break;
        case 7: // About
            active_submenu_items = about_submenu_items;
            active_submenu_size = about_NUM_SUBMENU_ITEMS;
            active_submenu_icons = about_submenu_icons;
            break;

        default:
            active_submenu_items = nullptr;
            active_submenu_size = 0;
            active_submenu_icons = nullptr;
            break;
    }
}

// Draws or updates the currently active submenu on the display
void displaySubmenu() {
    Serial.println("displaySubmenu: Drawing submenu.");
    // Reset main menu flags as we're entering a submenu
    menu_initialized = false;
    last_menu_index = -1;

    tft.setTextFont(2);
    tft.setTextSize(1);

    // If the submenu hasn't been drawn yet, clear screen and draw all items
    if (!submenu_initialized) {
        tft.fillScreen(TFT_BLACK); // Clear screen with black background

        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30; // Vertical position for each item
            if (i == active_submenu_size - 1) yPos += 10; // Extra spacing for "Back" button

            // Set text color (white for unselected, black background)
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            // Draw icon
            tft.drawBitmap(10, yPos, active_submenu_icons[i], 16, 16, TFT_WHITE);
            // Print separator for regular items
            tft.setCursor(30, yPos);
            if (i < active_submenu_size - 1) {
                tft.print("| ");
            }
            // Print submenu item text
            tft.print(active_submenu_items[i]);
        }

        submenu_initialized = true; // Mark submenu as drawn
        last_submenu_index = -1;    // Reset last index for initial selection highlight
    }

    // Only redraw if the selected item has changed
    if (last_submenu_index != current_submenu_index) {
        // Redraw previous item to remove highlight
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

        // Draw new selected item with highlight
        int new_yPos = 30 + current_submenu_index * 30;
        if (current_submenu_index == active_submenu_size - 1) new_yPos += 10;

        tft.setTextColor(ORANGE, TFT_BLACK); // Highlight with ORANGE color
        tft.drawBitmap(10, new_yPos, active_submenu_icons[current_submenu_index], 16, 16, ORANGE);
        tft.setCursor(30, new_yPos);
        if (current_submenu_index < active_submenu_size - 1) {
            tft.print("| ");
        }
        tft.print(active_submenu_items[current_submenu_index]);

        last_submenu_index = current_submenu_index; // Update last selected index
    }
}


// Draws or updates the main menu on the display
void displayMenu() {
    Serial.println("displayMenu: Drawing main menu.");
    // Manage backlight based on user inactivity
    handleBacklight();

    // Reset submenu flags as we're entering the main menu
    submenu_initialized = false;
    last_submenu_index = -1;
    tft.setTextFont(2); // Set font for menu items

    // Colors for menu icons
    const uint16_t icon_colors[NUM_MENU_ITEMS] = {
        TFT_WHITE, // WiFi
        TFT_WHITE, // Bluetooth
        TFT_WHITE, // 2.4GHz
        TFT_WHITE, // SubGHz
        TFT_WHITE, // IR Remote
        TFT_WHITE, // Tools
        TFT_RED,   // Setting
        TFT_WHITE  // About
    };

    // If the menu hasn't been drawn yet, clear screen and draw all items
    if (!menu_initialized) {
        tft.fillScreen(0x20e4); // Background color for the main menu

        for (int i = 0; i < NUM_MENU_ITEMS; i++) {
            int column = i / 4; // Determine column (0 or 1)
            int row = i % 4;    // Determine row (0-3)
            int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT; // X position for the menu item
            int y_position = Y_START + row * Y_SPACING; // Y position for the menu item

            // Draw rounded rectangle background for the item
            tft.fillRoundRect(x_position, y_position, 100, 60, 5, TFT_DARKBLUE);
            // Draw border (unselected)
            tft.drawRoundRect(x_position, y_position, 100, 60, 5, TFT_DARKGREY);
            // Draw icon
            tft.drawBitmap(x_position + 42, y_position + 10, bitmap_icons[i], 16, 16, icon_colors[i]);

            // Set text color (white for unselected, dark blue background)
            tft.setTextColor(TFT_WHITE, TFT_DARKBLUE);
            // Calculate text width and position for centering
            int textWidth = tft.textWidth(menu_items[i]); // Using tft.textWidth for accurate width
            int textX = x_position + (100 - textWidth) / 2;
            int textY = y_position + 30;
            tft.setCursor(textX, textY);
            tft.print(menu_items[i]);
        }
        menu_initialized = true; // Mark menu as drawn
        last_menu_index = -1;    // Reset last index for initial selection highlight
    }

    // Only redraw if the selected item has changed
    if (last_menu_index != current_menu_index) {
        // Redraw previous item to remove highlight
        for (int i = 0; i < NUM_MENU_ITEMS; i++) {
            int column = i / 4;
            int row = i % 4;
            int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
            int y_position = Y_START + row * Y_SPACING;

            if (i == last_menu_index) {
                tft.fillRoundRect(x_position, y_position, 100, 60, 5, TFT_DARKBLUE);
                tft.drawRoundRect(x_position, y_position, 100, 60, 5, TFT_DARKGREY);
                tft.setTextColor(TFT_WHITE, TFT_DARKBLUE);
                tft.drawBitmap(x_position + 42, y_position + 10, bitmap_icons[last_menu_index], 16, 16, icon_colors[last_menu_index]);
                int textWidth = tft.textWidth(menu_items[last_menu_index]);
                int textX = x_position + (100 - textWidth) / 2;
                int textY = y_position + 30;
                tft.setCursor(textX, textY);
                tft.print(menu_items[last_menu_index]);
            }
        }

        // Draw new selected item with highlight
        int column = current_menu_index / 4;
        int row = current_menu_index % 4;
        int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
        int y_position = Y_START + row * Y_SPACING;

        tft.fillRoundRect(x_position, y_position, 100, 60, 5, TFT_DARKBLUE);
        tft.drawRoundRect(x_position, y_position, 100, 60, 5, ORANGE); // Highlight with ORANGE border

        tft.setTextColor(ORANGE, TFT_DARKBLUE); // Highlight with ORANGE text, background TFT_DARKBLUE
        tft.drawBitmap(x_position + 42, y_position + 10, bitmap_icons[current_menu_index], 16, 16, SELECTED_ICON_COLOR);
        int textWidth = tft.textWidth(menu_items[current_menu_index]);
        int textX = x_position + (100 - textWidth) / 2;
        int textY = y_position + 30;
        tft.setCursor(textX, textY);
        tft.print(menu_items[current_menu_index]);

        last_menu_index = current_menu_index; // Update last selected index
    }
}

// Placeholder for IR submenu button/touch handling
void handleIRSubmenuButtons() {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY) && !feature_active) {
        Serial.println("handleIRSubmenuButtons: Touch detected.");
        updateLastInteractionTime();
        waitForTouchRelease(); // Debounce and wait for release

        int x = touchX;
        int y = touchY;
        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30;
            if (i == active_submenu_size - 1) yPos += 10;
            int button_x1 = 10;
            int button_y1 = yPos;
            int button_x2 = 10 + tft.textWidth(active_submenu_items[i]) + 20;
            int button_y2 = yPos + 25;
            if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
                current_submenu_index = i;
                displaySubmenu();
                if (current_submenu_index == (ir_NUM_SUBMENU_ITEMS - 1)) {
                    Serial.println("handleIRSubmenuButtons: 'Back to Main Menu' selected.");
                    feature_exit_requested = true; // Signal to exit submenu
                } else {
                    showNotification("IR Remote", "Feature coming soon!");
                    delay(2000); // Keep delay for notification display
                    hideNotification();
                    displaySubmenu();
                }
                break;
            }
        }
    }
}


// Placeholder for WiFi submenu button/touch handling
void handleWiFiSubmenuButtons() {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY) && !feature_active) {
        Serial.println("handleWiFiSubmenuButtons: Touch detected.");
        updateLastInteractionTime();
        waitForTouchRelease(); // Debounce and wait for release

        int x = touchX;
        int y = touchY;
        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30;
            if (i == active_submenu_size - 1) yPos += 10;
            int button_x1 = 10;
            int button_y1 = yPos;
            int button_x2 = 10 + tft.textWidth(active_submenu_items[i]) + 20;
            int button_y2 = yPos + 25;
            if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
                current_submenu_index = i;
                displaySubmenu();
                if (current_submenu_index == (NUM_SUBMENU_ITEMS - 1)) {
                    Serial.println("handleWiFiSubmenuButtons: 'Back to Main Menu' selected.");
                    feature_exit_requested = true; // Signal to exit submenu
                } else {
                    Serial.print("handleWiFiSubmenuButtons: Starting WiFi feature index: ");
                    Serial.println(current_submenu_index);
                    wificonfig::startWiFiFeature(current_submenu_index); // Call the namespace function
                    showNotification("WiFi Feature", "Feature active!"); // Example for active feature
                    feature_active = true; // Set feature_active to true
                    updateLastInteractionTime(); // Reset interaction time
                }
                break;
            }
        }
    }
}

// Placeholder for Bluetooth submenu button/touch handling
void handleBluetoothSubmenuButtons() {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY) && !feature_active) {
        Serial.println("handleBluetoothSubmenuButtons: Touch detected.");
        updateLastInteractionTime();
        waitForTouchRelease(); // Debounce and wait for release

        int x = touchX;
        int y = touchY;
        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30;
            if (i == active_submenu_size - 1) yPos += 10;
            int button_x1 = 10;
            int button_y1 = yPos;
            int button_x2 = 10 + tft.textWidth(active_submenu_items[i]) + 20;
            int button_y2 = yPos + 25;
            if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
                current_submenu_index = i;
                displaySubmenu();
                if (current_submenu_index == (bluetooth_NUM_SUBMENU_ITEMS - 1)) {
                    Serial.println("handleBluetoothSubmenuButtons: 'Back to Main Menu' selected.");
                    feature_exit_requested = true;
                } else {
                    Serial.print("handleBluetoothSubmenuButtons: Starting BLE feature index: ");
                    Serial.println(current_submenu_index);
                    bleconfig::startBleFeature(current_submenu_index);
                    showNotification("Bluetooth Feature", "Feature active!");
                    feature_active = true;
                    updateLastInteractionTime();
                }
                break;
            }
        }
    }
}

// Placeholder for NRF (2.4GHz) submenu button/touch handling
void handleNRFSubmenuButtons() {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY) && !feature_active) {
        Serial.println("handleNRFSubmenuButtons: Touch detected.");
        updateLastInteractionTime();
        waitForTouchRelease(); // Debounce and wait for release

        int x = touchX;
        int y = touchY;
        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30;
            if (i == active_submenu_size - 1) yPos += 10;
            int button_x1 = 10;
            int button_y1 = yPos;
            int button_x2 = 10 + tft.textWidth(active_submenu_items[i]) + 20;
            int button_y2 = yPos + 25;
            if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
                current_submenu_index = i;
                displaySubmenu();
                if (current_submenu_index == (nrf_NUM_SUBMENU_ITEMS - 1)) {
                    Serial.println("handleNRFSubmenuButtons: 'Back to Main Menu' selected.");
                    feature_exit_requested = true;
                } else {
                    Serial.print("handleNRFSubmenuButtons: Starting NRF feature index: ");
                    Serial.println(current_submenu_index);
                    // nrfconfig::startNrfFeature(current_submenu_index); // This call needs nrfconfig namespace to be known
                    showNotification("2.4GHz Feature", "Feature coming soon!"); // Temporary, as nrfconfig is not defined yet
                    delay(2000); hideNotification(); displaySubmenu();
                    feature_active = false; // Set feature_active to false if coming soon
                    updateLastInteractionTime();
                }
                break;
            }
        }
    }
}

// Handles button presses and touch input for SubGHz submenu
void handleSubGHzSubmenuButtons() {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY) && !feature_active) {
        Serial.println("handleSubGHzSubmenuButtons: Touch detected.");
        updateLastInteractionTime();
        waitForTouchRelease(); // Debounce and wait for release

        int x = touchX;
        int y = touchY;

        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30;
            if (i == active_submenu_size - 1) yPos += 10;

            int button_x1 = 10;
            int button_y1 = yPos;
            int button_x2 = 10 + tft.textWidth(active_submenu_items[i]) + 20;
            int button_y2 = yPos + 25;

            if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
                current_submenu_index = i;
                displaySubmenu();
                if (current_submenu_index == (subghz_NUM_SUBMENU_ITEMS - 1)) { // "Back to Main Menu"
                    Serial.println("handleSubGHzSubmenuButtons: 'Back to Main Menu' selected.");
                    feature_exit_requested = true; // Signal to exit submenu
                } else {
                    // Temporarily replace actual feature calls with a "Coming soon!" notification
                    // subconfig::startSubGHzFeature(current_submenu_index); // Call the namespace function
                    showNotification("SubGHz Feature", "Feature coming soon!");
                    delay(2000); // Keep delay for notification display
                    hideNotification();
                    displaySubmenu();
                    feature_active = false; // Set feature_active to false if coming soon
                }
                break; // Exit loop after a button is handled
            }
        }
    }
}

// Placeholder for Tools submenu button/touch handling
void handleToolsSubmenuButtons() {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY) && !feature_active) {
        Serial.println("handleToolsSubmenuButtons: Touch detected.");
        updateLastInteractionTime();
        waitForTouchRelease(); // Debounce and wait for release

        int x = touchX;
        int y = touchY;
        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30;
            if (i == active_submenu_size - 1) yPos += 10;
            int button_x1 = 10;
            int button_y1 = yPos;
            int button_x2 = 10 + tft.textWidth(active_submenu_items[i]) + 20;
            int button_y2 = yPos + 25;
            if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
                current_submenu_index = i;
                displaySubmenu();
                if (current_submenu_index == (tools_NUM_SUBMENU_ITEMS - 1)) {
                    Serial.println("handleToolsSubmenuButtons: 'Back to Main Menu' selected.");
                    feature_exit_requested = true;
                } else if (current_submenu_index == 0) { // Serial Monitor
                    Serial.println("handleToolsSubmenuButtons: Starting Terminal feature.");
                    feature_active = true;
                    Terminal::terminalSetup();
                    updateLastInteractionTime();
                } else {
                    showNotification("Tools Feature", "Feature coming soon!");
                    delay(2000); // Keep delay for notification display
                    hideNotification();
                    displaySubmenu();
                    feature_active = false; // Set feature_active to false if coming soon
                }
                break;
            }
        }
    }
}


// Handles the "About" page display and navigation
void handleAboutPage() {
    tft.setTextColor(TFT_GREEN, TFT_BLACK); // Text color for the About page
    tft.setTextSize(1);
    tft.setTextFont(2);

    const char* title = "[About This Project]";
    tft.setCursor(10, 90);
    tft.println(title);

    int lineHeight = 30;
    int text_x = 10;
    int text_y = 130;
    tft.setCursor(text_x, text_y);
    tft.print("- "); tft.println(BUILD_VERSION); // Use BUILD_VERSION from config.h, fixed concatenation
    text_y += lineHeight;
    tft.setCursor(text_x, text_y);
    tft.println("- Edited by: 21keju");
    text_y += lineHeight;
    tft.setCursor(text_x, text_y);
    tft.println("- Version: 1.1.0"); // Consider a BUILD_VERSION for this too
    text_y += lineHeight;
    tft.setCursor(text_x, text_y);
    tft.println("- Contact: realkeju@icloud.com");
    text_y += lineHeight;
    tft.setCursor(text_x, text_y);
    tft.println("- GitHub: github.com/BlackWire");
    text_y += lineHeight;
    tft.setCursor(text_x, text_y);
    tft.println("- Website: BlackWire.net");
    text_y += lineHeight;

    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY) && !feature_active) {
        Serial.println("handleAboutPage: Touch detected.");
        updateLastInteractionTime();
        waitForTouchRelease(); // Debounce and wait for release

        int x = touchX;
        int y = touchY;

        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30;
            if (i == active_submenu_size - 1) yPos += 10;

            // Define touchable area for the "Back" button
            int button_x1 = 10;
            int button_y1 = yPos;
            int button_x2 = 10 + tft.textWidth(active_submenu_items[i]) + 20;
            int button_y2 = yPos + 25;

            if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
                current_submenu_index = i;
                displaySubmenu(); // Redraw with selection
                if (current_submenu_index == (about_NUM_SUBMENU_ITEMS - 1)) {
                    Serial.println("handleAboutPage: 'Back to Main Menu' selected.");
                    feature_exit_requested = true; // Signal to exit submenu
                }
                break; // Exit loop after a button is handled
            }
        }
    }
}


// Manages overall button and touch input, delegating to sub-handlers
void handleButtonsAndTouch() {
    // If currently in a submenu
    if (in_sub_menu) {
        if (feature_active) {
          // If a specific feature is active, its loop will run.
          // The feature itself should set 'feature_exit_requested = true' to exit.
          // Note: Notification "Feature active!" is shown in the submenu handlers,
          // not here, to avoid constant re-showing.
            switch (current_menu_index) {
                case 0: // WiFi
                    wificonfig::runActiveWiFiFeature();
                    break;
                case 1: // Bluetooth (Placeholder, will need similar setup)
                    bleconfig::runActiveBleFeature();
                    break;
                // case 2: // NRF (2.4GHz) - Temporary "Coming soon!" handled by handleNRFSubmenuButtons
                //     nrfconfig::runActiveNrfFeature(); // Placeholder if NRF had a loop
                //     break;
                // case 3: // SubGHz - Temporary "Coming soon!" handled by handleSubGHzSubmenuButtons
                //     subconfig::runActiveSubGHzFeature(); // Placeholder if SubGHz had a loop
                //     break;
                case 5: // Tools menu, specifically for Serial Monitor
                    Terminal::terminalLoop();
                    break;
                default:
                    // For other features, if they somehow set feature_exit_requested, handle it.
                    if(feature_exit_requested) {
                        Serial.println("handleButtonsAndTouch: Generic feature requested exit.");
                    }
                    break;
            }
             // Check if feature requested exit during its loop (e.g., Terminal exiting)
            if (feature_exit_requested) {
                 Serial.println("handleButtonsAndTouch: Feature exited, returning to submenu.");
                 // Call the appropriate exit function for the active feature
                 if (current_menu_index == 0) { wificonfig::exitWiFiFeature(); }
                 if (current_menu_index == 1) { bleconfig::exitBleFeature(); }
                 // if (current_menu_index == 2) { nrfconfig::exitNrfFeature(); } // Placeholder
                 // if (current_menu_index == 3) { subconfig::exitSubGHzFeature(); } // Placeholder
                 if (current_menu_index == 5) { Terminal::exitTerminal(); }

                // After exiting the feature, reset state and return to submenu
                feature_active = false;
                feature_exit_requested = false;
                in_sub_menu = true; // Stay in the submenu after feature exit
                submenu_initialized = false; // Force redraw of submenu
                displaySubmenu();
                waitForTouchRelease(); // Ensure touch is released for clean transition
            }

        } else { // In submenu, but no feature is active (navigating submenu)
            switch (current_menu_index) {
                case 0: handleWiFiSubmenuButtons(); break;
                case 1: handleBluetoothSubmenuButtons(); break;
                case 2: handleNRFSubmenuButtons(); break;
                case 3: handleSubGHzSubmenuButtons(); break;
                case 4: handleIRSubmenuButtons(); break;
                case 5: handleToolsSubmenuButtons(); break;
                case 6: // Setting - direct action (no submenu)
                    // If 'Setting' had a submenu, its handler would go here.
                    // Currently, it's handled in main menu touch logic.
                    break;
                case 7: handleAboutPage(); break;
                default: break;
            }

            // If a submenu button (e.g., "Back to Main Menu") set feature_exit_requested
            // This specifically handles returning from a submenu to the main menu.
            if (feature_exit_requested) {
                Serial.println("handleButtonsAndTouch: Submenu 'Back to Main Menu' pressed, returning to main menu.");
                feature_exit_requested = false; // Consume the flag
                in_sub_menu = false; // Go back to main menu
                menu_initialized = false; // Force redraw of main menu
                displayMenu();
                waitForTouchRelease(); // Essential for clean transition
            }
        }
    } else { // Not in a submenu (we are in the main menu), handling main menu touch inputs

        // --- TFT_eSPI Touch Handling for Main Menu ---
        uint16_t touchX, touchY;
        // Check if the screen is touched and retrieve coordinates
        if (tft.getTouch(&touchX, &touchY)) {
            updateLastInteractionTime(); // Update last interaction time on any touch
            waitForTouchRelease(); // Ensure touch is released before processing

            int x = touchX;
            int y = touchY;

            for (int i = 0; i < NUM_MENU_ITEMS; i++) {
                int column = i / 4;
                int row = i % 4;
                int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
                int y_position = Y_START + row * Y_SPACING;

                // Define touchable area for each main menu item
                int button_x1 = x_position;
                int button_y1 = y_position;
                int button_x2 = x_position + 100; // Width of the menu item button
                int button_y2 = y_position + 60;  // Height of the menu item button

                // Check if touch is within the bounds of a menu item
                if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
                    current_menu_index = i;
                    Serial.print("handleButtonsAndTouch: Main menu item selected: ");
                    Serial.println(menu_items[current_menu_index]);
                    displayMenu(); // Update menu to show selection highlight

                    updateActiveSubmenu(); // Set up the correct submenu pointers

                    // Transition to submenu or perform direct action
                    if (active_submenu_items && active_submenu_size > 0) {
                        current_submenu_index = 0;
                        in_sub_menu = true;
                        submenu_initialized = false;
                        displaySubmenu();
                    } else {
                        // Handle direct actions for menu items without submenus (e.g., "Setting")
                        if (current_menu_index == 6) { // "Setting" menu item
                            showNotification("Settings", "Settings feature coming soon!");
                            delay(2000); // Keep delay for notification display
                            hideNotification();
                            displayMenu(); // Redraw menu after notification
                        }
                    }
                    // No delay here, waitForTouchRelease handles debouncing
                    break; // Exit loop after a button is handled
                }
            }
        }
    }
}


// --- Arduino Setup and Loop Functions ---

// Initializes the display, buttons, and initial UI
void setup() {
    Serial.begin(115200);
    Serial.println("Setup: Starting initialization...");

    Serial.println("Setup: Initializing TFT display...");
    tft.init();        // Initialize TFT display
    tft.setRotation(0); // Set display rotation
    tft.fillScreen(TFT_BLACK); // Clear screen
    Serial.println("Setup: TFT display initialized.");

    setupTouchscreen(); // This now includes tft.setTouch(calData)

    Serial.println("Setup: Displaying loading animation...");
    // Restore skull loading animation (2 repeats of 10 frames, 100ms delay per frame)
    loading(100, ORANGE, 0, 0, 2, true);
    Serial.println("Setup: Loading animation finished.");

    tft.fillScreen(TFT_BLACK); // Clear screen after loading

    Serial.println("Setup: Displaying startup logo...");
    // Restore CiferTech logo with original duration (2000ms)
    displayLogo(TFT_WHITE, 2000); // Changed back to 2000ms from 500ms as per prior instruction to be prominent
    Serial.println("Setup: Startup logo finished.");

    Serial.println("Setup: Configuring backlight pin...");
    // Configure backlight pin
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH); // Ensure backlight is on initially
    Serial.println("Setup: Backlight configured.");

    Serial.println("Setup: Physical button pins are now fully bypassed for touchscreen-only operation.");

    Serial.println("Setup: Initializing WiFi...");
    WiFi.mode(WIFI_STA); // Set WiFi to station mode
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Start connecting to WiFi
    Serial.print("Setup: Connecting to WiFi ");
    Serial.print(WIFI_SSID);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Try for ~10 seconds (20 * 500ms)
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nSetup: WiFi connected! IP: " + WiFi.localIP().toString());
    } else {
        Serial.println("\nSetup: Failed to connect to WiFi.");
    }
    Serial.println("Setup: WiFi initialization complete.");


    Serial.println("Setup: Displaying main menu...");
    displayMenu();
    drawStatusBar("Main Menu", feature_active); // Initial status bar draw
    Serial.println("Setup: Main menu displayed.");

    updateLastInteractionTime(); // Initialize interaction time
    Serial.println("Setup: Initialization complete. Entering loop()...");
}

// Main loop for continuous operation
void loop() {
    handleButtonsAndTouch();       // Process all button and touch inputs
    updateStatusBar();             // Update status bar periodically
    handleBacklight();             // Manage backlight

    // Handle a requested feature exit. This means a feature (like Terminal)
    // signaled to stop and return to its *submenu*.
    if (feature_exit_requested && feature_active) { // ONLY handle feature exits here
        Serial.println("LOOP: Feature exited, returning to submenu.");
        feature_active = false;
        feature_exit_requested = false;
        // After feature exit, we always return to the submenu it was called from
        in_sub_menu = true;
        submenu_initialized = false; // Force redraw of submenu
        displaySubmenu();
        waitForTouchRelease(); // Ensure touch is released before continuing
    }
    // Note: Exiting from a submenu (e.g., "Back to Main Menu")
    // is handled directly within handleButtonsAndTouch() by setting in_sub_menu = false.
}
