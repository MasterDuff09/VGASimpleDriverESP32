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
abbiamo un totale di 400 * 525 = 210000 pixel. Questo sarà la grandezza del pattern totale del H-sync e V-sync.
usiamo direttamente la combinazione di 0/0.7v per i singoli RGB signals, o acceso o spento, perciò in tutto 8 colori.
Salviamo lo stato dei 5 segnali in un array di 8 bit, dove i primi 2 bit meno significativi sono V-sync e H-sync, i 3 successivi sono i colori RGB.
Il tutto salvato nell'array di  allocato nell'heap, per comandare ogni singolo pixel. 
*/

#define PATTERN_LEN              210000                   //numero di pixel
#define MEM_CHUNK_1KB            1024                     //dimensione del chunk in KB
#define PATTERN_TO_BUF_SIZE      (206 *  MEM_CHUNK_1KB)   //numero di byte indirizzabili più vicino a Pattern_len, multiplo di 8




static uint8_t* pattern_chunks [PATTERN_TO_BUF_SIZE / MEM_CHUNK_1KB];


void app_main(void)
{
    // Initialize the UART
    const char *message = "Hello, world!";
    printf("%s\n", message);
    fflush(stdout); // Ensure the message is printed immediately
    int chunk_counter = PATTERN_TO_BUF_SIZE / MEM_CHUNK_1KB;
    for (uint8_t i = 0; i < chunk_counter; i++){
        pattern_chunks[i] = heap_caps_malloc(MEM_CHUNK_1KB, MALLOC_CAP_DMA);
        if (pattern_chunks[i] == NULL) {
            printf("Failed to allocate memory for chunk %d\n", i);
            fflush(stdout); // Ensure the message is printed immediately
            return;
        }
    }
    printf("Allocated %d chunks of %d bytes each\n", chunk_counter, MEM_CHUNK_1KB);
    fflush(stdout); // Ensure the message is printed immediately
    return;
}