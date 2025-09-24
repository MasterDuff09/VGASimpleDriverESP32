#include "buffer_resolution.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "rom/lldesc.h"
#include "soc/lldesc.h"

#define USE_A   1
#define USE_B   0

#define H_LOW_V_LOW     0b00000000
#define H_HIGH_V_LOW    0b00000001
#define H_LOW_V_HIGH    0b00000010
#define H_HIGH_V_HIGH   0b00000011


void frame_init(void);

