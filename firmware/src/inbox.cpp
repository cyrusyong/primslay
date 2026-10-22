#include "inbox.h"
#include "secrets.h"
#include "certs.h"
#include "config.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


static WiFiClientSecure client;
static HTTPClient http;
static String url = "https://" + String(NTFY_HOST) + "/" + String(NTFY_TOPIC) + "/json?poll=1&since=latest";

bool ntfyConnect() {
  client.setCACert(ROOT_CA);
  http.setConnectTimeout(20000);
  http.setTimeout(20000);
  return http.begin(client, url);
}

bool inboxPoll(ntfy_response& out) {
  int status_code = http.GET();
  if (status_code != 200) {
    return false;
  }

  String body = http.getString();
  JsonDocument doc;
  if (deserializeJson(doc, body)) {
    return false;
  }

  out.id = doc["id"] | "";
  out.time = doc["time"] | (uint32_t)0;
  out.expires = doc["expires"] | (uint32_t)0;
  out.event = doc["event"] | "";
  out.topic = doc["topic"] | "";
  out.message = doc["message"] | "";

  return true;
}
