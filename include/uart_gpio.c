#include "uart_gpio.h"

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
        while (!uart_gpio_can_send());
        uart_gpio[UART_GPIO_TX_INDEX] = *s++;
    }
    while (!uart_gpio_can_send());
    uart_gpio[UART_GPIO_TX_INDEX] = '\n';
}