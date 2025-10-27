#include "buffer_handler.h"

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


#include "hal/i2s_hal.h"
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


#define I2S_PORT        I2S1
#define PIXEL_CLK_HZ    12587500 * 2

#define PIN_HSYNC       18
#define PIN_VSYNC       19
#define PIN_R           27
#define PIN_G           26
#define PIN_B           25

#define PIN_CLK         23

void init_sem(void);
//void i2s_enable_interrupts(void);
//void IRAM_ATTR i2s_tx_isr(void *arg);
//void map_data_pins(void);
//void i2s_set_clock(void);
void start_buffer_i2s(void);
void i2s_start();


extern SemaphoreHandle_t line_ready;
extern SemaphoreHandle_t display_done;
extern volatile bool last_eof_A;
extern volatile uint16_t current_y_line;




