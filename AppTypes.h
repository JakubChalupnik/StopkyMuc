#pragma once

#include <Arduino.h>

enum AppState {
  BOOT,
  WIFI_CONNECT,
  WIFI_SETUP,
  CLOCK,
  TIMER_SET,
  TIMER_RUN,
  ALARM,
  TEXT
};

enum DisplayTheme {
  THEME_7SEG,
  THEME_NIXIE,
  THEME_TEXT
};

enum ButtonId : uint8_t {
  BUTTON_PLUS = 0,
  BUTTON_MINUS = 1,
  BUTTON_COUNT = 2
};

struct TimerState {
  int minutes = 0;
  int seconds = 0;
  bool running = false;
};

struct WifiCredentials {
  String ssid;
  String password;
};
