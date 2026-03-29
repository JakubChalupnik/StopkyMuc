#include "WifiService.h"

#include <WiFiManager.h>
#include <esp_wifi.h>
#include <time.h>

bool WifiService::begin(StorageManager& storage) {
  WiFi.mode(WIFI_STA);
  softApMode_ = false;

  if (connectFromStorage(storage)) {
    return true;
  }

  softApMode_ = startProvisioningPortal(storage);
  return !softApMode_ ? WiFi.status() == WL_CONNECTED : false;
}

bool WifiService::connectFromStorage(StorageManager& storage) {
  WifiCredentials creds;
  if (!storage.loadWifi(creds)) {
    return false;
  }

  WiFi.begin(creds.ssid.c_str(), creds.password.c_str());
  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < AppConfig::WIFI_TIMEOUT_MS) {
    delay(50);
  }

  return WiFi.status() == WL_CONNECTED;
}

bool WifiService::startProvisioningPortal(StorageManager& storage) {
  WiFiManager wm;
  wm.setConfigPortalTimeout(180);

  if (!wm.startConfigPortal(AppConfig::AP_NAME)) {
    return true;
  }

  const WifiCredentials creds = {WiFi.SSID(), WiFi.psk()};
  storage.saveWifi(creds);
  return false;
}

bool WifiService::ensureTime() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  configTzTime(AppConfig::TIMEZONE_BERLIN, AppConfig::NTP_SERVER_1, AppConfig::NTP_SERVER_2);

  tm timeInfo = {};
  return getLocalTime(&timeInfo, 5000);
}

bool WifiService::isConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

bool WifiService::softApMode() const {
  return softApMode_;
}
