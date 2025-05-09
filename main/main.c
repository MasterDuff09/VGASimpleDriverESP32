// main.c
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

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

#define H_RES_ACT_FRAMES                320
#define H_FRONT_PORCH_FRAMES            8
#define H_SYNC_PULSE_FRAMES             48
#define H_BACK_PORCH_FRAMES             24
#define H_BLANKING_FRAMES               (H_FRONT_PORCH_FRAMES + H_SYNC_PULSE_FRAMES + H_BACK_PORCH_FRAMES)

#define V_RES_ACT_FRAMES                480
#define V_FRONT_PORCH_FRAMES            10
#define V_SYNC_PULSE_FRAMES             2
#define V_BACK_PORCH_FRAMES             33
#define V_BLANKING_FRAMES               (V_FRONT_PORCH_FRAMES + V_SYNC_PULSE_FRAMES + V_BACK_PORCH_FRAMES)

#define RGB_RES_BITS                    3
#define H_SYNC_V_SYNC_BITS              2
#define ACTIVE_BITS                     (RGB_RES_BITS + H_SYNC_V_SYNC_BITS)

#define ROW_ACTIVE_BITS                 (H_RES_ACT_FRAMES * ACTIVE_BITS) // 320 frames * 5 bits (01011 --> 0R, 1G, 0B, 1H, 1V)
#define ROW_FRONT_PORCH_BITS            (H_FRONT_PORCH_FRAMES * H_SYNC_V_SYNC_BITS) //only H/V-SYNC bits
#define ROW_SYNC_PULSE_BITS             (H_SYNC_PULSE_FRAMES * H_SYNC_V_SYNC_BITS)
#define ROW_BACK_PORCH_BITS             (H_BACK_PORCH_FRAMES * H_SYNC_V_SYNC_BITS)

#define V_ACTIVE_ROW_TOTAL_BITS         (ROW_ACTIVE_BITS + ROW_FRONT_PORCH_BITS + ROW_SYNC_PULSE_BITS + ROW_BACK_PORCH_BITS)
#define V_ACTIVE_ROW_TOTAL_BYTES        (V_ACTIVE_ROW_TOTAL_BITS / 8)

#define V_BLANKING_ROW_TOTAL_BITS       ((H_RES_ACT_FRAMES + H_BLANKING_FRAMES) * 2) //total row frames == 400 * 2bit (h/v-sync) == 800
#define V_BLANKING_ROW_TOTAL_BYTES      (V_BLANKING_ROW_TOTAL_BITS / 8)

#define TOTAL_ACTIVE_FRAMES_BYTES       (V_ACTIVE_ROW_TOTAL_BYTES * V_RES_ACT_FRAMES)
#define TOTAL_BLANKING_FRAMES_BYTES     (V_BLANKING_ROW_TOTAL_BYTES * V_BLANKING_FRAMES)

#define TOTAL_BYTES                     (TOTAL_ACTIVE_FRAMES_BYTES + TOTAL_BLANKING_FRAMES_BYTES)
#define TOTAL_FRAMES                    ((H_RES_ACT_FRAMES + H_BLANKING_FRAMES) * (V_RES_ACT_FRAMES + V_BLANKING_FRAMES))

#define ACTIVE_CHUNK_LEN                V_RES_ACT_FRAMES
#define BLANKING_CHUNK_LEN              V_BLANKING_FRAMES
#define ACTIVE_SINGLE_CHUNK_SIZE        V_ACTIVE_ROW_TOTAL_BYTES
#define BLANKING_SINGLE_CHUNK_SIZE      V_BLANKING_ROW_TOTAL_BYTES





static uint8_t* pattern_chunk_active[ACTIVE_CHUNK_LEN];
static uint8_t* pattern_chunk_blanking[BLANKING_CHUNK_LEN];




void app_main(void){
    const char *message = "Hello, world!";
    printf("%s\n", message);
    fflush(stdout); // Ensure the message is printed immediately
    for (int i = 0; i < ACTIVE_CHUNK_LEN; i++){
        printf("allocating chunk %d\n", i);
        pattern_chunk_active[i] = heap_caps_malloc(ACTIVE_SINGLE_CHUNK_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
        if (pattern_chunk_active[i] == NULL) {
            printf("Failed to allocate memory for chunk %d\n", i);
            fflush(stdout); // Ensure the message is printed immediately
            return;
        }
        vTaskDelay(200 / portTICK_PERIOD_MS); // Delay to simulate some processing time
    }
    printf("Allocated %d chunks of %d bytes each\n", ACTIVE_CHUNK_LEN, ACTIVE_SINGLE_CHUNK_SIZE);
    fflush(stdout); // Ensure the message is printed immediately
    for (int i = 0; i < BLANKING_CHUNK_LEN; i++){
        printf("allocating chunk %d\n", i);
        pattern_chunk_blanking[i] = heap_caps_malloc(BLANKING_SINGLE_CHUNK_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
        if (pattern_chunk_blanking[i] == NULL) {
            printf("Failed to allocate memory for chunk %d\n", i);
            fflush(stdout); // Ensure the message is printed immediately
            return;
        }
        vTaskDelay(200 / portTICK_PERIOD_MS); // Delay to simulate some processing time
    }
    printf("Allocated %d chunks of %d bytes each\n", BLANKING_CHUNK_LEN, BLANKING_SINGLE_CHUNK_SIZE);
    fflush(stdout); // Ensure the message is printed immediately
    return;

}