#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <HTTPClient.h>
#include "config.h"

// Buffer sizes
#define IMAGE_BUFFER_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT / 4)        // 1-bit: 1 pixel per bit
#define IMAGE_BUFFER_SIZE_4LEVEL (DISPLAY_WIDTH * DISPLAY_HEIGHT / 4) // 4-level: 2 bits per pixel

class APIClient
{
private:
    bool processBMPData(uint8_t *tempBuffer, int bytesRead, uint8_t *imageBuffer);

public:
    APIClient();
    bool fetchImage(const char *endpoint, uint8_t *imageBuffer);

    // NEW: Send button event to server
    // Returns true if server says "refresh needed" or command success
    bool sendButtonEvent(uint8_t buttonMask, uint8_t eventType);
};

#endif
