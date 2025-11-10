#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>
#include "config.h"

enum ButtonAction
{
    BUTTON_NONE,
    BUTTON_PREV_PRESSED,
    BUTTON_NEXT_PRESSED,
    BUTTON_MENU_PRESSED
};

class ButtonManager
{
private:
    unsigned long lastPressTime;
    bool prevPressed;
    bool nextPressed;
    bool menuPressed;

public:
    ButtonManager();
    void setup();
    ButtonAction checkButtons();
    void setupDeepSleepWakeup();
    ButtonAction getWakeupButton();
};

#endif
