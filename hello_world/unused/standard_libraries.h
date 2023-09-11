#ifndef STANDARD_LIBRARIES_H
#define STANDARD_LIBRARIES_H

#include <stdarg.h>

extern unsigned long strlen(char* str);
extern void strcpy(char* dest, char* src);
extern void vsprintf(char *buf, char *fmt, va_list args);
extern void itoa(int i, char* str, int radix);
extern int atoi(char *s);
extern int atoi_radix(char *s, int radix);
extern int strcmp(char *s1, char *s2);
extern void memcpy(char *dest, char *src, int size);
extern void memset(char *dest, char val, int size);
extern void swap(void *a, void *b, int size);
extern void qsort(void *base, int n, int size, int (*comparator)(const void *, const void *));

#endif