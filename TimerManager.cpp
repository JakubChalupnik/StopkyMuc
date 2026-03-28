#include "TimerManager.h"

void TimerManager::begin() {
  state_ = {};
  lastTickMs_ = millis();
}

void TimerManager::addMinutes(int delta) {
  const int total = constrain((state_.minutes * 60) + state_.seconds + (delta * 60), 0, 99 * 60 + 59);
  state_.minutes = total / 60;
  state_.seconds = total % 60;
}

void TimerManager::start() {
  if (state_.minutes == 0 && state_.seconds == 0) {
    return;
  }
  state_.running = true;
  lastTickMs_ = millis();
}

void TimerManager::stop() {
  state_.running = false;
}

void TimerManager::reset() {
  state_ = {};
}

bool TimerManager::update() {
  if (!state_.running) {
    return false;
  }

  const uint32_t now = millis();
  while ((now - lastTickMs_) >= 1000 && state_.running) {
    lastTickMs_ += 1000;
    const int total = (state_.minutes * 60) + state_.seconds;
    if (total <= 1) {
      state_.minutes = 0;
      state_.seconds = 0;
      state_.running = false;
      return true;
    }

    const int nextTotal = total - 1;
    state_.minutes = nextTotal / 60;
    state_.seconds = nextTotal % 60;
  }

  return false;
}

const TimerState& TimerManager::state() const {
  return state_;
}
