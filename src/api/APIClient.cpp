#include "APIClient.h"

APIClient::APIClient() {}

bool APIClient::fetchImage(const char *endpoint, uint8_t *imageBuffer)
{
    HTTPClient http;
    http.begin(endpoint);
    http.setTimeout(15000);
    http.addHeader("User-Agent", "ESP32-ePaper/1.0");

    Serial.print("Fetching: ");
    Serial.println(endpoint);

    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK)
    {
        Serial.printf("HTTP GET failed: %d - %s\n", httpCode, http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }

    int contentLength = http.getSize();
    Serial.printf("Content length: %d bytes\n", contentLength);

    if (contentLength <= 0 || contentLength > 50000)
    {
        Serial.println("Invalid content length!");
        http.end();
        return false;
    }

    WiFiClient *stream = http.getStreamPtr();

    // Allocate temporary buffer
    uint8_t *tempBuffer = (uint8_t *)malloc(contentLength);
    if (!tempBuffer)
    {
        Serial.println("Failed to allocate memory!");
        http.end();
        return false;
    }

    // Optimized read with larger chunks
    int bytesRead = 0;
    unsigned long startTime = millis();
    const int CHUNK_SIZE = 512; // Read in larger chunks

    while (http.connected() && bytesRead < contentLength)
    {
        size_t available = stream->available();
        if (available)
        {
            // Read in chunks for better performance
            int toRead = min((int)available, min(CHUNK_SIZE, contentLength - bytesRead));
            int readBytes = stream->readBytes(&tempBuffer[bytesRead], toRead);
            bytesRead += readBytes;

            // Progress indicator every 5KB instead of 1KB
            if (bytesRead % 5000 == 0)
            {
                Serial.print(".");
            }
        }

        // Timeout check
        if (millis() - startTime > 20000)
        {
            Serial.println("\nDownload timeout!");
            free(tempBuffer);
            http.end();
            return false;
        }

        yield(); // Better than delay(1) for ESP32
    }

    http.end();
    Serial.printf("\nDownloaded: %d bytes\n", bytesRead);

    // Process the downloaded data
    bool success = processBMPData(tempBuffer, bytesRead, imageBuffer);
    free(tempBuffer);

    return success;
}

bool APIClient::processBMPData(uint8_t *tempBuffer, int bytesRead, uint8_t *imageBuffer)
{
    // Check for BMP format
    if (bytesRead > 54 && tempBuffer[0] == 'B' && tempBuffer[1] == 'M')
    {
        Serial.println("Processing BMP...");

        // Read BMP header efficiently
        uint32_t pixelDataOffset = *((uint32_t *)&tempBuffer[10]);
        uint32_t width = *((uint32_t *)&tempBuffer[18]);
        uint32_t height = *((uint32_t *)&tempBuffer[22]);
        uint16_t bitsPerPixel = *((uint16_t *)&tempBuffer[28]);

        Serial.printf("Size: %lux%lu, BPP: %u\n", width, height, bitsPerPixel);

        if (width != DISPLAY_WIDTH || height != DISPLAY_HEIGHT)
        {
            Serial.println("Display size mismatch!");
            return false;
        }

        // Clear buffer once
        memset(imageBuffer, 0, IMAGE_BUFFER_SIZE);

        if (bitsPerPixel == 1)
        {
            Serial.println("Processing 1-bit BMP...");
            int rowBytes = (DISPLAY_WIDTH + 7) / 8;
            int bmpRowBytes = ((DISPLAY_WIDTH + 31) / 32) * 4;

            // Process all rows
            for (int y = 0; y < DISPLAY_HEIGHT; y++)
            {
                int srcRow = DISPLAY_HEIGHT - 1 - y;
                const uint8_t *src = &tempBuffer[pixelDataOffset + srcRow * bmpRowBytes];
                memcpy(&imageBuffer[y * rowBytes], src, rowBytes);
            }

            Serial.println("1-bit BMP processed!");
            return true;
        }
        else if (bitsPerPixel == 4)
        {
            Serial.println("Processing 4-bit BMP...");

            int bmpRowBytes = ((DISPLAY_WIDTH + 1) / 2 + 3) & ~3;

            // Optimized 4-bit to 2-bit conversion
            for (int y = 0; y < DISPLAY_HEIGHT; y++)
            {
                int srcRow = DISPLAY_HEIGHT - 1 - y;
                const uint8_t *src = &tempBuffer[pixelDataOffset + srcRow * bmpRowBytes];

                // Process row in chunks of 4 pixels (1 output byte)
                for (int x = 0; x < DISPLAY_WIDTH; x += 4)
                {
                    uint8_t outputByte = 0;

                    // Process 4 pixels at once
                    for (int i = 0; i < 4 && (x + i) < DISPLAY_WIDTH; i++)
                    {
                        int pixelX = x + i;
                        uint8_t byte = src[pixelX / 2];
                        uint8_t nibble = (pixelX % 2 == 0) ? (byte >> 4) : (byte & 0x0F);
                        uint8_t pixel2bit = nibble >> 2;

                        // Pack into output byte
                        outputByte |= (pixel2bit << (6 - i * 2));
                    }

                    // Write packed byte
                    int idx = (y * DISPLAY_WIDTH + x) / 4;
                    imageBuffer[idx] = outputByte;
                }
            }

            Serial.println("4-bit BMP processed!");
            return true;
        }
        else
        {
            Serial.printf("Unsupported BMP bit depth: %u\n", bitsPerPixel);
            return false;
        }
    }
    // Raw bitmap data
    else if (bytesRead == IMAGE_BUFFER_SIZE)
    {
        Serial.println("Raw bitmap detected");
        memcpy(imageBuffer, tempBuffer, IMAGE_BUFFER_SIZE);
        return true;
    }
    else
    {
        Serial.println("Unknown format!");
        return false;
    }
}

bool APIClient::sendButtonEvent(uint8_t buttonMask, uint8_t eventType)
{
    if (WiFi.status() != WL_CONNECTED)
        return false;

    HTTPClient http;
    http.begin(API_BUTTON);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "ESP32-ePaper/1.0");

    // 1. Create JSON Payload
    // We map bitmasks to readable characters for the Python server
    char btnChar = '?';
    if (buttonMask & BTN_A_MASK)
        btnChar = 'A';
    else if (buttonMask & BTN_B_MASK)
        btnChar = 'B';
    else if (buttonMask & BTN_C_MASK)
        btnChar = 'C';
    else if (buttonMask & BTN_D_MASK)
        btnChar = 'D';
    else if (buttonMask & BTN_E_MASK)
        btnChar = 'E';
    else if (buttonMask & BTN_F_MASK)
        btnChar = 'F';

    // Map Event Type
    const char *actionStr = "single";
    if (eventType == EVENT_LONG_PRESS)
        actionStr = "hold";

    // Construct JSON string manually to save memory/complexity if preferred,
    // or use ArduinoJson. Here is the manual string way (lighter):
    char payload[64];
    snprintf(payload, sizeof(payload), "{\"button\":\"%c\",\"type\":\"%s\"}", btnChar, actionStr);

    Serial.printf("Sending Payload: %s\n", payload);

    // 2. Send POST
    int httpCode = http.POST(payload);
    bool success = false;

    if (httpCode > 0)
    {
        Serial.printf("API Response: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK)
        {
            success = true;
            // Optional: You could read the response body here if the server
            // sends back instructions like {"refresh": true}
        }
    }
    else
    {
        Serial.printf("API Error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    return success;
}
