#ifndef STRUCT_DIMENSIONS
#define STRUCT_DIMENSIONS

#include "freertos/FreeRTOS.h"



typedef struct {

    uint16_t len_front_porch;
    uint16_t len_back_porch;
    uint16_t len_h_sync_frames;
    uint16_t len_active_frames;
    uint16_t len_v_front_porch;
    uint16_t len_v_back_porch;
    uint16_t len_v_sync_frames;
    uint16_t len_v_active_frames;

}   vga_dimensions_t;

#endif