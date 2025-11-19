#include "i2s_manager.h"
#include "vga_lldesc.h"
#include "vga_buffer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_private/periph_ctrl.h"
#include "esp_private/rtc_clk.h"
#include "soc/rtc.h"
#include "soc/clk_tree_defs.h"

#include "hal/hal_utils.h"
#include "soc/soc.h"
#include "hal/regi2c_ctrl.h"

#include "hal/i2s_types.h"
#include "soc/i2s_periph.h"
#include "soc/i2s_struct.h"
#include "soc/i2s_reg.h"

#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"

#include "esp_log.h"

#include "stdint.h"
#include <stdio.h>

static vga_buffer_manager_t buf_m = {0};
static vga_lldesc_manager_t lld_m = {0};


static inline bool is_in_vsync(uint16_t curr_y, uint16_t len_active, uint16_t len_front_porch, uint16_t len_v_sync_frames){
    uint16_t y1 = len_active + len_front_porch;
    uint16_t y2 = y1 + len_v_sync_frames;
    return (curr_y >= y1) && (curr_y < y2);
};

void IRAM_ATTR vga_i2s_tx_isr(void* arg){

    vga_i2s_manager_t* i2s_s = (vga_i2s_manager_t*) (arg);

    uint32_t st = i2s_s->i2s_c.dev->int_st.val;

    if(st & I2S_OUT_EOF_INT_ST_M){

        i2s_s->i2s_c.dev->int_clr.out_eof = 1;

        lldesc_t *eof_desc = (lldesc_t*) i2s_s->i2s_c.dev->out_eof_des_addr;

        i2s_s->last_eof_A = (eof_desc == &(lld_m.desc_activeA));

        uint8_t* last_buffer_used = (uint8_t*) (i2s_s->last_eof_A? lld_m.desc_activeA.buf : lld_m.desc_activeB.buf);

        buf_m.fill_next = last_buffer_used;

        i2s_s->current_y_line = ()

    }



}


void init_i2s_semaphore(vga_i2s_manager_t* i2s_s){

    i2s_s->new_line_ready = xSemaphoreCreateBinary();
    i2s_s->frame_display_is_done = xSemaphoreCreateBinary();
    configASSERT(i2s_s->new_line_ready);
    configASSERT(i2s_s->frame_display_is_done);

};

void init_vga_i2s_buffer(vga_i2s_manager_t* i2s_s, vga_dimensions_t* dim){

    vga_buffer_init(&buf_m, dim);
    set_addr_next_buf_to_fill(&buf_m, &(buf_m.lineA));

    lldesc_buf_config_t l_buf = {

        .h_front = buf_m.h_front,
        .h_hsync = buf_m.h_hsync,
        .h_back = buf_m.h_back,
        .v_back = buf_m.v_back,
        .v_front = buf_m.v_front,
        .v_hsync = buf_m.v_hsync,
        .lineA = buf_m.lineA,
        .lineB = buf_m.lineB,
        .len_active_frames = dim->len_active_frames,
        .len_back_porch = dim->len_back_porch,
        .len_front_porch = dim->len_front_porch,
        .len_h_sync_frames = dim->len_h_sync_frames

    };

    vga_lldesc_init(&lld_m, &l_buf);

};

void i2s_set_pins(vga_i2s_manager_t* i2s_s){
    int pins[5] = {i2s_s->pin_hsync, i2s_s->pin_vsync, i2s_s->pin_r, i2s_s->pin_g, i2s_s->pin_b};
    gpio_config_t io = {
        .pin_bit_mask = (1ULL<<pins[0]) | (1ULL<<pins[1]) | (1ULL<<pins[2]) |
                        (1ULL<<pins[3]) | (1ULL<<pins[4]),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 0, .pull_up_en = 0, .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);

    esp_rom_gpio_connect_out_signal(pins[0],     I2S1O_DATA_OUT0_IDX, false, false);
    //gpio_set_direction(PIN_HSYNC, GPIO_MODE_OUTPUT);
    //gpio_set_level(PIN_HSYNC, 1);
    esp_rom_gpio_connect_out_signal(pins[1],     I2S1O_DATA_OUT1_IDX, false, false);
    esp_rom_gpio_connect_out_signal(pins[2],     I2S1O_DATA_OUT2_IDX, false, false);
    esp_rom_gpio_connect_out_signal(pins[3],     I2S1O_DATA_OUT3_IDX, false, false);
    esp_rom_gpio_connect_out_signal(pins[4],     I2S1O_DATA_OUT4_IDX, false, false);
    
    for (int i=0;i<5;i++) {
        GPIO.func_out_sel_cfg[pins[i]].oen_sel = 1;
        GPIO.enable_w1ts = (1U << pins[i]);
    }
}

