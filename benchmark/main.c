
#include <stdio.h>
#include <string.h>
#include <eembc_adaptation.h>

// throw compiler error if both SINGLE_ALGORITHM and MULTIPLE_ALGORITHMS are defined
#if defined(SINGLE_ALGORITHM) && defined(MULTIPLE_ALGORITHMS)
#error "Both SINGLE_ALGORITHM and MULTIPLE_ALGORITHMS are defined. Only one of them should be defined."
#endif

#ifdef SINGLE_ALGORITHM
extern int PROG_NAME(int argc, char *argv[]);
#endif 

#ifdef MULTIPLE_ALGORITHMS
extern int a2time(int argc, char *argv[]);
extern int bitmnp(int argc, char *argv[]);
extern int idctrn(int argc, char *argv[]);
extern int puwmod(int argc, char *argv[]);
extern int rspeed(int argc, char *argv[]);
extern int tblook(int argc, char *argv[]);
extern int ttsprk(int argc, char *argv[]);
#endif

void main(void) {
    // each argument is supplied as csv string in stdin
    // all arguments are input indices, allowing to run the same test multiple times
    char *argv[MAX_PROGRAM_ARGS]; 
    int argc = 0;
    // tokenized_inp_str may be deallocated after argv's are not needed anymore
    char *tokenized_inp_str = parse_args_from_stdin_csv(&argc, argv);

#ifdef SINGLE_ALGORITHM
    if (argc < 1) 
        th_exit("ERROR: missing input index argument.\nStopping execution.\n");

    PROG_NAME(argc, argv);
#endif

#ifdef MULTIPLE_ALGORITHMS

    if (argc < 2) 
        th_exit("ERROR: prog name and/or input index argument.\nStopping execution.\n");

    // When multiple algorithms are used, it is expected that the stdin csv string
    // will have the following format:
    //      "prog_name,input_index,prog_name,input_index,..."
    // For example:
    //      "a2time,0,bitmnp,0,idctrn,0,puwmod,0,rspeed,0,tblook,0,ttsprk,0"
    
    // store function pointer
    int (*func_ptr)(int, char *argv[]);
    for (int i=0; i < argc; i+=2) {
        if      (strcmp(argv[i], "a2time") == 0) { func_ptr = a2time; } 
        else if (strcmp(argv[i], "bitmnp") == 0) { func_ptr = bitmnp; } 
        else if (strcmp(argv[i], "idctrn") == 0) { func_ptr = idctrn; } 
        else if (strcmp(argv[i], "puwmod") == 0) { func_ptr = puwmod; } 
        else if (strcmp(argv[i], "rspeed") == 0) { func_ptr = rspeed; } 
        else if (strcmp(argv[i], "tblook") == 0) { func_ptr = tblook; } 
        else if (strcmp(argv[i], "ttsprk") == 0) { func_ptr = ttsprk; } 
        else {
            th_exit("ERROR: unknown program name \"%s\".\nStopping execution.\n", argv[i]);
        }
        // individual functions expect that the first argument is input index
        // that's why "+1" is used
        printf("Executing %s with input index %s\n", argv[i], argv[i+1]);
        func_ptr(1, &argv[i+1]);
        printf("%s returned\n", argv[i]);
    }

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



