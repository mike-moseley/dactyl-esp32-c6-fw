#include "ble_hid.h"
#include "ble_split.h"
#include "common.h"
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "host/ble_att.h"
#include "host/ble_hs_adv.h"
#include "host/ble_hs_id.h"
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
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include <stdint.h>
#include <string.h>

static const char *TAG = "ble_central";
static uint16_t s_conn = BLE_HS_CONN_HANDLE_NONE;
static uint16_t s_chr_val_handle;

static int on_cccd_write(uint16_t conn_handle,
                         const struct ble_gatt_error *error,
                         struct ble_gatt_attr *attr, void *arg) {
  if (error->status == BLE_ERR_SUCCESS) {
    ESP_LOGI(TAG, "Subscribed");
  } else {
    ESP_LOGE(TAG, "Error writing CCCD: %d", error->status);
  }
  return 0;
}

static int on_chr_discovered(uint16_t conn_handle,
                             const struct ble_gatt_error *error,
                             const struct ble_gatt_chr *chr, void *arg) {
  if (error->status != 0)
    return 0;
  if (chr == NULL)
    return 0;
  if (ble_uuid_cmp(&chr->uuid.u, &s_chr_uuid.u) == 0) {
    s_chr_val_handle = chr->val_handle;
    uint8_t cccd[2] = {0x01, 0x00};
    struct os_mbuf *om = ble_hs_mbuf_from_flat(cccd, sizeof(cccd));
    ble_gattc_write(conn_handle, s_chr_val_handle + 1, om, on_cccd_write, NULL);
  }
  return 0;
}

static int on_svc_discovered(uint16_t conn_handle,
                             const struct ble_gatt_error *error,
                             const struct ble_gatt_svc *svc, void *arg) {
  if (error->status != 0)
    return 0;
  if (svc == NULL)
    return 0;
  if (ble_uuid_cmp(&svc->uuid.u, &s_svc_uuid.u) == 0) {
    ble_gattc_disc_all_chrs(conn_handle, svc->start_handle, svc->end_handle,
                            on_chr_discovered, NULL);
  }
  return 0;
}

static bool adv_has_svc_uuid(const uint8_t *data, uint8_t length_data) {
  struct ble_hs_adv_fields adv_fields;
  memset(&adv_fields, 0, sizeof(adv_fields));
  int err = ble_hs_adv_parse_fields(&adv_fields, data, length_data);
  if (err != 0) {
    ESP_LOGE(TAG, "Error parsing advertisement fields: %d", err);
    return false;
  }

  for (int i = 0; i < adv_fields.num_uuids128; i++) {
    if (ble_uuid_cmp(&adv_fields.uuids128[i].u, &s_svc_uuid.u) == 0) {
      return true;
    }
  }
  return false;
}

static void scan_start(void);

static int central_gap_cb(struct ble_gap_event *event, void *arg) {
  switch (event->type) {
  case BLE_GAP_EVENT_DISC:
    if (adv_has_svc_uuid(event->disc.data, event->disc.length_data)) {
      ble_gap_disc_cancel();
      ble_gap_connect(BLE_OWN_ADDR_PUBLIC, &event->disc.addr, BLE_HS_FOREVER,
                      NULL, central_gap_cb, NULL);
    }
    break;

  case BLE_GAP_EVENT_CONNECT:
    if (event->connect.status == 0) {
      s_conn = event->connect.conn_handle;
      ESP_LOGI(TAG, "Connected: %d", s_conn);
      ble_gattc_disc_all_svcs(s_conn, on_svc_discovered, NULL);
    } else {
      ESP_LOGE(TAG, "Error on GAP connect");
      scan_start();
    }
    break;

  case BLE_GAP_EVENT_DISCONNECT:
    s_conn = BLE_HS_CONN_HANDLE_NONE;
    ESP_LOGI(TAG, "Disconnected");
    scan_start();
    break;

  case BLE_GAP_EVENT_NOTIFY_RX:
    if (event->notify_rx.attr_handle == s_chr_val_handle) {
      if (OS_MBUF_PKTLEN(event->notify_rx.om) >= 3) {
        uint8_t buf[3];
        os_mbuf_copydata(event->notify_rx.om, 0, 3, buf);
        key_event_t key_event = {
            .row = buf[0],
            .col = buf[1],
            .pressed = buf[2],
            .half = !CENTRAL_HALF,
        };
        xQueueSend(key_event_queue, &key_event, 0);
      }
    }
    break;
  }
  return 0;
}

static void scan_start(void) {
  struct ble_gap_disc_params params = {
      .filter_duplicates = 1,
      .passive = 0,
  };
  int err = ble_gap_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER, &params,
                         central_gap_cb, NULL);
  if (err != 0) {
    ESP_LOGE(TAG, "Error starting scan: %d", err);
  }
}

static void on_sync(void) {
  ble_hs_id_infer_auto(0, NULL);
  scan_start();
  hid_start_adv();
}

static void on_reset(int reason) {
  ESP_LOGE(TAG, "BLE host reset, reason: %d", reason);
}
void central_init(void) {
  ble_hs_cfg.sync_cb = on_sync;
  ble_hs_cfg.reset_cb = on_reset;
  ble_svc_gap_init();
  ble_svc_gatt_init();
  hid_register_svcs();
}
