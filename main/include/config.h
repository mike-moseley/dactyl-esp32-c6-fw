#pragma once

// Which half this firmware is compiled for.
// Set via: idf.py -DEXTRA_CFLAGS="-DCENTRAL_HALF" build
// or define here directly for now.
#include <stdint.h>
#define CENTRAL_HALF 1   // 1 = left (connects to PC), 0 = right (peripheral)

// Matrix dimensions
#define MATRIX_ROWS 5
#define MATRIX_COLS 5

// Row GPIO pins (output, driven low to select)
#define ROW_PINS  { 0, 1, 2, 3, 4 }

// Column GPIO pins (input, pulled up, read low when key pressed)
#define COL_PINS  { 10, 11, 12, 13, 14 }

// Key map: 1 = key exists, 0 = no switch at this position
// [row][col], col 0 = leftmost
#if CENTRAL_HALF
static const uint8_t KEY_EXISTS[MATRIX_ROWS][MATRIX_COLS] = {
    { 0, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 0, 0, 0, 1, 1 },
};
#else
static const uint8_t KEY_EXISTS[MATRIX_ROWS][MATRIX_COLS] = {
    { 1, 1, 1, 1, 0 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0 },
};
#endif

// Debounce time in milliseconds
#define DEBOUNCE 5

// Matrix scan interval in milliseconds
#define SCAN_INTERVAL_MS 1
