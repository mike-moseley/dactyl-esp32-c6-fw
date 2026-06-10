#include "ble_split.h"
#include "common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void main_peripheral(void) {
  ble_split_init();
  while (1) vTaskDelay(portMAX_DELAY);
}
