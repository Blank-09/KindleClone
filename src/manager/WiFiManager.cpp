#include "WiFiManager.h"

WiFiManager::WiFiManager() : lastActivityTime(0) {}

bool WiFiManager::connect()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }

    Serial.print("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Optimized connection loop
    unsigned long startTime = millis();
    const unsigned long TIMEOUT = 15000; // 15 seconds timeout

    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - startTime > TIMEOUT)
        {
            Serial.println("\nWiFi connection failed!");
            return false;
        }

        delay(500);
        Serial.print(".");
    }

    Serial.printf("\nWiFi connected! IP: %s, RSSI: %d dBm\n",
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());

    lastActivityTime = millis();
    return true;
}

void WiFiManager::disconnect()
{
    Serial.println("WiFi disconnecting");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    // Enable WiFi sleep for power saving
    esp_wifi_stop();
}

void WiFiManager::manageConnection()
{
    if (WiFi.status() == WL_CONNECTED &&
        millis() - lastActivityTime > WIFI_TIMEOUT)
    {
        disconnect();
    }
}

void WiFiManager::updateActivity()
{
    lastActivityTime = millis();
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

int WiFiManager::getSignalStrength()
{
    return WiFi.RSSI();
}
