#include "uart_gpio.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

int uart_gpio_data_available() {
    return uart_gpio[UART_GPIO_STATUS_INDEX] & (1 << UART_GPIO_STATUS_RX_VALID_DATA_BIT);
}

int uart_gpio_can_send() {
    return uart_gpio[UART_GPIO_STATUS_INDEX] & (1 << UART_GPIO_STATUS_TX_EMPTY_BIT);
}
// int uart_gpio_can_send() {
//     return !(uart_gpio[UART_GPIO_STATUS_INDEX] & (1 << UART_GPIO_STATUS_TX_FULL_BIT));
// }

void uart_gpio_puts(char *s) {
    while (*s) {
        uart_gpio_putchar(*s++);
    }
    uart_gpio_putchar('\n');
}

void uart_gpio_gets(char *s) {
    char ch;
    while ((ch = uart_gpio_getchar()) != '\n') {
        *s++ = ch;
    }
    *s = '\0';
}

char uart_gpio_getchar() {
    while (!uart_gpio_data_available());
    return uart_gpio[UART_GPIO_RX_INDEX];
}

void uart_gpio_putchar(char ch) {
    while (!uart_gpio_can_send());
    uart_gpio[UART_GPIO_TX_INDEX] = ch;
}

void uart_gpio_printf(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[256] = {0};
    vsprintf(buffer, fmt, args);
    va_end(args);
    uart_gpio_puts(buffer);
}

int uart_gpio_scanf(char *format, ...) {
    // Initialize variable argument list
    va_list args;
    va_start(args, format);

    // Read input from the user
    char input[256];  // Assuming a maximum input length of 255 characters
    uart_gpio_gets(input);

    // Use sscanf to parse the input
    int result = vsscanf(input, format, args);

    // Clean up variable argument list
    va_end(args);

    return result;
}