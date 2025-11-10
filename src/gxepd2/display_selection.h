#pragma once
#include <GxEPD2_4G_4G.h> // Changed from GxEPD2_4G_BW

// -----------------------------------------------------
// DISPLAY SELECTION
// -----------------------------------------------------
#define GxEPD2_DISPLAY_CLASS GxEPD2_420_4G
#define GxEPD2_DRIVER_CLASS GxEPD2_420_GDEY042T81 // 400x300 SSD1683

#ifndef EPD_CS
#define EPD_CS SS
#endif

#define GxEPD2_4G_4G_IS_GxEPD2_4G_4G true
#define IS_GxEPD(c, x) (c##x)
#define IS_GxEPD2_4G_4G(x) IS_GxEPD(GxEPD2_4G_4G_IS_, x)

// -----------------------------------------------------
// BUFFER SIZE LOGIC (needed everywhere display is used)
// -----------------------------------------------------
#if defined(ARDUINO_ARCH_ESP32)
#define MAX_DISPLAY_BUFFER_SIZE 65536ul // 64KB RAM chunk

// MAX_HEIGHT decides how many rows fit in RAM at once.
// For 4-level grayscale, we need 2 bits per pixel
#define MAX_HEIGHT(EPD) (                                       \
    (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 4)) \
        ? EPD::HEIGHT                                           \
        : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 4))
#endif

// -----------------------------------------------------
// GLOBAL DISPLAY OBJECT (declaration only – NO constructor)
// -----------------------------------------------------
extern GxEPD2_4G_4G<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display;
