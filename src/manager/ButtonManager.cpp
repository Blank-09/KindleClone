#include "ButtonManager.h"

ButtonManager::ButtonManager()
    : pressStartTime(0), lastStableMask(0), longPressTriggered(false) {}

void ButtonManager::setup()
{
    // Configure all 6 pins with internal pull-ups
    pinMode(BUTTON_PIN_A, INPUT_PULLUP);
    pinMode(BUTTON_PIN_B, INPUT_PULLUP);
    pinMode(BUTTON_PIN_C, INPUT_PULLUP);
    pinMode(BUTTON_PIN_D, INPUT_PULLUP);
    pinMode(BUTTON_PIN_E, INPUT_PULLUP);
    pinMode(BUTTON_PIN_F, INPUT_PULLUP);

    Serial.println("6-Button Matrix Configured");
}

void ButtonManager::setupDeepSleepWakeup()
{
    // Wake up when Button A is pressed (Low)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN_A, LOW);
}

uint8_t ButtonManager::readRawState()
{
    uint8_t mask = 0;
    // Active LOW logic: !digitalRead returns true (1) if pressed (LOW)
    if (!digitalRead(BUTTON_PIN_A))
        mask |= BTN_A_MASK;
    if (!digitalRead(BUTTON_PIN_B))
        mask |= BTN_B_MASK;
    if (!digitalRead(BUTTON_PIN_C))
        mask |= BTN_C_MASK;
    if (!digitalRead(BUTTON_PIN_D))
        mask |= BTN_D_MASK;
    if (!digitalRead(BUTTON_PIN_E))
        mask |= BTN_E_MASK;
    if (!digitalRead(BUTTON_PIN_F))
        mask |= BTN_F_MASK;
    return mask;
}

ButtonEvent ButtonManager::checkButtons()
{
    ButtonEvent event = {EVENT_NONE, 0};
    uint8_t currentMask = readRawState();
    unsigned long now = millis();

    // 1. Detect State Change
    if (currentMask != lastStableMask)
    {
        delay(20); // Simple mechanical debounce
        uint8_t stableNow = readRawState();

        // If signal is noisy, ignore
        if (stableNow != currentMask)
            return event;

        // Button PRESS (0 -> Something)
        if (lastStableMask == 0 && stableNow != 0)
        {
            pressStartTime = now;
            longPressTriggered = false;
        }

        // Button RELEASE (Something -> 0)
        // Only trigger Short Press if we haven't already fired Long Press
        if (stableNow == 0 && lastStableMask != 0 && !longPressTriggered)
        {
            event.type = EVENT_SHORT_PRESS;
            event.mask = lastStableMask; // Return the buttons that were just released
        }

        lastStableMask = stableNow;
    }

    // 2. Detect Long Press (Hold)
    // If buttons are held down longer than threshold
    if (lastStableMask != 0 && !longPressTriggered)
    {
        if (now - pressStartTime > LONG_PRESS_MS)
        {
            event.type = EVENT_LONG_PRESS;
            event.mask = lastStableMask;
            longPressTriggered = true; // Prevent re-firing
        }
    }

    return event;
}
