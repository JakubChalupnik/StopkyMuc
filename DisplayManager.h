#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <time.h>

#include "AppConfig.h"
#include "AppTypes.h"

class DisplayManager {
 public:
  DisplayManager();
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
  void beginWrite();
  void endWrite();
  void writeCommand(uint8_t command);
  void writeData8(uint8_t value);
  void writeData16(uint16_t value);
  void setAddressWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  void initDisplayDirect(uint8_t index);
  void pushSpriteToDisplay(uint8_t index);
  uint8_t physicalDisplayIndex(uint8_t logicalIndex) const;

  TFT_eSPI tft_;
  TFT_eSprite sprite_;
  bool spriteReady_ = false;
  DisplayTheme theme_ = THEME_7SEG;
  char current_[AppConfig::DISPLAY_COUNT] = {' ', ' ', ' ', ' ', ' '};
  char next_[AppConfig::DISPLAY_COUNT] = {' ', ' ', ' ', ' ', ' '};
};
