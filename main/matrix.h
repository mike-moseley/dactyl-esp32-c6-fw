#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MATRIX_ROWS 5
#define MATRIX_COLS 5

typedef struct {
    uint8_t row;
    uint8_t col;
    bool    pressed;
} key_event_t;

void matrix_init(void);

// Scans the matrix and pushes key_event_t into the event queue.
// Call from a FreeRTOS task.
void matrix_scan_task(void *arg);
