#pragma once

#include <Arduino.h>
#include <LittleFS.h>

#include "AppConfig.h"
#include "AppTypes.h"

class StorageManager {
 public:
  bool begin();
  bool loadWifi(WifiCredentials& creds);
  bool saveWifi(const WifiCredentials& creds);
  DisplayTheme loadTheme(DisplayTheme fallback);
  bool saveTheme(DisplayTheme theme);
};
