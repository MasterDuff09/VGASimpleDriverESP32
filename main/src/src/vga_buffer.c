#include "vga_buffer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"


static inline void* buffer_malloc(uint16_t size){
    return heap_caps_malloc(size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
}

void vga_buffer_init(vga_buffer_manager_t* buf_t, vga_dimensions_t* dim){

    const uint8_t H_LOW_V_LOW  =     0b00000000;
    const uint8_t H_HIGH_V_LOW =     0b00000001;
    const uint8_t H_LOW_V_HIGH =     0b00000010;
    const uint8_t H_HIGH_V_HIGH=     0b00000011;

    buf_t->h_front = buffer_malloc(dim->len_front_porch);
    buf_t->v_front = buffer_malloc(dim->len_front_porch);

    buf_t->h_hsync = buffer_malloc(dim->len_h_sync_frames);
    buf_t->v_hsync = buffer_malloc(dim->len_h_sync_frames);

    buf_t->h_back = buffer_malloc(dim->len_back_porch);
    buf_t->v_back = buffer_malloc(dim->len_back_porch);

    buf_t->lineA = buffer_malloc(dim->len_active_frames);
    buf_t->lineB = buffer_malloc(dim->len_active_frames);

    memset(buf_t->lineA, H_HIGH_V_HIGH, dim->len_active_frames);
    memset(buf_t->lineB, H_HIGH_V_HIGH, dim->len_active_frames);

    memset(buf_t->h_hsync, H_LOW_V_HIGH, dim->len_h_sync_frames);
    memset(buf_t->v_hsync, H_LOW_V_LOW, dim->len_h_sync_frames);

    memset(buf_t->h_back, H_HIGH_V_HIGH, dim->len_back_porch);
    memset(buf_t->v_back, H_HIGH_V_LOW, dim->len_back_porch);

    memset(buf_t->h_front, H_HIGH_V_HIGH, dim->len_front_porch);
    memset(buf_t->v_front, H_HIGH_V_LOW, dim->len_front_porch);
}

inline void* get_addr_next_buf_to_fill(vga_buffer_manager_t* buf_t){

    return  buf_t->fill_next;

}

inline void set_addr_next_buf_to_fill(vga_buffer_manager_t* buf_t, uint8_t* addr){

    buf_t->fill_next = addr;

}


/*


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
*/



