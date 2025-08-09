#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Wire.h>

TFT_eSPI tft = TFT_eSPI();  // TFT instance

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);  // Adjust rotation if needed
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);

  // Draw zone indicators
  tft.fillRect(0, 0, 320, 80, TFT_BLUE);     // UP zone
  tft.drawCentreString("UP ZONE", 160, 30, 2);

  tft.fillRect(0, 240, 320, 80, TFT_RED);    // DOWN zone
  tft.drawCentreString("DOWN ZONE", 160, 270, 2);
}

void loop() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    tft.fillRect(0, 100, 320, 120, TFT_BLACK);  // Clear middle area
    tft.setCursor(10, 120);
    tft.printf("Touch detected at: X=%d Y=%d\n", x, y);

    if (y < 80) {
      tft.drawCentreString("You touched: UP", 160, 150, 2);
    } else if (y > 240) {
      tft.drawCentreString("You touched: DOWN", 160, 150, 2);
    } else {
      tft.drawCentreString("You touched: CENTER", 160, 150, 2);
    }

    delay(300);  // Debounce delay
  }
}
