#include "Renderer.h"

#include <ctype.h>

#include "AppConfig.h"

namespace Renderer {
namespace {
TFT_eSPI* g_tft = nullptr;

constexpr uint8_t SEG_A = 1 << 0;
constexpr uint8_t SEG_B = 1 << 1;
constexpr uint8_t SEG_C = 1 << 2;
constexpr uint8_t SEG_D = 1 << 3;
constexpr uint8_t SEG_E = 1 << 4;
constexpr uint8_t SEG_F = 1 << 5;
constexpr uint8_t SEG_G = 1 << 6;

void drawHorizontalSegment(bool on, int32_t x0, int32_t x1, int32_t y, int32_t thickness, int32_t tip, uint16_t color) {
  if (!g_tft || !on) {
    return;
  }

  const int32_t midY = y + thickness / 2;
  const int32_t xA = x0 + tip;
  const int32_t xB = x1 - tip;

  g_tft->fillTriangle(xA, y, xB, y, x1, midY, color);
  g_tft->fillTriangle(xA, y, x0, midY, x1, midY, color);
  g_tft->fillTriangle(x0, midY, xB, y + thickness, x1, midY, color);
  g_tft->fillTriangle(x0, midY, xA, y + thickness, xB, y + thickness, color);
}

void drawVerticalSegment(bool on, int32_t x, int32_t y0, int32_t y1, int32_t thickness, int32_t tip, uint16_t color) {
  if (!g_tft || !on) {
    return;
  }

  const int32_t midX = x + thickness / 2;
  const int32_t yA = y0 + tip;
  const int32_t yB = y1 - tip;

  g_tft->fillTriangle(x, yA, midX, y0, x + thickness, yA, color);
  g_tft->fillTriangle(x, yA, x + thickness, yA, x + thickness, yB, color);
  g_tft->fillTriangle(x, yA, x + thickness, yB, x, yB, color);
  g_tft->fillTriangle(x, yB, midX, y1, x + thickness, yB, color);
}

uint8_t segmentMask(char c) {
  switch (toupper(static_cast<unsigned char>(c))) {
    case '0': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
    case '1': return SEG_B | SEG_C;
    case '2': return SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
    case '3': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_G;
    case '4': return SEG_B | SEG_C | SEG_F | SEG_G;
    case '5': return SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
    case '6': return SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
    case '7': return SEG_A | SEG_B | SEG_C;
    case '8': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
    case '9': return SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
    default: return 0;
  }
}

void drawSegment(bool on, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) {
  if (!g_tft || !on) {
    return;
  }
  g_tft->fillRoundRect(x, y, w, h, 4, color);
}

void drawCenteredGlyph(char c, uint16_t fg, uint16_t bg, uint8_t textSize) {
  if (!g_tft) {
    return;
  }

  g_tft->fillScreen(bg);
  g_tft->setTextDatum(MC_DATUM);
  g_tft->setTextColor(fg, bg);
  g_tft->setTextSize(textSize);
  g_tft->drawString(String(c), AppConfig::DISPLAY_WIDTH / 2, AppConfig::DISPLAY_HEIGHT / 2);
}
}  // namespace

void begin(TFT_eSPI* display) {
  g_tft = display;
}

void renderChar7Seg(uint8_t slot, char c) {
  (void)slot;
  if (!g_tft) {
    return;
  }

  g_tft->fillScreen(TFT_BLACK);
  if (c == ':') {
    const int32_t cx = AppConfig::DISPLAY_WIDTH / 2;
    g_tft->fillCircle(cx, AppConfig::DISPLAY_HEIGHT / 2 - 38, 12, TFT_WHITE);
    g_tft->fillCircle(cx, AppConfig::DISPLAY_HEIGHT / 2 + 38, 12, TFT_WHITE);
    return;
  }

  if (c == ' ') {
    return;
  }

  const uint8_t mask = segmentMask(c);
  if (mask == 0) {
    drawCenteredGlyph(c, TFT_WHITE, TFT_BLACK, 5);
    return;
  }

  const uint16_t color = TFT_WHITE;
  constexpr int32_t hThickness = 34;
  constexpr int32_t vThickness = 34;
  constexpr int32_t tip = 18;

  constexpr int32_t xH0 = 22;
  constexpr int32_t xH1 = AppConfig::DISPLAY_WIDTH - 22;
  constexpr int32_t xLeft = 6;
  constexpr int32_t xRight = AppConfig::DISPLAY_WIDTH - 6 - vThickness;

  constexpr int32_t yA = 6;
  constexpr int32_t yG = (AppConfig::DISPLAY_HEIGHT - hThickness) / 2;
  constexpr int32_t yD = AppConfig::DISPLAY_HEIGHT - 6 - hThickness;

  constexpr int32_t yUpper0 = 28;
  constexpr int32_t yUpper1 = yG - 8;
  constexpr int32_t yLower0 = yG + hThickness + 8;
  constexpr int32_t yLower1 = AppConfig::DISPLAY_HEIGHT - 28;

  drawHorizontalSegment(mask & SEG_A, xH0, xH1, yA, hThickness, tip, color);
  drawVerticalSegment(mask & SEG_B, xRight, yUpper0, yUpper1, vThickness, tip, color);
  drawVerticalSegment(mask & SEG_C, xRight, yLower0, yLower1, vThickness, tip, color);
  drawHorizontalSegment(mask & SEG_D, xH0, xH1, yD, hThickness, tip, color);
  drawVerticalSegment(mask & SEG_E, xLeft, yLower0, yLower1, vThickness, tip, color);
  drawVerticalSegment(mask & SEG_F, xLeft, yUpper0, yUpper1, vThickness, tip, color);
  drawHorizontalSegment(mask & SEG_G, xH0, xH1, yG, hThickness, tip, color);
}

void renderCharNixie(uint8_t slot, char c) {
  (void)slot;
  if (!g_tft) {
    return;
  }

  const uint16_t nixieColor = g_tft->color565(255, 120, 0);
  g_tft->fillScreen(TFT_BLACK);
  g_tft->drawRoundRect(14, 14, AppConfig::DISPLAY_WIDTH - 28, AppConfig::DISPLAY_HEIGHT - 28, 10, TFT_DARKGREY);
  g_tft->setTextDatum(MC_DATUM);
  g_tft->setTextColor(nixieColor, TFT_BLACK);
  g_tft->setTextSize(4);
  g_tft->drawString(String(c), AppConfig::DISPLAY_WIDTH / 2, AppConfig::DISPLAY_HEIGHT / 2);
}

void renderCharText(uint8_t slot, char c) {
  (void)slot;
  drawCenteredGlyph(c, TFT_WHITE, TFT_BLACK, 3);
}
}  // namespace Renderer
