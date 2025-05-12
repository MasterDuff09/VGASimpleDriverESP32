file(GLOB SRC_FILES "../src/src/*.c")

idf_component_register(SRCS "main.c" ${SRC_FILES}
                       PRIV_REQUIRES spi_flash
                       REQUIRES driver
                       INCLUDE_DIRS "../src/include")
