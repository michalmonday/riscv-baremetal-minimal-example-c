#include <eembc_adaptation.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


void DebugOut(char *str) { printf("DebugOut: %s\n", str); }
// void WriteOut(varsize val) { printf("WriteOut: %d\n", val); }
// void DebugOut(int val) { printf("DebugOut: %d\n", val); }
void WriteOut(int val) { 
    // printf("WriteOut: %d\n", val); 
}
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
}


char *parse_args_from_stdin_csv(int *argc, char *argv[]) {
    // returns pointer to memory allocated with malloc
    // it could be free'd by the caller but not before argv is fully used
    // freeing it and attempting to use argv later will cause exception
    // or other issues
    char *inp = (char*)malloc(MAX_PROGRAM_ARGS_STDIN_SIZE);
    printf("inp = %p\n", inp);
    if (!inp) {
        th_exit("ERROR: input malloc failed\nStopping execution\n");
        asm volatile ("wfi");
    }
    // char *res = gets_s(inp, MAX_PROGRAM_ARGS_STDIN_SIZE-1);
    char *res = fgets(inp, MAX_PROGRAM_ARGS_STDIN_SIZE-1, stdin);
    if (!res) {
        printf("ERROR: stdin reading fail\n");
        printf("inp = %s ferror = %d feof = %d\n", inp, ferror(stdin), feof(stdin));
        printf("stopping execution\n");
        asm volatile ("wfi");
    }
    printf("Received inp string:");
    puts(inp);
    *argc = 0;
    char *token = strtok(inp, ",");
    while (token != NULL) {
        argv[*argc] = token;
        *argc += 1;
        token = strtok(NULL, ",");
        printf("argc = %d new arg = %s\n", *argc, argv[*argc-1]);
        if (*argc > MAX_PROGRAM_ARGS) {
            th_exit("ERROR: too many arguments, max is %d\nstoppin execution", MAX_PROGRAM_ARGS);
            asm volatile ("wfi");
        }
    }
    return inp;
}


