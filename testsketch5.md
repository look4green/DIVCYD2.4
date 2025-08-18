#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Wire.h>

TFT_eSPI tft = TFT_eSPI();  // TFT and touch instance

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);  // Adjust if needed for landscape/portrait
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);

  // Draw zone indicators
  tft.fillRect(0, 0, 240, 80, TFT_BLUE);     // UP zone
  tft.drawCentreString("UP ZONE", 120, 30, 2);

  tft.fillRect(0, 240, 240, 80, TFT_RED);    // DOWN zone
  tft.drawCentreString("DOWN ZONE", 120, 270, 2);

  tft.drawCentreString("Touch the screen...", 120, 150, 2);
}

void loop() {
  uint16_t x, y;

  if (tft.getTouch(&x, &y)) {
    Serial.printf("Touch detected at: X=%d Y=%d\n", x, y);

    // Clear middle area
    tft.fillRect(0, 100, 240, 120, TFT_BLACK);
    tft.setCursor(10, 120);
    tft.printf("Touch: X=%d Y=%d", x, y);

    if (y < 80) {
      tft.drawCentreString("You touched: UP", 120, 150, 2);
    } else if (y > 240) {
      tft.drawCentreString("You touched: DOWN", 120, 150, 2);
    } else {
      tft.drawCentreString("You touched: CENTER", 120, 150, 2);
    }

    delay(300);  // Debounce
  }
}
