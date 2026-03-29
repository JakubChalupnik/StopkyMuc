#include "DisplayManager.h"

#include <SPI.h>

#include "Renderer.h"

namespace {
constexpr uint16_t kPanelXOffset = 35;
constexpr uint16_t kPanelYOffset = 0;
constexpr uint32_t kSpiFrequency = 40000000;

constexpr uint8_t kCmdSlpOut = 0x11;
constexpr uint8_t kCmdNorOn = 0x13;
constexpr uint8_t kCmdInvOn = 0x21;
constexpr uint8_t kCmdDispOn = 0x29;
constexpr uint8_t kCmdCaseT = 0x2A;
constexpr uint8_t kCmdRaseT = 0x2B;
constexpr uint8_t kCmdRamWr = 0x2C;
constexpr uint8_t kCmdMadCtl = 0x36;
constexpr uint8_t kCmdColMod = 0x3A;
constexpr uint8_t kMadCtlRotation0 = 0x08;

void pulseSharedReset() {
  pinMode(Pins::TFT_RST_PIN, OUTPUT);
  digitalWrite(Pins::TFT_RST_PIN, HIGH);
  delay(5);
  digitalWrite(Pins::TFT_RST_PIN, LOW);
  delay(20);
  digitalWrite(Pins::TFT_RST_PIN, HIGH);
  delay(150);
}
}  // namespace

DisplayManager::DisplayManager() : sprite_(&tft_) {}

uint8_t DisplayManager::physicalDisplayIndex(uint8_t logicalIndex) const {
  return (AppConfig::DISPLAY_COUNT - 1) - logicalIndex;
}

void DisplayManager::beginWrite() {
  SPI.beginTransaction(SPISettings(kSpiFrequency, MSBFIRST, SPI_MODE0));
}

void DisplayManager::endWrite() {
  SPI.endTransaction();
}

void DisplayManager::writeCommand(uint8_t command) {
  digitalWrite(Pins::TFT_DC_PIN, LOW);
  SPI.transfer(command);
}

void DisplayManager::writeData8(uint8_t value) {
  digitalWrite(Pins::TFT_DC_PIN, HIGH);
  SPI.transfer(value);
}

void DisplayManager::writeData16(uint16_t value) {
  digitalWrite(Pins::TFT_DC_PIN, HIGH);
  SPI.transfer16(value);
}

void DisplayManager::setAddressWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  const uint16_t x0 = x + kPanelXOffset;
  const uint16_t x1 = x0 + w - 1;
  const uint16_t y0 = y + kPanelYOffset;
  const uint16_t y1 = y0 + h - 1;

  writeCommand(kCmdCaseT);
  writeData16(x0);
  writeData16(x1);

  writeCommand(kCmdRaseT);
  writeData16(y0);
  writeData16(y1);

  writeCommand(kCmdRamWr);
}

void DisplayManager::initDisplayDirect(uint8_t index) {
  selectDisplay(index);
  beginWrite();
  writeCommand(kCmdSlpOut);
  endWrite();
  delay(120);

  beginWrite();
  writeCommand(kCmdNorOn);
  writeCommand(kCmdMadCtl);
  writeData8(kMadCtlRotation0);
  writeCommand(kCmdColMod);
  writeData8(0x55);
  writeCommand(kCmdInvOn);
  writeCommand(kCmdDispOn);
  endWrite();
  delay(20);
  digitalWrite(Pins::TFT_CS_PINS[physicalDisplayIndex(index)], HIGH);
}

void DisplayManager::pushSpriteToDisplay(uint8_t index) {
  if (!spriteReady_) {
    return;
  }

  uint16_t* pixels = static_cast<uint16_t*>(sprite_.getPointer());
  if (!pixels) {
    return;
  }

  selectDisplay(index);
  beginWrite();
  setAddressWindow(0, 0, AppConfig::DISPLAY_WIDTH, AppConfig::DISPLAY_HEIGHT);
  digitalWrite(Pins::TFT_DC_PIN, HIGH);

  const uint32_t pixelCount = static_cast<uint32_t>(AppConfig::DISPLAY_WIDTH) * AppConfig::DISPLAY_HEIGHT;
  for (uint32_t i = 0; i < pixelCount; ++i) {
    SPI.transfer16(pixels[i]);
  }

  endWrite();
  digitalWrite(Pins::TFT_CS_PINS[index], HIGH);
}

void DisplayManager::begin() {
  SPI.begin(Pins::TFT_SCLK_PIN, -1, Pins::TFT_MOSI_PIN, -1);

  pinMode(Pins::TFT_DC_PIN, OUTPUT);
  digitalWrite(Pins::TFT_DC_PIN, HIGH);

  pulseSharedReset();

  pinMode(Pins::TFT_BL_PIN, OUTPUT);
  digitalWrite(Pins::TFT_BL_PIN, HIGH);

  for (uint8_t i = 0; i < AppConfig::DISPLAY_COUNT; ++i) {
    pinMode(Pins::TFT_CS_PINS[i], OUTPUT);
    digitalWrite(Pins::TFT_CS_PINS[i], HIGH);
  }

  tft_.setAttribute(PSRAM_ENABLE, 1);
  sprite_.setColorDepth(16);
  spriteReady_ = sprite_.createSprite(AppConfig::DISPLAY_WIDTH, AppConfig::DISPLAY_HEIGHT) != nullptr;
  Renderer::begin(spriteReady_ ? static_cast<TFT_eSPI*>(&sprite_) : &tft_);

  for (uint8_t i = 0; i < AppConfig::DISPLAY_COUNT; ++i) {
    initDisplayDirect(i);
    clearDisplay(i);
  }
}

void DisplayManager::selectDisplay(uint8_t index) {
  for (uint8_t i = 0; i < AppConfig::DISPLAY_COUNT; ++i) {
    digitalWrite(Pins::TFT_CS_PINS[i], HIGH);
  }

  if (index < AppConfig::DISPLAY_COUNT) {
    digitalWrite(Pins::TFT_CS_PINS[physicalDisplayIndex(index)], LOW);
  }
}

void DisplayManager::clearDisplay(uint8_t index) {
  if (spriteReady_) {
    sprite_.fillSprite(TFT_BLACK);
    pushSpriteToDisplay(index);
  } else {
    selectDisplay(index);
    tft_.fillScreen(TFT_BLACK);
    digitalWrite(Pins::TFT_CS_PINS[physicalDisplayIndex(index)], HIGH);
  }
  current_[index] = ' ';
}

void DisplayManager::drawChar(uint8_t index, char c) {
  if (index >= AppConfig::DISPLAY_COUNT) {
    return;
  }

  if (spriteReady_) {
    sprite_.fillSprite(TFT_BLACK);
  } else {
    selectDisplay(index);
    tft_.fillScreen(TFT_BLACK);
  }

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

  if (spriteReady_) {
    pushSpriteToDisplay(index);
  } else {
    digitalWrite(Pins::TFT_CS_PINS[physicalDisplayIndex(index)], HIGH);
  }
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
