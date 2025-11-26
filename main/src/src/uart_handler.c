#include "uart_handler.h"



char msg[BUF_SIZE] = {0};





static void uart_task(void *arg){

    uart_config_t uart_config = {
        .baud_rate = UART_BR,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    int intr_alloc_flags = 0;
    

    uart_driver_install(UART_PORT_NUM, BUF_SIZE * 4, 0, 0, NULL, intr_alloc_flags);
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, PIN_TXD, PIN_RXD, PIN_RTS, PIN_CTS);

    char data[1024]; 
    const char fail[] = FAIL_MSG;
    const char suffix[] = SUFFIX;

    int len_suffix = strlen(suffix);

    xSemaphoreGive(uart_send_avail);


    while(true){

        xSemaphoreTake(uart_send_avail, portMAX_DELAY);

        int len = 0;

        //size_t recvd_left_len = 0;


        //memcpy(msg, suffix, len_suffix);
        len = uart_read_bytes(UART_PORT_NUM, data, 1024, 20 / portTICK_PERIOD_MS);
        
        //uart_get_buffered_data_len(UART_PORT_NUM, &recvd_left_len);

        if (len == 0){

            xSemaphoreGive(uart_send_avail);
            continue;
            /*
            memcpy(msg, suffix, len_suffix);
            memcpy(msg + len_suffix, data, len);
            msg[len + len_suffix] = '\0';
            */

        } else if ((len > 0) && (len < (BUF_SIZE - 1))){

            
            data[len] = '\0';
            memcpy(msg, data, len + 1);

        } else {

            memcpy(msg, fail, sizeof(fail));
            
        }
        xSemaphoreGive(msg_ready);

    }
}

void uart_start(){

    vTaskDelay(10/portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(uart_task, "uart_task", TASK_STACK_SIZE, NULL, 5, NULL, 0);
    
}