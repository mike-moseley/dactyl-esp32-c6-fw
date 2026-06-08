#pragma once

#include <stdint.h>

// TODO: add mod-tap support (hold = modifier, tap = keycode)
// TODO: add shifted keycodes for symbols like !@#$%^&*() using key_t.modifier

typedef struct {
    uint8_t keycode;
    uint8_t modifier;
} key_t;

key_t coord_to_keycode(uint8_t half, uint8_t row, uint8_t col);
void keymap_set_layer(uint8_t layer);
