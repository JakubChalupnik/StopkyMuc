#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <time.h>

#include "AppConfig.h"
#include "AppTypes.h"

class DisplayManager {
 public:
  void begin();
  void selectDisplay(uint8_t index);
  void clearDisplay(uint8_t index);
  void drawChar(uint8_t index, char c);
  void setTheme(DisplayTheme theme);
  void showText(const char* text5);
  void showClock(const tm& timeInfo);
  void showTimer(const TimerState& timerState);
  void commit();

 private:
  TFT_eSPI tft_;
  DisplayTheme theme_ = THEME_7SEG;
  char current_[AppConfig::DISPLAY_COUNT] = {' ', ' ', ' ', ' ', ' '};
  char next_[AppConfig::DISPLAY_COUNT] = {' ', ' ', ' ', ' ', ' '};
};
