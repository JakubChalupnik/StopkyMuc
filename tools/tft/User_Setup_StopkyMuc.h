// StopkyMuc TFT_eSPI setup for WeMos LOLIN S2 Mini + ST7789 170x320 panels.
// Copy this file over your TFT_eSPI User_Setup.h before compiling.

#define USER_SETUP_INFO "StopkyMuc_ESP32S2_ST7789_170x320"

#define ST7789_DRIVER

#define TFT_WIDTH  170
#define TFT_HEIGHT 320

#define TFT_MISO -1
#define TFT_MOSI 35
#define TFT_SCLK 36
#define TFT_CS   -1
#define TFT_DC   37
#define TFT_RST  33
#define TFT_BL   34
#define TFT_BACKLIGHT_ON HIGH

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

#define SPI_FREQUENCY  40000000
