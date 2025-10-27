#include "uart_handler.h"


SemaphoreHandle_t msg_ready = NULL;
SemaphoreHandle_t uart_send_avail = NULL;

volatile char msg[BUF_SIZE] = {0};

const char fail[] = FAIL_MSG;
const char suffix[] = SUFFIX;

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

    uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags);
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, PIN_TXD, PIN_RXD, PIN_RTS, PIN_CTS);

    char data[BUF_SIZE]; 

    uart_send_avail = xSemaphoreCreateBinary();
    msg_ready = xSemaphoreCreateBinary();

    xSemaphoreGive(uart_send_avail);

    while(true){

        xSemaphoreTake(uart_send_avail, portMAX_DELAY);

        int len = 0;

        size_t recvd_left_len = 0;
        
        len = uart_read_bytes(UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        
        uart_get_buffered_data_len(UART_PORT_NUM, &recvd_left_len);

        if (len > 0 && len < BUF_SIZE - 1 && recvd_left_len == 0){

            memcpy(msg, data, len);
            msg[len] = '\0';
            xSemaphoreGive(msg_ready);

        } else if (len == 0){

            continue;

        } else {

            memcpy(msg, fail, sizeof(fail));
            
        }

    }
}

void uart_start(){

    vTaskDelay(10/portTICK_PERIOD_MS);
    xTaskCreate(uart_task, "uart_task", TASK_STACK_SIZE, NULL, 10, 0);
    
}