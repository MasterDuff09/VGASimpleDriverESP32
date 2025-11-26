#include "i2s_handler.h"


i2s_hal_context_t i2s_c;

SemaphoreHandle_t line_ready;
SemaphoreHandle_t display_done;

intr_handle_t i2s_isr_handle;

volatile bool last_eof_A = false;

volatile uint16_t current_y_line = 0;

static inline bool is_vsync(uint16_t y){
    return (y >= (V_ACTIVE_FRAMES + V_FRONT_PORCH_FRAMES)) && (y < (V_ACTIVE_FRAMES + V_FRONT_PORCH_FRAMES + V_SYNC_PULSE_FRAMES));
}


/******   I2S HANDLER TO BUFFER   *******/

void IRAM_ATTR i2s_tx_isr(void *arg){
    
    uint32_t st = i2s_c.dev->int_st.val;

    if (st & I2S_OUT_EOF_INT_ST_M){

        i2s_c.dev->int_clr.out_eof = 1;
        

        lldesc_t *eof_desc = (lldesc_t*) i2s_c.dev->out_eof_des_addr;
        last_eof_A = (eof_desc == &desc_activeA);

        uint8_t *just_used = (uint8_t*) (last_eof_A? desc_activeA.buf : desc_activeB.buf);

        //tx_next = other;
        fill_next = just_used;
        //fill_next = (uint8_t*) (last_eof_A? desc_activeB.buf : desc_activeA.buf);

        current_y_line = (current_y_line + 1) % TOTAL_V_FRAMES;

        if (last_eof_A){
            
            if (is_vsync((current_y_line + 1) % TOTAL_V_FRAMES)){

                desc_frontA.buf = v_front;
                desc_hsyncA.buf = v_hsync;
                desc_backA.buf  = v_back;

            }   else    {

                desc_frontA.buf = h_front;
                desc_hsyncA.buf = h_hsync;
                desc_backA.buf  = h_back;

            }

        }   else    {

            if (is_vsync((current_y_line + 1) % TOTAL_V_FRAMES)){

                desc_frontB.buf = v_front;
                desc_hsyncB.buf = v_hsync;
                desc_backB.buf  = v_back;

            }   else    {

                desc_frontB.buf = h_front;
                desc_hsyncB.buf = h_hsync;
                desc_backB.buf  = h_back;

            }

        }

        __asm__ __volatile__ ("" ::: "memory");

        if (current_y_line == 0){
            BaseType_t hpw_frame = pdFALSE;
            xSemaphoreGiveFromISR(display_done, &hpw_frame);
            if (hpw_frame) portYIELD_FROM_ISR();
        }

        BaseType_t hpw = pdFALSE;
        xSemaphoreGiveFromISR(line_ready, &hpw);
        if (hpw) portYIELD_FROM_ISR();

    }  
    if (st & I2S_OUT_DSCR_ERR_INT_ST_M) {

        //isr_flags |= 0x1;
        i2s_c.dev->int_clr.out_dscr_err = 1;

    }

    if (st & I2S_OUT_TOTAL_EOF_INT_ST_M) {

            //isr_flags |= 0x4;
            i2s_c.dev->int_clr.out_total_eof = 1;

    }
    
}



void i2s_enable_interrupts(void){

    i2s_c.dev->int_clr.val = 0xFFFFFFFF;
    i2s_c.dev->int_ena.val = 0;
    i2s_c.dev->int_ena.out_eof = 1;
    i2s_c.dev->int_ena.out_dscr_err = 1;
    i2s_c.dev->int_ena.out_total_eof = 1;
    i2s_c.dev->lc_conf.out_eof_mode = 1;

    ESP_ERROR_CHECK(esp_intr_alloc(ETS_I2S1_INTR_SOURCE, ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LEVEL3, i2s_tx_isr, NULL, &i2s_isr_handle));

}



void init_sem(void){

    line_ready = xSemaphoreCreateBinary();//xSemaphoreCreateCounting(4, 0);
    display_done = xSemaphoreCreateBinary();
    configASSERT(line_ready);
    configASSERT(display_done);

    uart_send_avail = xSemaphoreCreateBinary();
    msg_ready = xSemaphoreCreateBinary();

    configASSERT(uart_send_avail);
    configASSERT(msg_ready);
}


/******   I2S HANDLER TO VGA   *******/
static void map_data_pins(void){
    int pins[5] = {PIN_HSYNC, PIN_VSYNC, PIN_R, PIN_G, PIN_B};

    
    gpio_config_t io = {
        .pin_bit_mask = (1ULL<<pins[0]) | (1ULL<<pins[1]) | (1ULL<<pins[2]) |
                        (1ULL<<pins[3]) | (1ULL<<pins[4]),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 0, .pull_up_en = 0, .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);

    
    
    esp_rom_gpio_connect_out_signal(PIN_HSYNC, I2S1O_DATA_OUT0_IDX, false, false);
    //gpio_set_direction(PIN_HSYNC, GPIO_MODE_OUTPUT);
    //gpio_set_level(PIN_HSYNC, 1);
    esp_rom_gpio_connect_out_signal(PIN_VSYNC, I2S1O_DATA_OUT1_IDX, false, false);
    esp_rom_gpio_connect_out_signal(PIN_R,     I2S1O_DATA_OUT2_IDX, false, false);
    esp_rom_gpio_connect_out_signal(PIN_G,     I2S1O_DATA_OUT3_IDX, false, false);
    esp_rom_gpio_connect_out_signal(PIN_B,     I2S1O_DATA_OUT4_IDX, false, false);
    //esp_rom_gpio_connect_out_signal(PIN_CLK,   I2S1O_BCK_OUT_IDX,    false, false);

    for (int i=0;i<5;i++) {
        GPIO.func_out_sel_cfg[pins[i]].oen_sel = 1;
        GPIO.enable_w1ts = (1U << pins[i]);
    }

}

