#include <Arduino.h>
#include <time.h>

#include "AppConfig.h"
#include "AppTypes.h"
#include "AudioManager.h"
#include "ButtonManager.h"
#include "DisplayManager.h"
#include "StorageManager.h"
#include "TimerManager.h"
#include "WifiService.h"

namespace {
DisplayManager g_display;
ButtonManager g_buttons;
TimerManager g_timer;
StorageManager g_storage;
WifiService g_wifi;
AudioManager g_audio;

AppState g_state = BOOT;
DisplayTheme g_theme = THEME_7SEG;
uint32_t g_lastClockRefresh = 0;

void setState(AppState next) {
  g_state = next;
}

void updateClockScreen() {
  tm timeInfo = {};
  if (getLocalTime(&timeInfo, 5)) {
    g_display.showClock(timeInfo);
  } else {
    g_display.showText("-----");
  }
}

void handleButtons() {
  g_buttons.updateButtons();

  const bool plusPress = g_buttons.isPressed(BUTTON_PLUS);
  const bool minusPress = g_buttons.isPressed(BUTTON_MINUS);
  const bool plusLong = g_buttons.isLongPressed(BUTTON_PLUS);
  const bool minusLong = g_buttons.isLongPressed(BUTTON_MINUS);

  if (g_state == ALARM && (plusPress || minusPress || plusLong || minusLong)) {
    g_audio.stop();
    setState(TIMER_SET);
    return;
  }

  if (plusLong) {
    g_theme = static_cast<DisplayTheme>((g_theme + 1) % 3);
    g_storage.saveTheme(g_theme);
    g_display.setTheme(g_theme);
  }

  if (g_state == CLOCK || g_state == TIMER_SET) {
    if (plusPress) {
      g_timer.addMinutes(1);
      setState(TIMER_SET);
    }
    if (minusPress) {
      g_timer.addMinutes(-1);
      setState(TIMER_SET);
    }
  }

  if (g_state == TIMER_SET && minusLong) {
    if (g_timer.state().minutes == 0 && g_timer.state().seconds == 0) {
      setState(CLOCK);
      return;
    }
    g_timer.start();
    setState(TIMER_RUN);
  }

  if (g_state == TIMER_RUN && minusPress) {
    g_timer.stop();
    setState(TIMER_SET);
  }
}

void updateState() {
  switch (g_state) {
    case BOOT:
      g_display.showText("BOOT ");
      setState(WIFI_CONNECT);
      break;

    case WIFI_CONNECT: {
      const bool wifiOk = g_wifi.begin(g_storage);
      if (wifiOk) {
        g_wifi.ensureTime();
        setState(CLOCK);
      } else if (g_wifi.softApMode()) {
        setState(WIFI_SETUP);
      } else {
        setState(CLOCK);
      }
      break;
    }

    case WIFI_SETUP:
      g_display.showText("SETUP");
      if (g_wifi.isConnected()) {
        g_wifi.ensureTime();
        setState(CLOCK);
      }
      break;

    case CLOCK:
      if (millis() - g_lastClockRefresh >= 500) {
        g_lastClockRefresh = millis();
        updateClockScreen();
      }
      break;

    case TIMER_SET:
      g_display.showTimer(g_timer.state());
      break;

    case TIMER_RUN:
      g_display.showTimer(g_timer.state());
      if (g_timer.update()) {
        g_audio.startLoop();
        setState(ALARM);
      }
      break;

    case ALARM:
      g_display.showText("ALARM");
      break;

    case TEXT:
      g_display.showText("HELLO");
      break;
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);

  g_storage.begin();
  g_display.begin();
  g_buttons.begin();
  g_timer.begin();
  g_audio.begin();
  g_audio.loadAlarm(AppConfig::ALARM_AUDIO_PATH);

  g_theme = g_storage.loadTheme(THEME_7SEG);
  g_display.setTheme(g_theme);
}

void loop() {
  handleButtons();
  updateState();
  g_audio.update();
  g_display.commit();
}
