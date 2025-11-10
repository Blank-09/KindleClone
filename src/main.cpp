#include <Arduino.h>
#include <GxEPD2_4G_4G.h>
#include <Preferences.h>
#include "gxepd2/display_selection.h"

#include "config.h"
#include "api/APIClient.h"
#include "manager/ButtonManager.h"
#include "manager/WiFiManager.h"
#include "manager/DisplayManager.h"
#include "manager/PowerManager.h"

#define ENABLE_GxEPD2_GFX 1

GxEPD2_4G_4G<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(
    GxEPD2_DRIVER_CLASS(/*CS=*/EPD_CS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4));

// Global instances
ButtonManager buttonManager;
WiFiManager wifiManager;
APIClient apiClient;
DisplayManager displayManager;
PowerManager powerManager;
Preferences prefs;

// Image buffer
uint8_t imageBuffer[IMAGE_BUFFER_SIZE_4LEVEL];
bool imageLoaded = false;
int currentPage = 0;
int threshold = 128;
const char *modes[] = {"floyd_steinberg", "threshold"};
uint8_t currentModeIndex = 0;

// Function declarations
void handleButtonAction(ButtonAction action);
bool loadAndDisplayImage();
void updatePage(int delta);
void toggleMode();

void setup()
{
    Serial.begin(115200);
    Serial.println("ePaper Reader - Starting...");

    // Initialize preferences first to restore state
    prefs.begin("epaper", false);
    currentPage = prefs.getInt("page", 0);
    currentModeIndex = prefs.getUChar("mode", 0);

    // Initialize managers
    displayManager.init();
    buttonManager.setup();
    buttonManager.setupDeepSleepWakeup();
    powerManager.begin();

    // Check wake-up reason
    esp_sleep_wakeup_cause_t wakeupReason = powerManager.getWakeupCause();

    if (wakeupReason == ESP_SLEEP_WAKEUP_EXT0)
    {
        Serial.println("Woke from button press");
        ButtonAction action = buttonManager.getWakeupButton();
        handleButtonAction(action);
    }
    else
    {
        Serial.println("Initial startup");

        if (wifiManager.connect())
        {
            imageLoaded = loadAndDisplayImage();
            if (!imageLoaded)
            {
                displayManager.showError(true);
            }
        }
        else
        {
            displayManager.showError(false);
        }
    }

    powerManager.updateActivity();
}

void loop()
{
    // Check for button presses
    ButtonAction action = buttonManager.checkButtons();
    if (action != BUTTON_NONE)
    {
        handleButtonAction(action);
        powerManager.updateActivity();
    }

    // Manage WiFi connection
    wifiManager.manageConnection();

    // Check if should go to sleep
    if (powerManager.shouldSleep())
    {
        prefs.end(); // Save preferences before sleep
        displayManager.hibernate();
        powerManager.goToDeepSleep();
    }

    delay(100);
}

void handleButtonAction(ButtonAction action)
{
    // Ensure WiFi is connected
    if (!wifiManager.isConnected())
    {
        wifiManager.connect();
    }

    switch (action)
    {
    case BUTTON_PREV_PRESSED:
        Serial.println("Previous page");
        updatePage(-1);
        break;

    case BUTTON_NEXT_PRESSED:
        Serial.println("Next page");
        updatePage(1);
        break;

    case BUTTON_MENU_PRESSED:
        Serial.println("Toggle mode");
        toggleMode();
        break;

    default:
        break;
    }
}

void updatePage(int delta)
{
    currentPage += delta;
    prefs.putInt("page", currentPage);

    imageLoaded = loadAndDisplayImage();
    if (!imageLoaded)
    {
        displayManager.showError(wifiManager.isConnected());
    }
}

void toggleMode()
{
    currentModeIndex = (currentModeIndex + 1) % 2;
    prefs.putUChar("mode", currentModeIndex);

    Serial.printf("Mode: %s\n", modes[currentModeIndex]);

    imageLoaded = loadAndDisplayImage();
    if (!imageLoaded)
    {
        displayManager.showError(wifiManager.isConnected());
    }
}

bool loadAndDisplayImage()
{
    // Build URL
    static char url[256];
    snprintf(url, sizeof(url), CHAPTER_API, 250, currentPage,
             modes[currentModeIndex], threshold);

    // Fetch and display
    wifiManager.updateActivity();

    if (apiClient.fetchImage(url, imageBuffer))
    {
        displayManager.showImage(imageBuffer);
        return true;
    }

    return false;
}
