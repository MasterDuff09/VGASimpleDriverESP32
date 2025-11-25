#ifndef STRUCT_VGA
#define STRUCT_VGA

#include "freertos/FreeRTOS.h"

typedef struct {

    bool scaling;
    uint8_t bg_color;
    uint8_t text_color;

} VGA;

#endif