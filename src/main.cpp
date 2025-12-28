#include <Arduino.h>
#include <GxEPD2_4G_4G.h>
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

// Image buffer
uint8_t imageBuffer[IMAGE_BUFFER_SIZE_4LEVEL];
bool imageLoaded = false;

// Function declarations
void handleButtonEvent(ButtonEvent event);
bool loadAndDisplayImage();

void setup()
{
    Serial.begin(115200);
    Serial.println("ePaper Reader - Starting...");

    // Initialize managers
    displayManager.init();
    buttonManager.setup();
    buttonManager.setupDeepSleepWakeup();
    powerManager.begin();

    // Check wake-up reason
    esp_sleep_wakeup_cause_t wakeupReason = powerManager.getWakeupCause();

    if (wakeupReason == ESP_SLEEP_WAKEUP_EXT0)
    {
        Serial.println("Woke from Deep Sleep (Button A)");

        // CRITICAL CHANGE:
        // We know Button A woke us. We manually create an event for it.
        // This ensures the device immediately goes to the "Previous Page"
        // without waiting for the button to be released.

        ButtonEvent wakeEvent;
        wakeEvent.type = EVENT_SHORT_PRESS;
        wakeEvent.mask = BTN_A_MASK; // We know it was Button A (Pin 32)

        handleButtonEvent(wakeEvent);
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
    ButtonEvent event = buttonManager.checkButtons();
    if (event.type != EVENT_NONE)
    {
        handleButtonEvent(event);
        powerManager.updateActivity();
    }

    // Manage WiFi connection
    wifiManager.manageConnection();

    // Check if should go to sleep
    if (powerManager.shouldSleep())
    {
        displayManager.hibernate();
        powerManager.goToDeepSleep();
    }

    delay(20);
}

void handleButtonEvent(ButtonEvent event)
{
    // Ensure WiFi for any action
    if (!wifiManager.isConnected())
        wifiManager.connect();

    Serial.printf("Event: Type=%d, Mask=%d\n", event.type, event.mask);

    // --- GLOBAL SHORTCUTS ---

    // Reboot: Hold A + F together
    if (event.mask == (BTN_A_MASK | BTN_F_MASK) && event.type == EVENT_LONG_PRESS)
    {
        Serial.println("Rebooting...");
        ESP.restart();
    }

    bool serverReceived = apiClient.sendButtonEvent(event.mask, event.type);

    if (serverReceived)
    {
        Serial.println("Server processed event. Updating view...");

        // 3. Update View
        // We assume the server updated its internal state (changed page, opened menu).
        // Now we just fetch the "current" view.
        imageLoaded = loadAndDisplayImage();
        if (!imageLoaded)
        {
            displayManager.showError(true);
        }
    }
    else
    {
        Serial.println("Server failed to respond.");
        // Optional: Show a small "!" icon or error
    }
}

bool loadAndDisplayImage()
{
    // Fetch and display
    wifiManager.updateActivity();

    if (apiClient.fetchImage(CURRENT_PAGE_API, imageBuffer))
    {
        displayManager.showImage(imageBuffer);
        return true;
    }

    return false;
}
