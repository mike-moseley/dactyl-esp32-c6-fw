#pragma once

#include "freertos/idf_additions.h"
#include "host/ble_gatt.h"
#include <stdint.h>

void ble_split_init(void);

static int svc_cb_fn(uint16_t conn_handle, uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt, void *arg);
