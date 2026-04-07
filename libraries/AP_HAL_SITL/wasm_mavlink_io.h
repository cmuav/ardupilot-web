#pragma once

#ifdef __EMSCRIPTEN__

#include <stdint.h>
#include <stddef.h>

// Ring buffer size - Browser reads MAVLINK from here
#define WASM_MAVLINK_BUF_SIZE 65536

#ifdef __cplusplus
extern "C" {
#endif

// Called by C++ UARTDriver to push outbound MAVLink bytes (SITL -> JS)
void wasm_mavlink_tx_write(uint8_t port, const uint8_t *data, size_t len);

// Called by C++ UARTDriver to read inbound bytes (JS -> SITL)
size_t wasm_mavlink_rx_read(uint8_t port, uint8_t *data, size_t max_len);

// Called from JS to read outbound MAVLink bytes (SITL -> JS)
// Returns number of bytes written into buf.
size_t wasm_mavlink_tx_read(uint8_t port, uint8_t *buf, size_t max_len);

// Called from JS to push inbound bytes (JS -> SITL)
void wasm_mavlink_rx_write(uint8_t port, const uint8_t *data, size_t len);

// Returns number of bytes available in the TX buffer for a port
size_t wasm_mavlink_tx_available(uint8_t port);

#ifdef __cplusplus
}
#endif

#endif // __EMSCRIPTEN__
