#include "ble_hid.h"
#include "host/ble_att.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_hs.h"
#include "host/ble_hs_adv.h"
#include "host/ble_sm.h"
#include "host/ble_uuid.h"
#include "nimble/ble.h"
#include "os/os_mbuf.h"
#include "esp_log.h"
#include <stdint.h>
#include <string.h>

// static const char *manuf_name = "mikem";
static const char *model_num = "Dactyl Manuform 5x5 BLE";
static const char *TAG = "ble_hid";
static uint16_t dm_kb_handle;

// s_report[0] is the modifier key
// s_report[1] is reserved (0x00)
// s_report[2..7] is currently pressed keys
static uint8_t s_report[8];

// Codes from USB HID Usage Tables
// Codes at https://www.usb.org/sites/default/files/hut1_7.pdf
static const uint8_t hid_report_map[] = {
    0x05, 0x01, // Usage Page (Generic Desktop)
    0x09, 0x06, // Usage (Keyboard)
    0xA1, 0x01, // Collection (Application)
    0x05, 0x07, // Usage Page (Keyboard/Keypad)
    0x19, 0xE0, // Usage Minimum (Left Ctrl)
    0x29, 0xE7, // Usage Maximum (Right GUI)
    0x15, 0x00, // Logical Minimum (0)
    0x25, 0x01, // Logical Maximum (1)
    0x75, 0x01, // Report Size (1)
    0x95, 0x08, // Report Count (8)
    0x81, 0x02, // Input (Data, Variable, Absolute)
    0x95, 0x01, // Report Count (1)
    0x75, 0x08, // Report Size (8)
    0x81, 0x01, // Input (Constant)
    0x95, 0x06, // Report Count (6)
    0x75, 0x08, // Report Size (8)
    0x15, 0x00, // Logical Minimum (0)
    0x25, 0x65, // Logical Maximum (101)
    0x05, 0x07, // Usage Page (Keyboard/Keypad)
    0x19, 0x00, // Usage Minimum (0)
    0x29, 0x65, // Usage Maximum (101)
    0x81, 0x00, // Input (Data, Array)
    0xC0,       // End Collection
};
static int gatt_svr_chr_kb(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);

static volatile uint16_t s_conn = BLE_HS_CONN_HANDLE_NONE;
static bool s_subscribed;

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(BT_KEYBOARD_HID_UUID),
     .characteristics =
         (struct ble_gatt_chr_def[]){
             {
                 .uuid = BLE_UUID16_DECLARE(BT_KEYBOARD_REPORT_MAP_UUID),
                 .access_cb = gatt_svr_chr_kb,
                 .flags = BLE_GATT_CHR_F_READ,
             },
             {
                 .uuid = BLE_UUID16_DECLARE(BT_KEYBOARD_HID_INFO_UUID),
                 .access_cb = gatt_svr_chr_kb,
                 .flags = BLE_GATT_CHR_F_READ,
             },
             {
                 .uuid = BLE_UUID16_DECLARE(BT_KEYBOARD_HID_CTL_POINT_UUID),
                 .access_cb = gatt_svr_chr_kb,
                 .flags = BLE_GATT_CHR_F_WRITE_NO_RSP,
             },
             {
                 .uuid = BLE_UUID16_DECLARE(BT_KEYBOARD_PROTOCOL_MODE_UUID),
                 .access_cb = gatt_svr_chr_kb,
                 .flags = BLE_GATT_CHR_F_READ,
             },
             {
                 .uuid = BLE_UUID16_DECLARE(BT_KEYBOARD_REPORT_UUID),
                 .access_cb = gatt_svr_chr_kb,
                 .val_handle = &dm_kb_handle,
                 .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
             },
             {
                 0, /* No more characteristics in this service */
             },
         }},

    {
        0, /* No more services */
    },
};

