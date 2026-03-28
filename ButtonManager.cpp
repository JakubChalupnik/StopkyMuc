#include "ButtonManager.h"

void ButtonManager::begin() {
  buttons_[BUTTON_PLUS].pin = Pins::BTN_PLUS_PIN;
  buttons_[BUTTON_MINUS].pin = Pins::BTN_MINUS_PIN;

  for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
    pinMode(buttons_[i].pin, INPUT_PULLUP);
    buttons_[i].stableLevel = digitalRead(buttons_[i].pin);
    buttons_[i].lastRead = buttons_[i].stableLevel;
    buttons_[i].changedAt = millis();
  }
}

void ButtonManager::updateButtons() {
  const uint32_t now = millis();

  for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
    ButtonState& btn = buttons_[i];
    const bool raw = digitalRead(btn.pin);

    if (raw != btn.lastRead) {
      btn.lastRead = raw;
      btn.changedAt = now;
    }

    if ((now - btn.changedAt) < AppConfig::BUTTON_DEBOUNCE_MS || raw == btn.stableLevel) {
      continue;
    }

    btn.stableLevel = raw;
    if (btn.stableLevel == LOW) {
      btn.pressEvent = true;
      btn.longEvent = false;
      btn.repeatArmed = false;
      btn.pressedAt = now;
      btn.repeatedAt = now;
      continue;
    }

    btn.repeatArmed = false;
  }

  for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
    ButtonState& btn = buttons_[i];
    if (btn.stableLevel != LOW) {
      continue;
    }

    if (!btn.repeatArmed && (now - btn.pressedAt) >= AppConfig::BUTTON_LONG_PRESS_MS) {
      btn.longEvent = true;
      btn.repeatArmed = true;
      btn.repeatedAt = now;
      continue;
    }

    if (btn.repeatArmed && (now - btn.repeatedAt) >= AppConfig::BUTTON_REPEAT_MS) {
      btn.pressEvent = true;
      btn.repeatedAt = now;
    }
  }
}

bool ButtonManager::isPressed(uint8_t btn) {
  if (btn >= BUTTON_COUNT) {
    return false;
  }

  const bool event = buttons_[btn].pressEvent;
  buttons_[btn].pressEvent = false;
  return event;
}

bool ButtonManager::isLongPressed(uint8_t btn) {
  if (btn >= BUTTON_COUNT) {
    return false;
  }

  const bool event = buttons_[btn].longEvent;
  buttons_[btn].longEvent = false;
  return event;
}
