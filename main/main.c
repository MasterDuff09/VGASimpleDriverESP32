#define H_ACTIVE_FRAMES                 640
#define H_FRONT_PORCH_FRAMES            16
#define H_SYNC_PULSE_FRAMES             96
#define H_BACK_PORCH_FRAMES             48


#define V_ACTIVE_FRAMES                 480
#define V_FRONT_PORCH_FRAMES            11
#define V_SYNC_PULSE_FRAMES             2
#define V_BACK_PORCH_FRAMES             31
#define TOTAL_V_FRAMES                  (V_ACTIVE_FRAMES + V_FRONT_PORCH_FRAMES + V_BACK_PORCH_FRAMES + V_SYNC_PULSE_FRAMES)

#define PIXEL_HZ                        25175000
#define UART_BR                         115200

#include "VGA.h"

VGA vga = {0};

void app_main(void){

    VGAinit(&vga, PIXEL_HZ, H_ACTIVE_FRAMES, H_FRONT_PORCH_FRAMES, H_SYNC_PULSE_FRAMES, H_BACK_PORCH_FRAMES, V_ACTIVE_FRAMES, V_FRONT_PORCH_FRAMES, V_SYNC_PULSE_FRAMES, V_BACK_PORCH_FRAMES);
    VGAsetPins(&vga, 27, 26, 25, 19, 18);
    VGAsetSerial(&vga, UART_BR, 8, 0, 1);
    VGAsetScaling(true);

    VGA_Start();

    return;    
}
