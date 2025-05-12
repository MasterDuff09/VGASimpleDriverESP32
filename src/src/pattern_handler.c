#include "pattern_handler.h"

static uint8_t** pattern_chunk_active;
static uint8_t** pattern_chunk_blanking;



/**
 * @brief Initializes the VGA pattern handler by allocating memory for active and blanking pattern chunks.
 * 
 * @return 1 if initialization is successful, 0 otherwise.
 * 
 * This function allocates memory for the active and blanking pattern chunks and fills them with base values.
 */
int VGAinit(void){

    pattern_chunk_active = pattern_memory_alloc(ACTIVE_CHUNK_LEN, ACTIVE_SINGLE_CHUNK_SIZE);
    if (pattern_chunk_active == NULL) return 0;

    pattern_chunk_blanking = pattern_memory_alloc(BLANKING_CHUNK_LEN, BLANKING_SINGLE_CHUNK_SIZE);
    if (pattern_chunk_blanking == NULL) return 0;

    pattern_fill(pattern_chunk_active, pattern_chunk_blanking);
    return 1;

}

void VGAprint(void){

    int sig_sel = 0;
    int current_h_sync_bit = 0;

    for (int i = 0; i < ACTIVE_CHUNK_LEN; i++){
        current_h_sync_bit = 0;
        sig_sel = 0;
        for (int j = 0; j < ACTIVE_SINGLE_CHUNK_SIZE; j++){
            for (int bit_sel = 0; bit_sel < 8; bit_sel++){
                if (current_h_sync_bit < H_RES_ACT_FRAMES) {

                    if (sig_sel > (ACTIVE_BITS -1)){
                        sig_sel = 0;
                        current_h_sync_bit++;
                        printf("\n");
                    }

                    switch (sig_sel){
                        case 0:
                            vTaskDelay(10/portTICK_PERIOD_MS);
                            printf("current row: %d, current pixel(s): %d, ",i, current_h_sync_bit);
                            printf("H_sync: %d, ", (pattern_chunk_active[i][j] >> bit_sel) & 0x1);
                            break;
                        case 1:
                            printf("V_sync: %d, ", (pattern_chunk_active[i][j] >> bit_sel) & 0x1);
                            break;
                        case 2:
                            printf("R: %d, ", (pattern_chunk_active[i][j] >> bit_sel) & 0x1);
                            break;
                        case 3:
                            printf("G: %d, ", (pattern_chunk_active[i][j] >> bit_sel) & 0x1);
                            break;
                        case 4:
                            printf("B: %d\n", (pattern_chunk_active[i][j] >> bit_sel) & 0x1);
                            fflush(stdout);

                            break;
                        default:
                            break;
                    }
                } else if (current_h_sync_bit >= H_RES_ACT_FRAMES){
                    if (sig_sel > 1){
                        sig_sel = 0;
                        current_h_sync_bit++;
                        printf("\n");
                    }
                    switch (sig_sel){
                        case 0:
                            vTaskDelay(10/portTICK_PERIOD_MS);
                            printf("current row: %d, current pixel(s): %d, ",i, current_h_sync_bit);
                            printf("H_sync: %d, ", (pattern_chunk_active[i][j] >> bit_sel) & 0x1);
                            break;
                        case 1:
                            printf("V_sync: %d, R: 0, G: 0; B: 0\n", (pattern_chunk_active[i][j] >> bit_sel) & 0x1);
                            fflush(stdout);
                            break;
                        default:
                            break;
                    }
                }
                sig_sel++;
            }     
        }
    }

}




/**
 * @brief Allocates memory for pattern chunks.
 * 
 * @param frames Number of frames to allocate memory for.
 * @param chunk_size Size of each chunk in bytes.
 * @return Pointer to the allocated memory, or NULL if allocation fails.
 * 
 */
uint8_t** pattern_memory_alloc(int frames, int chunk_size){

    uint8_t** pattern_memory = (uint8_t**) heap_caps_malloc(frames * sizeof(uint8_t*), MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    if (pattern_memory == NULL) {
        return NULL;
    }
    for (int i = 0; i < frames; i++){
        pattern_memory[i] = heap_caps_malloc(chunk_size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
        if (pattern_memory[i] == NULL) {
            return NULL;
        }
    }
    return pattern_memory;
}

/**
 * @brief Fills the pattern chunks with the base values (white screen).
 * 
 * @param pattern_chunk_active Pointer to the active pattern chunk.
 * @param pattern_chunk_blanking Pointer to the blanking pattern chunk.
 * 
 * This function fills the pattern chunks with the base values based on the resolution and timing parameters.
 */
void pattern_fill(uint8_t** pattern_chunk_active, uint8_t** pattern_chunk_blanking){
    int sig_sel = 0; // starts with h-sync
    int current_h_sync_bit = 0;

    for (int i = 0; i < ACTIVE_CHUNK_LEN; i++){

        current_h_sync_bit = 0;
        sig_sel = 0;

        for (int j = 0; j < ACTIVE_SINGLE_CHUNK_SIZE; j++){

            pattern_chunk_active[i][j] = 0x0;

            for (int bit_sel = 0; bit_sel < 8; bit_sel++){

                if (current_h_sync_bit < H_RES_ACT_FRAMES) {

                    if (sig_sel > (ACTIVE_BITS - 1)){

                        sig_sel = 0;
                        current_h_sync_bit++;

                    }

                    pattern_chunk_active[i][j] |= (1 << bit_sel);

                } else {

                    if (sig_sel > 1){

                        sig_sel = 0;
                        current_h_sync_bit++;

                    }

                    if ((current_h_sync_bit > (H_RES_ACT_FRAMES - 1) && current_h_sync_bit < (H_RES_ACT_FRAMES + H_FRONT_PORCH_FRAMES)) || (current_h_sync_bit > (H_RES_ACT_FRAMES + H_FRONT_PORCH_FRAMES + H_SYNC_PULSE_FRAMES - 1) && current_h_sync_bit < (H_RES_ACT_FRAMES + H_BLANKING_FRAMES))){
                        
                        pattern_chunk_active[i][j] |= (1 << bit_sel);

                    } else {

                        if (sig_sel == 1) pattern_chunk_active[i][j] |= (1 << bit_sel);
                        else pattern_chunk_active[i][j] |= (0 << bit_sel);

                    }
                    
                }
        
                sig_sel++;
            }
        }
    }

    for (int i = 0; i < BLANKING_CHUNK_LEN; i++){

        current_h_sync_bit = 0;
        sig_sel = 0;

        for(int j = 0; j < BLANKING_SINGLE_CHUNK_SIZE; j++){

            pattern_chunk_blanking[i][j]=0x0;

            for (int bit_sel = 0; bit_sel < 8; bit_sel++){

                if (sig_sel > 1){
                    sig_sel = 0;
                    current_h_sync_bit++;
                }

                if ((current_h_sync_bit > (H_RES_ACT_FRAMES + H_FRONT_PORCH_FRAMES - 1) && current_h_sync_bit < (H_RES_ACT_FRAMES + H_FRONT_PORCH_FRAMES + H_SYNC_PULSE_FRAMES) && sig_sel == 0) || (i > (V_FRONT_PORCH_FRAMES - 1) && i < (V_FRONT_PORCH_FRAMES + V_SYNC_PULSE_FRAMES) && sig_sel == 1)) pattern_chunk_active[i][j] |= (0 << bit_sel);
                else pattern_chunk_active[i][j] |= (1 << bit_sel);
                
                sig_sel++;
            }
        }
    }
}

