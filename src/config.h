#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials
static const char *WIFI_SSID = "<your_wifi_name>";
static const char *WIFI_PASSWORD = "<your_wifi_password>";

// API endpoints - replace with your server IP
static const char *CHAPTER_API = "http://<server-domain-ip>:8000/chapter/image/%d?format=bmp&page=%d&dither_mode=%s&threshold=%d&color_mode=4level";
static const char *API_PREVIOUS = "http://<server-domain-ip>:5000/previous";
static const char *API_NEXT = "http://<server-domain-ip>:5000/next";

// Button pins - safe GPIOs for ESP32 dev module
static const int BUTTON_PREV = 32;
static const int BUTTON_NEXT = 33;
static const int BUTTON_MENU = 25; // GPIO25 for Menu button

// Power saving settings
static const unsigned long WIFI_TIMEOUT = 30000; // 30s
static const unsigned long SLEEP_AFTER = 300000; // 5 minutes
static const unsigned long DEBOUNCE_DELAY = 300; // 300ms

// Display settings
static const int DISPLAY_WIDTH = 400;
static const int DISPLAY_HEIGHT = 300;

#endif
