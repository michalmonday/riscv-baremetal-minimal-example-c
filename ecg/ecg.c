
// Location of a small block memory dedicated for sensors (that constantly gets updated by XADC wizard,
// receiving data from up to 16 sensors using multiplexer, one of the sensors is ECG sensor)
static volatile long *sensors = (long *)(long *)0xC0003000;
#define ECG_SENSOR_INDEX 7

// The same small block memory is used for digital inputs (provided from 2 SN74HC165 cascaded shift registers)
static volatile long *digital_inputs = (long *)(long *)(0xC0003000 + (sizeof(long) * 16));

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

#define CLK_SPEED 50000000 // 50MHz

void wait_ms(unsigned long ms) {
    // 12 is arbitrary number to roughly scale it into correct value
    // the problem is that this nested loop doesn't really take 1 cycle (but longer) for each iteration of the inner loop
    // it was observed that without "/ 12" it took around 60ms when "wait_ms(5)" was used, hence the "/ 12"
    static unsigned long cycles_per_ms = CLK_SPEED / 1000 / 12; 
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

//static int putchar2(char ch) {
//    uart_pynq[UART_REG_TXFIFO] = ch;
//}
//
//static int data_available() {
//    return uart_pynq[UART_REG_LSR] & LSR_BITFIELD_DATA_READY;
//}
//
//static int getchar2() {
//    while (!data_available());
//    return uart_pynq[UART_REG_RXFIFO] & 0xFF;
//}
//
//void puts2(char *s) {
//    while (*s) {
//        putchar2(*s++);
//    }
//    putchar2('\n');
//}
//
void int_to_str(int num, char *str) {
   int i = 0;
   while (num > 0) {
       str[i++] = num % 10 + '0';
       num /= 10;
   }
}
//
//int strlen2(char *s) {
//    int len = 0;
//    while (*s++) 
//        len++;
//    return len;
//}
//
//void strcpy2(char *dest, char *src) {
//    while (*src) {
//        *dest++ = *src++;
//    }
//}

int uart_gpio_data_available() {
    return uart_gpio[UART_GPIO_STATUS_INDEX] & (1 << UART_GPIO_STATUS_RX_VALID_DATA_BIT);
}

int uart_gpio_can_send() {
    return uart_gpio[UART_GPIO_STATUS_INDEX] & (1 << UART_GPIO_STATUS_TX_EMPTY_BIT);
}
// int uart_gpio_can_send() {
//     return !(uart_gpio[UART_GPIO_STATUS_INDEX] & (1 << UART_GPIO_STATUS_TX_FULL_BIT));
// }

void puts_gpio(char *s) {
    while (*s) {
        while (!uart_gpio_can_send());
        uart_gpio[UART_GPIO_TX_INDEX] = *s++;
    }
    while (!uart_gpio_can_send());
    uart_gpio[UART_GPIO_TX_INDEX] = '\n';
}


int main() {
    int sum = 0;
    int iterations = 0;
    char c;

    // while ((c = getchar2()) != '\n') {
    while(1) {
        int leads_off = digital_inputs[0] + digital_inputs[1];
        if (leads_off) {
            puts_gpio("Leads off");
            wait_ms(1000);
            continue;
        }
        char str[100] = {0};
        int ecg_reading = sensors[ECG_SENSOR_INDEX];
        int_to_str(ecg_reading, str);
        // puts2(str);
        puts_gpio(str);
        char *temp_str = str;
        while (*temp_str++)
            *temp_str = 0;
        wait_ms(8);
    }

    asm ("wfi");
    return 0;
}