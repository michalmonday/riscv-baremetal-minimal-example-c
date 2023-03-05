
static volatile int *uart = (int *)(void *)0xC0000000;
//static volatile int *uart = (int *)(void *)0x62300000;
static volatile long *sensors = (long *)(long *)0xC0003000;

// It seems that receive and transmit addresses of console are the same
// in Flute which distinguishes them at hardware level.
#define UART_REG_TXFIFO 0x0
#define UART_REG_RXFIFO 0x0
#define UART_REG_LSR    0x5 // line status reg (read only)

// Bit fields of LSR
#define LSR_BITFIELD_TRANSMITTER_EMPTY 0x40
#define LSR_BITFIELD_THR_EMPTY         0x20
#define LSR_BITFIELD_DATA_READY        0x1

#define CLK_SPEED 50000000 // 50MHz

void wait_ms(unsigned long ms) {
    static unsigned long cycles_per_ms = CLK_SPEED / 1000;
    for (unsigned long i = 0; i < ms; i++) {
        for (unsigned long j = 0; j < cycles_per_ms; j++) {
            asm volatile ("nop");
        }
    }
}

void wait_s(unsigned long s) {
    for (unsigned long i = 0; i < s; i++) 
        wait_ms(1000);
}

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

void int_to_str(int num, char *str) {
    int i = 0;
    while (num > 0) {
        str[i++] = num % 10 + '0';
        num /= 10;
    }
}

int strlen2(char *s) {
    int len = 0;
    while (*s++) 
        len++;
    return len;
}

void strcpy2(char *dest, char *src) {
    while (*src) {
        *dest++ = *src++;
    }
}

int main() {
    int sum = 0;
    int iterations = 0;
    char c;

    // receive the number of function calls through console input
    while ((c = getchar2()) != '\n') {
        iterations = iterations * 10 + (c - '0'); 
        char str[100] = {0};
        char *str_p = str;

        for (int j=0; j < 10; j++) {
            for (int i = 0; i < 16; i++) {
                int result = sensors[i];
                int_to_str(result, str);
                puts2(str);
                char *temp_str = str;
                while (*temp_str++)
                    *temp_str = 0;
            }
            wait_ms(1);
            // puts2(str);
        }
    }

    asm ("wfi");
    return 0;
}