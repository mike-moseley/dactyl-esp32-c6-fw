#pragma once

// Which half this firmware is compiled for.
// Left half:  idf.py build
// Right half: idf.py build -DEXTRA_CFLAGS="-DCENTRAL_HALF=0"
#include <stdint.h>
#ifndef CENTRAL_HALF
#define CENTRAL_HALF 1
#endif
#ifndef MIRROR_COLS
#define MIRROR_COLS (!CENTRAL_HALF)
#endif

// Matrix dimensions
#define MATRIX_ROWS 5
#define MATRIX_COLS 5

// Row GPIO pins (output, driven low to select)
#define ROW_PINS  { 18,  19,  20,  21, 22 }
// My row wire colors = {Orange, Brown, Purple, Black, White}

// Column GPIO pins (input, pulled up, read low when key pressed)
#define COL_PINS  { 1, 3, 4, 5, 15 }
// My Column wire colors = {Red, Yellow, Green, Blue, Gray}

// Key map: 1 = key exists, 0 = no switch at this position
// [row][col], col 0 = leftmost
#if CENTRAL_HALF
static const uint8_t KEY_EXISTS[MATRIX_ROWS][MATRIX_COLS] = {
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 1 },
    { 0, 0, 0, 1, 1 },
};
#else
static const uint8_t KEY_EXISTS[MATRIX_ROWS][MATRIX_COLS] = {
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 1 },
    { 0, 0, 0, 1, 1 },
};
#endif

// Debounce time in milliseconds
#define DEBOUNCE 5

// Matrix scan interval in milliseconds
#define SCAN_INTERVAL_MS 1
