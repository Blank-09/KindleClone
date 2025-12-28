#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>
#include "config.h"

class PowerManager
{
private:
    unsigned long lastActivityTime;

public:
    PowerManager();
    void begin();
    void updateActivity();
    bool shouldSleep();
    void goToDeepSleep();
    esp_sleep_wakeup_cause_t getWakeupCause();
};

#endif
