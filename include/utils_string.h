#ifndef UTILS_STRING_H
#define UTILS_STRING_H

extern int strcmp(char *s1, char *s2);
// extern int sprintf(char *s, char *format, ...);
extern void itoa(int i, char* str, int radix);
extern int atoi(char *s, int radix);
extern int strlen(char *s);
extern void strcpy(char *dest, char *src);

#endif