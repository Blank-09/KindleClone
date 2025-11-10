#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <WiFi.h>
#include "config.h"

class DisplayManager
{
private:
    bool firstFullRefreshDone;

public:
    void init();
    void showImage(const uint8_t *imageBuffer);
    void showError(bool wifiConnected);
    void showLoading(const char *message);
    void showMenu();
    void hibernate();
};

#endif
