#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

namespace {
constexpr uint8_t kDisplayCount = 5;

constexpr uint8_t kTftMosi = 35;
constexpr uint8_t kTftSclk = 36;
constexpr uint8_t kTftDc = 37;
constexpr uint8_t kTftRst = 33;
constexpr uint8_t kTftBl = 34;
constexpr uint8_t kCsPins[kDisplayCount] = {1, 2, 3, 4, 5};

constexpr uint16_t kPanelWidth = 170;
constexpr uint16_t kPanelHeight = 320;

TFT_eSPI tft;

void selectDisplay(uint8_t index) {
  for (uint8_t i = 0; i < kDisplayCount; ++i) {
    digitalWrite(kCsPins[i], HIGH);
  }

  if (index < kDisplayCount) {
    digitalWrite(kCsPins[index], LOW);
  }
}

void releaseDisplay(uint8_t index) {
  if (index < kDisplayCount) {
    digitalWrite(kCsPins[index], HIGH);
  }
}

void drawPanelFrame(uint8_t index) {
  selectDisplay(index);

  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, kPanelWidth, kPanelHeight, TFT_RED);
  tft.drawRect(2, 2, kPanelWidth - 4, kPanelHeight - 4, TFT_BLUE);
  tft.drawFastVLine(kPanelWidth / 2, 0, kPanelHeight, TFT_DARKGREY);
  tft.drawFastHLine(0, kPanelHeight / 2, kPanelWidth, TFT_DARKGREY);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  tft.drawString(String(index + 1), kPanelWidth / 2, 28);
  tft.drawString("170x320", kPanelWidth / 2, 60);
  tft.fillRect(20, 100, 30, 140, TFT_RED);
  tft.fillRect(70, 100, 30, 140, TFT_GREEN);
  tft.fillRect(120, 100, 30, 140, TFT_BLUE);
  tft.drawString("TOP", kPanelWidth / 2, kPanelHeight - 24);

  releaseDisplay(index);
}

void drawTestPattern() {
  for (uint8_t i = 0; i < kDisplayCount; ++i) {
    drawPanelFrame(i);
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);

  SPI.begin(kTftSclk, -1, kTftMosi, -1);

  pinMode(kTftDc, OUTPUT);
  digitalWrite(kTftDc, HIGH);

  pinMode(kTftRst, OUTPUT);
  digitalWrite(kTftRst, HIGH);

  pinMode(kTftBl, OUTPUT);
  digitalWrite(kTftBl, HIGH);

  for (uint8_t i = 0; i < kDisplayCount; ++i) {
    pinMode(kCsPins[i], OUTPUT);
    digitalWrite(kCsPins[i], HIGH);
  }

  for (uint8_t i = 0; i < kDisplayCount; ++i) {
    selectDisplay(i);
    tft.init();
    tft.setRotation(0);
    releaseDisplay(i);
  }

  drawTestPattern();
}

void loop() {
  static uint32_t lastMs = 0;
  static bool inverted = false;

  if (millis() - lastMs < 2000) {
    return;
  }

  lastMs = millis();
  inverted = !inverted;

  for (uint8_t i = 0; i < kDisplayCount; ++i) {
    selectDisplay(i);
    if (inverted) {
      tft.invertDisplay(true);
    } else {
      tft.invertDisplay(false);
    }
    releaseDisplay(i);
  }
}
