#include "ble_split.h"
#include "common.h"
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "host/ble_att.h"
#include "host/ble_hs_adv.h"
#include "host/ble_hs_mbuf.h"
#include "host/ble_uuid.h"
#include "matrix.h"

#include "ble_split_priv.h"
#include "esp_log.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_hs.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "os/os_mbuf.h"
#include "portmacro.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include <stdint.h>
#include <string.h>

static const char *TAG = "ble_split";
static QueueHandle_t s_queue;

const ble_uuid128_t s_svc_uuid =
    BLE_UUID128_INIT(0xfb, 0xec, 0x19, 0x00, 0x16, 0x46, 0x1c, 0xb0, 0xac, 0x4f,
                     0xc2, 0x4b, 0x00, 0x10, 0xc7, 0xda);

const ble_uuid128_t s_chr_uuid =
    BLE_UUID128_INIT(0xfb, 0xec, 0x19, 0x00, 0x16, 0x46, 0x1c, 0xb0, 0xac, 0x4f,
                     0xc2, 0x4b, 0x01, 0x10, 0xc7, 0xda);

void nimble_host_task(void *param) {
  nimble_port_run();
  nimble_port_freertos_deinit();
}

void ble_split_init(void) {
  nimble_port_init();
#if CENTRAL_HALF
  // central_init();
#else
  peripheral_init();
#endif
  nimble_port_freertos_init(nimble_host_task);
}
