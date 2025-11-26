#include "vga_uart_manager.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
//#include "esp_log.h"
#include <string.h>
#include <stdio.h>


#define PIN_TXD     1 //17
#define PIN_RXD     3 //16
#define UART_PORT_NUM   2 //1

#define PIN_RTS     (UART_PIN_NO_CHANGE)
#define PIN_CTS     (UART_PIN_NO_CHANGE)

#define TASK_STACK_SIZE 3072

#define FAIL_MSG    "FAILED_MSG\0"
#define PREFIX      "$user: \0"




static void uart_task(void* arg){

    vga_uart_manager_t* vga = (vga_uart_manager_t*) (arg);
    uart_config_t conf = {
        .baud_rate = vga->baudrate,
        .data_bits = vga->data_bits,
        .parity = vga->parity,
        .stop_bits = vga->stop_bits,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    int intr_alloc_flags = 0;
    uart_driver_install(UART_PORT_NUM, 1024, 0, 0, NULL, intr_alloc_flags);
    uart_param_config(UART_PORT_NUM, &conf);
    uart_set_pin(UART_PORT_NUM, PIN_TXD, PIN_RXD, PIN_RTS, PIN_CTS);
    uart_flush_input(UART_PORT_NUM);

    char data[1024];
    const char fail[] = FAIL_MSG;
    const char prefix[] = PREFIX;

    int len_prefix = strlen(prefix);

    xSemaphoreGive(vga->uart_buffer_available);

    while(true){

        xSemaphoreTake(vga->uart_buffer_available, portMAX_DELAY);
        int len = uart_read_bytes(UART_PORT_NUM, data, 1024, 20 / portTICK_PERIOD_MS);

        if(len == 0){
                
            xSemaphoreGive(vga->uart_buffer_available);
            continue;

        } else if ((len > 0) && (len < (vga->screen_line_buffer_size - 1)) && (!vga->terminal_mode)){

            data[len] = '\0';
            memcpy(vga->message, data, len + 1);

        } else if ((len > 0) && (len < (vga->screen_line_buffer_size - (len_prefix + 1))) && (vga->terminal_mode)){
            
            data[len] = '\0';
            memcpy(vga->message, prefix, len_prefix);
            memcpy(vga->message + len_prefix, data, len + 1);
            

        } else {

            memcpy(vga->message, fail, sizeof(fail));

        }

        xSemaphoreGive(vga->message_ready);
    }



}



void uart_init_sem(vga_uart_manager_t* uart){

    uart->message_ready = xSemaphoreCreateBinary();
    uart->uart_buffer_available = xSemaphoreCreateBinary();
    configASSERT(uart->message_ready);
    configASSERT(uart->uart_buffer_available);

}

void uart_start_task(vga_uart_manager_t* uart){

    vTaskDelay(10/portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(uart_task, "uart_task", TASK_STACK_SIZE, (void*)(uart), 5, NULL, 0);

}

