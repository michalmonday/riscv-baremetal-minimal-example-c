#include "utils_string.h"

// Functions in this file were generated mostly by GitHub Copilot

int strcmp(char *s1, char *s2);
int sprintf(char *s, char *format, ...);
void itoa(int i, char* str, int radix);
int atoi(char *s, int radix);
int strlen(char *s);
void strcpy(char *dest, char *src);

int strcmp(char *s1, char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) {
            return 0;
        }
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

// int sprintf(char *s, char *format, ...) {
//     va_list args;
//     va_start(args, format);
//     int i = 0;
//     while (*format) {
//         if (*format == '%') {
//             format++;
//             if (*format == 'd') {
//                 int num = va_arg(args, int);
//                 char str[100] = {0};
//                 itoa(num, str, 10);
//                 strcpy(s + i, str);
//                 i += strlen(str);
//             }
//             else if (*format == 's') {
//                 char *str = va_arg(args, char *);
//                 strcpy(s + i, str);
//                 i += strlen(str);
//             } else if (*format == 'c') {
//                 char ch = va_arg(args, int);
//                 s[i++] = ch;
//             } else if (*format == 'x') {
//                 int num = va_arg(args, int);
//                 char str[100] = {0};
//                 itoa(num, str, 16);
//                 strcpy(s + i, str);
//                 i += strlen(str);
//             } else if (*format == '%') {
//                 s[i++] = '%';
//             } else if (*format == 'f') {
//                 float num = va_arg(args, double);
//                 char str[100] = {0};
//                 float_to_str(num, str);
//                 strcpy(s + i, str);
//                 i += strlen(str);
//             } else {
//                 s[i++] = *format;
//             }
//         }
//         else {
//             s[i++] = *format;
//         }
//         format++;
//     }
//     va_end(args);
//     s[i] = '\0';
//     return i;
// }

void itoa(int i, char* str, int radix) {
    char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned unum;
    int j = 0, k, temp;
    if (radix == 10 && i < 0) {
        unum = (unsigned)-i;
        str[j++] = '-';
    } else {
        unum = (unsigned)i;
    }
    do {
        str[j++] = index[unum % (unsigned)radix];
        unum /= radix;
    } while (unum);
    str[j] = '\0';
    if (str[0] == '-') {
        k = 1;
    } else {
        k = 0;
    }
    char temp_str[100] = {0};
    strcpy(temp_str, str);
    for (int i = k; i < j; i++) {
        str[i] = temp_str[j - i - 1];
    }
}

int atoi(char *s, int radix) {
    int i = 0;
    int sign = 1;
    if (*s == '-') {
        sign = -1;
        s++;
    }
    while (*s) {
        i = i * radix + *s - '0';
        s++;
    }
    return i * sign;
}

int strlen(char *s) {
    int len = 0;
    while (*s++) 
        len++;
    return len;
}

void strcpy(char *dest, char *src) {
    while (*src) {
        *dest++ = *src++;
    }
}