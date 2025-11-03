#ifndef LLDESC_H
#define LLDESC_H


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/lldesc.h"
#include "soc/lldesc.h"

#include "constants.h"

typedef struct {

    lldesc_t desc_frontA, desc_hsyncA, desc_backA, desc_activeA;
    lldesc_t desc_frontB, desc_hsyncB, desc_backB, desc_activeB;

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

}  lldesc_buf_config_t;


void vga_lldesc_init(vga_lldesc_manager_t* d, lldesc_buf_config_t* b);


#endif