#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "resolution.h"

int VGAinit(void);
void VGAprint(void);
uint8_t** pattern_memory_alloc(int frames, int chunk_size);
void pattern_fill(uint8_t** pattern_chunk_active, uint8_t** pattern_chunk_blanking);
