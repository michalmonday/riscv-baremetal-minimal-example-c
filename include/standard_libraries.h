#ifndef STANDARD_LIBRARIES_H
#define STANDARD_LIBRARIES_H

#include <stdarg.h>

extern unsigned long strlen(char* str);
extern void strcpy(char* dest, char* src);
extern void vsprintf(char *buf, char *fmt, va_list args);
extern void itoa(int i, char* str, int radix);
extern int atoi(char *s, int radix);
extern int strcmp(char *s1, char *s2);


#endif