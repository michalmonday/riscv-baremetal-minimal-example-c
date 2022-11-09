
#include "uart_lite.h"

static volatile int *uart = (int *)(void *)0x62300000;

#define UART_REG_TXFIFO 0x0
#define UART_REG_RXFIFO 0x0

#define UART_REG_STAT 0x8
#define UART_REG_CTRL 0xC

#define STAT_REG_BIT_TX_FIFO_VALID_DATA 0x1

int putchar(char ch) {
    uart[UART_REG_TXFIFO] = ch;
}

int data_available() {
    return uart[UART_REG_STAT] & STAT_REG_BIT_TX_FIFO_VALID_DATA;
}

int getchar() {
    while (!data_available());
    return uart[UART_REG_RXFIFO] & 0xFF;
}
