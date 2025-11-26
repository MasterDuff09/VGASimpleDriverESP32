#include "struct_vga_private.h"

#include "vga_video_manager.h"
#include "vga_i2s_manager.h"
#include "vga_uart_manager.h"

#include "font_basic_ascii_8x8.h"


static vga_i2s_manager_t* i2s;
static vga_uart_manager_t* uart;
static VGA_private* priv;
static vga_dimensions_t* dim;

static WORD_ALIGNED_ATTR uint8_t char_lut[TOTAL_CH][CHAR_H][CHAR_W];
static bool wrote_on_screen = true;
static char* msg_ptr;


///////////////////////////////////////////////////////////////////////////////


static inline IRAM_ATTR bool is_in_vsync(uint16_t curr_y){
    uint16_t y1 = i2s->len_v_active + i2s->len_v_front;
    uint16_t y2 = y1 + i2s->len_v_sync;
    return (curr_y >= y1) && (curr_y < y2);
};

static inline bool is_visible(uint16_t curr_y){
    return (curr_y < i2s->len_v_active);
};

///////////////////////////////////////////////////////////////////////////////

static void build_lut(void){

    //priv->char_lut = heap_caps_malloc(TOTAL_CH * sizeof(uint8_t**), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    //if (vga_p->char_lut == NULL) ESP_LOGE("Lut_Table", "mem not allocated");
    for (int ci = 0; ci < TOTAL_CH; ci++){
        //priv->char_lut[ci] = heap_caps_malloc(CHAR_H * sizeof(uint8_t*), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        for(int cy = 0; cy < 8; cy++){
            //priv->char_lut[ci][cy] = heap_caps_malloc(CHAR_W, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            for(int cx = 0; cx < 8; cx++){
                char_lut[ci][cy][cx] = ((((font[ci] >> (8 * (7 - cy))) & 0xFF) >> (7 - cx)) & 0x1) ? priv->vga->text_color : priv->vga->bg_color;//Font8x8Pixels[((ci*8) + cy)*8 + cx] | H_HIGH_V_HIGH;
            }
        }
    }
}

