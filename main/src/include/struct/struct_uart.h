#ifndef STRUCT_UART
#define STRUCT_UART

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


typedef struct {

    SemaphoreHandle_t uart_buffer_available;
    SemaphoreHandle_t message_ready;
    char message[1024];
    uint16_t screen_line_buffer_size;
    bool terminal_mode;
    uint64_t baudrate;
    uint8_t data_bits;
    uint8_t parity;
    uint8_t stop_bits;

}   vga_uart_manager_t;

#endif