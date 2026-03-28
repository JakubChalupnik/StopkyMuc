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
    g_tft->fillCircle(cx, 90, 8, TFT_WHITE);
    g_tft->fillCircle(cx, 150, 8, TFT_WHITE);
    return;
  }

  if (c == ' ') {
    return;
  }

  const uint8_t mask = segmentMask(c);
  if (mask == 0) {
    drawCenteredGlyph(c, TFT_WHITE, TFT_BLACK, 4);
    return;
  }

  const uint16_t color = TFT_WHITE;
  drawSegment(mask & SEG_A, 28, 20, 78, 16, color);
  drawSegment(mask & SEG_B, 98, 34, 16, 72, color);
  drawSegment(mask & SEG_C, 98, 126, 16, 72, color);
  drawSegment(mask & SEG_D, 28, 204, 78, 16, color);
  drawSegment(mask & SEG_E, 20, 126, 16, 72, color);
  drawSegment(mask & SEG_F, 20, 34, 16, 72, color);
  drawSegment(mask & SEG_G, 28, 112, 78, 16, color);
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
