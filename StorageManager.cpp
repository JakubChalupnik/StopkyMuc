#include "StorageManager.h"

namespace {
bool splitPair(const String& line, String& key, String& value) {
  const int separator = line.indexOf('=');
  if (separator <= 0) {
    return false;
  }
  key = line.substring(0, separator);
  value = line.substring(separator + 1);
  value.trim();
  return true;
}
}  // namespace

bool StorageManager::begin() {
  return LittleFS.begin(true);
}

bool StorageManager::loadWifi(WifiCredentials& creds) {
  File file = LittleFS.open(AppConfig::WIFI_CONFIG_PATH, "r");
  if (!file) {
    return false;
  }

  while (file.available()) {
    String key;
    String value;
    if (!splitPair(file.readStringUntil('\n'), key, value)) {
      continue;
    }
    if (key == "ssid") {
      creds.ssid = value;
    } else if (key == "password") {
      creds.password = value;
    }
  }

  return !creds.ssid.isEmpty();
}

bool StorageManager::saveWifi(const WifiCredentials& creds) {
  File file = LittleFS.open(AppConfig::WIFI_CONFIG_PATH, "w");
  if (!file) {
    return false;
  }

  file.printf("ssid=%s\n", creds.ssid.c_str());
  file.printf("password=%s\n", creds.password.c_str());
  return true;
}

DisplayTheme StorageManager::loadTheme(DisplayTheme fallback) {
  File file = LittleFS.open(AppConfig::THEME_CONFIG_PATH, "r");
  if (!file) {
    return fallback;
  }

  const int raw = file.parseInt();
  if (raw < THEME_7SEG || raw > THEME_TEXT) {
    return fallback;
  }
  return static_cast<DisplayTheme>(raw);
}

bool StorageManager::saveTheme(DisplayTheme theme) {
  File file = LittleFS.open(AppConfig::THEME_CONFIG_PATH, "w");
  if (!file) {
    return false;
  }

  file.print(static_cast<int>(theme));
  return true;
}
