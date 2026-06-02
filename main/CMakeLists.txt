idf_component_register(
    SRCS
        "src/main.c"
        "src/matrix.c"
        "src/main_central.c"
        "src/main_peripheral.c"
        "src/ble_split.c"
    INCLUDE_DIRS "include"
    REQUIRES
        bt
        nvs_flash
        esp_driver_gpio
)
