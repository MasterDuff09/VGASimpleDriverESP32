#include "buffer_handler.h"

#include "esp_private/periph_ctrl.h"
#include "hal/hal_utils.h"
#include "soc/clk_tree_defs.h"

#include "hal/i2s_hal.h"
#include "hal/i2s_types.h"
#include "soc/i2s_periph.h"
#include "soc/i2s_struct.h"

#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "soc/gpio_sig_map.h"

#include "esp_check.h"


#define I2S_PORT        I2S0
#define PIXEL_CLK_HZ    12587500

#define PIN_HSYNC       18
#define PIN_VSYNC       19
#define PIN_R           27
#define PIN_G           26
#define PIN_B           25

#define PIN_CLK         16



