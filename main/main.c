// main.c
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "res.h"


/*
L'idea è di fare un resizing della risoluzione VGA da 640x480 a 320x480.
Dimezzando la frequenza di Pixel clock da 25.175 MHz a 12.5875 MHz mantenendo la lunghezza complessiva della riga. 
Tenendo però che tengo la risoluzione verticale al massimo, dimezzo il refresh rate a 30Hz invece che agli standard 60 Hz.
Contando un totale di 400 pixel per riga (320 pixel visibili + 80 pixel di sync) e 525 righe (480 righe visibili + 45 righe di sync)
abbiamo un totale di 400 * 525 = 210000 pixel. Questo sarà la lunghezza del pattern totale del H-sync e V-sync.
Uso direttamente la combinazione di 0/0.7v per i singoli RGB signals, o acceso o spento, perciò in tutto 8 colori.
Per risparmiare memoria, calcolo direttamente il numero di bit complessivi necessari per il controllo preciso dei singoli segnali.
Il tutto salvato nell'array allocato nell'heap, per comandare ogni singolo pixel.
*/

static uint8_t* pattern_chunk_active[ACTIVE_CHUNK_LEN];
static uint8_t* pattern_chunk_blanking[BLANKING_CHUNK_LEN];




void app_main(void){
    for (int i = 0; i < ACTIVE_CHUNK_LEN; i++){
        printf("allocating chunk %d\n", i);
        pattern_chunk_active[i] = heap_caps_malloc(ACTIVE_SINGLE_CHUNK_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
        if (pattern_chunk_active[i] == NULL) {
            return;
        }
    }
    for (int i = 0; i < BLANKING_CHUNK_LEN; i++){
        printf("allocating chunk %d\n", i);
        pattern_chunk_blanking[i] = heap_caps_malloc(BLANKING_SINGLE_CHUNK_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
        if (pattern_chunk_blanking[i] == NULL) {
            return;
        }
    }

    int sig_sel = 0; // starts with h-sync
    int current_h_sync_bit = 0;
    for (int i = 0; i < ACTIVE_CHUNK_LEN; i++){
        current_h_sync_bit = 0;
        sig_sel = 0;
        for (int j = 0; j < ACTIVE_SINGLE_CHUNK_SIZE; j++){
            pattern_chunk_active[i][j] = 0x0;
            for (int bit_sel = 0; bit_sel < 8; bit_sel++){

                if (current_h_sync_bit < H_RES_ACT_FRAMES && sig_sel > (ACTIVE_BITS -1)) {
                    sig_sel = 0;
                    current_h_sync_bit++;
                } else if ((current_h_sync_bit >= H_RES_ACT_FRAMES) &&  sig_sel > 1){
                    sig_sel = 0;
                    current_h_sync_bit++;
                }
                
                if ((current_h_sync_bit > (H_RES_ACT_FRAMES - 1) && current_h_sync_bit < (H_RES_ACT_FRAMES + H_FRONT_PORCH_FRAMES)) || (current_h_sync_bit > (H_RES_ACT_FRAMES + H_FRONT_PORCH_FRAMES + H_SYNC_PULSE_FRAMES - 1) && current_h_sync_bit < (H_RES_ACT_FRAMES + H_BLANKING_FRAMES))){
                    pattern_chunk_active[i][j] |= (1 << bit_sel);
                } else if (current_h_sync_bit > (H_RES_ACT_FRAMES + H_FRONT_PORCH_FRAMES - 1) && current_h_sync_bit < (H_RES_ACT_FRAMES + H_FRONT_PORCH_FRAMES + H_SYNC_PULSE_FRAMES)){
                    if (sig_sel == 1) pattern_chunk_active[i][j] |= (1 << bit_sel);
                    else pattern_chunk_active[i][j] |= (0 << bit_sel);    
                } else pattern_chunk_active[i][j] |= (1 << bit_sel);
                sig_sel++;
            }
        }
    }
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
                            printf("current row: %d, current pixel: %d, ",i, current_h_sync_bit);
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
                            printf("current row: %d, current pixel: %d, ",i, current_h_sync_bit);
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
    printf("done\n");
    fflush(stdout);
    return;

}