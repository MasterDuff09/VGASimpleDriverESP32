#ifndef     STRUCT_I2S
#define     STRUCT_I2S

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "hal/i2s_hal.h"


typedef struct {

    i2s_hal_context_t i2s_c;
    SemaphoreHandle_t new_line_ready;
    SemaphoreHandle_t frame_display_is_done;
    intr_handle_t i2s_isr_handle;
    volatile bool last_eof_A;
    volatile uint16_t current_y_line;
    uint8_t pin_r;
    uint8_t pin_g;
    uint8_t pin_b;
    uint8_t pin_hsync;
    uint8_t pin_vsync;
    int clock_hz; 

} vga_i2s_manager_t ;

#endif