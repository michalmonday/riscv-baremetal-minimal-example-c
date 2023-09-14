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

    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    // uart_pynq_getchar();
    // getchar();
    char *s = malloc(50);
    gets(s);



    printf("test %c\n", 'a');
    printf("test %c", 'b');
    // printf("test %c\n", 'c');
    // getchar();
    putchar('a');
    putchar('a');
    // putchar('\n');
    uart_pynq_putchar('b');
    uart_pynq_putchar('b');
    uart_pynq_puts("Hello, world!\n");


    uart_pynq_printf("1\n");
    malloc(10000);
    uart_pynq_printf("222222222222222");
    malloc(10000);
    uart_pynq_printf("3\n");

    printf("printf - %d\n", 0x88);
    free(malloc(1024));
    printf("after_malloc - \n");

    while (1) {
        asm("nop");
    }
    return 0;
}