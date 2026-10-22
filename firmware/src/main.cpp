#include <Arduino.h>
#include <time.h>

#include "config.h"
#include "display.h"
#include "inbox.h"
#include "wifi_link.h"

static uint32_t current_time = 0;
static bool wifi_connected = false;
static bool ntfy_connected = false;
static bool clock_synced = false;

static bool sync_clock() {
  configTzTime("EST5EDT,M3.2.0,M11.1.0", "pool.ntp.org");

  while (time(NULL) < 1700000000) { 
    delay(1000);
  }

  return true;
}

void setup() {
  Serial.begin(115200);
  displayBegin();
  current_time = millis();

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

  static String last_id;

  if (current_time + POLL_INTERVAL_MS < millis()) {
    ntfy_response resp;
    inboxPoll(resp);

    if (resp.id != last_id) {
      last_id = resp.id;
      char footer[6];
      time_t stamp = resp.time;
      struct tm parts;
      localtime_r(&stamp, &parts);
      strftime(footer, sizeof(footer), "%H:%M", &parts);
      displayMessage(resp.message.c_str(), footer);
    }

    current_time = millis();
  }
}
