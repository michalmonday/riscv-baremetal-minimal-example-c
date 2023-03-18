#include "utils_flute.h"

unsigned long get_ticks_count() {
    return utils_rom[UTILS_ROM_TICKS_COUNT_INDEX];
}
unsigned long get_overlay_ticks_count() {
    return utils_rom[UTILS_ROM_OVERLAY_TICKS_COUNT_INDEX];
}
unsigned long get_random_number() {
    return utils_rom[UTILS_ROM_RANDOM_NUMBER_INDEX];
}

void wait_ms(unsigned long ms) {
    // // 12 is arbitrary number to roughly scale it into correct value
    // // the problem is that this nested loop doesn't really take 1 cycle (but longer) for each iteration of the inner loop
    // // it was observed that without "/ 12" it took around 60ms when "wait_ms(5)" was used, hence the "/ 12"
    // static unsigned long cycles_per_ms = CLK_SPEED / 1000 / 12; 
    // for (unsigned long i = 0; i < ms; i++) {
    //     for (unsigned long j = 0; j < cycles_per_ms; j++) {
    //         asm volatile ("nop");
    //     }
    // }

    unsigned long tick_count = get_ticks_count();
    unsigned long cycles_per_ms = CLK_SPEED / 1000;
    unsigned long cycles_to_wait = ms * cycles_per_ms;
    while ((get_ticks_count() - tick_count) < cycles_to_wait) {
        // do nothing
    }
}

void wait_s(unsigned long s) {
    for (unsigned long i = 0; i < s; i++) 
        wait_ms(1000);
}