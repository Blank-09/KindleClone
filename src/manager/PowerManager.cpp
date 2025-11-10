#include "PowerManager.h"

PowerManager::PowerManager() : lastActivityTime(0) {}

void PowerManager::begin()
{
    preferences.begin("epaper", false);
    lastActivityTime = millis();

    // Configure power-saving features
    setCpuFrequencyMhz(80); // Reduce from 240MHz to 80MHz for power saving
}

void PowerManager::updateActivity()
{
    lastActivityTime = millis();
}

bool PowerManager::shouldSleep()
{
    return (millis() - lastActivityTime > SLEEP_AFTER);
}

void PowerManager::goToDeepSleep()
{
    Serial.println("Entering deep sleep...");

    preferences.end();

    // Configure ext0 wakeup on PREV button
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PREV, LOW);

    // Additional power optimization
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);

    esp_deep_sleep_start();
}

esp_sleep_wakeup_cause_t PowerManager::getWakeupCause()
{
    return esp_sleep_get_wakeup_cause();
}

void PowerManager::end()
{
    preferences.end();
}
