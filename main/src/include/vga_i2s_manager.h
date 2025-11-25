#ifndef VGA_I2S_MANAGER_H
#define VGA_I2S_MANAGER_H


#include "struct_i2s.h"
#include "struct_dimensions.h"

#endif

void init_i2s_semaphore(vga_i2s_manager_t* i2s_s);
void init_vga_i2s_buffer(vga_i2s_manager_t* i2s_s, vga_dimensions_t* dim);
void i2s_set_pins(vga_i2s_manager_t* i2s_s);
void start_i2s_hal(vga_i2s_manager_t* i2s_s);
void vga_i2s_set_clock_apll(vga_i2s_manager_t* i2s_s);
void vga_i2s_set_register(vga_i2s_manager_t* i2s_s);
void vga_i2s_set_interrupt (vga_i2s_manager_t* i2s_s);
void vga_i2s_enable_dma(vga_i2s_manager_t* i2s_s);
void vga_i2s_start(vga_i2s_manager_t* i2s_s);
void* get_next_buf_to_fill();
void* get_black_line_hsync();
void* get_black_line_vsync();

