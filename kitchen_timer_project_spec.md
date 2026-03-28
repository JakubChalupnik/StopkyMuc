# Project Specification: Wi-Fi Kitchen Timer with Multi-Display Character Modules (Task-Oriented)

## 1. Objective
Implement firmware for a **Wi-Fi-enabled kitchen timer / clock device** using:
- **WeMos LOLIN S2 Mini (ESP32-S2)**
- **5 SPI TFT displays (ST7789)**
- **2 buttons**
- **I2S audio (MAX98357A)**

Primary goal: deliver a **working, user-friendly kitchen timer** with readable UI and reliable behavior.

---

## 2. High-Level Tasks

### Task 1: Define Pin Mapping
Provide a complete GPIO mapping for:
- SPI (shared): MOSI, SCLK
- Display control: DC, RST, BL
- 5× CS pins (one per display)
- I2S: BCLK, LRCK, DIN
- Buttons: PLUS, MINUS

Constraints:
- Avoid boot-critical pins
- Avoid conflicts with USB or flash
- Prefer stable pins for SPI/I2S

---

### Task 2: Initialize Hardware
Implement initialization for:

#### Displays
- Initialize **TFT_eSPI**
- Configure for ST7789
- Ensure manual CS switching works

#### Filesystem
- Initialize **LittleFS**
- Handle mount failure

#### Audio
- Initialize **I2S driver**
- Configure for mono output

#### Buttons
- Configure as INPUT_PULLUP
- Prepare debounce logic

---

### Task 3: Multi-Display Driver
Implement display control layer:

Required API:
```cpp
void selectDisplay(uint8_t index);
void clearDisplay(uint8_t index);
void drawChar(uint8_t index, char c);
```

Behavior:
- Shared SPI bus
- Only one CS active at a time
- All other CS pins HIGH

Optimization:
- Only redraw changed slots

---

### Task 4: Character Rendering
Implement rendering system for:

#### Supported characters
- A–Z
- 0–9
- : (colon)
- space

#### Modes
1. **Seven-segment (vector)**
   - white on black
   - scalable

2. **Nixie style (bitmap or styled vector)**
   - orange digits
   - dark background

3. **Text mode**
   - bold readable letters

Required API:
```cpp
void renderChar7Seg(uint8_t slot, char c);
void renderCharNixie(uint8_t slot, char c);
void renderCharText(uint8_t slot, char c);
```

---

### Task 5: Timer Logic
Implement countdown timer:

State structure:
```cpp
struct TimerState {
  int minutes;
  int seconds;
  bool running;
};
```

Behavior:
- + button → add minute
- − button → subtract minute
- countdown using millis()
- stop at 0
- trigger alarm

Display format:
- MM:SS

---

### Task 6: Button Handling
Implement:
- debounce
- short press
- long press
- repeat behavior

Required API:
```cpp
void updateButtons();
bool isPressed(uint8_t btn);
bool isLongPressed(uint8_t btn);
```

---

### Task 7: Audio Playback
Implement:

- Load audio file from LittleFS
- Play via I2S
- Loop playback for alarm

Minimum:
- 1 alarm sound

Optional:
- short UI beep

---

### Task 8: Wi-Fi Connection
Implement:

- Load credentials from storage
- Connect as STA
- Retry logic

On failure:
- start SoftAP mode

---

### Task 9: Wi-Fi Provisioning
Implement provisioning flow:

- Start AP (e.g. "CLOCK-SETUP")
- Provide web interface
- Save credentials
- restart or reconnect

Library choice:
- WiFiManager or equivalent

---

### Task 10: Time Synchronization
Implement:

- NTP using `configTime()`
- timezone setup using `setenv()`

Required:
- correct local time
- DST support

---

### Task 11: App State Machine
Implement global states:

```cpp
enum AppState {
  BOOT,
  WIFI_CONNECT,
  WIFI_SETUP,
  CLOCK,
  TIMER_SET,
  TIMER_RUN,
  ALARM,
  TEXT
};
```

Transitions:
- boot → wifi connect
- wifi fail → setup
- timer end → alarm

---

### Task 12: Display Update Strategy

Maintain:
```cpp
char current[5];
char next[5];
```

Behavior:
- compare arrays
- redraw only changed slots

---

### Task 13: Clock Display
Implement:

- display HH:MM
- colon in middle slot

Example:
```text
[ 1 ][ 2 ][ : ][ 3 ][ 4 ]
```

---

### Task 14: Text Mode
Implement:

- display 5-character strings

Examples:
```text
HELLO
START
ERROR
```

---

### Task 15: Alarm Behavior

When timer reaches zero:
- switch to ALARM state
- play sound in loop
- stop on button press

---

### Task 16: Storage

Use LittleFS to store:
- Wi-Fi credentials
- selected theme
- audio files

---

## 3. Required Libraries

### Core
- WiFi.h
- SPI.h
- time.h
- LittleFS.h

### Display
- TFT_eSPI

### Buttons
- optional: Bounce2

### Audio
- ESP32 I2S driver

### Wi-Fi provisioning
- WiFiManager (or similar)

---

## 4. Constraints

- No blocking delays in main loop
- Use millis() for timing
- Timer must work without Wi-Fi
- Audio must work offline

---

## 5. Expected Output from Codex

Codex should produce:

1. Full Arduino project skeleton
2. GPIO mapping
3. Display driver for 5 TFT modules
4. Character rendering implementation
5. Timer logic
6. Wi-Fi + provisioning implementation
7. NTP time setup
8. I2S audio playback implementation
9. App state machine

---

## 6. Priority Order

1. Display initialization (5 displays)
2. Character rendering
3. Timer logic
4. Buttons
5. Audio
6. Wi-Fi + NTP
7. Provisioning
8. UI improvements

---

## 7. Final Goal

Deliver a working firmware that:
- shows time
- runs a countdown timer
- plays alarm sound
- supports Wi-Fi setup
- renders clear large characters across 5 displays

Focus on **simplicity, reliability, and readability** over complexity.

