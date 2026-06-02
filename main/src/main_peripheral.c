#include "ble_split.h"
#include "common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void peripheral_main(void) {
  ble_split_init();
  while (1) vTaskDelay(portMAX_DELAY);
}
