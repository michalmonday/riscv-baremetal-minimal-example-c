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
#include "file.h" // change to the name of the main file (eg. "a2time.h")

// forward declarations
int t_run_test( int argc, const char *argv[] );

// input data (inpROM)

const int inpROM[] = {        

}; /* End of test values :  inpROM[] */


int main(void) {
    t_run_test(0, NULL);

    // stop the program
    asm("wfi");
    return 0;
}

/*  DECLARATIONS */    
n_int   *RAMfile ;          /* Pointer to test output RAM file */
n_int   *RAMfilePtr ;       /* Pointer to position in output RAM file */
n_int   RAMfileSize ;       /* Size of the debug output RAM file */
n_int   tableCount ;        /* Number of passes through table */
n_int   *RAMfileEOF;        /* points to end of RAM file */
n_int   RAMfile_increment;  /* difference between varsize and n_int */

int input_index;

int t_run_test(int argc, const char *argv[] )
{    
    int iterations = 1;
    scanf("%d", &input_index);
    int inputs_count = sizeof(inpROM) / sizeof(inpROM[0]);
    if (input_index >= inputs_count) {
        printf("ERROR: input_index %d is out of range, max is %d\n", input_index, inputs_count-1);
        printf("stopping execution\n");
        asm volatile ("wfi");
    }

    return 0;
} /* End of function 't_run_test' */

