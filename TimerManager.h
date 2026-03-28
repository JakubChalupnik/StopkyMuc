#pragma once

#include <Arduino.h>

#include "AppTypes.h"

class TimerManager {
 public:
  void begin();
  void addMinutes(int delta);
  void start();
  void stop();
  void reset();
  bool update();
  const TimerState& state() const;

 private:
  TimerState state_;
  uint32_t lastTickMs_ = 0;
};
