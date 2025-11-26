#ifndef VGA_VIDEO_MANAGER
#define VGA_VIDEO_MANAGER

#include "struct_vga_private.h"
#include "struct_i2s.h"
#include "struct_dimensions.h"
#include "struct_uart.h"

typedef struct{

    vga_i2s_manager_t* i2s_m;
    vga_uart_manager_t* uart_m;
    vga_dimensions_t* dim_m;
    VGA_private* vga_m;

} VGA_s;

#endif

void VGAllstart(VGA_s* v);
