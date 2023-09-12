#include <stdio.h>
#include <stdlib.h>

void __libc_init_array(void);

void init() {
    __asm__ volatile  (
        ".option push;"
        // The 'norelax' option is critical here.
        // Without 'norelax' the global pointer will
        // be loaded relative to the global pointer!
         ".option norelax;"
        "la    gp, __global_pointer$;"
        ".option pop;"
    );

	extern char bss_start asm("BSS_START");
    extern char bss_end asm("BSS_END");
    extern char sbss_start asm("SBSS_START");
    extern char sbss_end asm("SBSS_END");

    char *bss = &bss_start;
    char *sbss = &sbss_start;

    for (; bss < &bss_end; bss++)
        *bss = 0;
    for (; sbss < &sbss_end; sbss++) 
        *sbss = 0;
}

int main(void) {
    init();
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
    putchar('a');
    putchar('a');
    uart_pynq_putchar('b');
    uart_pynq_putchar('b');
    uart_pynq_puts("Hello, world!\n");

    uart_pynq_puts("1\n");
    malloc(10000);
    uart_pynq_puts("2\n");
    malloc(10000);
    uart_pynq_puts("3\n");

    printf("printf - ");
    free(malloc(1024));
    printf("after_malloc - ");

    while (1) {
        asm("nop");
    }
    return 0;
}