static void i2s_set_clock(void){

    rtc_clk_apll_enable(true);
    uint32_t div = 0, sdm0 = 0, sdm1 = 0, sdm2 = 0;
    
    int freq = rtc_clk_apll_coeff_calc((PIXEL_CLK_HZ)*3, &div, &sdm0, &sdm1, &sdm2);
    ESP_LOGI("apll", "freq=%u, div=%u, sdm0=%u, sdm1=%u, sdm2=%u", freq, div, sdm0, sdm1, sdm2);
    rtc_clk_apll_coeff_set(div, sdm0, sdm1, sdm2);

    i2s_c.dev->clkm_conf.clka_en = 1;

    i2s_c.dev->clkm_conf.clkm_div_a = 1;
    i2s_c.dev->clkm_conf.clkm_div_b = 0;
    i2s_c.dev->clkm_conf.clkm_div_num = 3;
    i2s_c.dev->sample_rate_conf.tx_bck_div_num= 1;

    
    i2s_hal_tx_reset_fifo(&i2s_c);
    //
    
    

}

/*
static void i2s_set_clock(void){

    const int bitCount = 8; 
    long freq = PIXEL_CLK_HZ * 2 * (bitCount / 8); // freq = 25175000 * 2 * (16 / 8) = 100700000

    int sdm, sdmn;
    int odir = -1;
    do
    {	
        odir++;
        sdm = (long)(((double)(freq) / (20000000. / (odir + 2))) * 0x10000) - 0x40000;
        sdmn = (long)(((double)(freq) / (20000000. / (odir + 2 + 1))) * 0x10000) - 0x40000;
    } while(sdm < 0x8c0ecL && odir < 31 && sdmn < 0xA1fff);

    if(sdm > 0xA1fff) sdm = 0xA1fff;
    
    rtc_clk_apll_enable(true);
    rtc_clk_apll_coeff_set(odir, sdm & 255, (sdm >> 8) & 255, sdm >> 16);
    
    ESP_LOGI("apll", "freq=%u, div=%u, sdm0=%u, sdm1=%u, sdm2=%u", (unsigned int)freq, odir, sdm & 255, (sdm >> 8) & 255, sdm >> 16);

    i2s_c.dev->clkm_conf.clka_en = 1;

    i2s_c.dev->clkm_conf.clkm_div_num = 2;
    i2s_c.dev->clkm_conf.clkm_div_a = 1;
    i2s_c.dev->clkm_conf.clkm_div_b = 0;
    i2s_c.dev->sample_rate_conf.tx_bck_div_num = 1;
    

    
    i2s_hal_tx_reset_fifo(&i2s_c);
}
    */
void i2s_start(void){

    map_data_pins();
    
    periph_module_enable(PERIPH_I2S1_MODULE);

    i2s_hal_init(&i2s_c, 1);

    i2s_hal_tx_reset(&i2s_c);
    i2s_hal_tx_reset_fifo(&i2s_c);
    i2s_hal_tx_reset_dma(&i2s_c);

    i2s_c.dev->conf2.val = 0;
    i2s_c.dev->conf2.lcd_en = 1;
    i2s_c.dev->conf2.lcd_tx_wrx2_en = 1;
    i2s_c.dev->conf2.lcd_tx_sdx2_en = 0;

    i2s_c.dev->sample_rate_conf.val = 0;
    i2s_c.dev->sample_rate_conf.tx_bits_mod=8;

    i2s_set_clock();
    
    i2s_c.dev->fifo_conf.val = 0;
    i2s_c.dev->fifo_conf.tx_fifo_mod_force_en = 1;
    i2s_c.dev->fifo_conf.tx_fifo_mod = 1;
    i2s_c.dev->fifo_conf.dscr_en = 1;

    i2s_c.dev->conf1.val = 0;
    i2s_c.dev->conf1.tx_pcm_bypass = 1;

    i2s_c.dev->conf_chan.val = 0;
    i2s_c.dev->conf_chan.tx_chan_mod = 1;

    i2s_c.dev->conf.tx_msb_right = 0;
    i2s_c.dev->conf.tx_right_first = 1;
    i2s_c.dev->conf.tx_msb_shift = 0;
    i2s_c.dev->conf.tx_slave_mod = 0;

    

    i2s_enable_interrupts();
    
    i2s_hal_tx_enable_dma(&i2s_c);
    

    i2s_c.dev->out_link.stop  = 0;
    i2s_c.dev->out_link.addr = (uint32_t)(&desc_frontA);
    i2s_c.dev->out_link.start = 1;

    i2s_c.dev->clkm_conf.clka_en = 1;

    i2s_hal_tx_start(&i2s_c);

    

}

void start_buffer_i2s(void){

    frame_init();

}




