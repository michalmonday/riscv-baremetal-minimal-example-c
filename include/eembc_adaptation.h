#ifndef EEMBC_ADAPTATION_H
#define EEMBC_ADAPTATION_H

#include <stddef.h>

#define varsize int
#define n_ulong unsigned long
#define n_long long
#define n_int int
#define n_char char
#define n_uchar unsigned char
#define n_float float

// arguments are passed as a csv string through stdin
#define MAX_PROGRAM_ARGS 300
#define MAX_PROGRAM_ARGS_STDIN_SIZE 600

extern char *parse_args_from_stdin_csv(int *argc, char *argv[]);

extern void DebugOut(char *str);
// extern void WriteOut(char *str);
// extern void DebugOut(int val);
extern void WriteOut(int val);
extern void *th_malloc(size_t size);
extern void th_free(void *ptr);
extern int th_sprintf(char *str, const char *format, ...);
extern void th_exit(const char *format, ...);




#endif /* EEMBC_ADAPTATION_H */