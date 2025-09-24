// main.c
#include <stdio.h>
#include <string.h>
#include "buffer_handler.h"

#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "driver/i2s_common.h"
#include "sdkconfig.h"


void app_main(void){
    printf("start\n");
    frame_init();
    printf("done\n");
    fflush(stdout);
    return;
}