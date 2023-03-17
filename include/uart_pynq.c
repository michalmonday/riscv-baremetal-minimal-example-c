#include "uart_pynq.h"

int uart_pynq_putchar(char ch) {
   uart_pynq[UART_REG_TXFIFO] = ch;
}

int uart_pynq_data_available() {
   return uart_pynq[UART_REG_LSR] & LSR_BITFIELD_DATA_READY;
}

int uart_pynq_getchar() {
   while (!uart_pynq_data_available());
   return uart_pynq[UART_REG_RXFIFO] & 0xFF;
}

void uart_pynq_puts(char *s) {
   while (*s) {
       uart_pynq_putchar(*s++);
   }
   uart_pynq_putchar('\n');
}
