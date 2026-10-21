#include "display.h"

LGFX tft;

void displayBegin() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

void displayStatus(const char* line) {
  Serial.write(line);
  Serial.write("\r\n");
}

void displayMessage(const char* text, const char* timeLine) {
  // Draw `text` wrapped on the 320x240 panel.
  // Larger type when the message is short, smaller type when it is long.
  // Draw `timeLine` along the bottom when it is not null.
  (void)text;
  (void)timeLine;
}
