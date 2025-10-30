#include "vga_handler.h"

static uint16_t last_delay = 0;
uint16_t write_on_y = 0;

static uint8_t ch_lut[TOTAL_CH][CHAR_H][CHAR_W] __attribute__((aligned(4)));
char screen[V_ACTIVE_FRAMES / 8][H_ACTIVE_FRAMES / 8];

static inline bool is_visible(uint16_t y){
    return (y < V_ACTIVE_FRAMES);
}

static inline bool is_vsync(uint16_t y){
    return (y >= (V_ACTIVE_FRAMES + V_FRONT_PORCH_FRAMES)) && (y < (V_ACTIVE_FRAMES + V_FRONT_PORCH_FRAMES + V_SYNC_PULSE_FRAMES));
}

static void build_lut(void){
    for (int ci = 0; ci < TOTAL_CH; ++ci){
        for(int cy = 0; cy < CHAR_H; ++cy){
            for(int cx = 0; cx < CHAR_W; ++cx){
                ch_lut[ci][cy][cx] = Font8x8Pixels[(ci*CHAR_H + cy)*CHAR_W + cx];
            }
        }
    }
}

static void clear_screen(void){
    for (uint8_t i = 0; i < V_ACTIVE_FRAMES / 8; i++){
        for (uint8_t j = 0; j < H_ACTIVE_FRAMES / 8; j++){
            screen[i][j] = ' ';
        }
    }
}

static void update_screen(void){
    uint8_t i = 0;
    for (; i < H_ACTIVE_FRAMES / 8; i++){
        screen[write_on_y][i] = ' ';
    }
    i = 0;
    while (msg[i] != '\0'){
        if (msg[i] == '\r') break; 
        screen[write_on_y][i] = msg[i];
        i++;
    }
    write_on_y = write_on_y + 1 % (V_ACTIVE_FRAMES / 8);
    
}

static void render_line(uint16_t y, uint8_t* dest){
    uint16_t row_y = y / CHAR_H;
    uint16_t line_pixel_y = y % CHAR_H;

    uint16_t line_pixel_x = 0;

    for (uint8_t row_x = 0; row_x < V_ACTIVE_FRAMES / 8; row_x++){

        uint8_t ch = screen[row_y][row_x];
        if (ch < FIRST_CH || ch >= FIRST_CH + TOTAL_CH){
            ch = ' ';
        }

        for (uint8_t pix_char_x = 0; pix_char_x < 8; pix_char_x++){
            dest[line_pixel_x] = (ch_lut[ch - FIRST_CH][line_pixel_y][pix_char_x]) | H_HIGH_V_HIGH;
            line_pixel_x++;
        }
    }
}


void main_vga_task(void *arg){

    build_lut();
    clear_screen();


    while (true){

        if (xSemaphoreTake(msg_ready, 0) == pdTRUE){
            update_screen();
            xSemaphoreTake(display_done, portMAX_DELAY);
            xSemaphoreGive(uart_send_avail);
        }
        xSemaphoreTake(line_ready, portMAX_DELAY);


        //uint16_t next_line = (current_y_line + 1) % 525;
        uint16_t next_line_2 = (current_y_line + 1) % TOTAL_V_FRAMES;

        
        
        uint8_t *dest = (uint8_t*) fill_next;

        if (is_visible(next_line_2)){ //&& len > 0){

            //render_char(next_line_fill, dst);
            render_line(next_line_2, dest);


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
    uart_start();
    i2s_start();
    
    xTaskCreatePinnedToCore(main_vga_task, "render", 4096, NULL, 10, NULL, 1);
}