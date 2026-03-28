#pragma once

#include <Arduino.h>

namespace Pins {
static constexpr uint8_t TFT_MOSI_PIN = 35;
static constexpr uint8_t TFT_SCLK_PIN = 36;
static constexpr uint8_t TFT_DC_PIN = 37;
static constexpr uint8_t TFT_RST_PIN = 33;
static constexpr uint8_t TFT_BL_PIN = 34;

static constexpr uint8_t TFT_CS_PINS[5] = {1, 2, 3, 4, 5};

static constexpr uint8_t I2S_BCLK_PIN = 6;
static constexpr uint8_t I2S_LRCK_PIN = 7;
static constexpr uint8_t I2S_DIN_PIN = 8;

static constexpr uint8_t BTN_PLUS_PIN = 9;
static constexpr uint8_t BTN_MINUS_PIN = 10;
}  // namespace Pins

namespace AppConfig {
static constexpr uint8_t DISPLAY_COUNT = 5;
static constexpr uint16_t DISPLAY_WIDTH = 170;
static constexpr uint16_t DISPLAY_HEIGHT = 320;

static constexpr uint32_t BUTTON_DEBOUNCE_MS = 30;
static constexpr uint32_t BUTTON_LONG_PRESS_MS = 700;
static constexpr uint32_t BUTTON_REPEAT_MS = 180;

static constexpr uint32_t WIFI_TIMEOUT_MS = 15000;
static constexpr char AP_NAME[] = "CLOCK-SETUP";
static constexpr char TIMEZONE[] = "CET-1CEST,M3.5.0/2,M10.5.0/3";
static constexpr char NTP_SERVER_1[] = "pool.ntp.org";
static constexpr char NTP_SERVER_2[] = "time.nist.gov";

static constexpr char WIFI_CONFIG_PATH[] = "/wifi.cfg";
static constexpr char THEME_CONFIG_PATH[] = "/theme.cfg";
static constexpr char ALARM_AUDIO_PATH[] = "/alarm.raw";
}  // namespace AppConfig
