#ifndef STRUCT_BUFFER
#define STRUCT_BUFFER

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


typedef struct {

    uint8_t* h_front;
    uint8_t* h_hsync;
    uint8_t* h_back;
    uint8_t* v_front;
    uint8_t* v_hsync;
    uint8_t* v_back;
    uint8_t* lineA;
    uint8_t* lineB;
    uint8_t* black_line_hsync;
    uint8_t* black_line_vsync;
    volatile uint8_t* volatile fill_next;

}  vga_buffer_manager_t;


#endif