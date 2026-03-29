#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

namespace {
constexpr uint8_t kDisplayCount = 5;

constexpr uint8_t kTftMosi = 35;
constexpr uint8_t kTftSclk = 36;
constexpr uint8_t kTftDc = 37;
constexpr uint8_t kTftRst = 33;
constexpr uint8_t kTftBl = 34;
constexpr uint8_t kCsPins[kDisplayCount] = {1, 2, 3, 4, 5};

constexpr uint16_t kPanelWidth = 170;
constexpr uint16_t kPanelHeight = 320;
constexpr uint16_t kPanelXOffset = 35;
constexpr uint16_t kPanelYOffset = 0;
constexpr uint32_t kSpiFrequency = 40000000;

constexpr uint8_t kCmdSlpOut = 0x11;
constexpr uint8_t kCmdNorOn = 0x13;
constexpr uint8_t kCmdInvOn = 0x21;
constexpr uint8_t kCmdDispOn = 0x29;
constexpr uint8_t kCmdCaseT = 0x2A;
constexpr uint8_t kCmdRaseT = 0x2B;
constexpr uint8_t kCmdRamWr = 0x2C;
constexpr uint8_t kCmdMadCtl = 0x36;
constexpr uint8_t kCmdColMod = 0x3A;
constexpr uint8_t kMadCtlRotation0 = 0x08;  // BGR, portrait, 170x320 CGRAM offset panels

TFT_eSPI tft;
TFT_eSprite sprite(&tft);
bool gSpriteReady = false;
uint8_t gActiveDisplay = 0;
uint32_t gFrame = 0;

const uint16_t kAccentColors[kDisplayCount] = {
    TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_CYAN};

void pulseSharedReset() {
  pinMode(kTftRst, OUTPUT);
  digitalWrite(kTftRst, HIGH);
  delay(5);
  digitalWrite(kTftRst, LOW);
  delay(20);
  digitalWrite(kTftRst, HIGH);
  delay(150);
}

void beginWrite() {
  SPI.beginTransaction(SPISettings(kSpiFrequency, MSBFIRST, SPI_MODE0));
}

void endWrite() {
  SPI.endTransaction();
}

void writeCommand(uint8_t command) {
  digitalWrite(kTftDc, LOW);
  SPI.transfer(command);
}

void writeData8(uint8_t value) {
  digitalWrite(kTftDc, HIGH);
  SPI.transfer(value);
}

void writeData16(uint16_t value) {
  digitalWrite(kTftDc, HIGH);
  SPI.transfer16(value);
}

void selectDisplay(uint8_t index) {
  for (uint8_t i = 0; i < kDisplayCount; ++i) {
    digitalWrite(kCsPins[i], HIGH);
  }

  if (index < kDisplayCount) {
    digitalWrite(kCsPins[index], LOW);
  }
}

void releaseDisplay(uint8_t index) {
  if (index < kDisplayCount) {
    digitalWrite(kCsPins[index], HIGH);
  }
}

void setAddressWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  const uint16_t x0 = x + kPanelXOffset;
  const uint16_t x1 = x0 + w - 1;
  const uint16_t y0 = y + kPanelYOffset;
  const uint16_t y1 = y0 + h - 1;

  writeCommand(kCmdCaseT);
  writeData16(x0);
  writeData16(x1);

  writeCommand(kCmdRaseT);
  writeData16(y0);
  writeData16(y1);

  writeCommand(kCmdRamWr);
}

void initDisplayDirect(uint8_t index) {
  selectDisplay(index);
  beginWrite();
  writeCommand(kCmdSlpOut);
  endWrite();
  delay(120);

  beginWrite();
  writeCommand(kCmdNorOn);
  writeCommand(kCmdMadCtl);
  writeData8(kMadCtlRotation0);
  writeCommand(kCmdColMod);
  writeData8(0x55);
  writeCommand(kCmdInvOn);
  writeCommand(kCmdDispOn);
  endWrite();
  delay(20);
  releaseDisplay(index);
}

void drawPattern(TFT_eSPI& target, uint8_t index, uint32_t frame) {
  const uint16_t accent = kAccentColors[index % kDisplayCount];

  target.fillScreen(TFT_BLACK);
  target.drawRect(0, 0, kPanelWidth, kPanelHeight, accent);
  target.drawRect(4, 4, kPanelWidth - 8, kPanelHeight - 8, TFT_WHITE);
  target.fillRoundRect(18, 18, kPanelWidth - 36, 52, 10, accent);

  target.setTextColor(TFT_BLACK, accent);
  target.setTextDatum(MC_DATUM);
  target.setTextSize(2);
  target.drawString("DISPLAY", kPanelWidth / 2, 44);

  target.setTextColor(TFT_WHITE, TFT_BLACK);
  target.setTextSize(5);
  target.drawString(String(index + 1), kPanelWidth / 2, 112);

  target.setTextSize(2);
  target.drawString("170x320", kPanelWidth / 2, 162);
  target.drawString("SPRITE", kPanelWidth / 2, 190);

  target.fillRect(18, 222, 38, 56, TFT_RED);
  target.fillRect(66, 222, 38, 56, TFT_GREEN);
  target.fillRect(114, 222, 38, 56, TFT_BLUE);

  target.setTextColor(accent, TFT_BLACK);
  target.drawString(String("F") + frame, kPanelWidth / 2, 298);
}

void renderForDisplay(uint8_t index, uint32_t frame) {
  if (gSpriteReady) {
    drawPattern(sprite, index, frame);
    return;
  }

  drawPattern(tft, index, frame);
}

void pushToDisplay(uint8_t index) {
  if (!gSpriteReady) {
    return;
  }

  uint16_t* pixels = static_cast<uint16_t*>(sprite.getPointer());
  if (!pixels) {
    return;
  }

  selectDisplay(index);
  beginWrite();
  setAddressWindow(0, 0, kPanelWidth, kPanelHeight);
  digitalWrite(kTftDc, HIGH);
  const uint32_t pixelCount = static_cast<uint32_t>(kPanelWidth) * kPanelHeight;
  for (uint32_t i = 0; i < pixelCount; ++i) {
    SPI.transfer16(pixels[i]);
  }
  endWrite();
  releaseDisplay(index);
}
}  // namespace

void setup() {
  Serial.begin(115200);

  SPI.begin(kTftSclk, -1, kTftMosi, -1);

  pinMode(kTftDc, OUTPUT);
  digitalWrite(kTftDc, HIGH);

  pulseSharedReset();

  pinMode(kTftBl, OUTPUT);
  digitalWrite(kTftBl, HIGH);

  for (uint8_t i = 0; i < kDisplayCount; ++i) {
    pinMode(kCsPins[i], OUTPUT);
    digitalWrite(kCsPins[i], HIGH);
  }

  tft.setAttribute(PSRAM_ENABLE, 1);
  sprite.setColorDepth(16);
  gSpriteReady = sprite.createSprite(kPanelWidth, kPanelHeight) != nullptr;

  for (uint8_t i = 0; i < kDisplayCount; ++i) {
    initDisplayDirect(i);
    renderForDisplay(i, i + 1);
    pushToDisplay(i);
    delay(250);
  }
}

void loop() {
  static uint32_t lastMs = 0;

  if (millis() - lastMs < 1000) {
    return;
  }

  lastMs = millis();
  ++gFrame;
  renderForDisplay(gActiveDisplay, gFrame);
  pushToDisplay(gActiveDisplay);
  gActiveDisplay = (gActiveDisplay + 1) % kDisplayCount;
}
