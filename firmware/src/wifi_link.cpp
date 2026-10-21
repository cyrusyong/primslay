#include "wifi_link.h"
#include "secrets.h"
#include "config.h"
#include <WiFi.h>

static bool ntfyOnIPv4() {
  IPAddress ip;
  if (!WiFi.hostByName(NTFY_HOST, ip) || ip.type() == IPv6) {
    return false;
  }
  WiFiClient plain;
  bool ok = plain.connect(ip, 443, 3000);
  plain.stop();
  return ok;
}

bool wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (WiFi.waitForConnectResult(TIMEOUT_LENGTH_MS) != WL_CONNECTED) {
    return false;
  }
  if (ntfyOnIPv4()) {
    return true;
  }

  WiFi.enableIPv6(true);
  for (int i = 0; i < 50 && WiFi.globalIPv6() == IPAddress(IPv6); ++i) {
    delay(100);
  }
  return true;
}

bool wifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}
