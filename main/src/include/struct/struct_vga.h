#ifndef STRUCT_VGA
#define STrUCT_VGA

#include "freertos/FreeRTOS.h"

typedef struct {

    bool scaling;
    uint8_t bg_color;
    uint8_t text_color;
    bool init;
    bool pins;
    bool serial;
    

} VGA;

#endif