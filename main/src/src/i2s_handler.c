#include "i2s_handler.h"

static i2s_hal_context_t i2s_c;

static inline void i2s_set_clock(void){
    
    i2s_hal_clock_info_t clk = {

        .sclk = 0,
        .mclk = PIXEL_CLK_HZ * 2,
        .mclk_div = 2
    };

    i2s_hal_set_tx_clock(&i2s_c, &clk, I2S_CLK_SRC_APLL, NULL);

}

static void i2s_start(void){
    
    periph_module_enable(PERIPH_I2S0_MODULE);

    
}