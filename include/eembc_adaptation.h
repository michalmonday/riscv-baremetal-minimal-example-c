#ifndef EEMBC_ADAPTATION_H
#define EEMBC_ADAPTATION_H

#include <stddef.h>

#define varsize int
#define n_ulong unsigned long
#define n_long long
#define n_int int
#define n_char char

extern void DebugOut(char *str);
// extern void WriteOut(char *str);
// extern void DebugOut(int val);
extern void WriteOut(int val);
extern void *th_malloc(size_t size);
extern void th_free(void *ptr);
extern int th_sprintf(char *str, const char *format, ...);
extern void th_exit(const char *format, ...);




#endif /* EEMBC_ADAPTATION_H */