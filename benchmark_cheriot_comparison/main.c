
#include <stdio.h>
#include <string.h>
#include <eembc_adaptation.h>

extern int a2time(int argc, char *argv[]);
extern int bitmnp(int argc, char *argv[]);
extern int idctrn(int argc, char *argv[]);
extern int puwmod(int argc, char *argv[]);
extern int rspeed(int argc, char *argv[]);
extern int tblook(int argc, char *argv[]);
extern int ttsprk(int argc, char *argv[]);

#define INPUT_INDEX "0"
#define ITERATIONS_EACH "2"
#define ITERATIONS_ALL 10

void main(void) {
    char *fake_argv[] = {INPUT_INDEX, ITERATIONS_EACH};
    for (int i = 0; i < ITERATIONS_ALL; i++) {
        a2time(0, fake_argv);
        bitmnp(0, fake_argv);
        idctrn(0, fake_argv);
        puwmod(0, fake_argv);
        rspeed(0, fake_argv);
        tblook(0, fake_argv);
        ttsprk(0, fake_argv);
    }
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
}