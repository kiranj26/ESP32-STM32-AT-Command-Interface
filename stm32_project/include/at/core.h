/* stm32_project/include/at/core.h */

#ifndef AT_CORE_H
#define AT_CORE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the AT command core
void AT_Init(void);

// Register a callback function for AT responses
typedef void (*AT_ResponseCallback)(const char *response);
void AT_RegisterCallback(AT_ResponseCallback callback);

// Process a received byte from UART
void AT_ProcessReceivedByte(uint8_t received_byte);

#ifdef __cplusplus
}
#endif

#endif // AT_CORE_H
