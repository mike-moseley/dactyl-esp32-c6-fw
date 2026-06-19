#include "ble_hid.h"
#include "ble_split.h"
#include "common.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "keycodes.h"
#include "keymap.h"
#include "matrix.h"

static const char *TAG = "main";
static uint8_t s_modifier = 0;
static uint8_t s_keys[6] = {0};

void main_central(void) {
  key_t key;
  key_event_t event;
  ble_split_init();
  while (1) {
    if (xQueueReceive(key_event_queue, &event, portMAX_DELAY)) {
      key = coord_to_keycode(event.half, event.row, event.col);
      ESP_LOGI(TAG, "key half=%d r=%d c=%d %s -> kc=0x%02x mod=0x%02x",
               event.half, event.row, event.col,
               event.pressed ? "down" : "up",
               key.keycode, key.modifier);
      if (key.keycode == KEY_NONE) {
        continue;
      } else if (IS_LAYER_KEY(key.keycode)) {
        if (event.pressed) {
          keymap_set_layer(LAYER_FROM_KEY(key.keycode));
        } else {
          keymap_set_layer(0);
        }
      } else if (key.keycode >= 0xE0) {
        uint8_t bit = 1 << (key.keycode - 0xE0);
        if (event.pressed) {
          s_modifier |= bit;
        } else {
          s_modifier &= ~bit;
        }
      } else {
        if (event.pressed) {
          for (int i = 0; i < 6; i++) {
            if (s_keys[i] == key.keycode) break;
            if (s_keys[i] == 0x00) {
              s_keys[i] = key.keycode;
              s_modifier |= key.modifier;
              break;
            }
          }
        } else {
          for (int i = 0; i < 6; i++) {
            if (s_keys[i] == key.keycode) {
              s_keys[i] = 0x00;
              s_modifier &= ~key.modifier;
              break;
            }
          }
        }
      }
      hid_send_report(s_modifier, s_keys);
    }
  }
}
