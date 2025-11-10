#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

class PowerManager
{
private:
    unsigned long lastActivityTime;
    Preferences preferences;

public:
    PowerManager();
    void begin();
    void updateActivity();
    bool shouldSleep();
    void goToDeepSleep();
    esp_sleep_wakeup_cause_t getWakeupCause();
    void end();
};

#endif
