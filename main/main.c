// main.c
#include <stdio.h>
#include <string.h>

#include "pattern_handler.h"

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






void app_main(void){
    

    VGAinit();
    //VGAprint();
    printf("done\n");
    fflush(stdout);
    return;

}



/*

*/