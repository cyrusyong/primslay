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

struct Line {
  const char* s;
  uint16_t n;
};

static int wrapText(const char* text, int maxW, Line* lines, int maxLines) {
  int count = 0;
  const char* p = text;
  while (*p) {
    while (*p == ' ') p++;
    if (*p == '\n') { p++; continue; }
    if (!*p) break;
    if (count >= maxLines) return -1;

    char slice[128];
    int avail = 0;
    while (p[avail] && p[avail] != '\n' && avail < 127) {
      slice[avail] = p[avail];
      avail++;
    }
    slice[avail] = 0;

    int fit = tft.textLength(slice, maxW);
    if (fit < 1) fit = 1;
    if (fit > avail) fit = avail;
    if (fit < avail && slice[fit] != ' ') {
      int sp = fit;
      while (sp > 0 && slice[sp] != ' ') sp--;
      if (sp > 0) fit = sp;
    }
    int drawN = fit;
    while (drawN > 0 && slice[drawN - 1] == ' ') drawN--;

    int step = fit;
    while (p[step] == ' ') step++;
    if (step >= avail && p[step] == '\n') step++;
    if (step < 1) step = 1;

    if (drawN > 0) {
      lines[count].s = p;
      lines[count].n = (uint16_t)drawN;
      count++;
    }
    p += step;
  }
  return count;
}

void displayMessage(const char* text, const char* timeLine) {
  if (!text) text = "";
  const bool hasFooter = timeLine && timeLine[0];
  const int footerH = hasFooter ? 20 : 0;
  const int maxW = tft.width() - 16;

  Line lines[32];
  int nlines = 0;
  int size = 1;
  for (int trySize = 3; trySize >= 1; trySize--) {
    tft.setTextSize(trySize);
    int lineH = tft.fontHeight();
    if (lineH < 1) lineH = 1;
    int maxLines = (tft.height() - footerH) / lineH;
    if (maxLines > 32) maxLines = 32;
    if (maxLines < 1) maxLines = 1;
    int n = wrapText(text, maxW, lines, maxLines);
    size = trySize;
    if (n >= 0) { nlines = n; break; }
    nlines = maxLines;
  }

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(size);
  tft.setTextDatum(top_center);

  int lineH = tft.fontHeight();
  int y = (tft.height() - footerH - nlines * lineH) / 2;
  if (y < 0) y = 0;
  const int cx = tft.width() / 2;
  char buf[128];
  for (int i = 0; i < nlines; i++) {
    int n = lines[i].n;
    if (n > 127) n = 127;
    memcpy(buf, lines[i].s, n);
    buf[n] = 0;
    tft.drawString(buf, cx, y);
    y += lineH;
  }

  if (hasFooter) {
    tft.setTextSize(2);
    tft.setTextDatum(bottom_center);
    tft.drawString(timeLine, cx, tft.height() - 2);
  }
}
