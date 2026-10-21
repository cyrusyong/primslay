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
  // Default is 5s. The ESP32 TLS handshake to ntfy.sh does not finish in that.
  http.setConnectTimeout(20000);
  http.setTimeout(20000);
  return http.begin(client, url);
}

bool inboxPoll() {
  // HTTPS GET https://NTFY_HOST/<NTFY_TOPIC>/json?poll=1&since=<id>
  // Trust the server with ROOT_CA (WiFiClientSecure).
  // Each line of the body is one JSON object. The fields you need are
  // id, event, message, and time. Keep events whose event is "message".
  // Copy the body into out->text, the id into out->id, and the unix time
  // into out->sentAt. Remember that id so the next poll does not repeat it.
  // The first poll after boot should ask for a recent window, so a message
  // ntfy still has stored shows up again after power loss.

  int code = http.GET();
  if (code != 200) {
    return false;
  }

  String body = http.getString();
  Serial.println(body);
  return false;
}
