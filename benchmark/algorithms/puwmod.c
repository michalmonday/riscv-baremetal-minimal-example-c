#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Notes about adapting EEMBC algorithms to run on a standalone
baremetal Flute processor, without having the th_lib.h and 
without following the official/proper EEMBC benchmark harness:

- replace every occurence of tcdef
- copy data input (algotst.c) 
- remove CRC check
- replace functions that start with "th_" except those declared in "includes/eembc_adaptation.h"
- remove first parameter from each call of th_exit (this is some kind of ID that is defined in the unavailable th_lib.h)
- remove all lines that mention RAMfile...
- remove GetTestData function, it is reduntant (possibly by design, it would allow to load data from a file into ram, but in this case the data is already in ram)
- rename data variable names to match "inpAngleROM" array name
- remove "struct TCDef *tcdef parameter" from "t_run_test" function
- copy algo.h to algorithms directory, rename it to "<main file name>.h" and put guard headers "ifndef" inside it. Remove functions declarations from it.
- the "<main file name>.h" should include <eembc_adaptation.h> at the top
- the code has 2 output functions: "WriteOut" and "DebugOut"

- replace each call to GetInputValues with:

    isTableLooped = loop_cnt >= sizeof(inpAngleROM)/sizeof(inpAngleROM[0]);
    angleCounter = inpAngleROM[input_index]; //    angleCounter = inpAngleROM[loop_cnt % (sizeof(inpAngleROM)/sizeof(inpAngleROM[0]))];

*/

// originally file.h was "algo.h"
#include "puwmod.h" // change to the name of the main file (eg. "a2time.h")

// forward declarations
static int t_run_test( int argc, const char *argv[] );
static n_int GetInputValues(n_void);

// function to be called from a separate main file
int puwmod(int argc, const char *argv[]) {
    return t_run_test(argc, argv);
}

// input data (inpCmdROM)
static const varsize inpCmdROM[] = {        
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 
    200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 
    200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 
    300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 400, 400, 400, 400, 400, 400, 
    400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 
    400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 
    500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 
    500, 500, 500, 500, 500, 500, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 
    700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 
    700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 
    800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 
    900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 
    900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 900, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 
    1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 
} ; /* End of test values :  inpCmdROM[] */



// #ifndef RANDOM_FUNCTION_GENERATOR
// int main(void) {
//     t_run_test(0, NULL);

//     // stop the program
//     asm("wfi");
//     return 0;
// }
// #endif

/*  DECLARATIONS */    
static n_int   *RAMfile ;          /* Pointer to test output RAM file */
static n_int   *RAMfilePtr ;       /* Pointer to position in output RAM file */
static n_int   RAMfileSize ;       /* Size of the debug output RAM file */
static n_int   tableCount ;        /* Number of passes through table */
static n_int   *RAMfileEOF;        /* points to end of RAM file */
static n_int   RAMfile_increment;  /* difference between varsize and n_int */

static varsize commandPos ;        /* Commanded position from external world */

static int input_index;

