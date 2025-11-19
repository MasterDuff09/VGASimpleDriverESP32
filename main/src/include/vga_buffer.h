#ifndef BUFFER_H
#define BUFFER_H

#include "struct_buffer.h"
#include "struct_dimensions.h"

#endif


void vga_buffer_init(vga_buffer_manager_t* buf_t, vga_dimensions_t* dim);
void* get_addr_next_buf_to_fill(vga_buffer_manager_t* buf_t);
void set_addr_next_buf_to_fill(vga_buffer_manager_t* buf_t, uint8_t* addr);
/*

void* get_addr_buf_h_front(vga_buffer_manager_t* buf_t);
void* get_addr_buf_h_back(vga_buffer_manager_t* buf_t);
void* get_addr_buf_h_hsync(vga_buffer_manager_t* buf_t);
void* get_addr_buf_v_front(vga_buffer_manager_t* buf_t);
void* get_addr_buf_v_back(vga_buffer_manager_t* buf_t);
void* get_addr_buf_v_hsync(vga_buffer_manager_t* buf_t);
void* get_addr_buf_line_a(vga_buffer_manager_t* buf_t);
void* get_addr_buf_line_b(vga_buffer_manager_t* buf_t);
*/