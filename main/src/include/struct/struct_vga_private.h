#ifndef STRUCT_VGA_PRIVATE
#define STRUCT_VGA_PRIVATE

#include "freertos/FreeRTOS.h"
#include "struct_vga.h"

typedef struct{

    VGA *vga;
    bool init;
    bool pins;
    bool serial;
    bool start;
    uint8_t*** char_lut;
    uint8_t**  screen;
    uint8_t real_char_w;
    uint8_t real_char_h;
    uint8_t char_blocks_y;
    uint8_t char_blocks_x;
    uint16_t write_on_y;
    uint8_t last_bg_color;
    uint8_t last_text_color;
    uint8_t index_update_color_bg_char;
    uint8_t index_update_color_txt_char;

} VGA_private;


#endif