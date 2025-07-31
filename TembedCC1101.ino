#include <TFT_eSPI.h>
#include <RotaryEncoder.h>

#define ENCODER_INA 4
#define ENCODER_INB 5
#define ENCODER_KEY 0

TFT_eSPI tft = TFT_eSPI();
RotaryEncoder encoder(ENCODER_INA, ENCODER_INB, RotaryEncoder::LatchMode::TWO03);

const char* menuItems[] = {"Transmit", "Receive", "Settings"};
const int menuCount = sizeof(menuItems) / sizeof(menuItems[0]);

int currentIndex = 0;
int display_rotation = 3;
unsigned long lastPress = 0;

void setup() {
  pinMode(ENCODER_KEY, INPUT_PULLUP);
  tft.init();
  tft.setRotation(display_rotation);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  drawMenu();
}

void loop() {
  encoder.tick();

  static int lastIndex = -1;
  int newIndex = encoder.getPosition();

  if (newIndex != lastIndex) {
    currentIndex = (newIndex % menuCount + menuCount) % menuCount;
    drawMenu();
    lastIndex = newIndex;
  }

  if (digitalRead(ENCODER_KEY) == LOW && millis() - lastPress > 300) {
    lastPress = millis();
    handleSelection(currentIndex);
  }
}

void drawMenu() {
  tft.fillScreen(TFT_BLACK);

  // 💡 Left Pane – Display large icon box
  tft.fillRect(10, 30, 100, 100, TFT_DARKGREY); // Icon background
  tft.drawRect(10, 30, 100, 100, TFT_WHITE);    // Icon border

  tft.setCursor(20, 70);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.print(menuItems[currentIndex]);           // Temporary icon label

  // 📜 Right Pane – Scrollable menu
  for (int i = 0; i < menuCount; i++) {
    int y = 40 + i * 30;
    int menuX = 130;

    if (i == currentIndex) {
      tft.fillRect(menuX - 5, y - 5, 100, 25, TFT_BLUE);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
    } else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }

    tft.setCursor(menuX + 10, y);
    tft.print(menuItems[i]);
  }
}

void handleSelection(int index) {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 60);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.print("Selected: ");
  tft.println(menuItems[index]);
  delay(1000);
  drawMenu();
}
