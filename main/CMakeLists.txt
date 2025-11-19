file(GLOB SRC_FILES "./src/src/*.c")

idf_component_register(
    SRCS "main.c" ${SRC_FILES}
    INCLUDE_DIRS  "." "./src/include" "./src/include/struct"
    PRIV_REQUIRES driver esp_timer esp_hw_support esp_driver_i2s esp_driver_gpio freertos esp_driver_uart)