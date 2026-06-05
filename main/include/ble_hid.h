#pragma once
#include "stdint.h"

#define BT_KEYBOARD_HID_UUID 0x1812
#define BT_KEYBOARD_REPORT_MAP_UUID 0x2A4B
#define BT_KEYBOARD_HID_INFO_UUID 0x2A4A
#define BT_KEYBOARD_HID_CTL_POINT_UUID 0x2A4C
#define BT_KEYBOARD_PROTOCOL_MODE_UUID 0x2A4E
#define BT_KEYBOARD_REPORT_UUID 0x2A4D

#define HID_MOD_LCTRL (1 << 0)
#define HID_MOD_LSHIFT (1 << 1)
#define HID_MOD_LALT (1 << 2)
#define HID_MOD_LGUI (1 << 3)
#define HID_MOD_RCTRL (1 << 4)
#define HID_MOD_RSHIFT (1 << 5)
#define HID_MOD_RALT (1 << 6)
#define HID_MOD_RGUI (1 << 7)

void hid_register_svcs(void);
void hid_start_adv(void);
void hid_send_report(uint8_t modifier, const uint8_t keys[6]);
