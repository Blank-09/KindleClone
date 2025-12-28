#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>
#include "config.h"

struct ButtonEvent
{
    ButtonEventType type;
    uint8_t mask; // Which buttons triggered this (e.g., A+B = 3)
};

class ButtonManager
{
private:
    unsigned long pressStartTime;
    uint8_t lastStableMask;
    bool longPressTriggered;

    uint8_t readRawState();

public:
    ButtonManager();
    void setup();
    void setupDeepSleepWakeup();

    // Returns EVENT_NONE if nothing happened
    ButtonEvent checkButtons();
};

#endif
