#ifndef SHARED_H
#define SHARED_H

// ─── Global State ─────────────────────────────────────────────
extern int current_menu_index;
extern int current_submenu_index;
extern bool in_sub_menu;
extern bool is_main_menu;
extern bool feature_active;
extern bool feature_exit_requested;
extern bool submenu_initialized;
extern bool menu_initialized;
extern unsigned long last_interaction_time;

// ─── Battery ──────────────────────────────────────────────────
extern float currentBatteryVoltage;

// ─── Touchscreen Calibration ──────────────────────────────────
#define TS_MINX 100
#define TS_MAXX 900
#define TS_MINY 100
#define TS_MAXY 900
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// ─── UI Layout ────────────────────────────────────────────────
//#define X_OFFSET_LEFT 20
//#define X_OFFSET_RIGHT 130
//#define Y_START 40
//#define Y_SPACING 70
//#define COLUMN_WIDTH 120
//#define NUM_MENU_ITEMS 8  // Keep as macro if not redefining elsewhere

// ─── Submenu Items ────────────────────────────────────────────
// Use pointer-to-pointer for compatibility with dynamic assignment
extern const char** active_submenu_items;
extern const unsigned char** active_submenu_icons;
extern int active_submenu_size;

// ─── UI Color Constants (RGB565 format) ───────────────────────
#define ORANGE         0xFBE4
#define GRAY           0x8410
#define BLUE           0x001F
#define RED            0xF800
#define GREEN          0x07E0
#define BLACK          0x0000
#define WHITE          0xFFFF
#define LIGHT_GRAY     0xC618
#define DARK_GRAY      0x4208
#define TFTWHITE       WHITE
#define TFT_DARKBLUE   DARK_GRAY
#define SELECTED_ICON_COLOR ORANGE

#define TFT_GRAY GRAY

// ─── Function Prototypes ──────────────────────────────────────
void displayMenu();
void displaySubmenu();
void updateActiveSubmenu();
void waitForTouchRelease();
void manageBacklight();
void drawStatusBar(float voltage, bool showBattery);
float readBatteryVoltage();

void activateReplayAttack();
void activateSubJammer();
void activateProfileSaver();
void exitSubMenu();

void launchFeature(void (*setupFunc)(), void (*loopFunc)());
void handleSubmenuTouch();
void handleMainMenuTouch(uint16_t x, uint16_t y);
void displayLogo(uint16_t color, int duration);
void loading(int percent, uint16_t color, int x, int y, int size, bool showText);

#endif // SHARED_H
