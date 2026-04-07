#ifdef __EMSCRIPTEN__

#include "wasm_mavlink_io.h"
#include <string.h>
#include <emscripten.h>

// Simple ring buffer
struct RingBuf {
    uint8_t data[WASM_MAVLINK_BUF_SIZE];
    volatile size_t head;  // write position
    volatile size_t tail;  // read position
};

// Per-port TX (SITL->JS) and RX (JS->SITL) buffers, support ports 0-2
#define MAX_PORTS 3
static RingBuf tx_buf[MAX_PORTS];
static RingBuf rx_buf[MAX_PORTS];

static inline size_t ring_available(const RingBuf &r) {
    return (r.head - r.tail) % WASM_MAVLINK_BUF_SIZE;
}

static inline size_t ring_space(const RingBuf &r) {
    return WASM_MAVLINK_BUF_SIZE - 1 - ring_available(r);
}

static void ring_write(RingBuf &r, const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        size_t next = (r.head + 1) % WASM_MAVLINK_BUF_SIZE;
        if (next == r.tail) break;  // full, drop
        r.data[r.head] = data[i];
        r.head = next;
    }
}

static size_t ring_read(RingBuf &r, uint8_t *buf, size_t max_len) {
    size_t count = 0;
    while (count < max_len && r.tail != r.head) {
        buf[count++] = r.data[r.tail];
        r.tail = (r.tail + 1) % WASM_MAVLINK_BUF_SIZE;
    }
    return count;
}

// --- C++ side (called by UARTDriver) ---

extern "C" void wasm_mavlink_tx_write(uint8_t port, const uint8_t *data, size_t len) {
    if (port >= MAX_PORTS) return;
    ring_write(tx_buf[port], data, len);
}

extern "C" size_t wasm_mavlink_rx_read(uint8_t port, uint8_t *data, size_t max_len) {
    if (port >= MAX_PORTS) return 0;
    return ring_read(rx_buf[port], data, max_len);
}

// --- JS side (called via cwrap/ccall) ---

extern "C" EMSCRIPTEN_KEEPALIVE
size_t wasm_mavlink_tx_read(uint8_t port, uint8_t *buf, size_t max_len) {
    if (port >= MAX_PORTS) return 0;
    return ring_read(tx_buf[port], buf, max_len);
}

extern "C" EMSCRIPTEN_KEEPALIVE
void wasm_mavlink_rx_write(uint8_t port, const uint8_t *data, size_t len) {
    if (port >= MAX_PORTS) return;
    ring_write(rx_buf[port], data, len);
}

extern "C" EMSCRIPTEN_KEEPALIVE
size_t wasm_mavlink_tx_available(uint8_t port) {
    if (port >= MAX_PORTS) return 0;
    return ring_available(tx_buf[port]);
}

#endif // __EMSCRIPTEN__
