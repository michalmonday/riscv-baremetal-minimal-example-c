#include <stdio.h>
#include <stdlib.h>

int main(void) {
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