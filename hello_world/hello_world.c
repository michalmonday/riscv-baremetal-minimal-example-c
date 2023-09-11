#include <stdio.h>
#include <stdlib.h>

void __libc_init_array(void);

void init_bss() {
	extern char bss_start asm("BSS_START");
    extern char bss_end asm("BSS_END");
    extern char sbss_start asm("SBSS_START");
    extern char sbss_end asm("SBSS_END");

    char *bss = &bss_start;
    char *sbss = &sbss_start;

    for (; bss < bss_end; bss++)
        *bss = 0;
    for (; sbss < sbss_end; sbss++) 
        *sbss = 0;
}

int main(void) {
    init_bss();
    __libc_init_array();

    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    malloc(4);
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    uart_pynq_getchar();
    // getchar();
    // putchar('a');
    // putchar('a');
    uart_pynq_putchar('b');
    uart_pynq_putchar('b');
    uart_pynq_puts("Hello, world!\n");

    printf("printf - ");
    free(malloc(1024));
    printf("after_malloc - ");

    while (0) {
        asm("nop");
    }
    return 0;
}