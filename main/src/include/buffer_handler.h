#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "soc/soc_memory_layout.h"
#include "rom/lldesc.h"
#include "soc/lldesc.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define HALF_H_RES

#ifndef HALF_H_RES
#define H_ACTIVE_FRAMES                 640
#define H_FRONT_PORCH_FRAMES            16
#define H_SYNC_PULSE_FRAMES             96
#define H_BACK_PORCH_FRAMES             48
#else
#define H_ACTIVE_FRAMES                 320
#define H_FRONT_PORCH_FRAMES            8
#define H_SYNC_PULSE_FRAMES             48
#define H_BACK_PORCH_FRAMES             24
#endif


#define V_ACTIVE_FRAMES                 480
#define V_FRONT_PORCH_FRAMES            10
#define V_SYNC_PULSE_FRAMES             2
#define V_BACK_PORCH_FRAMES             33
#define TOTAL_V_FRAMES                  (V_ACTIVE_FRAMES + V_FRONT_PORCH_FRAMES + V_BACK_PORCH_FRAMES + V_SYNC_PULSE_FRAMES)

#define H_LOW_V_LOW     0b00000000
#define H_HIGH_V_LOW    0b00000001
#define H_LOW_V_HIGH    0b00000010
#define H_HIGH_V_HIGH   0b00000011

extern lldesc_t desc_frontA, desc_hsyncA, desc_backA, desc_activeA;
extern lldesc_t desc_frontB, desc_hsyncB, desc_backB, desc_activeB;
extern uint8_t h_front[H_FRONT_PORCH_FRAMES];
extern uint8_t h_hsync[H_SYNC_PULSE_FRAMES];
extern uint8_t h_back[H_BACK_PORCH_FRAMES];
extern uint8_t v_front[H_FRONT_PORCH_FRAMES];
extern uint8_t v_hsync[H_SYNC_PULSE_FRAMES];
extern uint8_t v_back[H_BACK_PORCH_FRAMES];
extern uint8_t lineA[H_ACTIVE_FRAMES];
extern uint8_t lineB[H_ACTIVE_FRAMES];
extern uint8_t black_lineH[H_ACTIVE_FRAMES];
extern uint8_t black_lineL[H_ACTIVE_FRAMES];
//extern volatile uint8_t *tx_next;
extern volatile uint8_t *fill_next;
extern SemaphoreHandle_t msg_ready;
extern SemaphoreHandle_t uart_send_avail;


void frame_init(void);

