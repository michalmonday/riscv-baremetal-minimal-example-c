#include <eembc_adaptation.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


void DebugOut(char *str) { printf("DebugOut: %s\n", str); }
// void WriteOut(varsize val) { printf("WriteOut: %d\n", val); }
// void DebugOut(int val) { printf("DebugOut: %d\n", val); }
void WriteOut(int val) { printf("WriteOut: %d\n", val); }
void *th_malloc(size_t size) { return malloc(size); }
void th_free(void *ptr) { free(ptr); }

int th_sprintf(char *str, const char *format, ...) {
    va_list args;
    int ret;

    va_start(args, format);
    ret = vsprintf(str, format, args);
    va_end(args);

    return ret;
}

void th_exit(const char *format, ...) {
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    asm("wfi");
    while(1);
}



