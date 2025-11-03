#include "lldesc.h"

static void lldesc_link(lldesc_t *d, void *buf, int len_bytes, lldesc_t *next){

    d->size   = len_bytes;     
    d->length = len_bytes;     
    d->buf    = buf;           
    d->eof    = 0;             
    d->sosf   = 0;             
    d->owner  = 1;             
    d->qe.stqe_next = next;

}

void vga_lldesc_init(vga_lldesc_manager_t* d, lldesc_buf_config_t* b){
    
    lldesc_link(&(d->desc_frontA), b->h_front, H_FRONT_PORCH_FRAMES, &(d->desc_hsyncA));
    lldesc_link(&(d->desc_hsyncA), b->h_hsync, H_SYNC_PULSE_FRAMES,  &(d->desc_backA));
    lldesc_link(&(d->desc_backA), b->h_back, H_BACK_PORCH_FRAMES, &(d->desc_activeA));
    lldesc_link(&(d->desc_activeA), b->lineA, H_ACTIVE_FRAMES, &(d->desc_frontB));
    d->desc_activeA.eof=1;

    lldesc_link(&(d->desc_frontB), b->h_front, H_FRONT_PORCH_FRAMES, &(d->desc_hsyncB));
    lldesc_link(&(d->desc_hsyncB), b->h_hsync, H_SYNC_PULSE_FRAMES,  &(d->desc_backB));
    lldesc_link(&(d->desc_backB), b->h_back, H_BACK_PORCH_FRAMES, &(d->desc_activeB));
    lldesc_link(&(d->desc_activeB), b->lineB, H_ACTIVE_FRAMES, &(d->desc_frontA));

    d->desc_activeB.eof=1;
}