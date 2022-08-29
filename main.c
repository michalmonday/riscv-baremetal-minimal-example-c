// At the end of this file, there is a copy of Flute UART offsets/registers
// definitions. Noteworthy is the fact that "addr_UART_ier" being "1" means
// refers to address value already shifted 2 bits to right (because of
// address alignment to multiples of 4)
//
// Using information from: Flute/src_Testbench/UART_Model.bsv
static volatile int *uart = (int *)(void *)0xC0000000;

// It seems that receive and transmit addresses of console are the same
// in Flute which distinguishes them at hardware level.
#define UART_REG_TXFIFO 0x0
#define UART_REG_RXFIFO 0x0
#define UART_REG_LSR    0x5 // line status reg (read only)

// Bit fields of LSR
#define LSR_BITFIELD_TRANSMITTER_EMPTY 0x40
#define LSR_BITFIELD_THR_EMPTY         0x20
#define LSR_BITFIELD_DATA_READY        0x1


static int putchar(char ch) {
    uart[UART_REG_TXFIFO] = ch;
}

static int data_available() {
    return uart[UART_REG_LSR] & LSR_BITFIELD_DATA_READY;
}

static int getchar() {
    while (!data_available());
    return uart[UART_REG_RXFIFO] & 0xFF;
}

int main(void) {
    const char *s = "Hello world!\n";
    while (*s)
        putchar(*s++);

    char c = getchar();
    putchar(c);

    while (1);
}


/*
// ================================================================
// UART registers and their address offsets

Bit #(3)  addr_UART_rbr  = 3'h_0;    // receiver buffer register (read only)
Bit #(3)  addr_UART_thr  = 3'h_0;    // transmitter holding register (write only)
Bit #(3)  addr_UART_ier  = 3'h_1;    // interrupt enable register
Bit #(3)  addr_UART_iir  = 3'h_2;    // interrupt id register    (read-only)
Bit #(3)  addr_UART_lcr  = 3'h_3;    // line control reg
Bit #(3)  addr_UART_mcr  = 3'h_4;    // modem control reg
Bit #(3)  addr_UART_lsr  = 3'h_5;    // line status reg     (read-only)
Bit #(3)  addr_UART_msr  = 3'h_6;    // modem status reg    (read-only)
Bit #(3)  addr_UART_scr  = 3'h_7;    // scratch pad reg

// Aliased registers, depending on control bits
Bit #(3)  addr_UART_dll  = 3'h_0;    // divisor latch low
Bit #(3)  addr_UART_dlm  = 3'h_1;    // divisor latch high
Bit #(3)  addr_UART_fcr  = 3'h_2;    // fifo control reg    (write-only)

// Bit fields of ier (Interrupt Enable Register)
Bit #(8)  uart_ier_erbfi = 8'h_01;     // Enable Received Data Available Interrupt
Bit #(8)  uart_ier_etbei = 8'h_02;     // Enable Transmitter Holding Register Empty Interrupt
Bit #(8)  uart_ier_elsi  = 8'h_04;     // Enable Receiver Line Status Interrupt
Bit #(8)  uart_ier_edssi = 8'h_08;     // Enable Modem Status Interrupt

// iir values (Interrupt Identification Register) in decreasing priority of interrupts
Bit #(8)  uart_iir_none  = 8'h_01;     // None (no interrupts pending)
Bit #(8)  uart_iir_rls   = 8'h_06;     // Receiver Line Status
Bit #(8)  uart_iir_rda   = 8'h_04;     // Received Data Available
Bit #(8)  uart_iir_cti   = 8'h_0C;     // Character Timeout Indication
Bit #(8)  uart_iir_thre  = 8'h_02;     // Transmitter Holding Register Empty
Bit #(8)  uart_iir_ms    = 8'h_00;     // Modem Status

// Bit fields of LCR
Bit #(8)  uart_lcr_dlab  = 8'h_80;     // Divisor latch access bit
Bit #(8)  uart_lcr_bc    = 8'h_40;     // Break control
Bit #(8)  uart_lcr_sp    = 8'h_20;     // Stick parity
Bit #(8)  uart_lcr_eps   = 8'h_10;     // Even parity
Bit #(8)  uart_lcr_pen   = 8'h_08;     // Parity enable
Bit #(8)  uart_lcr_stb   = 8'h_04;     // # of stop bits (0=1b,1=2b)
Bit #(8)  uart_lcr_wls   = 8'h_03;     // word len (0:5b,1:6b,2:7b,3:8b)

// Bit fields of LSR
Bit #(8)  uart_lsr_rxfe  = 8'h_80;    // Receiver FIFO error
Bit #(8)  uart_lsr_temt  = 8'h_40;    // Transmitter empty
Bit #(8)  uart_lsr_thre  = 8'h_20;    // THR empty
Bit #(8)  uart_lsr_bi    = 8'h_10;    // Break interrupt
Bit #(8)  uart_lsr_fe    = 8'h_08;    // Framing Error
Bit #(8)  uart_lsr_pe    = 8'h_04;    // Parity Error
Bit #(8)  uart_lsr_oe    = 8'h_02;    // Overrun Error
Bit #(8)  uart_lsr_dr    = 8'h_01;    // Data Ready
*/
