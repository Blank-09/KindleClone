#include "PowerManager.h"

PowerManager::PowerManager() : lastActivityTime(0) {}

void PowerManager::begin()
{
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

    // 1. Configure Wake-up Source
    // We use ext0 because it uses less power than ext1.
    // It only monitors a single pin (Button A / Pin 32).
    // Logic: Wake up when Pin 32 goes LOW (Pressed).
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN_A, LOW);

    // 2. Power Down Peripherals
    // We don't need RTC memory or peripherals during sleep for this project
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);

    // 3. Start Deep Sleep
    esp_deep_sleep_start();

    // Code never reaches here
}

esp_sleep_wakeup_cause_t PowerManager::getWakeupCause()
{
    return esp_sleep_get_wakeup_cause();
}
