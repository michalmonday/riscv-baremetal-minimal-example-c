#ifndef UART_GPIO_H
#define UART_GPIO_H

static volatile int *uart_gpio = (int *)(void *)0xC0004000; 
////////////////////////////////////////////////
// definitions for UART GPIO (AXI UartLite IP), allowing to communicate with Esp32-based display
#define UART_GPIO_RX_INDEX 0
#define UART_GPIO_TX_INDEX 1
#define UART_GPIO_STATUS_INDEX 2
#define UART_GPIO_CONTROL_INDEX 3

// status register bits
#define UART_GPIO_STATUS_RX_VALID_DATA_BIT 0
#define UART_GPIO_STATUS_RX_FULL_BIT 1
#define UART_GPIO_STATUS_TX_EMPTY_BIT 2
#define UART_GPIO_STATUS_TX_FULL_BIT 3
#define UART_GPIO_STATUS_INTERRUPT_ENABLED_BIT 4
#define UART_GPIO_STATUS_OVERRUN_ERROR_BIT 5
#define UART_GPIO_STATUS_FRAME_ERROR_BIT 6
#define UART_GPIO_STATUS_PARITY_ERROR_BIT 7

extern int uart_gpio_data_available();
extern int uart_gpio_can_send();
extern void uart_gpio_puts(char *s);

#endif