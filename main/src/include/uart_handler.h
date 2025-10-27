#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#define PIN_TXD     1
#define PIN_RXD     3
#define PIN_RTS     (UART_PIN_NO_CHANGE)
#define PIN_CTS     (UART_PIN_NO_CHANGE)

#define UART_PORT_NUM   2
#define UART_BR         115200
#define TASK_STACK_SIZE 3072

#define BUF_SIZE (80)

#define FAIL_MSG    "FAILED_MSG"
#define SUFFIX      "$user: "

extern volatile char msg[BUF_SIZE];

extern SemaphoreHandle_t uart_send_avail;
extern SemaphoreHandle_t msg_ready;

void uart_start();

