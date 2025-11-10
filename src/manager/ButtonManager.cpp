#include "ButtonManager.h"

ButtonManager::ButtonManager()
    : lastPressTime(0), prevPressed(false), nextPressed(false), menuPressed(false) {}

void ButtonManager::setup()
{
    pinMode(BUTTON_PREV, INPUT_PULLUP);
    pinMode(BUTTON_NEXT, INPUT_PULLUP);
    pinMode(BUTTON_MENU, INPUT_PULLUP);

    Serial.println("Buttons configured");
}

void ButtonManager::setupDeepSleepWakeup()
{
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PREV, LOW);
    Serial.println("Deep sleep wake-up configured");
}

ButtonAction ButtonManager::getWakeupButton()
{
    if (digitalRead(BUTTON_PREV) == LOW)
    {
        return BUTTON_PREV_PRESSED;
    }
    else if (digitalRead(BUTTON_NEXT) == LOW)
    {
        return BUTTON_NEXT_PRESSED;
    }
    else if (digitalRead(BUTTON_MENU) == LOW)
    {
        return BUTTON_MENU_PRESSED;
    }
    return BUTTON_NONE;
}

ButtonAction ButtonManager::checkButtons()
{
    unsigned long currentTime = millis();

    // Debounce check
    if (currentTime - lastPressTime < DEBOUNCE_DELAY)
    {
        return BUTTON_NONE;
    }

    bool prevState = digitalRead(BUTTON_PREV) == LOW;
    bool nextState = digitalRead(BUTTON_NEXT) == LOW;
    bool menuState = digitalRead(BUTTON_MENU) == LOW;

    ButtonAction action = BUTTON_NONE;

    // Previous button
    if (prevState && !prevPressed)
    {
        prevPressed = true;
        lastPressTime = currentTime;
        action = BUTTON_PREV_PRESSED;
        Serial.println("Previous button pressed!");
    }
    else if (!prevState)
    {
        prevPressed = false;
    }

    // Next button
    if (nextState && !nextPressed)
    {
        nextPressed = true;
        lastPressTime = currentTime;
        action = BUTTON_NEXT_PRESSED;
        Serial.println("Next button pressed!");
    }
    else if (!nextState)
    {
        nextPressed = false;
    }

    // Menu button
    if (menuState && !menuPressed)
    {
        menuPressed = true;
        lastPressTime = currentTime;
        action = BUTTON_MENU_PRESSED;
        Serial.println("Menu button pressed!");
    }
    else if (!menuState)
    {
        menuPressed = false;
    }

    return action;
}
