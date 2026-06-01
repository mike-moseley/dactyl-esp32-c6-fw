#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "matrix.h"
#include "config.h"

static const char *TAG = "main";

QueueHandle_t key_event_queue;

void app_main(void)
{
    key_event_queue = xQueueCreate(32, sizeof(key_event_t));

    xTaskCreate(matrix_scan_task, "matrix", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "dactyl-fw started, half=%s",
             CENTRAL_HALF ? "central" : "peripheral");

    key_event_t ev;
    while (1) {
        if (xQueueReceive(key_event_queue, &ev, portMAX_DELAY)) {
            ESP_LOGI(TAG, "key r=%d c=%d %s",
                     ev.row, ev.col, ev.pressed ? "dn" : "up");
        }
    }
}
