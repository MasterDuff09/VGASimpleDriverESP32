#include "VGA.h"
#include "vga_i2s_manager.h"
#include "vga_uart_manager.h"
#include "font_basic_ascii_8x8.h"

#include "esp_log.h"


static vga_i2s_manager_t i2s = {0};
static vga_uart_manager_t uart = {0};
static vga_dimensions_t dim = {0};

static inline bool is_in_vsync(uint16_t curr_y){
    uint16_t y1 = i2s.len_v_active + i2s.len_v_front;
    uint16_t y2 = y1 + i2s.len_v_sync;
    return (curr_y >= y1) && (curr_y < y2);
};

static inline bool is_visible(uint16_t curr_y){
    return (curr_y < i2s.len_v_active);
}

void VGAinit(VGA* vga, long freq, uint16_t WIDTH, uint16_t h_front_porch_length, uint16_t h_sync_pulse, uint16_t h_back_porch_length, uint16_t HEIGHT, uint16_t v_front_porch_length, uint16_t v_sync_pulse, uint16_t v_back_porch_length){
    
    i2s.clock_hz = freq;
    i2s.len_v_active = HEIGHT;
    i2s.len_v_front = v_front_porch_length;
    i2s.len_v_back = v_back_porch_length;
    i2s.len_v_sync = v_sync_pulse;

    dim.len_active_frames = WIDTH;
    dim.len_front_porch = h_front_porch_length;
    dim.len_back_porch = h_back_porch_length;
    dim.len_h_sync_frames = h_sync_pulse;

    dim.len_v_active_frames = HEIGHT;
    dim.len_v_front_porch = v_front_porch_length;
    dim.len_v_back_porch = v_back_porch_length;
    dim.len_v_sync_frames = v_sync_pulse;
    vga->init = true;

}

void VGAsetPins(VGA* vga, int pin_r, int pin_g, int pin_b, int pin_hsync, int pin_vsync){

    i2s.pin_r = pin_r;
    i2s.pin_g = pin_g;
    i2s.pin_b = pin_b;
    i2s.pin_hsync = pin_hsync;
    i2s.pin_vsync = pin_vsync;
    vga->pins = true;

}


void VGAsetSerial(VGA* vga, uint64_t baudrate, uint8_t data_bits, uint8_t parity, uint8_t stop_bits){
    
    uart.baudrate = baudrate;
    uart.data_bits = data_bits;
    uart.parity = parity;
    uart.stop_bits = stop_bits;
    vga->serial = true;

}

void VGAsetMode(bool terminal_mode){

    uart.terminal_mode = terminal_mode;

}

void VGAsetScaling(VGA* vga, bool set){

    vga->scaling = set;

}

void VGAsetBGcolor(VGA* vga, uint8_t c){

    vga->bg_color = c;

}

void VGAsetTEXTcolor(VGA* vga, uint8_t c){

    vga->text_color = c;

}



void VGA_Start(VGA* vga){

    if (!vga->init) ESP_LOGE("VGA", "Parameters not initialized");
    if (!vga->pins) ESP_LOGE("VGA", "Pins not initialized");
    if (!vga->serial) ESP_LOGE("VGA", "Serial not configured");

    uart_init_sem(&uart);
    init_i2s_semaphore(&i2s);

    init_vga_i2s_buffer(&i2s, &dim);
    uart_start_task(&uart);

    //
    //

    i2s_set_pins(&i2s);
    start_i2s_hal(&i2s);
    vga_i2s_set_clock_apll(&i2s);
    vga_i2s_set_register(&i2s);
    vga_i2s_set_interrupt(&i2s);
    vga_i2s_enable_dma(&i2s);
    vga_i2s_start(&i2s);
    vTaskDelete(NULL);

}





