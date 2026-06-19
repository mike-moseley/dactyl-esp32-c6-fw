idf_component_register(
    SRCS
        "src/main.c"
        "src/matrix.c"
        "src/main_central.c"
        "src/main_peripheral.c"
        "src/ble_split.c"
        "src/ble_peripheral.c"
        "src/ble_central.c"
        "src/ble_hid.c"
        "src/keymap.c"
    INCLUDE_DIRS "include"
    REQUIRES
        bt
        nvs_flash
        esp_driver_gpio
)

if(DEFINED CENTRAL_HALF)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC CENTRAL_HALF=${CENTRAL_HALF})
endif()
