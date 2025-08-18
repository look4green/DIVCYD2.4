#include <Arduino.h>
#include <TFT_eSPI.h>              // TFT display library
#include <XPT2046_Touchscreen.h>   // Resistive touch controller
#include <SPI.h>                   // For SD card
#include <SD.h>                    // SD card library

// Define pins
#define TOUCH_CS  33               // Touchscreen chip select
#define TOUCH_IRQ 36               // Touchscreen interrupt
#define SD_CS     5                // SD card chip select
#define TFT_BL    32               // Backlight control pin

// RGB LED pins (active LOW)
#define RED_PIN   4
#define GREEN_PIN 16
#define BLUE_PIN  17

// Initialize TFT and Touch
TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nCYD 2.4\" ESP32-2432S024R Hardware Test");

  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("CYD 2.4\" Test");

  // Turn on backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);  // HIGH = backlight ON

  // Initialize touchscreen
  ts.begin();
  ts.setRotation(1);
  tft.println("Touch: Initialized");

  // Initialize RGB LED
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
  tft.println("RGB LED: Ready");

  // Test SD card
  tft.print("SD Card: ");
  if (!SD.begin(SD_CS)) {
    tft.println("FAILED");
    Serial.println("SD Card initialization failed!");
  } else {
    tft.println("OK");
    Serial.println("SD Card initialized successfully.");

    // List root directory
    File root = SD.open("/");
    tft.println("Files:");
    printDirectory(root, 0);
  }

  // Display test pattern
  testDisplay();

  delay(1000);
}

void loop() {
  testRGB();
  testTouch();
  delay(500);
}

void testDisplay() {
  tft.fillScreen(TFT_RED);
  delay(500);
  tft.fillScreen(TFT_GREEN);
  delay(500);
  tft.fillScreen(TFT_BLUE);
  delay(500);
  tft.fillScreen(TFT_BLACK);

  // Gradient test
  for (int i = 0; i < 240; i++) {
    tft.drawFastVLine(i, 0, 320, tft.color565(i, 255 - i, (i + 128) % 255));
  }
  delay(1000);

  // Text test
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.println("Display Test Complete");
  tft.println("Now testing other hardware...");
}

void testRGB() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
  tft.fillRect(0, 200, 240, 40, TFT_RED);
  tft.setCursor(10, 210);
  tft.println("RED LED ON");
  delay(500);

  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, HIGH);
  tft.fillRect(0, 200, 240, 40, TFT_GREEN);
  tft.setCursor(10, 210);
  tft.println("GREEN LED ON");
  delay(500);

  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, LOW);
  tft.fillRect(0, 200, 240, 40, TFT_BLUE);
  tft.setCursor(10, 210);
  tft.println("BLUE LED ON");
  delay(500);

  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
  tft.fillRect(0, 200, 240, 40, TFT_BLACK);
}

void testTouch() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();

    int x = map(p.x, 200, 3700, 0, tft.width());
    int y = map(p.y, 240, 3700, 0, tft.height());

    tft.fillRect(0, 150, 240, 40, TFT_BLACK);
    tft.setCursor(10, 160);
    tft.print("Touch: ");
    tft.print(x);
    tft.print(", ");
    tft.print(y);
    tft.print("    ");

    tft.fillCircle(x, y, 5, TFT_YELLOW);

    Serial.print("Touch detected at: ");
    Serial.print(x);
    Serial.print(", ");
    Serial.println(y);
  }
}

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
