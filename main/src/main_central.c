#include "ble_split.h"
#include "common.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "matrix.h"

static const char *TAG = "main";
void central_main(void) {
  key_event_t event;
  ble_split_init();
  while (1) {
    if (xQueueReceive(key_event_queue, &event, portMAX_DELAY)) {
      ESP_LOGI(TAG, "key half=%d r=%d c=%d %s", event.half, event.row,
               event.col, event.pressed ? "down" : "up");
    }
  }
}
