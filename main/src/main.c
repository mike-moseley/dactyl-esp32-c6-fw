#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "config.h"
#include "matrix.h"
#include "nvs_flash.h"

static const char *TAG = "main";

QueueHandle_t key_event_queue;
void main_central(void);
void main_peripheral(void);

void app_main(void) {
  vTaskDelay(pdMS_TO_TICKS(1000));
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  key_event_queue = xQueueCreate(32, sizeof(key_event_t));

  xTaskCreate(matrix_scan_task, "matrix", 2048, NULL, 5, NULL);

#if CENTRAL_HALF
  central_main();
#else
  main_peripheral();
#endif

  ESP_LOGI(TAG, "dactyl-fw started, half=%s",
           CENTRAL_HALF ? "central" : "peripheral");

  key_event_t event;
  while (1) {
    if (xQueueReceive(key_event_queue, &event, portMAX_DELAY)) {
      ESP_LOGI(TAG, "key r=%d c=%d %s", event.row, event.col,
               event.pressed ? "dn" : "up");
    }
  }
}
