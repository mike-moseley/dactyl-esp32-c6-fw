#include "keymap.h"
#include "keycodes.h"
#include "matrix.h"
#include <stdint.h>

#define K(kc)   ((key_t){(kc), 0})
#define KS(kc)  ((key_t){(kc), MOD_LSHIFT})

static uint8_t s_layer = 0;

static key_t layout[2][MATRIX_ROWS][MATRIX_COLS] = {
    // right half
    {
        { K(KEY_Y),     K(KEY_U),      K(KEY_I),     K(KEY_O),     K(KEY_P)     },
        { K(KEY_H),     K(KEY_J),      K(KEY_K),     K(KEY_L),     K(KEY_SEMI)  },
        { K(KEY_N),     K(KEY_M),      K(KEY_COMMA),  K(KEY_DOT),   K(KEY_SLASH) },
        { K(KEY_ENTER), K(KEY_LAYER_2),K(KEY_BSLSH),  K(KEY_EQUAL), K(KEY_NONE)  },
        { K(KEY_RGUI),  K(KEY_RALT),   K(KEY_NONE),   K(KEY_NONE),  K(KEY_NONE)  },
    },
    // left half
    {
        { K(KEY_Q),    K(KEY_W),     K(KEY_E),     K(KEY_R),      K(KEY_T)     },
        { K(KEY_A),    K(KEY_S),     K(KEY_D),     K(KEY_F),      K(KEY_G)     },
        { K(KEY_Z),    K(KEY_X),     K(KEY_C),     K(KEY_V),      K(KEY_B)     },
        { K(KEY_NONE), K(KEY_LBRKT), K(KEY_RBRKT), K(KEY_LAYER_1),K(KEY_SPACE) },
        { K(KEY_NONE), K(KEY_NONE),  K(KEY_NONE),  K(KEY_LSHIFT), K(KEY_LCTRL) },
    }
};

// Layer 1: numpad (right), symbols + thumb remaps (left)
static key_t layer_1[2][MATRIX_ROWS][MATRIX_COLS] = {
    // right half
    {
        { K(KEY_NONE),     K(KEY_KP_7),   K(KEY_KP_8), K(KEY_KP_9),   K(KEY_NONE)  },
        { K(KEY_NONE),     K(KEY_KP_4),   K(KEY_KP_5), K(KEY_KP_6),   K(KEY_NONE)  },
        { K(KEY_NONE),     K(KEY_KP_1),   K(KEY_KP_2), K(KEY_KP_3),   K(KEY_NONE)  },
        { K(KEY_KP_ENTER), K(KEY_LAYER_3),K(KEY_KP_0), K(KEY_KP_DOT), K(KEY_NONE)  },
        { K(KEY_RGUI),     K(KEY_RALT),   K(KEY_NONE),  K(KEY_NONE),   K(KEY_NONE)  },
    },
    // left half
    {
        { K(KEY_NONE), K(KEY_NONE),  K(KEY_NONE),  K(KEY_NONE),   K(KEY_NONE)  },
        { K(KEY_NONE), K(KEY_NONE),  K(KEY_MINUS), K(KEY_QUOTE),  K(KEY_GRAVE) },
        { K(KEY_NONE), K(KEY_NONE),  K(KEY_NONE),  K(KEY_NONE),   K(KEY_NONE)  },
        { K(KEY_NONE), K(KEY_LBRKT), K(KEY_RBRKT), K(KEY_LAYER_1),K(KEY_BKSP)  },
        { K(KEY_NONE), K(KEY_NONE),  K(KEY_NONE),  K(KEY_TAB),    K(KEY_ESC)   },
    }
};

// Layer 2: numbers + arrows (right), numbers + shifted symbols (left)
static key_t layer_2[2][MATRIX_ROWS][MATRIX_COLS] = {
    // right half
    {
        { K(KEY_6),    K(KEY_7),      K(KEY_8),   K(KEY_9),    K(KEY_0)    },
        { K(KEY_LEFT), K(KEY_RIGHT),  K(KEY_UP),  K(KEY_DOWN), K(KEY_NONE) },
        { KS(KEY_6),   KS(KEY_7),     KS(KEY_8),  KS(KEY_9),   KS(KEY_0)   },
        { K(KEY_ENTER),K(KEY_LAYER_2),K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
        { K(KEY_RGUI), K(KEY_RALT),   K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
    },
    // left half
    {
        { K(KEY_1),    K(KEY_2),    K(KEY_3),    K(KEY_4),      K(KEY_5)     },
        { K(KEY_NONE), K(KEY_NONE), KS(KEY_TAB), K(KEY_TAB),    K(KEY_NONE)  },
        { KS(KEY_1),   KS(KEY_2),   KS(KEY_3),   KS(KEY_4),     KS(KEY_5)    },
        { K(KEY_NONE), K(KEY_LBRKT),K(KEY_RBRKT),K(KEY_LAYER_3),K(KEY_SPACE) },
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_LSHIFT), K(KEY_LCTRL) },
    }
};

// Layer 3: F-keys (both layer keys held)
static key_t layer_3[2][MATRIX_ROWS][MATRIX_COLS] = {
    // right half
    {
        { K(KEY_NONE), K(KEY_NONE),    K(KEY_NONE), K(KEY_NONE), K(KEY_NONE)  },
        { K(KEY_NONE), K(KEY_NONE),    K(KEY_NONE), K(KEY_NONE), K(KEY_NONE)  },
        { K(KEY_NONE), K(KEY_NONE),    K(KEY_NONE), K(KEY_NONE), K(KEY_NONE)  },
        { K(KEY_NONE), K(KEY_LAYER_2), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE)  },
        { K(KEY_NONE), K(KEY_NONE),    K(KEY_NONE), K(KEY_NONE), K(KEY_NONE)  },
    },
    // left half — F1-F12 across finger rows
    {
        { K(KEY_F1),   K(KEY_F2),     K(KEY_F3),  K(KEY_F4),     K(KEY_NONE)  },
        { K(KEY_F5),   K(KEY_F6),     K(KEY_F7),  K(KEY_F8),     K(KEY_NONE)  },
        { K(KEY_F9),   K(KEY_F10),    K(KEY_F11), K(KEY_F12),    K(KEY_NONE)  },
        { K(KEY_NONE), K(KEY_NONE),   K(KEY_NONE),K(KEY_LAYER_1),K(KEY_NONE)  },
        { K(KEY_NONE), K(KEY_NONE),   K(KEY_NONE),K(KEY_LSHIFT), K(KEY_LCTRL) },
    }
};

static key_t blank[2][MATRIX_ROWS][MATRIX_COLS] = {
    {
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
    },
    {
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
        { K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE), K(KEY_NONE) },
    }
};

static key_t (*const layouts[])[MATRIX_ROWS][MATRIX_COLS] = {
    layout, layer_1, layer_2, layer_3, blank
};

void keymap_set_layer(uint8_t layer) {
    s_layer = layer;
}

key_t coord_to_keycode(uint8_t half, uint8_t row, uint8_t col) {
    return layouts[s_layer][half][row][col];
}
