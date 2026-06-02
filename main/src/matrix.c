#include "matrix.h"
#include "common.h"
#include "config.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static const int row_pins[MATRIX_ROWS] = ROW_PINS;
static const int col_pins[MATRIX_COLS] = COL_PINS;

static bool prev_state[MATRIX_ROWS][MATRIX_COLS];
static uint8_t debounce[MATRIX_ROWS][MATRIX_COLS];

void matrix_init(void) {
  for (int r = 0; r < MATRIX_ROWS; r++) {
    gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << row_pins[r],
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);
    gpio_set_level(row_pins[r], 1);
  }

  for (int c = 0; c < MATRIX_COLS; c++) {
    gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << col_pins[c],
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);
  }
}

void matrix_scan_task(void *arg) {
  matrix_init();

  while (1) {
    for (int r = 0; r < MATRIX_ROWS; r++) {
      gpio_set_level(row_pins[r], 0);
      esp_rom_delay_us(5);

      for (int c = 0; c < MATRIX_COLS; c++) {
        if (!KEY_EXISTS[r][c])
          continue;

        bool pressed = gpio_get_level(col_pins[c]) == 0;

        if (pressed != prev_state[r][c]) {
          if (++debounce[r][c] >= DEBOUNCE) {
            debounce[r][c] = 0;
            prev_state[r][c] = pressed;
            key_event_t ev = {
                .row = r, .col = c, .half = CENTRAL_HALF, .pressed = pressed};
            xQueueSend(key_event_queue, &ev, 0);
          }
        } else {
          debounce[r][c] = 0;
        }
      }

      gpio_set_level(row_pins[r], 1);
    }

    vTaskDelay(pdMS_TO_TICKS(SCAN_INTERVAL_MS));
  }
}
