# StopkyMuc Skeleton

Arduino-style firmware skeleton generated from `kitchen_timer_project_spec.md`.

## Included modules

- `StopkyMuc.ino`: main app state machine and loop
- `AppConfig.h`: GPIO mapping and app constants
- `DisplayManager.*`: shared SPI + manual CS switching for 5 ST7789 displays
- `Renderer.*`: 7-segment, nixie-style, and text character rendering
- `ButtonManager.*`: debounce, long-press, and repeat handling
- `TimerManager.*`: non-blocking countdown timer
- `StorageManager.*`: LittleFS-backed config storage
- `WifiService.*`: stored-credential connect, captive portal fallback, NTP setup
- `AudioManager.*`: I2S mono alarm playback loop

## Proposed GPIO map

- TFT SPI: `MOSI=35`, `SCLK=36`
- TFT control: `DC=37`, `RST=33`, `BL=34`
- TFT CS: `1, 2, 3, 4, 5`
- I2S: `BCLK=6`, `LRCK=7`, `DIN=8`
- Buttons: `PLUS=9`, `MINUS=10`

This mapping is a conservative starting point for the ESP32-S2 Mini, but it still needs to be validated against your exact board revision and wiring.

## Important TFT_eSPI note

This skeleton assumes `TFT_eSPI` is configured with:

- ST7789 driver enabled
- display size `170x320`
- `TFT_CS` disabled or set aside for manual CS handling
- `TFT_MOSI`, `TFT_SCLK`, `TFT_DC`, `TFT_RST` matching `AppConfig.h`

Manual CS switching is handled in `DisplayManager`.

## Filesystem expectations

- Wi-Fi credentials: `/wifi.cfg`
- selected theme: `/theme.cfg`
- raw mono alarm audio: `/alarm.raw`

## Display Bring-Up

The repo now includes:

- `tools/tft/User_Setup_StopkyMuc.h`: checked-in `TFT_eSPI` setup for the project
- `display_test/display_test.ino`: standalone panel test sketch

Suggested bring-up flow:

1. Copy `tools/tft/User_Setup_StopkyMuc.h` to your local `TFT_eSPI/User_Setup.h`
2. Compile and upload `display_test/display_test.ino`
3. Verify each panel shows:
   - panel number near the top
   - `170x320` text
   - red/green/blue vertical bars
   - `TOP` at the physical top edge

The checked-in TFT setup uses `TFT_RST = -1` on purpose.
The five displays share one reset line, so reset is pulsed once in the sketch before panel initialization. If `TFT_eSPI` toggles reset during every `tft.init()`, only the last initialized panel will remain active.

If the image is clipped or shifted, the next thing to tune is the ST7789 panel offset and possibly `setRotation()`.
