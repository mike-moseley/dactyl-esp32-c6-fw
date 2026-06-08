#pragma once

// USB HID Keyboard Usage IDs (HUT 1.4, Section 10)
// https://www.usb.org/sites/default/files/hut1_4.pdf

// Letters
#define KEY_A    0x04
#define KEY_B    0x05
#define KEY_C    0x06
#define KEY_D    0x07
#define KEY_E    0x08
#define KEY_F    0x09
#define KEY_G    0x0A
#define KEY_H    0x0B
#define KEY_I    0x0C
#define KEY_J    0x0D
#define KEY_K    0x0E
#define KEY_L    0x0F
#define KEY_M    0x10
#define KEY_N    0x11
#define KEY_O    0x12
#define KEY_P    0x13
#define KEY_Q    0x14
#define KEY_R    0x15
#define KEY_S    0x16
#define KEY_T    0x17
#define KEY_U    0x18
#define KEY_V    0x19
#define KEY_W    0x1A
#define KEY_X    0x1B
#define KEY_Y    0x1C
#define KEY_Z    0x1D

// Numbers (unshifted)
#define KEY_1    0x1E
#define KEY_2    0x1F
#define KEY_3    0x20
#define KEY_4    0x21
#define KEY_5    0x22
#define KEY_6    0x23
#define KEY_7    0x24
#define KEY_8    0x25
#define KEY_9    0x26
#define KEY_0    0x27

// Control keys
#define KEY_ENTER  0x28
#define KEY_ESC    0x29
#define KEY_BKSP   0x2A
#define KEY_TAB    0x2B
#define KEY_SPACE  0x2C
#define KEY_CAPS   0x39

// Punctuation
#define KEY_MINUS  0x2D  // - / _
#define KEY_EQUAL  0x2E  // = / +
#define KEY_LBRKT  0x2F  // [ / {
#define KEY_RBRKT  0x30  // ] / }
#define KEY_BSLSH  0x31  // \ / |
#define KEY_SEMI   0x33  // ; / :
#define KEY_QUOTE  0x34  // ' / "
#define KEY_GRAVE  0x35  // ` / ~
#define KEY_COMMA  0x36  // , / <
#define KEY_DOT    0x37  // . / >
#define KEY_SLASH  0x38  // / / ?

// Function keys
#define KEY_F1   0x3A
#define KEY_F2   0x3B
#define KEY_F3   0x3C
#define KEY_F4   0x3D
#define KEY_F5   0x3E
#define KEY_F6   0x3F
#define KEY_F7   0x40
#define KEY_F8   0x41
#define KEY_F9   0x42
#define KEY_F10  0x43
#define KEY_F11  0x44
#define KEY_F12  0x45

// Navigation
#define KEY_PRTSC  0x46
#define KEY_INS    0x49
#define KEY_HOME   0x4A
#define KEY_PGUP   0x4B
#define KEY_DEL    0x4C
#define KEY_END    0x4D
#define KEY_PGDN   0x4E
#define KEY_RIGHT  0x4F
#define KEY_LEFT   0x50
#define KEY_DOWN   0x51
#define KEY_UP     0x52

// Numpad
#define KEY_KP_SLASH  0x54
#define KEY_KP_STAR   0x55
#define KEY_KP_MINUS  0x56
#define KEY_KP_PLUS   0x57
#define KEY_KP_ENTER  0x58
#define KEY_KP_1      0x59
#define KEY_KP_2      0x5A
#define KEY_KP_3      0x5B
#define KEY_KP_4      0x5C
#define KEY_KP_5      0x5D
#define KEY_KP_6      0x5E
#define KEY_KP_7      0x5F
#define KEY_KP_8      0x60
#define KEY_KP_9      0x61
#define KEY_KP_0      0x62
#define KEY_KP_DOT    0x63

// Modifiers (used as keycodes in keymap arrays — handled separately in event loop)
#define KEY_LCTRL   0xE0
#define KEY_LSHIFT  0xE1
#define KEY_LALT    0xE2
#define KEY_LGUI    0xE3
#define KEY_RCTRL   0xE4
#define KEY_RSHIFT  0xE5
#define KEY_RALT    0xE6
#define KEY_RGUI    0xE7

// Modifier bitmasks (for key_t.modifier field in keymap)
#define MOD_LCTRL   (1 << 0)
#define MOD_LSHIFT  (1 << 1)
#define MOD_LALT    (1 << 2)
#define MOD_LGUI    (1 << 3)
#define MOD_RCTRL   (1 << 4)
#define MOD_RSHIFT  (1 << 5)
#define MOD_RALT    (1 << 6)
#define MOD_RGUI    (1 << 7)

// Sentinel values (not HID keycodes)
#define KEY_NONE   0x00  // no key / empty slot
#define KEY_LAYER  0xFF  // layer switch
