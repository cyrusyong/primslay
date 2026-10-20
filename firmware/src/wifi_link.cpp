#include "wifi_link.h"
#include "secrets.h"
#include <WiFi.h>

bool wifiConnect() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  return WiFi.waitForConnectResult() == WL_CONNECTED;
}

bool wifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}
