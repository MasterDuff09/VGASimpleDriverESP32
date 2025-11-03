#include "buffer.h"


static inline void* buffer_malloc(uint16_t size){
    return heap_caps_malloc(size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
}

void vga_buffer_init(vga_buffer_manager_t* buf_t /*, dimensions_t *d */){

    buf_t->h_front = buffer_malloc(H_FRONT_PORCH_FRAMES);
    buf_t->v_front = buffer_malloc(H_FRONT_PORCH_FRAMES);

    buf_t->h_hsync = buffer_malloc(H_SYNC_PULSE_FRAMES);
    buf_t->v_hsync = buffer_malloc(H_SYNC_PULSE_FRAMES);

    buf_t->h_back = buffer_malloc(H_BACK_PORCH_FRAMES);
    buf_t->v_back = buffer_malloc(H_BACK_PORCH_FRAMES);

    buf_t->lineA = buffer_malloc(H_ACTIVE_FRAMES);
    buf_t->lineB = buffer_malloc(H_ACTIVE_FRAMES);

    memset(buf_t->lineA, H_HIGH_V_HIGH, H_ACTIVE_FRAMES);
    memset(buf_t->lineB, H_HIGH_V_HIGH, H_ACTIVE_FRAMES);

    memset(buf_t->h_hsync, H_LOW_V_HIGH, H_SYNC_PULSE_FRAMES);
    memset(buf_t->v_hsync, H_LOW_V_LOW, H_SYNC_PULSE_FRAMES);

    memset(buf_t->h_back, H_HIGH_V_HIGH, H_BACK_PORCH_FRAMES);
    memset(buf_t->v_back, H_HIGH_V_LOW, H_BACK_PORCH_FRAMES);

    memset(buf_t->h_front, H_HIGH_V_HIGH, H_FRONT_PORCH_FRAMES);
    memset(buf_t->v_front, H_HIGH_V_LOW, H_FRONT_PORCH_FRAMES);
}

inline void* get_addr_next_buf_to_fill(vga_buffer_manager_t* buf_t){

    return  buf_t->fill_next;

}

inline void set_addr_next_buf_to_fill(vga_buffer_manager_t* buf_t, void* addr){

    buf_t->fill_next = addr;

}

inline void* get_addr_buf_h_front(vga_buffer_manager_t* buf_t){

    return  buf_t->h_front;

}

inline void* get_addr_buf_h_back(vga_buffer_manager_t* buf_t){

    return  buf_t->h_back;

}

inline void* get_addr_buf_h_hsync(vga_buffer_manager_t* buf_t){

    return  buf_t->h_hsync;

}

inline void* get_addr_buf_v_front(vga_buffer_manager_t* buf_t){

    return  buf_t->v_front;

}

inline void* get_addr_buf_v_back(vga_buffer_manager_t* buf_t){

    return  buf_t->v_back;

}

inline void* get_addr_buf_v_hsync(vga_buffer_manager_t* buf_t){

    return  buf_t->v_hsync;

}

inline void* get_addr_buf_line_a(vga_buffer_manager_t* buf_t){

    return buf_t->lineA;

}

inline void* get_addr_buf_line_b(vga_buffer_manager_t* buf_t){

    return buf_t->lineB;

}



