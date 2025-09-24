#include "buffer_handler.h"


static lldesc_t desc_front, desc_hsync, desc_back, desc_active;
uint8_t* h_front;
uint8_t* h_hsync;
uint8_t* h_back;
uint8_t* v_front;
uint8_t* v_hsync;
uint8_t* v_back;
uint8_t* lineA;
uint8_t* lineB;


static inline void buffer_init(void){
    
    h_front = heap_caps_malloc(H_FRONT_PORCH_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    h_hsync = heap_caps_malloc(H_SYNC_PULSE_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    h_back  = heap_caps_malloc(H_BACK_PORCH_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    v_front = heap_caps_malloc(H_FRONT_PORCH_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    v_hsync = heap_caps_malloc(H_SYNC_PULSE_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    v_back  = heap_caps_malloc(H_BACK_PORCH_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    lineA   = heap_caps_malloc(H_ACTIVE_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    lineB   = heap_caps_malloc(H_ACTIVE_FRAMES, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
}

static inline void fill_costant(void){
    int i = 0;
    while (i < H_ACTIVE_FRAMES){

        lineA[i] = H_HIGH_V_HIGH;
        lineB[i] = H_HIGH_V_HIGH;

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

static inline void lldesc_link(lldesc_t *d, void *buf, int len_bytes, lldesc_t *next){

    d->size   = len_bytes;     
    d->length = len_bytes;     
    d->buf    = buf;           
    d->eof    = 0;             
    d->sosf   = 0;             
    d->owner  = 1;             
    d->qe.stqe_next = next;

}

static inline void lldesc_init(void){
    
    
    lldesc_link(&desc_front, h_front, H_FRONT_PORCH_FRAMES, &desc_hsync);
    lldesc_link(&desc_hsync, h_hsync, H_SYNC_PULSE_FRAMES,  &desc_back);
    lldesc_link(&desc_back, h_back, H_BACK_PORCH_FRAMES, &desc_active);
    lldesc_link(&desc_active, lineA, H_ACTIVE_FRAMES, &desc_front);
    desc_active.eof=1;

}


void frame_init(void){

    buffer_init();
    fill_costant();
    lldesc_init();
    
}