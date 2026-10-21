#include <Arduino.h>
#include <time.h>

#include "config.h"
#include "display.h"
#include "inbox.h"
#include "wifi_link.h"

static uint32_t current_time = 0;
bool wifi_connected = false;
bool ntfy_connected = false;
bool clock_synced = false;

static bool sync_clock() {
  configTzTime("EST5EDT,M3.2.0,M11.1.0", "pool.ntp.org");

  current_time = millis();
  while (time(nullptr) < 1700000000L) {
    delay(500);
  }

  return true;
}

void setup() {
  Serial.begin(115200);
  displayBegin();

  wifi_connected = wifiConnect();
  if (!wifi_connected) {
    displayStatus("Failed to connect to Wi-Fi");
    return;
  }

  clock_synced = sync_clock();
  if (!clock_synced) {
    displayStatus("Clock syncing");
    return;
  }

  ntfy_connected = ntfyConnect();
  if (!ntfy_connected) {
    displayStatus("Failed to connect to NTFY");
    return;
  }
}

void loop() {
  // 1. If wifiConnected() is false, reconnect and say so on screen.
  // 2. Every POLL_INTERVAL_MS, call inboxPoll.
  // 3. When the message text is exactly "clear", show "Waiting for a message".
  // 4. Otherwise call displayMessage with the text and a short time footer.
  // 5. Leave the last message on screen until a newer one arrives.

  if (!wifi_connected || !clock_synced || !ntfy_connected) {
    return;
  }

  if (current_time + POLL_INTERVAL_MS < millis()) {
    ntfy_response resp;
    inboxPoll(resp);
    char footer[6];
    time_t stamp = resp.time;
    struct tm parts;
    localtime_r(&stamp, &parts);
    strftime(footer, sizeof(footer), "%H:%M", &parts);
    displayMessage(resp.message.c_str(), footer);
    current_time = millis();
  }
}
