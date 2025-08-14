#include <TFT_eSPI.h>       // TFT display
#include <SPI.h>
#include <SD.h>

// TFT instance
TFT_eSPI tft = TFT_eSPI();  // Uses User_Setup.h

// RGB LED pins
#define RED_LED_PIN    4
#define GREEN_LED_PIN 16
#define BLUE_LED_PIN  17

// SD card CS pin
#define SD_CS 5

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting CYD2.4R Test...");

  // RGB LED setup
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(BLUE_LED_PIN, HIGH);

  // TFT setup
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 100);
  tft.println("CYD2.4R Test Sketch");

  // RGB LED test
  digitalWrite(RED_LED_PIN, LOW); delay(300);
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, LOW); delay(300);
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(BLUE_LED_PIN, LOW); delay(300);
  digitalWrite(BLUE_LED_PIN, HIGH);

  // SD card test
  if (!SD.begin(SD_CS)) {
    tft.setCursor(20, 140);
    tft.setTextColor(TFT_RED);
    tft.println("SD Card: Not Detected");
  } else {
    tft.setCursor(20, 140);
    tft.setTextColor(TFT_GREEN);
    tft.println("SD Card: OK");
  }

  // Touch test prompt
  tft.setCursor(20, 180);
  tft.setTextColor(TFT_YELLOW);
  tft.println("Touch screen to test...");
}

void loop() {
  // Resistive touch test
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    tft.fillCircle(x, y, 5, TFT_BLUE);
    delay(300);
    tft.fillCircle(x, y, 5, TFT_BLACK);
  }
  delay(100);
}
