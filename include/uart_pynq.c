#include "uart_pynq.h"

// #include "standard_libraries.h"

#include <stdarg.h>

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

void uart_pynq_printf(char *fmt, ...) {
   char buf[1024];
   va_list args;
   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);
   // uart_pynq_puts(buf);
   char *s = buf;
   while (*s) {
       uart_pynq_putchar(*s++);
   }
}