/* stm32_project/src/at/core.c */

#include "at/core.h"
#include "hal/uart.h"
#include <string.h>

#define RESPONSE_BUFFER_SIZE 256

static AT_ResponseCallback response_callback = NULL;
static char response_buffer[RESPONSE_BUFFER_SIZE];
static uint16_t response_length = 0;

void AT_Init(void) {
    memset(response_buffer, 0, sizeof(response_buffer));
    response_length = 0;
}

void AT_RegisterCallback(AT_ResponseCallback callback) {
    response_callback = callback;
}

void AT_ProcessReceivedByte(uint8_t received_byte) {
    if (response_length < RESPONSE_BUFFER_SIZE - 1) {
        response_buffer[response_length++] = received_byte;
        response_buffer[response_length] = '\0'; // Null-terminate
    }

    // Check if the response contains "OK" or "ERROR"
    if (strstr(response_buffer, "OK") != NULL) {
        if (response_callback) {
            response_callback("OK");
        }
        // Reset the buffer
        memset(response_buffer, 0, sizeof(response_buffer));
        response_length = 0;
    }
    else if (strstr(response_buffer, "ERROR") != NULL) {
        if (response_callback) {
            response_callback("ERROR");
        }
        // Reset the buffer
        memset(response_buffer, 0, sizeof(response_buffer));
        response_length = 0;
    }
    // Add more response checks as needed
}
