// _start:
//     csrr    t0, mhartid             # read current hart id
//     bnez    t0, park                # single core only, park hart != 0
//     la      sp, STACK_TOP           # setup stack
//     j       main                    # jump to c
//     j       park

// park:
//     wfi
//     j       park

#include <stdio.h>
void __libc_init_array(void);
void main();
__attribute__((section(".text_past"))) void init();

__attribute__((naked)) int _start(void) {
    // t0
    //extern register int t0 asm("t0");
    asm volatile (
        "csrr t0, mhartid;"
        "bnez t0, park;"
        "la sp, STACK_TOP;"
        ".option push;"
        // The 'norelax' option is critical here.
        // Without 'norelax' the global pointer will
        // be loaded relative to the global pointer!
         ".option norelax;"
        "la    gp, __global_pointer$;"
        ".option pop;"
    );
    init();
    __libc_init_array();
    // without 2 lines below, printf worked only if it had "\n" character
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    main();
    park();
}

void init() {
	extern char bss_start asm("BSS_START");
    extern char bss_end asm("BSS_END");
    extern char sbss_start asm("SBSS_START");
    extern char sbss_end asm("SBSS_END");

    char *bss = &bss_start;
    char *sbss = &sbss_start;

    for (; bss < &bss_end; bss++)
        *bss = 0;
    for (; sbss < &sbss_end; sbss++) 
        *sbss = 0;
}

void park() {
    asm volatile ("wfi");
}

