

#file(GLOB SRC_FILES "../src/src/*.c")

#idf_component_register(SRCS "main.c" ${SRC_FILES}
#                       PRIV_REQUIRES spi_flash esp_driver_i2s esp_driver_gpio
#                       REQUIRES driver
#                       INCLUDE_DIRS "." "../src/include")


#"resolution.c" "pattern_handler.c"


idf_component_register(
    SRCS     "main.c" "./src/src"
    INCLUDE_DIRS  "./src/include"
    PRIV_REQUIRES esp_driver_i2s esp_driver_gpio
)
