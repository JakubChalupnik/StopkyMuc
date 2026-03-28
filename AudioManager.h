#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <driver/i2s.h>
#include <memory>

#include "AppConfig.h"

class AudioManager {
 public:
  bool begin();
  bool loadAlarm(const char* path);
  void startLoop();
  void stop();
  void update();
  bool playing() const;

 private:
  static constexpr size_t kChunkSize = 512;

  std::unique_ptr<uint8_t[]> audio_;
  size_t audioSize_ = 0;
  size_t cursor_ = 0;
  bool playing_ = false;
};