static int t_run_test(int argc, const char *argv[] )
{    
    int iterations = 1;
    input_index = atoi(argv[0]);
    int inputs_count = sizeof(inpCmdROM) / sizeof(inpCmdROM[0]);
    if (input_index >= inputs_count)
        th_exit("ERROR: input_index %d is out of range, max is %d\nStopping execution.\n", input_index, inputs_count-1);


#if BMDEBUG
    n_char *szTitle = 
        "\nEEMBC Automotive/Industrial Subcommittee Benchmarks ( c )1998-1999\n"
        "Algorithm 3 :  Pulse-Width Modulation  Rev. 1.0F0 - puwmod01\n" ; 
    n_char *szHeader = 
        "\n\npwmCounter, commandPos, internalPos, upDir, incA, incB, "
        " pwmA, pwmB, dirA, enbA, dirB, enbB, counter\n" ; 
    n_char szDebug[100] ; 
#else
    varsize i ; 
#endif /* BMDEBUG */
    static n_char szDataSize[40] ; 
    int isTableLooped = false ;     /* Input test data table looped */
    static n_int upDirection1 ;       /* Initial direction is 'up' */
    static n_int upDirection2 ; 
    static n_int upDirection3 ; 
    static n_int lastDirection1 ;     /* Memory of direction to detect change */
    static n_int lastDirection2 ; 
    static n_int lastDirection3 ; 
    static n_int enbPhaseA1 ;         /* Enable for the coil A driver */
    static n_int enbPhaseA2 ;     
    static n_int enbPhaseA3 ; 
    static n_int dirPhaseA1 ;         /* Dir. control for the coil A driver */
    static n_int dirPhaseA2 ; 
    static n_int dirPhaseA3 ; 
    static n_int enbPhaseB1 ;         /* Enable for the coil B driver */
    static n_int enbPhaseB2 ; 
    static n_int enbPhaseB3 ; 
    static n_int dirPhaseB1 ;         /* Dir. control for the coil B driver */
    static n_int dirPhaseB2 ; 
    static n_int dirPhaseB3 ; 
    static n_int incrementA1 ;        /* Incr./decrement coil A's duty cycle */
    static n_int incrementA2 ; 
    static n_int incrementA3 ; 
    static n_int incrementB1 ;        /* Incr./decrement coil B's duty cycle */
    static n_int incrementB2 ; 
    static n_int incrementB3 ; 
    static varsize internalPos1 ;   /* Alg.'s internal electrical position */
    static varsize internalPos2 ; 
    static varsize internalPos3 ; 
    static n_int pwmCounter1 ;        /* The PWM phase counter */
    static n_int pwmCounter2 ; 
    static n_int pwmCounter3 ; 
    static n_int pwmPhaseA1 ;         /* The counter-comparator for coil A */
    static n_int pwmPhaseA2 ; 
    static n_int pwmPhaseA3 ; 
    static n_int pwmPhaseB1 ;         /* The counter-comparator for coil B */
    static n_int pwmPhaseB2 ; 
    static n_int pwmPhaseB3 ; 

    /* Unused */
    argc = argc ;
    argv = argv ;

    /*
     * First, initialize the data structures we need for the test
     * and allocate memory as needed.  Report an error if we can't.
     *
     */    

    /* Variable initializations at t=0 */    
    pwmCounter1 = 0 ;   /* Initialize the PWM phase counter... */
    pwmCounter2 = 0 ; 
    pwmCounter3 = 0 ; 
    dirPhaseA1 = 0 ;    /* ...and the coil driver bits */
    dirPhaseA2 = 0 ; 
    dirPhaseA3 = 0 ; 
    dirPhaseB1 = 0 ; 
    dirPhaseB2 = 0 ; 
    dirPhaseB3 = 0 ; 
    enbPhaseA1 = 1 ; 
    enbPhaseA2 = 1 ; 
    enbPhaseA3 = 1 ; 
    enbPhaseB1 = 1 ; 
    enbPhaseB2 = 1 ; 
    enbPhaseB3 = 1 ; 
    pwmPhaseA1 = MID_PHASE ;    /* ...and set 50% duty cycle */
    pwmPhaseA2 = MID_PHASE ; 
    pwmPhaseA3 = MID_PHASE ; 
    pwmPhaseB1 = MID_PHASE ; 
    pwmPhaseB2 = MID_PHASE ; 
    pwmPhaseB3 = MID_PHASE ; 
    upDirection1 = true ;       /* ...and presume we'll move 'up' */
    upDirection2 = true ; 
    upDirection3 = true ; 
    lastDirection1 = true ; 
    lastDirection2 = true ; 
    lastDirection3 = true ; 
    incrementA1 = true ;        /* ...which means we increase */
    incrementA2 = true ;        /* coil A duty cycle */
    incrementA3 = true ; 
    incrementB1 = false ;       /* ...and decrease coil B duty cycle */
    incrementB2 = false ; 
    incrementB3 = false ; 

    internalPos1 = 0 ;          /* Start out at zero position */
    internalPos2 = 0 ; 
    internalPos3 = 0 ; 
    tableCount = 0 ;    /* Start out at beginning of input test data */

    /* Tell us the compiled data size */    
#if BMDEBUG
    th_sprintf( szDataSize, "Data size = %s\n\n", ( DATA_SIZE ? "LONG" : "SHORT" ) ) ; 
#else
    szDataSize[0] = ( n_char )( '0' + DATA_SIZE ) ; 
    szDataSize[1] = '\0' ; 
#endif /* BMDEBUG */
 
    /* Initialize the test data -- stimuli for the algorithm. */    

    // if( !GetTestData() )
    // {
    //     /* Allocate for the test input data table */
    //     th_exit( "Cannot Allocate Memory %s : %d",
    //         __FILE__, __LINE__ ) ; 
    // }

#if BMDEBUG
    DebugOut( szTitle ) ;    /* Print the title message in RAM file */
    DebugOut( szDataSize ) ; /*  and the data size */
    DebugOut( szHeader ) ;   /* Print the test output file hdr in RAM file */
#endif /* BMDEBUG */

    /* This is the actual benchmark algorithm. */    

    /*
     * The Pulse-Width Modulation algorithm will take an input variable, 
     * presumed to be a position command for a stepper motor controlled
     * fuel-control valve, and will move the motor to that position using
     * micro-stepping control.  Micro-stepping requires that a proportional
     * current be applied in each motor coil to cause the armature to sit
     * at fractional positions within the fixed step angle of the motor.
     * This requires a PWM signal to be developed for each motor coil.
     *
     * Normally, in a two-phase stepper motor, each coil is energized in
     * either of two polarities of current flow, the four combinations of
     * which causing the stepping action when applied in the correct
     * sequence : 
     *
     * Motor coil :    A    B
     *                +    -        1    \
     *                +    +        2     |__   one cycle, 4 steps
     *                -    +        3     |
     *                -    -        4    /
     *                +    -        5
     *
     * By applying a proportional current in each phase, as well as the
     * major step polarity shown above, we can "micro-step" to positions
     * between steps 1 and 2.  This will be the purpose of the PWM
     * algorithm.  Micro-stepping not only provides more resolution for
     * motor position, but also results in smoother positioning.
     *
     * The following algorithm will presume that each pass through the loop
     * is caused by an interrupt which occurs at a fixed rate, which is
     * substantially higher than the maximum step rate applied to the
     * motor, so that a loop counter can be used to perform the timing
     * for the PWM.  If an 8-bit counter is used, then the algorithm will
     * provide up to 256 micro-step positions within each major motor step.
     *
     * For maximum torque, both motor coils will be energized at
     * 50% duty cycle in each coil applied at the major-step( motor
     * resolution )positions, while 100% duty cycle and 0% duty cycle, 
     * respectively, will be applied to the coils at the middle 
     * micro-step position between two major-step positions.  The PWM duty-
     * cycle will then increase from 0% on the de-energized coil, in the
     * reverse polarity, and decrease proportionally from 100% on the other
     * coil, until 50%/50% is reached again at the next major-step position.
     *
     *
     */    

    for( int loop_cnt = 0 ; loop_cnt < iterations  ; loop_cnt++ )
    {
        /* no stopping! */

#if BMDEBUG
        if( isTableLooped )
        {
            DebugOut( "END--END--END\n" ) ;    /* Flag end-of-testfile */
        }
#endif /* BMDEBUG */

        /***********************************************************************
            First Pass                                                          
        ***********************************************************************/

        /* 
         * First, check the PWM phase, and drive the two motor coils depending
         * on their duty-cycle( DC )setpoints.
         */    

        pwmCounter1++ ;                 /* Bump the PWM phase counter */

        if( pwmCounter1 == MAX_COUNT )  /* Counter overflowed ? */
        {
            pwmCounter1 = 0 ;           /* Yes, reset the counter */
        }

#if BMDEBUG
        th_sprintf( szDebug, "%4d", pwmCounter1 ) ; 
        DebugOut( szDebug ) ; 
#endif     /* BMDEBUG */

        if( ( pwmCounter1 == pwmPhaseA1 ) &&    /* Reached end of phase A */
            ( pwmPhaseA1 < MAX_PHASE ) )        /*  and not 100% DC ? */
        {
            enbPhaseA1 = 0 ;    /* Yes, turn phase A off now */
        }
        if( ( pwmCounter1 == pwmPhaseB1 ) &&    /* Reached end of phase B */
            ( pwmPhaseB1 < MAX_PHASE ) )        /*  and not 100% DC ? */
        {
            enbPhaseB1 = 0 ;    /* Yes, turn phase B off now */
        }

        /* Now, check to see if the counter rolled over, indicating the end 
         * of a PWM cycle, and maybe time to change the motor position. We 
         * change the motor position if the commanded position is different 
         * from the current internal position. If so, we will increment the
         * internal position by one micro-step, which will require a new
         * proportional duty-cycle to be applied to each coil.
         *
         * When one phase is at 0% duty-cycle, and is incremented by one
         * micro-step, then the polarity for that phase must also be switched,
         * per the major-step table. Note that we may need to be decrementing 
         * motor position, depending on the sign of the difference between 
         * commanded and internal position.
         *
         */    

        if( pwmCounter1 == 0 ) /* Sync with pwm counter rollover */
        {
            /* Get 'commandPos' value from test data*/
            isTableLooped = GetInputValues() ;
            
            /* Are we commanded to move 'up'? */
            if( commandPos > internalPos1 ) 
            {
                /* Yes, remember that */
                upDirection1 = true ;
            }

            /* Are we commanded to move 'down' ? */
            if( commandPos < internalPos1 ) 
            {
                /* Yes, remember that */
                upDirection1 = false ;
            }

            if( upDirection1 != lastDirection1 )
            {        
                /* Change in direction ? */
                incrementA1 = !incrementA1 ;
                /* ...then, flip the duty-cycle bits */
                incrementB1 = !incrementB1 ;
            }

            /* Remember current direction for next pass */
            lastDirection1 = upDirection1 ;

            /* Are we commanded to move ? */
            if( commandPos != internalPos1 )
            {
                /* Yes, so micro-step per the duty-cycle bits */    
                if( incrementA1 )
                {
                    pwmPhaseA1++ ; 
                }
                else
                {
                    pwmPhaseA1-- ; 
                }

                if( ( pwmPhaseA1 == MAX_COUNT ) && incrementA1 )
                {        
                    pwmPhaseA1 = MAX_PHASE - 1 ; 
                    incrementA1 = false ; 
                }

                if( ( pwmPhaseA1 < 0 ) && !incrementA1 )
                {        
                    pwmPhaseA1 = 1 ; 
                    incrementA1 = true ; 
                }

                if( incrementB1 )
                {
                    pwmPhaseB1++ ; 
                }
                else
                {
                    pwmPhaseB1-- ; 
                }
                
                if( ( pwmPhaseB1 == MAX_COUNT ) && incrementB1 )
                {        
                    pwmPhaseB1 = MAX_PHASE - 1 ; 
                    incrementB1 = false ; 
                }

                if( ( pwmPhaseB1 < 0 ) && !incrementB1 )
                {        
                    pwmPhaseB1 = 1 ; 
                    incrementB1 = true ; 
                }

                /* If we reach the mid-point between major motor step 
                 * positions, then we must flip the coil driver polarity
                 *
                 */
                if( pwmPhaseA1 == 0 )
                {
                    if( dirPhaseA1 == 1 )
                    {
                        dirPhaseA1 = 0 ; 
                    }
                    else
                    {
                        dirPhaseA1 = 1 ; 
                    }
                }

                /* If we reach the mid-point between major motor step 
                 * positions, then we must flip the coil driver polarity
                 *
                 */
                if( pwmPhaseB1 == 0 )
                {
                    if( dirPhaseB1 == 1 )   
                    {
                        dirPhaseB1 = 0 ; 
                    }
                    else
                    {
                        dirPhaseB1 = 1 ; 
                    }
                }

                /* Depending on direction, bump the internal position */
                if( upDirection1 )   
                {
                    internalPos1++ ;
                }
                else
                {
                    internalPos1-- ; 
                }
            } /* End of 'commandPos != internalPos' */

            if( pwmPhaseA1 > 0 )   /* If >0% DC, turn A on again */
            {
                enbPhaseA1 = 1 ; 
            }

            if( pwmPhaseB1 > 0 )   /* And if >0% DC, turn B on again */
            {
                enbPhaseB1 = 1 ; 
            }
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %6ld, %6ld", (n_long)commandPos, 
            (n_long)internalPos1 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %2d, %2d, %2d", upDirection1, incrementA1, 
            incrementB1 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %4d, %4d", pwmPhaseA1, pwmPhaseB1 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

#if BMDEBUG
        th_sprintf( szDebug, ", %2d, %2d, %2d, %2d\n", dirPhaseA1, enbPhaseA1, 
            dirPhaseB1, enbPhaseB1 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( dirPhaseA1 ) ; 
        WriteOut( enbPhaseA1 ) ; 
        WriteOut( dirPhaseB1 ) ; 
        WriteOut( enbPhaseB1 ) ; 
#endif /* BMDEBUG */

        /***********************************************************************
            Second Pass                                                         
        ***********************************************************************/

        /* 
         * First, check the PWM phase, and drive the two motor coils depending
         * on their duty-cycle( DC )setpoints.
         */    

        pwmCounter2++ ;                 /* Bump the PWM phase counter */

        if( pwmCounter2 == MAX_COUNT )  /* Counter overflowed ? */
        {
            pwmCounter2 = 0 ;           /* Yes, reset the counter */
        }

#if BMDEBUG
        th_sprintf( szDebug, "%4d", pwmCounter2 ) ; 
        DebugOut( szDebug ) ; 
#endif     /* BMDEBUG */

        if( ( pwmCounter2 == pwmPhaseA2 ) &&    /* Reached end of phase A */
            ( pwmPhaseA2 < MAX_PHASE ) )        /*  and not 100% DC ? */
        {
            enbPhaseA2 = 0 ;    /* Yes, turn phase A off now */
        }
        if( ( pwmCounter2 == pwmPhaseB2 ) &&    /* Reached end of phase B */
            ( pwmPhaseB2 < MAX_PHASE ) )        /*  and not 100% DC ? */
        {
            enbPhaseB2 = 0 ;    /* Yes, turn phase B off now */
        }

        /* Now, check to see if the counter rolled over, indicating the end 
         * of a PWM cycle, and maybe time to change the motor position. We 
         * change the motor position if the commanded position is different 
         * from the current internal position. If so, we will increment the
         * internal position by one micro-step, which will require a new
         * proportional duty-cycle to be applied to each coil.
         *
         * When one phase is at 0% duty-cycle, and is incremented by one
         * micro-step, then the polarity for that phase must also be switched,
         * per the major-step table. Note that we may need to be decrementing 
         * motor position, depending on the sign of the difference between 
         * commanded and internal position.
         *
         */    

        if( pwmCounter2 == 0 ) /* Sync with pwm counter rollover */
        {
            /* Get 'commandPos' value from test data*/
            isTableLooped = GetInputValues() ;
            
            /* Are we commanded to move 'up'? */
            if( commandPos > internalPos2 ) 
            {
                /* Yes, remember that */
                upDirection2 = true ;
            }

            /* Are we commanded to move 'down' ? */
            if( commandPos < internalPos2 ) 
            {
                /* Yes, remember that */
                upDirection2 = false ;
            }

            if( upDirection2 != lastDirection2 )
            {        
                /* Change in direction ? */
                incrementA2 = !incrementA2 ;
                /* ...then, flip the duty-cycle bits */
                incrementB2 = !incrementB2 ;
            }

            /* Remember current direction for next pass */
            lastDirection2 = upDirection2 ;

            /* Are we commanded to move ? */
            if( commandPos != internalPos2 )
            {
                /* Yes, so micro-step per the duty-cycle bits */    
                if( incrementA2 )
                {
                    pwmPhaseA2++ ; 
                }
                else
                {
                    pwmPhaseA2-- ; 
                }

                if( ( pwmPhaseA2 == MAX_COUNT ) && incrementA2 )
                {        
                    pwmPhaseA2 = MAX_PHASE - 1 ; 
                    incrementA2 = false ; 
                }

                if( ( pwmPhaseA2 < 0 ) && !incrementA2 )
                {        
                    pwmPhaseA2 = 1 ; 
                    incrementA2 = true ; 
                }

                if( incrementB2 )
                {
                    pwmPhaseB2++ ; 
                }
                else
                {
                    pwmPhaseB2-- ; 
                }
                
                if( ( pwmPhaseB2 == MAX_COUNT ) && incrementB2 )
                {        
                    pwmPhaseB2 = MAX_PHASE - 1 ; 
                    incrementB2 = false ; 
                }

                if( ( pwmPhaseB2 < 0 ) && !incrementB2 )
                {        
                    pwmPhaseB2 = 1 ; 
                    incrementB2 = true ; 
                }

                /* If we reach the mid-point between major motor step 
                 * positions, then we must flip the coil driver polarity
                 *
                 */
                if( pwmPhaseA2 == 0 )
                {
                    if( dirPhaseA2 == 1 )
                    {
                        dirPhaseA2 = 0 ; 
                    }
                    else
                    {
                        dirPhaseA2 = 1 ; 
                    }
                }

                /* If we reach the mid-point between major motor step 
                 * positions, then we must flip the coil driver polarity
                 *
                 */
                if( pwmPhaseB2 == 0 )
                {
                    if( dirPhaseB2 == 1 )   
                    {
                        dirPhaseB2 = 0 ; 
                    }
                    else
                    {
                        dirPhaseB2 = 1 ; 
                    }
                }

                /* Depending on direction, bump the internal position */
                if( upDirection2 )   
                {
                    internalPos2++ ;
                }
                else
                {
                    internalPos2-- ; 
                }
            } /* End of 'commandPos != internalPos' */

            if( pwmPhaseA2 > 0 )   /* If >0% DC, turn A on again */
            {
                enbPhaseA2 = 1 ; 
            }

            if( pwmPhaseB2 > 0 )   /* And if >0% DC, turn B on again */
            {
                enbPhaseB2 = 1 ; 
            }
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %6ld, %6ld", (n_long)commandPos, 
            (n_long)internalPos2 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %2d, %2d, %2d", upDirection2, incrementA2, 
            incrementB2 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %4d, %4d", pwmPhaseA2, pwmPhaseB2 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

#if BMDEBUG
        th_sprintf( szDebug, ", %2d, %2d, %2d, %2d\n", dirPhaseA2, enbPhaseA2, 
            dirPhaseB2, enbPhaseB2 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( dirPhaseA2 ) ; 
        WriteOut( enbPhaseA2 ) ; 
        WriteOut( dirPhaseB2 ) ; 
        WriteOut( enbPhaseB2 ) ; 
#endif /* BMDEBUG */

        /***********************************************************************
            Third Pass                                                          
        ***********************************************************************/

        /* 
         * First, check the PWM phase, and drive the two motor coils depending
         * on their duty-cycle( DC )setpoints.
         */    

        pwmCounter3++ ;                 /* Bump the PWM phase counter */

        if( pwmCounter3 == MAX_COUNT )  /* Counter overflowed ? */
        {
            pwmCounter3 = 0 ;           /* Yes, reset the counter */
        }

#if BMDEBUG
        th_sprintf( szDebug, "%4d", pwmCounter3 ) ; 
        DebugOut( szDebug ) ; 
#endif     /* BMDEBUG */

        if( ( pwmCounter3 == pwmPhaseA3 ) &&    /* Reached end of phase A */
            ( pwmPhaseA3 < MAX_PHASE ) )        /*  and not 100% DC ? */
        {
            enbPhaseA3 = 0 ;    /* Yes, turn phase A off now */
        }
        if( ( pwmCounter3 == pwmPhaseB3 ) &&    /* Reached end of phase B */
            ( pwmPhaseB3 < MAX_PHASE ) )        /*  and not 100% DC ? */
        {
            enbPhaseB3 = 0 ;    /* Yes, turn phase B off now */
        }

        /* Now, check to see if the counter rolled over, indicating the end 
         * of a PWM cycle, and maybe time to change the motor position. We 
         * change the motor position if the commanded position is different 
         * from the current internal position. If so, we will increment the
         * internal position by one micro-step, which will require a new
         * proportional duty-cycle to be applied to each coil.
         *
         * When one phase is at 0% duty-cycle, and is incremented by one
         * micro-step, then the polarity for that phase must also be switched,
         * per the major-step table. Note that we may need to be decrementing 
         * motor position, depending on the sign of the difference between 
         * commanded and internal position.
         *
         */    

        if( pwmCounter3 == 0 ) /* Sync with pwm counter rollover */
        {
            /* Get 'commandPos' value from test data*/
            isTableLooped = GetInputValues() ;
            
            /* Are we commanded to move 'up'? */
            if( commandPos > internalPos3 ) 
            {
                /* Yes, remember that */
                upDirection3 = true ;
            }

            /* Are we commanded to move 'down' ? */
            if( commandPos < internalPos3 ) 
            {
                /* Yes, remember that */
                upDirection3 = false ;
            }

            if( upDirection3 != lastDirection3 )
            {        
                /* Change in direction ? */
                incrementA3 = !incrementA3 ;
                /* ...then, flip the duty-cycle bits */
                incrementB3 = !incrementB3 ;
            }

            /* Remember current direction for next pass */
            lastDirection3 = upDirection3 ;

            /* Are we commanded to move ? */
            if( commandPos != internalPos3 )
            {
                /* Yes, so micro-step per the duty-cycle bits */    
                if( incrementA3 )
                {
                    pwmPhaseA3++ ; 
                }
                else
                {
                    pwmPhaseA3-- ; 
                }

                if( ( pwmPhaseA3 == MAX_COUNT ) && incrementA3 )
                {        
                    pwmPhaseA3 = MAX_PHASE - 1 ; 
                    incrementA3 = false ; 
                }

                if( ( pwmPhaseA3 < 0 ) && !incrementA3 )
                {        
                    pwmPhaseA3 = 1 ; 
                    incrementA3 = true ; 
                }

                if( incrementB3 )
                {
                    pwmPhaseB3++ ; 
                }
                else
                {
                    pwmPhaseB3-- ; 
                }
                
                if( ( pwmPhaseB3 == MAX_COUNT ) && incrementB3 )
                {        
                    pwmPhaseB3 = MAX_PHASE - 1 ; 
                    incrementB3 = false ; 
                }

                if( ( pwmPhaseB3 < 0 ) && !incrementB3 )
                {        
                    pwmPhaseB3 = 1 ; 
                    incrementB3 = true ; 
                }

                /* If we reach the mid-point between major motor step 
                 * positions, then we must flip the coil driver polarity
                 *
                 */
                if( pwmPhaseA3 == 0 )
                {
                    if( dirPhaseA3 == 1 )
                    {
                        dirPhaseA3 = 0 ; 
                    }
                    else
                    {
                        dirPhaseA3 = 1 ; 
                    }
                }

                /* If we reach the mid-point between major motor step 
                 * positions, then we must flip the coil driver polarity
                 *
                 */
                if( pwmPhaseB3 == 0 )
                {
                    if( dirPhaseB3 == 1 )   
                    {
                        dirPhaseB3 = 0 ; 
                    }
                    else
                    {
                        dirPhaseB3 = 1 ; 
                    }
                }

                /* Depending on direction, bump the internal position */
                if( upDirection3 )   
                {
                    internalPos3++ ;
                }
                else
                {
                    internalPos3-- ; 
                }
            } /* End of 'commandPos != internalPos' */

            if( pwmPhaseA3 > 0 )   /* If >0% DC, turn A on again */
            {
                enbPhaseA3 = 1 ; 
            }

            if( pwmPhaseB3 > 0 )   /* And if >0% DC, turn B on again */
            {
                enbPhaseB3 = 1 ; 
            }
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %6ld, %6ld", (n_long)commandPos, 
            (n_long)internalPos3 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %2d, %2d, %2d", upDirection3, incrementA3, 
            incrementB3 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %4d, %4d", pwmPhaseA3, pwmPhaseB3 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

#if BMDEBUG
        th_sprintf( szDebug, ", %2d, %2d, %2d, %2d\n", dirPhaseA3, enbPhaseA3, 
            dirPhaseB3, enbPhaseB3 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( dirPhaseA3 ) ; 
        WriteOut( enbPhaseA3 ) ; 
        WriteOut( dirPhaseB3 ) ; 
        WriteOut( enbPhaseB3 ) ; 

#if DATA_SIZE == 0 /* Might break up the loop counter */
        i = (varsize )loop_cnt &0x0000FFFF ; 
        WriteOut( i ) ; /* ...in the output file */
        i = (varsize)loop_cnt >> 16 ; 
        WriteOut( i ) ; 
#else
        WriteOut( loop_cnt ) ; 
#endif
        i = 0xAAAA ; 
        WriteOut( i ) ;  /* Flag the end of data-block */
#endif /* BMDEBUG */


#if BMDEBUG
        if( !th_harness_poll() )
        {
            break ; 
        }
#endif /* BMDEBUG */
    }
    return 0;

} /* End of function 't_run_test' */



// /* *  Function :  GetTestData * *    Builds the table of input data which represents 'command position' *    input parameter.  This is the "real world" data stream which *    drives the algorithm.  This data is installed in RAM prior to execution of the *    algorithm.  * */ 
// n_int GetTestData(n_void)
// {
//     int i;
//     inpCmdPosition = (varsize *)th_malloc((NUM_TESTS + 1) * sizeof(varsize));
//     if (inpCmdPosition == NULL)
//     {
//         return false;
//     } /*  Copy the test values from ROM to RAM  */
//     for (i = 0; i < NUM_TESTS; i++)
//     {
//         inpCmdPosition[i] = inpCmdROM[i];
//     }
//     return true;
// } /* End of function 'GetTestData' */

/* *  Function :  GetInputValues * 
* On each pass of the table lookup, a value must be input for 'commandPos'.  
* Each time this function is called, the next input value is 
* pulled from the table in RAM.  The table wraps around, so that input data is *    continuous.     A flag is returned TRUE whenever the table wraps around.  * */
static n_int GetInputValues(n_void)
{
    commandPos = inpCmdROM[input_index++];
    if (input_index < NUM_TESTS) {
       return false;
    }
    input_index = 0;
    return true;

    //commandPos = inpCmdPosition[tableCount];
    //if (++tableCount < NUM_TESTS)
    //{
    //    return false;
    //}
    //tableCount = 0;
    //return true;
} /* End of function 'GetInputValues' */