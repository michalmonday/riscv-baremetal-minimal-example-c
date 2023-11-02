
#include <stdio.h>
#include <eembc_adaptation.h>

#ifdef SINGLE_ALGORITHM
extern int PROG_NAME(int argc, char *argv[]);
#endif 

void main(void) {
    
    // each argument is supplied as csv string in stdin
    // all arguments are input indices, allowing to run the same test multiple times
    char *argv[MAX_PROGRAM_ARGS]; 
    int argc = 0;
    // tokenized_inp_str may be deallocated after argv's are not needed anymore
    char *tokenized_inp_str = parse_args_from_stdin_csv(&argc, argv);
    if (argc < 1) 
        th_exit("ERROR: missing input index argument.\nStopping execution.\n");

#ifdef SINGLE_ALGORITHM
    PROG_NAME(argc, argv);
#endif
}


// extern int a2time(int argc, char *argv[]);
// //extern int aifftr(int argc, char *argv[]);
// // extern int bitmnp(int argc, char *argv[]);
// // extern int idctrn(int argc, char *argv[]);
// // extern int puwmod(int argc, char *argv[]);
// // extern int rspeed(int argc, char *argv[]);
// // extern int tblook(int argc, char *argv[]);
// // extern int ttsprk(int argc, char *argv[]);
// 
// void main() {
//     puts("main start");
//     char *argv[] = {"a2time", "test", "test", "test"};
// 
//     a2time(0, argv);
//     // bitmnp(0, argv);
//     // idctrn(0, argv);
//     // puwmod(0, argv);
//     // rspeed(0, argv);
//     // tblook(0, argv);
//     // ttsprk(0, argv);
// 
//     puts("main end");
//     asm volatile("wfi");
// }



