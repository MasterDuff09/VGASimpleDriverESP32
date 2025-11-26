#ifndef VGA_H
#define VGA_H

#include "struct_vga.h"

#endif

void VGAinit(VGA* vga, long freq, uint16_t WIDTH, uint16_t h_front_porch_length, uint16_t h_sync_pulse, uint16_t h_back_porch_length, uint16_t HEIGHT, uint16_t v_front_porch_length, uint16_t v_sync_pulse, uint16_t v_back_porch_length);
void VGAsetPins(VGA* vga, int pin_r, int pin_g, int pin_b, int pin_hsync, int pin_vsync);
void VGAsetSerial(VGA* vga, uint64_t baudrate, uint8_t data_bits, uint8_t parity, uint8_t stop_bits);
void VGAsetMode(bool terminal_mode);
void VGAsetScaling(bool set);
void VGAsetBGcolor(uint8_t c);
void VGAsetTEXTcolor(uint8_t c);
void VGA_Start();