static void build_screen(void){

    priv->screen = heap_caps_malloc(priv->char_blocks_y * sizeof(uint8_t*), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    for(int i = 0; i < priv->char_blocks_y; i++) priv->screen[i] = heap_caps_malloc(priv->char_blocks_x, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
}

static void clear_screen(void){

    for (int i = 0; i < priv->char_blocks_y; i++) memset(priv->screen[i], ' ', priv->char_blocks_x);
}

static void clear_screen_until_y(uint16_t y){

    for (int i = 0; i <= y; i++) memset(priv->screen[i], ' ', priv->char_blocks_x);
}

IRAM_ATTR static void getMessage(void){


    int mod = uart->terminal_mode? 7 : 0;
    char* m = uart->message + mod;
    char* invalid_set = "Update pending or colorbg = colortxt\0";
    bool color_error_flag = false;

    const char *clear_cmd_r = "clear\r";
    const char *clear_cmd_n = "clear\n";
    const char *clear_cmd_0 = "clear\0";
    //int len_clear_cmd = 5;

    if (!strcmp(m, clear_cmd_r) || !strcmp(m, clear_cmd_n) || !strcmp(m, clear_cmd_0)){

        //clear_screen_until_y(priv->write_on_y);
        clear_screen();
        priv->write_on_y = 0;
        msg_ptr = "";
        wrote_on_screen = false;
        return;

    }

    wrote_on_screen = true;

    const char *prefixbg = "setcolorbg=";
    int len_p_bg = 11;

    const char *prefixtxt= "setcolortext=";
    int len_p_txt = 13;

    const char *prefixterm= "setmode=";
    int len_p_term = 8;

    int val;

    if (!strncmp(m, prefixbg, len_p_bg)){

        char *start_val = m + len_p_bg;
        char *end_val;

        val = (int)strtol(start_val, &end_val, 10);
        if (start_val != end_val && (*end_val == '\0' || *end_val == '\r' || *end_val == '\n')) {
            
            uint8_t color = (((uint8_t)(val % 8)) << 2) | 0x3;
            if (!priv->index_update_color_bg_char && color != priv->vga->text_color)  priv->vga->bg_color = color;
            else color_error_flag = true;
        }
        
    } else if (!strncmp(m, prefixtxt, len_p_txt)){

        char *start_val = m + len_p_txt;
        char *end_val;

        val = (int)strtol(start_val, &end_val, 10);
        if (start_val != end_val && (*end_val == '\0' || *end_val == '\r' || *end_val == '\n')) {

            uint8_t color = (((uint8_t)(val % 8)) << 2) | 0x3;
            if (!priv->index_update_color_txt_char && color != priv->vga->bg_color) priv->vga->text_color = color;
            else color_error_flag = true;
        }
    } else if (!strncmp(m, prefixterm, len_p_term)){

        char *start_val = m + len_p_term;
        char *end_val;

        val = (int)strtol(start_val, &end_val, 10);
        if (start_val != end_val && (*end_val == '\0' || *end_val == '\r' || *end_val == '\n')) {
            uart->terminal_mode = (val > 0)? true : false;
            int mod = uart->terminal_mode? 1 : 0;
            uart->screen_line_buffer_size = (dim->len_active_frames/(priv->real_char_w)) - (7 * mod);
        }
    
    }
    msg_ptr = (color_error_flag)? invalid_set : uart->message;
}

IRAM_ATTR static void update(void){

    getMessage();

    uint8_t *row_ptr = priv->screen[priv->write_on_y];
    memset(row_ptr, ' ', priv->char_blocks_x);

    int i = 0;
    while (msg_ptr[i] != '\0'){
        priv->screen[priv->write_on_y][i] = msg_ptr[i];
        i++;
    }
    if (wrote_on_screen) priv->write_on_y = (priv->write_on_y + 1) % (priv->char_blocks_y);

}

IRAM_ATTR static void render_line(uint16_t y, uint8_t* dest){
    uint16_t row_y = y / priv->real_char_h;
    uint16_t y_pixel_char;

    if (priv->vga->scaling) y_pixel_char = (y % priv->real_char_h) / 2;
    else y_pixel_char = y % priv->real_char_h;

    uint8_t* screen_row = priv->screen[row_y];
    uint8_t* current_dest = dest;

    if(priv->vga->scaling){

        for (uint8_t row_x = 0; row_x < priv->char_blocks_x; row_x++){

            uint8_t ch = screen_row[row_x];

            if (ch < FIRST_CH || ch >= FIRST_CH + TOTAL_CH) ch = ' ';
            
            uint8_t* font_row = char_lut[ch - FIRST_CH][y_pixel_char];

            current_dest[0]  = font_row[1]; 
            current_dest[1]  = font_row[1];
            current_dest[2]  = font_row[0]; 
            current_dest[3]  = font_row[0];
            current_dest[4]  = font_row[3]; 
            current_dest[5]  = font_row[3];
            current_dest[6]  = font_row[2]; 
            current_dest[7]  = font_row[2];
            current_dest[8]  = font_row[5]; 
            current_dest[9]  = font_row[5];
            current_dest[10] = font_row[4]; 
            current_dest[11] = font_row[4];
            current_dest[12] = font_row[7]; 
            current_dest[13] = font_row[7];
            current_dest[14] = font_row[6]; 
            current_dest[15] = font_row[6];

            current_dest += priv->real_char_w;
        }
    } else {

        for (uint8_t row_x = 0; row_x < priv->char_blocks_x; row_x++){

            uint8_t ch = screen_row[row_x];

            if (ch < FIRST_CH || ch >= FIRST_CH + TOTAL_CH) ch = ' ';
            
            uint8_t* font_row = char_lut[ch - FIRST_CH][y_pixel_char];


            current_dest[0] = font_row[2];
            current_dest[1] = font_row[3];
            current_dest[2] = font_row[0];
            current_dest[3] = font_row[1];
            current_dest[4] = font_row[6];
            current_dest[5] = font_row[7];
            current_dest[6] = font_row[4];
            current_dest[7] = font_row[5];

            current_dest += priv->real_char_w;
        }
    }

    

}

static void update_lut(void){

    if (priv->vga->bg_color != priv->last_bg_color){
            for (int i = 0; i < CHAR_H; i++){
                for (int j = 0; j < CHAR_W; j++){
                    //priv->char_lut[priv->index_update_color_char][i][j] = ((priv->char_lut[priv->index_update_color_char][i][0]) & 0x3) | priv->vga->text_color;
                    //uint8_t bg_pixel = (priv->char_lut[priv->index_update_color_bg_char][i][j] == priv->last_bg_color);
                    uint8_t state_pixel = char_lut[priv->index_update_color_bg_char][i][j];
                    char_lut[priv->index_update_color_bg_char][i][j] = (state_pixel == priv->last_bg_color)? priv->vga->bg_color : state_pixel;
                }
            }
            priv->index_update_color_bg_char = (priv->index_update_color_bg_char + 1) % TOTAL_CH;
            if (!priv->index_update_color_bg_char) priv->last_bg_color = priv->vga->bg_color;
        }

        if (priv->vga->text_color != priv->last_text_color){
            for (int i = 0; i < CHAR_H; i++){
                for (int j = 0; j < CHAR_W; j++){
                    //priv->char_lut[priv->index_update_color_char][i][j] = ((priv->char_lut[priv->index_update_color_char][i][0]) & 0x3) | priv->vga->text_color;
                    //uint8_t bg_pixel = (priv->char_lut[priv->index_update_color_bg_char][i][j] == priv->last_bg_color);
                    uint8_t state_pixel = char_lut[priv->index_update_color_txt_char][i][j];
                    char_lut[priv->index_update_color_txt_char][i][j] = (state_pixel == priv->last_text_color)? priv->vga->text_color : state_pixel;
                }
            }
            priv->index_update_color_txt_char = (priv->index_update_color_txt_char + 1) % TOTAL_CH;
            if (!priv->index_update_color_txt_char) priv->last_text_color = priv->vga->text_color;
        }
    

}
static void main_vga_task(void *arg){

    priv->real_char_w = priv->vga->scaling? CHAR_W * 2 : CHAR_W;
    priv->real_char_h = priv->vga->scaling? CHAR_H * 2 : CHAR_H;
    priv->char_blocks_y = dim->len_v_active_frames / (priv->real_char_h);
    priv->char_blocks_x = dim->len_active_frames / (priv->real_char_w);

    if (uart->screen_line_buffer_size == 0) {
        uart->screen_line_buffer_size = dim->len_active_frames / priv->real_char_w;
    }

    uint16_t total_v_frames = dim->len_v_active_frames + dim->len_v_back_porch + dim->len_v_front_porch + dim->len_v_sync_frames;

    bool update_pending = false;

    build_lut();
    build_screen();
    clear_screen();

    while(true){

        if (xSemaphoreTake(uart->message_ready, 0) == pdTRUE){
                update();
                update_pending = true;
        }

        if (update_pending){

            if(xSemaphoreTake(i2s->frame_display_is_done, 0) == pdTRUE){

                xSemaphoreGive(uart->uart_buffer_available);
                update_pending = false;

            }
        }

        xSemaphoreTake(i2s->new_line_ready, portMAX_DELAY);

        update_lut();
        
        uint16_t y_2 = (i2s->current_y_line + 1);
        if (y_2 >= total_v_frames) y_2 = 0;
        uint8_t *dest = (uint8_t*) get_next_buf_to_fill();


        if(is_visible(y_2)){

            render_line(y_2, dest);// render_line;

        } else {
            memcpy(dest, is_in_vsync(y_2)? (uint8_t*)(get_black_line_vsync()) : (uint8_t*)(get_black_line_hsync()), dim->len_active_frames);
        }
        
    }

}


void VGAllstart(VGA_s* v){

    i2s = v->i2s_m;
    uart = v->uart_m;
    priv = v->vga_m;
    dim = v->dim_m;

    uart_init_sem(uart);
    init_i2s_semaphore(i2s);

    init_vga_i2s_buffer(i2s, dim);
    uart_start_task(uart);

    vTaskDelay(10/portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(main_vga_task, "render", 8192, NULL, 10, NULL, 1);

    i2s_set_pins(i2s);
    start_i2s_hal(i2s);
    //vga_i2s_set_clock_apll(&i2s);
    vga_i2s_set_register(i2s);
    vga_i2s_set_interrupt(i2s);
    vga_i2s_enable_dma(i2s);

    vga_i2s_start(i2s);


}