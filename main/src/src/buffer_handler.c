#include "buffer_handler.h"

DMA_ATTR lldesc_t desc_frontA, desc_hsyncA, desc_backA, desc_activeA;
DMA_ATTR lldesc_t desc_frontB, desc_hsyncB, desc_backB, desc_activeB;
DMA_ATTR uint8_t h_front[H_FRONT_PORCH_FRAMES];
DMA_ATTR uint8_t h_hsync[H_SYNC_PULSE_FRAMES];
DMA_ATTR uint8_t h_back[H_BACK_PORCH_FRAMES];
DMA_ATTR uint8_t v_front[H_FRONT_PORCH_FRAMES];
DMA_ATTR uint8_t v_hsync[H_SYNC_PULSE_FRAMES];
DMA_ATTR uint8_t v_back[H_BACK_PORCH_FRAMES];
DMA_ATTR uint8_t lineA[H_ACTIVE_FRAMES];
DMA_ATTR uint8_t lineB[H_ACTIVE_FRAMES];
DMA_ATTR uint8_t black_lineH[H_ACTIVE_FRAMES];
DMA_ATTR uint8_t black_lineL[H_ACTIVE_FRAMES];

//volatile uint8_t* tx_next = NULL;
volatile uint8_t* fill_next = NULL;
SemaphoreHandle_t msg_ready = NULL;
SemaphoreHandle_t uart_send_avail = NULL;

/*
static void buffer_init(void){
    
    h_front = heap_caps_malloc(H_FRONT_PORCH_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    h_hsync = heap_caps_malloc(H_SYNC_PULSE_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    h_back  = heap_caps_malloc(H_BACK_PORCH_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    v_front = heap_caps_malloc(H_FRONT_PORCH_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    v_hsync = heap_caps_malloc(H_SYNC_PULSE_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    v_back  = heap_caps_malloc(H_BACK_PORCH_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    lineA   = heap_caps_malloc(H_ACTIVE_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    lineB   = heap_caps_malloc(H_ACTIVE_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    black_lineH = heap_caps_malloc(H_ACTIVE_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    black_lineL = heap_caps_malloc(H_ACTIVE_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
}
*/

void fill_costant(void){
    int i = 0;
    while (i < H_ACTIVE_FRAMES){

        lineA[i] = H_HIGH_V_HIGH;
        lineB[i] = H_HIGH_V_HIGH;
        black_lineH[i] = H_HIGH_V_HIGH;
        black_lineL[i] = H_HIGH_V_LOW;

        if (i < H_SYNC_PULSE_FRAMES){

            h_hsync[i] = H_LOW_V_HIGH;
            v_hsync[i] = H_LOW_V_LOW;

            if (i < H_BACK_PORCH_FRAMES){

                h_back[i] = H_HIGH_V_HIGH;
                v_back[i] = H_HIGH_V_LOW;

                if (i < H_FRONT_PORCH_FRAMES){

                    h_front[i] = H_HIGH_V_HIGH;
                    v_front[i] = H_HIGH_V_LOW;

                }
            }
        }

        i++;
    }
}

static void lldesc_link(lldesc_t *d, void *buf, int len_bytes, lldesc_t *next){

    d->size   = len_bytes;     
    d->length = len_bytes;     
    d->buf    = buf;           
    d->eof    = 0;             
    d->sosf   = 0;             
    d->owner  = 1;             
    d->qe.stqe_next = next;

}

static void lldesc_init(void){
    
    
    lldesc_link(&desc_frontA, h_front, H_FRONT_PORCH_FRAMES, &desc_hsyncA);
    lldesc_link(&desc_hsyncA, h_hsync, H_SYNC_PULSE_FRAMES,  &desc_backA);
    lldesc_link(&desc_backA, h_back, H_BACK_PORCH_FRAMES, &desc_activeA);
    lldesc_link(&desc_activeA, lineA, H_ACTIVE_FRAMES, &desc_frontB);
    desc_activeA.eof=1;

    lldesc_link(&desc_frontB, h_front, H_FRONT_PORCH_FRAMES, &desc_hsyncB);
    lldesc_link(&desc_hsyncB, h_hsync, H_SYNC_PULSE_FRAMES,  &desc_backB);
    lldesc_link(&desc_backB, h_back, H_BACK_PORCH_FRAMES, &desc_activeB);
    lldesc_link(&desc_activeB, lineB, H_ACTIVE_FRAMES, &desc_frontA);

    desc_activeB.eof=1;

    //tx_next = lineA;
    fill_next = lineA;

}


void frame_init(void){

    //buffer_init();
    fill_costant();
    lldesc_init();
    
}