static int gatt_svr_chr_kb(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
  uint16_t uuid;
  int err;

  uuid = ble_uuid_u16(ctxt->chr->uuid);
  switch (uuid) {
  case BT_KEYBOARD_REPORT_MAP_UUID:
    err = os_mbuf_append(ctxt->om, hid_report_map, sizeof(hid_report_map));
    return err == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  case BT_KEYBOARD_HID_INFO_UUID: {
    uint8_t arr[4] = {0x11, 0x01, 0x00, 0x03};
    err = os_mbuf_append(ctxt->om, arr, sizeof(arr));
    return err == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  case BT_KEYBOARD_HID_CTL_POINT_UUID:
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
      return 0;
    }
    return BLE_ATT_ERR_UNLIKELY;
  case BT_KEYBOARD_PROTOCOL_MODE_UUID: {
    // Always in report mode
    uint8_t arr[1] = {0x01};
    err = os_mbuf_append(ctxt->om, arr, sizeof(arr));
    return err == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  case BT_KEYBOARD_REPORT_UUID:
    err = os_mbuf_append(ctxt->om, s_report, sizeof(s_report));
    return err == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  return 0;
}

void hid_register_svcs(void) {
  ble_hs_cfg.sm_bonding = 1;
  ble_hs_cfg.sm_io_cap = BLE_HS_IO_NO_INPUT_OUTPUT;
  ble_hs_cfg.sm_sc = 1;
  ble_hs_cfg.sm_our_key_dist =
      BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
  ble_hs_cfg.sm_their_key_dist =
      BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;

  ble_gatts_count_cfg(gatt_svr_svcs);
  ble_gatts_add_svcs(gatt_svr_svcs);
}

static void hid_adv_start(void);

static int hid_gap_cb(struct ble_gap_event *event, void *arg) {
  switch (event->type) {
  case BLE_GAP_EVENT_CONNECT:
    if (event->connect.status == 0) {
      s_conn = event->connect.conn_handle;
      ble_gap_security_initiate(s_conn);
      ESP_LOGI(TAG, "Connected to device");
    } else {
      hid_adv_start();
      ESP_LOGI(TAG, "Connection to device failed, advertising...");
    }
    break;
  case BLE_GAP_EVENT_DISCONNECT:
    s_conn = BLE_HS_CONN_HANDLE_NONE;
    hid_adv_start();
    ESP_LOGI(TAG, "Disconnected from device");
    break;
  case BLE_GAP_EVENT_SUBSCRIBE:
    s_subscribed = event->subscribe.cur_notify;
    break;
  case BLE_GAP_EVENT_REPEAT_PAIRING: {
    struct ble_gap_conn_desc desc;
    ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
    ble_gap_unpair(&desc.peer_id_addr);
    return BLE_GAP_REPEAT_PAIRING_RETRY;
  }
  }

  return BLE_ERR_SUCCESS;
}

static void hid_adv_start(void) {
  const struct ble_hs_adv_fields fields = {
      .flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP,
      .uuids16 = (ble_uuid16_t[]){BLE_UUID16_INIT(BT_KEYBOARD_HID_UUID)},
      .appearance = 0x03C1, // keyboard
      .appearance_is_present = 1,
      .num_uuids16 = 1,
      .uuids16_is_complete = 1};
  int err = ble_gap_adv_set_fields(&fields);
  if (err != 0) {
    ESP_LOGE(TAG, "Error setting advertise fields: %d", err);
  }

  const struct ble_hs_adv_fields rsp_fields = {.name =
                                                   (const uint8_t *)model_num,
                                               .name_len = strlen(model_num),
                                               .name_is_complete = 1};
  err = ble_gap_adv_rsp_set_fields(&rsp_fields);
  if (err != 0) {
    ESP_LOGE(TAG, "Error setting advertise response fields: %d", err);
  }

  struct ble_gap_adv_params adv_params = {
      .conn_mode = BLE_GAP_CONN_MODE_UND,
      .disc_mode = BLE_GAP_DISC_MODE_GEN,
  };

  err = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER,
                          &adv_params, hid_gap_cb, NULL);

  if (err != 0) {
    ESP_LOGE(TAG, "Error advertising: %d", err);
  } else {
    ESP_LOGI(TAG, "Advertising started.");
  }
}

void hid_send_report(uint8_t modifier, const uint8_t keys[6]) {
  if (s_conn == BLE_HS_CONN_HANDLE_NONE || !s_subscribed)
    return;

  s_report[0] = modifier;
  s_report[1] = 0x00;
  memcpy(&s_report[2], keys, 6);
  struct os_mbuf *om = ble_hs_mbuf_from_flat(s_report, sizeof(s_report));
  if (om) {
    ble_gatts_notify_custom(s_conn, dm_kb_handle, om);
  }
}
