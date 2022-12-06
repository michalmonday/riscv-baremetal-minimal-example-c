//#include "uart_lite.h"
//#include "uart_16550.h"
#define UART_REG_TXFIFO 0x0
#define UART_REG_RXFIFO 0x0
#define UART_REG_IER 0x1 // interrupt enable register
#define UART_REG_FCR 0x2 // fifo control register
#define UART_REG_LCR  0x3   // line control reg
#define UART_REG_LSR    0x5 // line status reg (read only)

// Bit fields of LSR
#define LSR_BITFIELD_TRANSMITTER_EMPTY 0x40
#define LSR_BITFIELD_THR_EMPTY         0x20
#define LSR_BITFIELD_DATA_READY        0x1

#define  ADDR_UART_DLL 0    // divisor latch low
#define  ADDR_UART_DLM 1    // divisor latch high


#define CPU_CLK_SPEED 50000000
#define BAUD_RATE 115200

static volatile int *uart = (int *)(void *)0x62300000;

// set uart16550 baud rate
void set_baudrate(int clk_speed, int baudrate)
{
    int divisor = (int)(clk_speed / (16 * baudrate));
    uart[UART_REG_LCR] = 0x80; // set DLAB
    uart[ADDR_UART_DLL] = divisor & 0xff;
    uart[ADDR_UART_DLM] = (divisor >> 8) & 0xff;
    //uart[UART_REG_IER] = 0x1 & 0xff;
    uart[UART_REG_LCR] = 0x03; // clear DLAB
}

// enable FIFOs
void enable_fifos()
{
    // no dma
    //uart[UART_REG_FCR] = 0b111;

    // dma (enabling RXRDYn and TXRDYn outputs indicating FIFOs empty/non-empty)
    uart[UART_REG_FCR] = 0b1111;
}

int putchar(char ch) {
    while ((uart[UART_REG_LSR] & LSR_BITFIELD_THR_EMPTY) == 0);
    uart[UART_REG_TXFIFO] = ch;
}

int data_available() {
    return uart[UART_REG_LSR] & LSR_BITFIELD_DATA_READY;
}

int getchar() {
    while (!data_available());
    return uart[UART_REG_RXFIFO] & 0xFF;
}

void puts(char *s) {
    while (*s)
        putchar(*s++);
    putchar('\n');
}

int main(int argc) {
    enable_fifos();
    set_baudrate(CPU_CLK_SPEED, BAUD_RATE);

    const char *s = "Hello Sword!\n";
    while (*s)
        putchar(*s++);

    char typed_str[9] = "typed:  ";
    while (1) {
        char c = getchar();
        typed_str[7] = c;
        puts(typed_str);
    }
}


