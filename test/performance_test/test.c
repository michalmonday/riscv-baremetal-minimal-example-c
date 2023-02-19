
static volatile int *uart = (int *)(void *)0xC0000000;
//static volatile int *uart = (int *)(void *)0x62300000;
static volatile int *some_peripheral = (int *)(void *)0xC0003000;

// It seems that receive and transmit addresses of console are the same
// in Flute which distinguishes them at hardware level.
#define UART_REG_TXFIFO 0x0
#define UART_REG_RXFIFO 0x0
#define UART_REG_LSR    0x5 // line status reg (read only)

// Bit fields of LSR
#define LSR_BITFIELD_TRANSMITTER_EMPTY 0x40
#define LSR_BITFIELD_THR_EMPTY         0x20
#define LSR_BITFIELD_DATA_READY        0x1

int function_a(char a) {
    return a * a;
}

static int putchar2(char ch) {
    uart[UART_REG_TXFIFO] = ch;
}

static int data_available() {
    return uart[UART_REG_LSR] & LSR_BITFIELD_DATA_READY;
}

static int getchar2() {
    while (!data_available());
    return uart[UART_REG_RXFIFO] & 0xFF;
}

// int main(int argc) {
//     const char *s = "New program!\n";
//     while (*s)
//         putchar(*s++);

//     asm("wfi");
//     return 0;
// }

void puts2(char *s) {
    while (*s) {
        putchar2(*s++);
    }
    putchar2('\n');
}

int main() {
    int sum = 0;
    int iterations = 0;
    char c;

    // some_peripheral[0] = 0x12345678;
    // int result = some_peripheral[0];


    // receive the number of function calls through console input
    while ((c = getchar2()) != '\n') {
        iterations = iterations * 10 + (c - '0'); 
    }

    // run specified number of function calls
    for (int i = 0; i < iterations; i++) {
        sum += function_a(i);
    }
    asm ("wfi");
    return 0;
}