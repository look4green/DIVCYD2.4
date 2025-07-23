#include "Touchscreen.h"

#define XPT2046_CS   33    // Touch chip select
#define TOUCH_IRQ    27    // Use GPIO27 instead of 34 (safe and exposed)

SPIClass touchscreenSPI = SPIClass(HSPI); // Use HSPI for separate SPI bus
XPT2046_Touchscreen ts(XPT2046_CS, TOUCH_IRQ); // Correct CS and IRQ pins

bool feature_active = false; // Define the global feature_active flag

void setupTouchscreen() {
    touchscreenSPI.begin(18, 19, 22, XPT2046_CS); // CLK, MISO, MOSI, CS
    ts.begin(touchscreenSPI);  // Initialize touchscreen
    ts.setRotation(1);         // Match display orientation
}