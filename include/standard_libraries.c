
#include "standard_libraries.h"

unsigned long strlen(char* str) {
    unsigned long len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void strcpy(char* dest, char* src) {
    unsigned long i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void vsprintf(char *buf, char *fmt, va_list args) {
   char *p;
   for (p = buf; *fmt; fmt++) {
       if (*fmt != '%') {
           *p++ = *fmt;
           continue;
       }
       fmt++;
       switch (*fmt) {
           case 'c':
               *p++ = va_arg(args, char);
               break;
           case 'd':
               *p++ = va_arg(args, int);
               break;
           case 'x':
               *p++ = va_arg(args, int);
               break;
           case 's':
               strcpy(p, va_arg(args, char *));
               p += strlen(p);
               break;
           default:
               break;
       }
   }
   *p = '\0';
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

int strcmp(char *s1, char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        }
        s1++;
        s2++;
    }
    if (*s1) {
        return 1;
    }
    if (*s2) {
        return -1;
    }
    return 0;
}