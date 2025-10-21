#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "rom/lldesc.h"
#include "soc/lldesc.h"


#define H_ACTIVE_FRAMES                 640
#define H_FRONT_PORCH_FRAMES            16
#define H_SYNC_PULSE_FRAMES             96
#define H_BACK_PORCH_FRAMES             48


#define V_ACTIVE_FRAMES                 480
#define V_FRONT_PORCH_FRAMES            10
#define V_SYNC_PULSE_FRAMES             2
#define V_BACK_PORCH_FRAMES             33

#define RGB_RES_BITS                    3
#define H_SYNC_V_SYNC_BITS              2

#define USE_A   1
#define USE_B   0

#define H_LOW_V_LOW     0b00000000
#define H_HIGH_V_LOW    0b00000001
#define H_LOW_V_HIGH    0b00000010
#define H_HIGH_V_HIGH   0b00000011

extern lldesc_t desc_frontA, desc_hsyncA, desc_backA, desc_activeA;
extern lldesc_t desc_frontB, desc_hsyncB, desc_backB, desc_activeB;
extern uint8_t* h_front;
extern uint8_t* h_hsync;
extern uint8_t* h_back;
extern uint8_t* v_front;
extern uint8_t* v_hsync;
extern uint8_t* v_back;
extern uint8_t* lineA;
extern uint8_t* lineB;
//extern volatile uint8_t *tx_next;
extern volatile uint8_t *fill_next;


void frame_init(void);

