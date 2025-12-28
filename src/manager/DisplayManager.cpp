#include "DisplayManager.h"
#include "gxepd2/display_selection.h"

void DisplayManager::init()
{
    display.init(115200);
    display.setRotation(0);
    display.setFullWindow();
    Serial.println("Display initialized with full refresh mode");
}

void DisplayManager::showImage(const uint8_t *imageBuffer)
{

    // Optimized rendering - process 4 pixels per byte
    display.firstPage();
    do
    {
        int bufferIndex = 0;

        // Render by rows for better cache performance
        for (int y = 0; y < DISPLAY_HEIGHT; y++)
        {
            for (int x = 0; x < DISPLAY_WIDTH; x += 4)
            {
                // Each byte contains 4 pixels (2 bits each)
                uint8_t pixelByte = imageBuffer[bufferIndex++];

                // Extract and draw 4 pixels at once
                uint16_t colors[4] = {
                    (pixelByte >> 6) & 0x03, // First pixel
                    (pixelByte >> 4) & 0x03, // Second pixel
                    (pixelByte >> 2) & 0x03, // Third pixel
                    pixelByte & 0x03         // Fourth pixel
                };

                // Draw the 4 pixels
                for (int i = 0; i < 4 && (x + i) < DISPLAY_WIDTH; i++)
                {
                    uint16_t color;
                    switch (colors[i])
                    {
                    case 0:
                        color = GxEPD_BLACK;
                        break;
                    case 1:
                        color = GxEPD_DARKGREY;
                        break;
                    case 2:
                        color = GxEPD_LIGHTGREY;
                        break;
                    case 3:
                        color = GxEPD_WHITE;
                        break;
                    default:
                        color = GxEPD_WHITE;
                        break;
                    }
                    display.drawPixel(x + i, y, color);
                }
            }
        }

    } while (display.nextPage());

    display.hibernate();
}

void DisplayManager::showError(bool wifiConnected)
{
    display.setRotation(0);
    display.setFullWindow();
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(50, 130);
        display.setTextColor(GxEPD_BLACK);
        display.setTextSize(2);
        display.print("Failed to load");

        display.setCursor(50, 160);
        display.print("Check connection");

        display.setTextSize(1);
        display.setCursor(50, 190);
        display.print(wifiConnected ? "WiFi: Connected" : "WiFi: Disconnected");

    } while (display.nextPage());

    Serial.println("Error message displayed");
}

void DisplayManager::hibernate()
{
    display.hibernate();
}
