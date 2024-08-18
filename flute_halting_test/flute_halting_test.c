
#include <stdio.h>
#include <stdlib.h>

void main(void) {
    char msg[64] = {0};
    while (1) {
        gets(msg);
        puts(msg);
    }
}