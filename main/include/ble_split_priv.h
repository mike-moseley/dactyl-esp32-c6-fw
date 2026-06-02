#pragma once

#include "host/ble_uuid.h"

extern const ble_uuid128_t s_svc_uuid;
extern const ble_uuid128_t s_chr_uuid;
extern void nimble_host_task(void *param);
void peripheral_init(void);
void central_init(void);
