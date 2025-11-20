#ifndef STRUCT_LLDESC
#define STRUCT_LLDESC


#include "freertos/FreeRTOS.h"
#include "rom/lldesc.h"
#include "soc/lldesc.h"

typedef struct {

    lldesc_t desc_frontA;
    lldesc_t desc_hsyncA;
    lldesc_t desc_backA;
    lldesc_t desc_activeA;
    lldesc_t desc_frontB;
    lldesc_t desc_hsyncB;
    lldesc_t desc_backB; 
    lldesc_t desc_activeB;

} vga_lldesc_manager_t;

typedef struct {

    uint8_t* h_front;
    uint8_t* h_hsync;
    uint8_t* h_back;
    uint8_t* v_front;
    uint8_t* v_hsync;
    uint8_t* v_back;
    uint8_t* lineA;
    uint8_t* lineB;
    uint16_t len_front_porch;
    uint16_t len_back_porch;
    uint16_t len_h_sync_frames;
    uint16_t len_active_frames;

}  lldesc_buf_config_t;

#endif
