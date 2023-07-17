#ifndef UART_PYNQ_H
#define UART_PYNQ_H

////////////////////////////////////////////////////////////////////////////////////////////////
// definitions for Soc_Top UART model (ns16550), allowing to communicate with PYNQ through DMA
static volatile int *uart_pynq = (int *)(void *)0xC0000000;
// It seems that receive and transmit addresses of console are the same
// in Flute which distinguishes them at hardware level.
#define UART_REG_TXFIFO 0x0
#define UART_REG_RXFIFO 0x0
#define UART_REG_LSR    0x5 // line status reg (read only)

// Bit fields of LSR
#define LSR_BITFIELD_TRANSMITTER_EMPTY 0x40
#define LSR_BITFIELD_THR_EMPTY         0x20
#define LSR_BITFIELD_DATA_READY        0x1


extern int uart_pynq_putchar(char ch);
extern int uart_pynq_data_available();
extern int uart_pynq_getchar();
extern void uart_pynq_puts(char *s);
extern void uart_pynq_printf(char *fmt, ...);

#endif