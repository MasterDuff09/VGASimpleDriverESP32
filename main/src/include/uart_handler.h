#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

#define BUF_SIZE (1024)