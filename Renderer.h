#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace Renderer {
void begin(TFT_eSPI* display);
void renderChar7Seg(uint8_t slot, char c);
void renderCharNixie(uint8_t slot, char c);
void renderCharText(uint8_t slot, char c);
}  // namespace Renderer
