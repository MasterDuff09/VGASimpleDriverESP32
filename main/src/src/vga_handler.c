#include "vga_handler.h"

static inline bool is_visible(uint16_t y){
    return (y < V_ACTIVE_FRAMES);
}

static inline bool is_vsync(uint16_t y){
    return (y >= (V_ACTIVE_FRAMES + V_FRONT_PORCH_FRAMES)) && (y < (V_ACTIVE_FRAMES + V_FRONT_PORCH_FRAMES + V_SYNC_PULSE_FRAMES));
}


static uint16_t last_delay = 0;

void main_vga_task(void *arg){

    while (true){
        xSemaphoreTake(line_ready, portMAX_DELAY);

        //uint16_t next_line = (current_y_line + 1) % 525;
        uint16_t next_line_2 = (current_y_line + 1) % TOTAL_V_FRAMES;

        /*
        if (is_vsync(next_line_tx)) {
            desc_front.buf = v_front;
            desc_hsync.buf = v_hsync;
            desc_back.buf = v_back;
        } else {
            desc_front.buf = h_front;
            desc_hsync.buf = h_hsync;
            desc_back.buf = h_back;
        }
        */

        
        
        uint8_t *dest = (uint8_t*) fill_next;

        if (is_visible(next_line_2)){

            //render_char(next_line_fill, dst);

        } else {

            //memset(dest, is_vsync(next_line_2)? H_HIGH_V_LOW : H_HIGH_V_HIGH, H_ACTIVE_FRAMES);
            memcpy(dest, is_vsync(next_line_2)? black_lineL : black_lineH, H_ACTIVE_FRAMES);

        }
        
        if (++last_delay >= 512){
            last_delay = 0;
            vTaskDelay(0);
        }
        
    }
}

void vga_start(void){

    init_sem();
    start_buffer_i2s();
    i2s_start();
    
    xTaskCreatePinnedToCore(main_vga_task, "render", 4096, NULL, 10, NULL, 1);
}