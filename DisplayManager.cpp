#include "DisplayManager.h"

#include <SPI.h>

#include "Renderer.h"

namespace {
void pulseSharedReset() {
  pinMode(Pins::TFT_RST_PIN, OUTPUT);
  digitalWrite(Pins::TFT_RST_PIN, HIGH);
  delay(5);
  digitalWrite(Pins::TFT_RST_PIN, LOW);
  delay(20);
  digitalWrite(Pins::TFT_RST_PIN, HIGH);
  delay(150);
}

void preparePanel(TFT_eSPI& tft) {
  tft.init();
  tft.setRotation(0);
}
}  // namespace

void DisplayManager::begin() {
  SPI.begin(Pins::TFT_SCLK_PIN, -1, Pins::TFT_MOSI_PIN, -1);

  pulseSharedReset();

  pinMode(Pins::TFT_BL_PIN, OUTPUT);
  digitalWrite(Pins::TFT_BL_PIN, HIGH);

  for (uint8_t i = 0; i < AppConfig::DISPLAY_COUNT; ++i) {
    pinMode(Pins::TFT_CS_PINS[i], OUTPUT);
    digitalWrite(Pins::TFT_CS_PINS[i], HIGH);
  }

  Renderer::begin(&tft_);

  for (uint8_t i = 0; i < AppConfig::DISPLAY_COUNT; ++i) {
    clearDisplay(i);
  }
}

void DisplayManager::selectDisplay(uint8_t index) {
  for (uint8_t i = 0; i < AppConfig::DISPLAY_COUNT; ++i) {
    digitalWrite(Pins::TFT_CS_PINS[i], HIGH);
  }

  if (index < AppConfig::DISPLAY_COUNT) {
    digitalWrite(Pins::TFT_CS_PINS[index], LOW);
  }
}

void DisplayManager::clearDisplay(uint8_t index) {
  selectDisplay(index);
  preparePanel(tft_);
  tft_.fillScreen(TFT_BLACK);
  digitalWrite(Pins::TFT_CS_PINS[index], HIGH);
  current_[index] = ' ';
}

void DisplayManager::drawChar(uint8_t index, char c) {
  if (index >= AppConfig::DISPLAY_COUNT) {
    return;
  }

  selectDisplay(index);
  preparePanel(tft_);
  switch (theme_) {
    case THEME_7SEG:
      Renderer::renderChar7Seg(index, c);
      break;
    case THEME_NIXIE:
      Renderer::renderCharNixie(index, c);
      break;
    case THEME_TEXT:
      Renderer::renderCharText(index, c);
      break;
  }
  digitalWrite(Pins::TFT_CS_PINS[index], HIGH);
  current_[index] = c;
}

void DisplayManager::setTheme(DisplayTheme theme) {
  theme_ = theme;
  for (uint8_t i = 0; i < AppConfig::DISPLAY_COUNT; ++i) {
    current_[i] = '\0';
  }
}

void DisplayManager::showText(const char* text5) {
  for (uint8_t i = 0; i < AppConfig::DISPLAY_COUNT; ++i) {
    next_[i] = text5 && text5[i] ? text5[i] : ' ';
  }
}

void DisplayManager::showClock(const tm& timeInfo) {
  char buffer[6] = {};
  snprintf(buffer, sizeof(buffer), "%02d:%02d", timeInfo.tm_hour, timeInfo.tm_min);
  showText(buffer);
}

void DisplayManager::showTimer(const TimerState& timerState) {
  char buffer[6] = {};
  snprintf(buffer, sizeof(buffer), "%02d:%02d", timerState.minutes, timerState.seconds);
  showText(buffer);
}

void DisplayManager::commit() {
  for (uint8_t i = 0; i < AppConfig::DISPLAY_COUNT; ++i) {
    if (next_[i] != current_[i]) {
      drawChar(i, next_[i]);
    }
  }
}
