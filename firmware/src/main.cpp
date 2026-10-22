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
  static int retries = 0;
  configTzTime("EST5EDT,M3.2.0,M11.1.0", "pool.ntp.org");

  while ((time(NULL) < 1700000000) && (retries < MAX_RETRIES)) { 
    delay(1000);
    retries++;
  }

  return (retries < MAX_RETRIES);
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

  if (!wifi_connected || !clock_synced || !ntfy_connected) {
    return;
  }

  static String last_id;
  static bool poll_status = false;

  if (current_time + POLL_INTERVAL_MS < millis()) {
    ntfy_response resp;
    poll_status = inboxPoll(resp);

    if (poll_status && resp.id != last_id) {
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
