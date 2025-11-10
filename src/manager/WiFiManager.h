#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "config.h"
#include "esp_wifi.h"

class WiFiManager
{
private:
    unsigned long lastActivityTime;

public:
    WiFiManager();
    bool connect();
    void disconnect();
    void manageConnection();
    void updateActivity();
    bool isConnected();
    int getSignalStrength();
};

#endif
