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
#include "ble_split_priv.h"

static const char *TAG = "ble_peripheral";

static volatile uint16_t s_conn = BLE_HS_CONN_HANDLE_NONE;
static uint16_t s_val_handle;

static int svc_cb_fn(uint16_t conn_handle, uint16_t attr_handle,
                     struct ble_gatt_access_ctxt *ctxt, void *arg) {
  return BLE_ATT_ERR_UNLIKELY;
}

static const struct ble_gatt_svc_def s_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &s_svc_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){{
                                            .uuid = &s_chr_uuid.u,
                                            .flags = BLE_GATT_CHR_F_NOTIFY,
                                            .access_cb = &svc_cb_fn,
                                            .val_handle = &s_val_handle,
                                        },
                                        {0}},
    },
    {0}};

static void adv_start(void);

static int peripheral_gap_cb(struct ble_gap_event *event, void *arg) {
  switch (event->type) {
  case BLE_GAP_EVENT_CONNECT:
    if (event->connect.status == 0) {
      s_conn = event->connect.conn_handle;
      ESP_LOGI(TAG, "Central connected");
    } else {
      adv_start();
      ESP_LOGI(TAG, "Connection failed, advertising...");
    }
    break;
  case BLE_GAP_EVENT_DISCONNECT:
    s_conn = BLE_HS_CONN_HANDLE_NONE;
    adv_start();
    ESP_LOGI(TAG, "Central disconnected");
    break;
  }

  return BLE_ERR_SUCCESS;
}

static void adv_start(void) {
  const struct ble_hs_adv_fields fields = {
      .flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP,
      .uuids128 = &s_svc_uuid,
      .num_uuids128 = 1,
      .uuids128_is_complete = 1,
  };
  int err = ble_gap_adv_set_fields(&fields);
  if (err != 0) {
    ESP_LOGE(TAG, "Error setting advertise fields: %d", err);
  }
  struct ble_gap_adv_params adv_params = {
      .conn_mode = BLE_GAP_CONN_MODE_UND,
      .disc_mode = BLE_GAP_DISC_MODE_GEN,
  };
  err = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER,
                          &adv_params, peripheral_gap_cb, NULL);

  if (err != 0) {
    ESP_LOGE(TAG, "Error advertising: %d", err);
  } else {
    ESP_LOGI(TAG, "Advertising started.");
  }
}

static void sender_task(void *arg) {
	key_event_t event;
	while(1) {
		if (!xQueueReceive(key_event_queue, &event, portMAX_DELAY)) continue;
		if (s_conn != BLE_HS_CONN_HANDLE_NONE) {
			uint8_t buf[3] = {event.row, event.col, event.pressed};
			struct os_mbuf *om = ble_hs_mbuf_from_flat(buf, sizeof(buf));
			if (om) {
				ble_gatts_notify_custom(s_conn, s_val_handle, om);
			}
		}
	}
}

static void on_sync(void) {
  uint8_t own_addr_type;
	ble_hs_id_infer_auto(0, &own_addr_type);
	adv_start();
}

static void on_reset(int reason) {
	ESP_LOGE(TAG, "BLE host reset, reason: %d", reason);
}

void peripheral_init(void) {
	ble_hs_cfg.sync_cb = on_sync;
	ble_hs_cfg.reset_cb = on_reset;
	ble_svc_gap_init();
	ble_svc_gatt_init();
	ble_gatts_count_cfg(s_svcs);
	ble_gatts_add_svcs(s_svcs);
	xTaskCreate(sender_task, "ble_sender", 4096, NULL, 4, NULL);
}
