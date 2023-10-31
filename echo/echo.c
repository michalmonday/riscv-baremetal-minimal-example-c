#include <stdio.h>
#include <stdlib.h>

int main(void) {
    while (1) {
        putchar(getchar());
    } 
    asm volatile ("wfi");
    return 0;
}