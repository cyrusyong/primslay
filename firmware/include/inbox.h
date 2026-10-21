#pragma once

#include <Arduino.h>

struct ntfy_response {
  String id;
  uint32_t time;
  uint32_t expires;
  String event;
  String topic;
  String message;
};

bool ntfyConnect();
bool inboxPoll(ntfy_response& out);
