#include "VGA.h"
#include "vga_video_manager.h"


//#include "struct_vga_private.h"
//#include "struct_i2s.h"
//#include "struct_dimensions.h"
//#include "struct_uart.h"

#include "esp_log.h"


static DRAM_ATTR vga_i2s_manager_t i2s = {0};
static DRAM_ATTR vga_uart_manager_t uart = {0};
static DRAM_ATTR vga_dimensions_t dim = {0};
static DRAM_ATTR VGA_private priv = {0};
static DRAM_ATTR VGA_s vga = {0};

///////////////////////////////////////////////////////////////////////////////


void VGAinit(VGA* vga, long freq, uint16_t WIDTH, uint16_t h_front_porch_length, uint16_t h_sync_pulse, uint16_t h_back_porch_length, uint16_t HEIGHT, uint16_t v_front_porch_length, uint16_t v_sync_pulse, uint16_t v_back_porch_length){
    
    priv.vga = vga;

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

    priv.vga->text_color = 0b00011111;
    priv.last_text_color = priv.vga->text_color;
    priv.vga->bg_color = 0x3;
    priv.last_bg_color = priv.vga->bg_color;

    i2s.total_v_frames = HEIGHT + v_front_porch_length + v_back_porch_length + v_sync_pulse;
    i2s.start_v_sync = HEIGHT + v_front_porch_length;
    i2s.end_v_sync = i2s.start_v_sync + v_sync_pulse;

    vga->scaling = false;

    priv.init = true;

};

void VGAsetPins(VGA* vga, int pin_r, int pin_g, int pin_b, int pin_hsync, int pin_vsync){

    priv.vga = vga;
    
    i2s.pin_r = pin_r;
    i2s.pin_g = pin_g;
    i2s.pin_b = pin_b;
    i2s.pin_hsync = pin_hsync;
    i2s.pin_vsync = pin_vsync;

    priv.pins = true;

};


void VGAsetSerial(VGA* vga, uint64_t baudrate, uint8_t data_bits, uint8_t parity, uint8_t stop_bits){
    
    priv.vga = vga;

    uart.baudrate = baudrate;
    uart.data_bits = data_bits;
    uart.parity = parity;
    uart.stop_bits = stop_bits;
    uart.screen_line_buffer_size = uart.screen_line_buffer_size? (dim.len_active_frames/(priv.real_char_w)) : uart.screen_line_buffer_size;
    uart.terminal_mode = false;

    priv.serial = true;

};

void VGAsetMode(bool terminal_mode){

    if (!priv.start){

        uart.terminal_mode = terminal_mode;
        int mod = terminal_mode? 1 : 0;
        uart.screen_line_buffer_size = (dim.len_active_frames/(priv.real_char_w)) - (7 * mod);
    
    }

};

void VGAsetScaling(bool set){

    if (!priv.vga) return;
    if (!priv.start) priv.vga->scaling = set;

};

void VGAsetBGcolor(uint8_t c){

    if (!priv.vga) return;
    priv.vga->bg_color = ((c % 8) << 2) | 0x3;
    priv.last_bg_color = priv.vga->bg_color;

};

void VGAsetTEXTcolor(uint8_t c){

    if (!priv.vga) return;
    priv.vga->text_color = ((c % 8) << 2) | 0x3;
    priv.last_text_color = priv.vga->text_color;

};

void VGA_Start(){


    if (!priv.init) ESP_LOGE("VGA", "Parameters not initialized");
    if (!priv.pins) ESP_LOGE("VGA", "Pins not initialized");
    if (!priv.serial) ESP_LOGE("VGA", "Serial not configured");
    priv.start = true;
    ESP_LOGI("VGA", "START");


    vga.dim_m = &dim;
    vga.i2s_m = &i2s;
    vga.uart_m = &uart;
    vga.vga_m = &priv;

    VGAllstart(&vga);

};









