#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials
static const char *WIFI_SSID = "xxx";
static const char *WIFI_PASSWORD = "xxx";

// API endpoints - replace with your server IP
static const char *CURRENT_PAGE_API = "http://<server-domain-ip>:8000/api/current";
static const char *API_BUTTON = "http://<server-domain-ip>:8000/api/button";

// --- 6-BUTTON CONFIGURATION ---
// GPIOs utilizing internal pull-ups (Active LOW)
static const int BUTTON_PIN_A = 32; // PREV / UP
static const int BUTTON_PIN_B = 33; // NEXT / DOWN
static const int BUTTON_PIN_C = 25; // SELECT / ENTER (Old Menu)
static const int BUTTON_PIN_D = 26; // BACK / CANCEL (New)
static const int BUTTON_PIN_E = 27; // MENU / SETTINGS (New)
static const int BUTTON_PIN_F = 14; // REFRESH / AUX (New)

#define BTN_A_MASK 0x01 // 000001
#define BTN_B_MASK 0x02 // 000010
#define BTN_C_MASK 0x04 // 000100
#define BTN_D_MASK 0x08 // 001000
#define BTN_E_MASK 0x10 // 010000
#define BTN_F_MASK 0x20 // 100000

// Power saving settings
static const unsigned long WIFI_TIMEOUT = 30000; // 30s
static const unsigned long SLEEP_AFTER = 300000; // 5 minutes
// static const unsigned long DEBOUNCE_DELAY = 300; // 300ms
static const unsigned long DEBOUNCE_DELAY = 50; // Reduced for better responsiveness
static const unsigned long LONG_PRESS_MS = 800; // Time to trigger long press

// Display settings
static const int DISPLAY_WIDTH = 400;
static const int DISPLAY_HEIGHT = 300;

enum ButtonEventType
{
    EVENT_NONE,
    EVENT_SHORT_PRESS, // Released quickly
    EVENT_LONG_PRESS   // Held down
};

#endif
