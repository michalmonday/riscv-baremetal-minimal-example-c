// This file contains modified code from:
// https://github.com/CTSRD-CHERI/cheri-exercises/blob/master/src/missions/uninitialized-stack-frame-control-flow/stack-mission.c

/*
 * SPDX-License-Identifier: BSD-2-Clause-DARPA-SSITH-ECATS-HR0011-18-C-0016
 * Copyright (c) 2020 SRI International
 */

#include <assert.h>
#include <ctype.h>
#include <stdalign.h>
#include <stddef.h>
// #include <stdio.h>
#include <stdlib.h>

#define EOF (-1)

// At the end of this file, there is a copy of Flute UART offsets/registers
// definitions. Noteworthy is the fact that "addr_UART_ier" being "1" means
// refers to address value already shifted 2 bits to right (because of
// address alignment to multiples of 4)
//
// Using information from: Flute/src_Testbench/UART_Model.bsv
static volatile int *uart = (int *)(void *)0xC0000000;
//static volatile int *uart = (int *)(void *)0x62300000;

// It seems that receive and transmit addresses of console are the same
// in Flute which distinguishes them at hardware level.
#define UART_REG_TXFIFO 0x0
#define UART_REG_RXFIFO 0x0
#define UART_REG_LSR    0x5 // line status reg (read only)

// Bit fields of LSR
#define LSR_BITFIELD_TRANSMITTER_EMPTY 0x40
#define LSR_BITFIELD_THR_EMPTY         0x20
#define LSR_BITFIELD_DATA_READY        0x1


static int putchar(char ch) {
    uart[UART_REG_TXFIFO] = ch;
}

static int data_available() {
    return uart[UART_REG_LSR] & LSR_BITFIELD_DATA_READY;
}

static int getchar() {
    while (!data_available());
    return uart[UART_REG_RXFIFO] & 0xFF;
}

// int main(int argc) {
//     const char *s = "New program!\n";
//     while (*s)
//         putchar(*s++);

//     asm("wfi");
//     return 0;
// }

void puts(char *s) {
    while (*s) {
        putchar(*s++);
    }
    putchar('\n');
}

void
success(void)
{
    puts("Exploit successful, yum!");
}

void
no_cookies(void)
{
	puts("No cookies??");
}

#pragma weak init_pointer
void
init_pointer(void *p)
{
}

static void __attribute__((noinline))
init_cookie_pointer(void)
{
	void *pointers[12];
	void (* volatile cookie_fn)(void);

	for (size_t i = 0; i < sizeof(pointers) / sizeof(pointers[0]); i++)
		init_pointer(&pointers[i]);
	cookie_fn = no_cookies;
}

static void __attribute__((noinline))
get_cookies(void)
{
	alignas(void *) char cookies[sizeof(void *) * 32];
	char *cookiep;
	int ch, cookie;

	puts("Cookie monster is hungry, provide some cookies!");
	char msg[100];
	sprintf(msg, "'=' skips the next %zu bytes", sizeof(void *));
	puts(msg);
	puts("'-' skips to the next character");
	puts("XX as two hex digits stores a single cookie");
	puts("> ");

	cookiep = cookies;
	for (;;) {
		ch = getchar();

		if (ch == '\n' || ch == EOF)
			break;

		if (isspace(ch))
			continue;

		if (ch == '-') {
			cookiep++;
			continue;
		}

		if (ch == '=') {
			cookiep += sizeof(void *);
			continue;
		}

		if (isxdigit(ch)) {
			cookie = digittoint(ch) << 4;
			ch = getchar();
			if (ch == EOF) {
				puts("Half-eaten cookie, yuck!");
				asm("wfi");
			}
			if (!isxdigit(ch)) {
				puts("Malformed cookie");
				asm("wfi");
			}
			cookie |= digittoint(ch);
			*cookiep++ = cookie;
			continue;
		}
		puts("Malformed cookie");
		asm("wfi");
	}
}

static void __attribute__((noinline))
eat_cookies(void)
{
	void *pointers[12];
	void (* volatile cookie_fn)(void);

	for (size_t i = 0; i < sizeof(pointers) / sizeof(pointers[0]); i++)
		init_pointer(&pointers[i]);
	cookie_fn();
}

int
main(void)
{
	init_cookie_pointer();
	get_cookies();
	eat_cookies();

    asm("wfi");
	return (0);
}

