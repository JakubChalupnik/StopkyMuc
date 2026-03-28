#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "AppConfig.h"
#include "AppTypes.h"
#include "StorageManager.h"

class WifiService {
 public:
  bool begin(StorageManager& storage);
  bool ensureTime();
  bool isConnected() const;
  bool softApMode() const;

 private:
  bool connectFromStorage(StorageManager& storage);
  bool startProvisioningPortal(StorageManager& storage);

  bool softApMode_ = false;
};
