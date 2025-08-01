#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // TFT instance

// Optional: only use if calibration improves accuracy
// uint16_t calData[5] = { 345, 678, 123, 456, 789 };

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  // tft.setTouch(calData);  // Optional
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Touch test...");
}

void loop() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    Serial.printf("Touch at x=%d y=%d\n", x, y);
    tft.fillCircle(x, y, 5, TFT_RED);
    delay(300);
  }
}
