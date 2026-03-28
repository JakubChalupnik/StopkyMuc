#pragma once

#include <Arduino.h>

#include "AppConfig.h"
#include "AppTypes.h"

class ButtonManager {
 public:
  void begin();
  void updateButtons();
  bool isPressed(uint8_t btn);
  bool isLongPressed(uint8_t btn);

 private:
  struct ButtonState {
    uint8_t pin = 0;
    bool stableLevel = HIGH;
    bool lastRead = HIGH;
    bool pressEvent = false;
    bool longEvent = false;
    bool repeatArmed = false;
    uint32_t changedAt = 0;
    uint32_t pressedAt = 0;
    uint32_t repeatedAt = 0;
  };

  ButtonState buttons_[BUTTON_COUNT];
};
