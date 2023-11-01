
#ifdef RANDOM_FUNCTION_GENERATOR

#include <stdio.h>

extern int a2time(int argc, char *argv[]);

int main() {
    puts("main start");
    char *argv[] = {"a2time", "test", "test", "test"};
    a2time(0, argv);
    puts("main end");
    asm volatile("wfi");
    return 0;
}


#endif