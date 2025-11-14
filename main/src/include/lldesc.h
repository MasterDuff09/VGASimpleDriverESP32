#ifndef LLDESC_H
#define LLDESC_H


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/lldesc.h"
#include "soc/lldesc.h"

#include "constants.h"

typedef struct {

    DMA_ATTR lldesc_t desc_frontA;
    DMA_ATTR lldesc_t desc_hsyncA;
    DMA_ATTR lldesc_t desc_backA;
    DMA_ATTR lldesc_t desc_activeA;
    DMA_ATTR lldesc_t desc_frontB;
    DMA_ATTR lldesc_t desc_hsyncB;
    DMA_ATTR lldesc_t desc_backB; 
    DMA_ATTR lldesc_t desc_activeB;

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