void start_i2s_hal (vga_i2s_manager_t* i2s_s){

    periph_module_enable(PERIPH_I2S1_MODULE);

    i2s_hal_init(&(i2s_s->i2s_c), 1);
    i2s_hal_tx_reset(&(i2s_s->i2s_c));
    i2s_hal_tx_reset_fifo(&(i2s_s->i2s_c));
    i2s_hal_tx_reset_dma(&(i2s_s->i2s_c));
}

void vga_i2s_set_clock_apll(vga_i2s_manager_t* i2s_s){

    rtc_clk_apll_enable(true);
    uint32_t div = 0, sdm0 = 0, sdm1 = 0, sdm2 = 0;
    long freq_apll = (i2s_s->clock_hz)*3;
    /*
    if ((freq_apll > 16000000) && (freq_apll < 128000000)){
        ESP_LOGE("APLL_CLK", "")
    }
    */
    int freq = rtc_clk_apll_coeff_calc(freq_apll, &div, &sdm0, &sdm1, &sdm2);
    
    rtc_clk_apll_coeff_set(div, sdm0, sdm1, sdm2);

    i2s_s->i2s_c.dev->clkm_conf.clka_en = 1;

    i2s_s->i2s_c.dev->clkm_conf.clkm_div_a = 1;
    i2s_s->i2s_c.dev->clkm_conf.clkm_div_b = 0;
    i2s_s->i2s_c.dev->clkm_conf.clkm_div_num = 3;
    i2s_s->i2s_c.dev->sample_rate_conf.tx_bck_div_num= 1;

    
    i2s_hal_tx_reset_fifo(&(i2s_s->i2s_c));

};

void vga_i2s_set_register(vga_i2s_manager_t* i2s_s){

    i2s_s->i2s_c.dev->conf2.val = 0;
    i2s_s->i2s_c.dev->conf2.lcd_en = 1;
    i2s_s->i2s_c.dev->conf2.lcd_tx_wrx2_en = 1;
    i2s_s->i2s_c.dev->conf2.lcd_tx_sdx2_en = 0;

    i2s_s->i2s_c.dev->sample_rate_conf.val = 0;
    i2s_s->i2s_c.dev->sample_rate_conf.tx_bits_mod=8;

    i2s_s->i2s_c.dev->fifo_conf.val = 0;
    i2s_s->i2s_c.dev->fifo_conf.tx_fifo_mod_force_en = 1;
    i2s_s->i2s_c.dev->fifo_conf.tx_fifo_mod = 1;
    i2s_s->i2s_c.dev->fifo_conf.dscr_en = 1;

    i2s_s->i2s_c.dev->conf1.val = 0;
    i2s_s->i2s_c.dev->conf1.tx_pcm_bypass = 1;

    i2s_s->i2s_c.dev->conf_chan.val = 0;
    i2s_s->i2s_c.dev->conf_chan.tx_chan_mod = 1;

    i2s_s->i2s_c.dev->conf.tx_msb_right = 0;
    i2s_s->i2s_c.dev->conf.tx_right_first = 1;
    i2s_s->i2s_c.dev->conf.tx_msb_shift = 0;
    i2s_s->i2s_c.dev->conf.tx_slave_mod = 0;
};

void vga_i2s_set_interrupt (vga_i2s_manager_t* i2s_s){

    i2s_s->i2s_c.dev->int_clr.val = 0xFFFFFFFF;
    i2s_s->i2s_c.dev->int_ena.val = 0;
    i2s_s->i2s_c.dev->int_ena.out_eof = 1;
    i2s_s->i2s_c.dev->int_ena.out_dscr_err = 1;
    i2s_s->i2s_c.dev->int_ena.out_total_eof = 1;
    i2s_s->i2s_c.dev->lc_conf.out_eof_mode = 1;

    ESP_ERROR_CHECK(esp_intr_alloc(ETS_I2S1_INTR_SOURCE, ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LEVEL3, vga_i2s_tx_isr, (void*)(i2s_s), &(i2s_s->i2s_isr_handle)));
}


