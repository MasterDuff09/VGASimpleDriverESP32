// main.c
#include <stdio.h>
#include <string.h>



void app_main(void)
{
    // Initialize the UART
    const char *message = "Hello, FreeRTOS!";
    printf("%s\n", message);
    fflush(stdout); // Ensure the message is printed immediately
}