#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Hello, world!\n");
    free(malloc(1024));
    return 0;
}