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
#include "aifftr.h" // change to the name of the main file (eg. "a2time.h")

#define BMDEBUG true

// forward declarations
static int t_run_test( int argc, const char *argv[] );

// input data (inpSignalROM)
static const int inpSignalROM[] = {        
    /* xsine512_t.dat */        
    0, 0,    19260, 0, 31164, 0, 31164, 0, 19260, 0, 0, 0, -19260, 0, -31164, 0, -31164, 0, -19260, 0, 0, 0, 19260, 0, 31164, 0, 31164, 0, 19260, 0, 0, 0, -19260, 0, -31164, 0, -31164, 0, -19260, 0, 0, 0, 19260, 0, 31164, 0, 31164, 0, 19260, 0, 0, 0, -19261, 0, -31164, 0, -31164, 0, -19260, 0, 0, 0, 19261, 0, 31164, 0, 31163, 0, 19259, 0, 0, 0, -19261, 0, -31164, 0, -31163, 0, -19259, 0, 0, 0, 19261, 0, 31164, 0, 31163, 0, 19259, 0, -1, 0, -19261, 0, -31164, 0, -31163, 0, -19259, 0, 1, 0, 19261, 0, 31164, 0, 31163, 0, 19259, 0, -1, 0, -19261, 0, -31164, 0, -31163, 0, -19259, 0, 1, 0, 19261, 0, 31164, 0, 31163, 0, 19259, 0, -1, 0, -19261, 0, -31164, 0, -31163, 0, -19259, 0, 1, 0, 19261, 0, 31164, 0, 31163, 0, 19259, 0, -1, 0, -19261, 0, -31164, 0, -31163, 0, -19259, 0, 1, 0, 19262, 0, 31164, 0, 31163, 0, 19258, 0, -1, 0, -19262, 0, -31164, 0, -31163, 0, -19258, 0, 2, 0, 19262, 0, 31164, 0, 31163, 0, 19258, 0, -2, 0, -19262, 0, -31164, 0, -31163, 0, -19258, 0, 2, 0, 19262, 0, 31164, 0, 31163, 0, 19258, 0, -2, 0, -19262, 0, -31164, 0, -31163, 0, -19258, 0, 2, 0, 19262, 0, 31165, 0, 31163, 0, 19258, 0, -2, 0, -19262, 0, -31165, 0, -31163, 0, -19258, 0, 2, 0, 19262, 0, 31165, 0, 31163, 0, 19258, 0, -2, 0, -19262, 0, -31165, 0, -31163, 0, -19258, 0, 3, 0, 19263, 0, 31165, 0, 31163, 0, 19258, 0, -3, 0, -19263, 0, -31165, 0, -31163, 0, -19257, 0, 3, 0, 19263, 0, 31165, 0, 31163, 0, 19257, 0, -3, 0, -19263, 0, -31165, 0, -31163, 0, -19257, 0, 3, 0, 19263, 0, 31165, 0, 31163, 0, 19257, 0, -3, 0, -19263, 0, -31165, 0, -31163, 0, -19257, 0, 3, 0, 19263, 0, 31165, 0, 31163, 0, 19257, 0, -3, 0, -19263, 0, -31165, 0, -31163, 0, -19257, 0, 3, 0, 19263, 0, 31165, 0, 31162, 0, 19257, 0, -4, 0, -19263, 0, -31165, 0, -31162, 0, -19257, 0, 4, 0, 19263, 0, 31165, 0, 31162, 0, 19257, 0, -4, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 4, 0, 19264, 0, 31165, 0, 31162, 0, 19256, 0, -4, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 4, 0, 19264, 0, 31165, 0, 31162, 0, 19256, 0, -4, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 4, 0, 19264, 0, 31165, 0, 31162, 0, 19256, 0, -4, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 5, 0, 19264, 0, 31165, 0, 31162, 0, 19256, 0, -5, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 5, 0, 19264, 0, 31165, 0, 31162, 0, 19256, 0, -5, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 5, 0, 19265, 0, 31165, 0, 31162, 0, 19255, 0, -5, 0, -19265, 0, -31165, 0, -31162, 0, -19255, 0, 5, 0, 19265, 0, 31166, 0, 31162, 0, 19255, 0, -5, 0, -19265, 0, -31166, 0, -31162, 0, -19255, 0, 6, 0, 19265, 0, 31166, 0, 31162, 0, 19255, 0, -6, 0, -19265, 0, -31166, 0, -31162, 0, -19255, 0, 6, 0, 19265, 0, 31166, 0, 31162, 0, 19255, 0, -6, 0, -19265, 0, -31166, 0, -31162, 0, -19255, 0, 6, 0, 19265, 0, 31166, 0, 31162, 0, 19255, 0, -6, 0, -19265, 0, -31166, 0, -31162, 0, -19255, 0, 6, 0, 19266, 0, 31166, 0, 31162, 0, 19255, 0, -6, 0, -19266, 0, -31166, 0, -31162, 0, -19254, 0, 6, 0, 19266, 0, 31166, 0, 31162, 0, 19254, 0, -7, 0, -19266, 0, -31166, 0, -31162, 0, -19254, 0, 7, 0, 19266, 0, 31166, 0, 31161, 0, 19254, 0, -7, 0, -19266, 0, -31166, 0, -31161, 0, -19254, 0, 7, 0, 19266, 0, 31166, 0, 31161, 0, 19254, 0, -7, 0, -19266, 0, -31166, 0, -31161, 0, -19254, 0, 7, 0, 19266, 0, 31166, 0, 31161, 0, 19254, 0, -7, 0, -19266, 0, -31166, 0, -31161, 0, -19254, 0, 7, 0, 19266, 0, 31166, 0, 31161, 0, 19254, 0, -8, 0, -19267, 0, -31166, 0, -31161, 0, -19253, 0, 8, 0, 19267, 0, 31166, 0, 31161, 0, 19253, 0, -8, 0, -19267, 0, -31166, 0, -31161, 0, -19253, 0, 8, 0, 19267, 0, 31166, 0, 31161, 0, 19253, 0, -8, 0, -19267, 0, -31166, 0, -31161, 0, -19253, 0, 8, 0, 19267, 0, 31166, 0, 31161, 0, 19253, 0, -8, 0, -19267, 0, -31166, 0, -31161, 0, -19253, 0, 8, 0, 19267, 0, 31166, 0, 31161, 0, 19253, 0, -8, 0, -19267, 0, -31166, 0, -31161, 0, -19253, 0, 9, 0, 19267, 0, 31167, 0, 31161, 0, 19253, 0, -9, 0, -19267, 0, -31167, 0, -31161, 0, -19253, 0, 9, 0, 19268, 0, 31167, 0, 31161, 0, 19252, 0, -9, 0, -19268, 0, -31167, 0, -31161, 0, -19252, 0, 9, 0, 19268, 0, 31167, 0, 31161, 0, 19252, 0, -9, 0, -19268, 0, -31167, 0, -31161, 0, -19252, 0, 9, 0, 19268, 0, 31167, 0, 31161, 0, 19252, 0, -9, 0, -19268, 0, -31167, 0, -31161, 0, -19252, 0, 9, 0, 19268, 0, 31167, 0, 31161, 0, 19252, 0, -10, 0, -19268, 0, -31167, 0, -31161, 0, -19252, 0, 10, 0, 19268, 0, 31167, 0, 31161, 0, 19252, 0, -10, 0, -19268, 0, -31167, 0, -31161, 0, -19252, 0, 10, 0, 19269, 0, 31167, 0, 31160, 0, 19252, 0, -10, 0, -19269, 0, -31167, 0, -31160, 0, -19251, 0, 
    10, 0, 19269, 0, 31167, 0, 31160, 0, 19251, 0, -10, 0, -19269, 0, -31167, 0, -31160, 0, -19251, 0, 10, 0, 19269, 0, 31167, 0, 31160, 0, 19251, 0, -11, 0, -19269, 0, -31167, 0, -31160, 0, -19251, 0, 11, 0, 19269, 0, 31167, 0, 31160, 0, 19251, 0, -11, 0, -19269, 0, -31167, 0, -31160, 0, -19251, 0, 11, 0, 19269, 0, 31167, 0, 31160, 0, 19251, 0, -11, 0, -19269, 0, -31167, 0, -31160, 0, -19251, 0, 11, 0, 19269, 0, 31167, 0, 31160, 0, 19251, 0, -11, 0, -19270, 0, -31167, 0, -31160, 0, -19250, 0, 11, 0, 19270, 0, 0, 0,    
    /* xsine512_t.dat */
    19260, 0, 31164, 0, 31164, 0, 19260, 0, 0, 0, -19260, 0, -31164, 0, -31164, 0, -19260, 0, 0, 0, 19260, 0, 31164, 0, 31164, 0, 19260, 0, 0, 0, -19260, 0, -31164, 0, -31164, 0, -19260, 0, 0, 0, 19260, 0, 31164, 0, 31164, 0, 19260, 0, 0, 0, -19261, 0, -31164, 0, -31164, 0, -19260, 0, 0, 0, 19261, 0, 31164, 0, 31163, 0, 19259, 0, 0, 0, -19261, 0, -31164, 0, -31163, 0, -19259, 0, 0, 0, 19261, 0, 31164, 0, 31163, 0, 19259, 0, -1, 0, -19261, 0, -31164, 0, -31163, 0, -19259, 0, 1, 0, 19261, 0, 31164, 0, 31163, 0, 19259, 0, -1, 0, -19261, 0, -31164, 0, -31163, 0, -19259, 0, 1, 0, 19261, 0, 31164, 0, 31163, 0, 19259, 0, -1, 0, -19261, 0, -31164, 0, -31163, 0, -19259, 0, 1, 0, 19261, 0, 31164, 0, 31163, 0, 19259, 0, -1, 0, -19261, 0, -31164, 0, -31163, 0, -19259, 0, 1, 0, 19262, 0, 31164, 0, 31163, 0, 19258, 0, -1, 0, -19262, 0, -31164, 0, -31163, 0, -19258, 0, 2, 0, 19262, 0, 31164, 0, 31163, 0, 19258, 0, -2, 0, -19262, 0, -31164, 0, -31163, 0, -19258, 0, 2, 0, 19262, 0, 31164, 0, 31163, 0, 19258, 0, -2, 0, -19262, 0, -31164, 0, -31163, 0, -19258, 0, 2, 0, 19262, 0, 31165, 0, 31163, 0, 19258, 0, -2, 0, -19262, 0, -31165, 0, -31163, 0, -19258, 0, 2, 0, 19262, 0, 31165, 0, 31163, 0, 19258, 0, -2, 0, -19262, 0, -31165, 0, -31163, 0, -19258, 0, 3, 0, 19263, 0, 31165, 0, 31163, 0, 19258, 0, -3, 0, -19263, 0, -31165, 0, -31163, 0, -19257, 0, 3, 0, 19263, 0, 31165, 0, 31163, 0, 19257, 0, -3, 0, -19263, 0, -31165, 0, -31163, 0, -19257, 0, 3, 0, 19263, 0, 31165, 0, 31163, 0, 19257, 0, -3, 0, -19263, 0, -31165, 0, -31163, 0, -19257, 0, 3, 0, 19263, 0, 31165, 0, 31163, 0, 19257, 0, -3, 0, -19263, 0, -31165, 0, -31163, 0, -19257, 0, 3, 0, 19263, 0, 31165, 0, 31162, 0, 19257, 0, -4, 0, -19263, 0, -31165, 0, -31162, 0, -19257, 0, 4, 0, 19263, 0, 31165, 0, 31162, 0, 19257, 0, -4, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 4, 0, 19264, 0, 31165, 0, 31162, 0, 19256, 0, -4, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 4, 0, 19264, 0, 31165, 0, 31162, 0, 19256, 0, -4, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 4, 0, 19264, 0, 31165, 0, 31162, 0, 19256, 0, -4, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 5, 0, 19264, 0, 31165, 0, 31162, 0, 19256, 0, -5, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 5, 0, 19264, 0, 31165, 0, 31162, 0, 19256, 0, -5, 0, -19264, 0, -31165, 0, -31162, 0, -19256, 0, 5, 0, 19265, 0, 31165, 0, 31162, 0, 19255, 0, -5, 0, -19265, 0, -31165, 0, -31162, 0, -19255, 0, 5, 0, 19265, 0, 31166, 0, 31162, 0, 19255, 0, -5, 0, -19265, 0, -31166, 0, -31162, 0, -19255, 0, 6, 0, 19265, 0, 31166, 0, 31162, 0, 19255, 0, -6, 0, -19265, 0, -31166, 0, -31162, 0, -19255, 0, 6, 0, 19265, 0, 31166, 0, 31162, 0, 19255, 0, -6, 0, -19265, 0, -31166, 0, -31162, 0, -19255, 0, 6, 0, 19265, 0, 31166, 0, 31162, 0, 19255, 0, -6, 0, -19265, 0, -31166, 0, -31162, 0, -19255, 0, 6, 0, 19266, 0, 31166, 0, 31162, 0, 19255, 0, -6, 0, -19266, 0, -31166, 0, -31162, 0, -19254, 0, 6, 0, 19266, 0, 31166, 0, 31162, 0, 19254, 0, -7, 0, -19266, 0, -31166, 0, -31162, 0, -19254, 0, 7, 0, 19266, 0, 31166, 0, 31161, 0, 19254, 0, -7, 0, -19266, 0, -31166, 0, -31161, 0, -19254, 0, 7, 0, 19266, 0, 31166, 0, 31161, 0, 19254, 0, -7, 0, -19266, 0, -31166, 0, -31161, 0, -19254, 0, 7, 0, 19266, 0, 31166, 0, 31161, 0, 19254, 0, -7, 0, -19266, 0, -31166, 0, -31161, 0, -19254, 0, 7, 0, 19266, 0, 31166, 0, 31161, 0, 19254, 0, -8, 0, -19267, 0, -31166, 0, -31161, 0, -19253, 0, 8, 0, 19267, 0, 31166, 0, 31161, 0, 19253, 0, -8, 0, -19267, 0, -31166, 0, -31161, 0, -19253, 0, 8, 0, 19267, 0, 31166, 0, 31161, 0, 19253, 0, -8, 0, -19267, 0, -31166, 0, -31161, 0, -19253, 0, 8, 0, 19267, 0, 31166, 0, 31161, 0, 19253, 0, -8, 0, -19267, 0, -31166, 0, -31161, 0, -19253, 0, 8, 0, 19267, 0, 31166, 0, 31161, 0, 19253, 0, -8, 0, -19267, 0, -31166, 0, -31161, 0, -19253, 0, 9, 0, 19267, 0, 31167, 0, 31161, 0, 19253, 0, -9, 0, -19267, 0, -31167, 0, -31161, 0, -19253, 0, 9, 0, 19268, 0, 31167, 0, 31161, 0, 19252, 0, -9, 0, -19268, 0, -31167, 0, -31161, 0, -19252, 0, 
    9, 0, 19268, 0, 31167, 0, 31161, 0, 19252, 0, -9, 0, -19268, 0, -31167, 0, -31161, 0, -19252, 0, 9, 0, 19268, 0, 31167, 0, 31161, 0, 19252, 0, -9, 0, -19268, 0, -31167, 0, -31161, 0, -19252, 0, 9, 0, 19268, 0, 31167, 0, 31161, 0, 19252, 0, -10, 0, -19268, 0, -31167, 0, -31161, 0, -19252, 0, 10, 0, 19268, 0, 31167, 0, 31161, 0, 19252, 0, -10, 0, -19268, 0, -31167, 0, -31161, 0, -19252, 0, 10, 0, 19269, 0, 31167, 0, 31160, 0, 19252, 0, -10, 0, -19269, 0, -31167, 0, -31160, 0, -19251, 0, 10, 0, 19269, 0, 31167, 0, 31160, 0, 19251, 0, -10, 0, -19269, 0, -31167, 0, -31160, 0, -19251, 0, 10, 0, 19269, 0, 31167, 0, 31160, 0, 19251, 0, -11, 0, -19269, 0, -31167, 0, -31160, 0, -19251, 0, 11, 0, 19269, 0, 31167, 0, 31160, 0, 19251, 0, -11, 0, -19269, 0, -31167, 0, -31160, 0, -19251, 0, 11, 0, 19269, 0, 31167, 0, 31160, 0, 19251, 0, -11, 0, -19269, 0, -31167, 0, -31160, 0, -19251, 0, 11, 0, 19269, 0, 31167, 0, 31160, 0, 19251, 0, -11, 0, -19270, 0, -31167, 0, -31160, 0, -19250, 0, 11, 0, 19270, 0, 3661, 0,    
    /* xspn512_t.dat */
    21852, 0, 25080, 0, 8831, 0, -3951, 0, 1523, 0, 10492, 0, 2255, 0, -14559, 0, -18684, 0, -9391, 0, -3150, 0, -2631, 0, 3119, 0, 12008, 0, 7450, 0, -10358, 0, -17388, 0, -1356, 0, 17880, 0, 17443, 0, 5264, 0, 1774, 0, 6671, 0, 3520, 0, -7059, 0, -10804, 0, -7029, 0, -10673, 0, -20871, 0, -17905, 0, 5779, 0, 26005, 0, 21646, 0, 4570, 0, -1918, 0, 2493, 0, 1909, 0, -4153, 0, -2306, 0, 5401, 0, 2758, 0, -10269, 0, -12979, 0, 895, 0, 9525, 0, -2290, 0, -19051, 0, -16734, 0, 413, 0, 11819, 0, 13627, 0, 17241, 0, 20542, 0, 10471, 0, -9297, 0, -16808, 0, -5751, 0, 1934, 0, -8323, 0, -19916, 0, -12869, 0, 5034, 0, 12346, 0, 7225, 0, 4515, 0, 5779, 0, -512, 0, -10326, 0, -5939, 0, 13584, 0, 23295, 0, 9335, 0, -9366, 0, -11259, 0, -3434, 0, -4890, 0, -13848, 0, -14106, 0, -5248, 0, -1244, 0, -1824, 0, 7459, 0, 25239, 0, 27823, 0, 7169, 0, -14019, 0, -13495, 0, -1145, 0, 1641, 0, -4724, 0, -4801, 0, 799, 0, -1735, 0, -10023, 0, -7588, 0, 5255, 0, 8155, 0, -5637, 0, -13628, 0, 533, 0, 21359, 0, 24392, 0, 11749, 0, 2209, 0, -2180, 0, -11521, 0, -21757, 0, -17603, 0, -1471, 0, 4774, 0, -5091, 0, -10080, 0, 3989, 0, 20151, 0, 17207, 0, 1764, 0, -5169, 0, -1848, 0, -1143, 0, -3499, 0, 3518, 0, 14655, 0, 10755, 0, -10697, 0, -24214, 0, -15238, 0, -208, 0, 37, 0, -7144, 0, -3282, 0, 9922, 0, 15273, 0, 12284, 0, 13268, 0, 16002, 0, 4972, 0, -17336, 0, -26694, 0, -11452, 0, 6817, 0, 6155, 0, -4978, 0, -6311, 0, 643, 0, 1198, 0, -3753, 0, -244, 0, 9278, 0, 8212, 0, -2996, 0, -3403, 0, 13217, 0, 23705, 0, 10112, 0, -12926, 0, -21534, 0, -16489, 0, -12927, 0, -11729, 0, -1653, 0, 12157, 0, 12876, 0, 1426, 0, -624, 0, 12656, 0, 19210, 0, 5046, 0, -13311, 0, -13679, 0, -1050, 0, 4233, 0, 1221, 0, 1538, 0, 3234, 0, -5913, 0, -21074, 0, -19944, 0, 410, 0, 15883, 0, 10805, 0, 882, 0, 5869, 0, 17014, 0, 15504, 0, 3055, 0, -4925, 0, -7179, 0, -14867, 0, -23963, 0, -16283, 0, 6245, 0, 18545, 0, 7006, 0, -7851, 0, -5914, 0, 5760, 0, 7331, 0, 1664, 0, 2357, 0, 8589, 0, 5834, 0, -2896, 0, -1382, 0, 9156, 0, 7238, 0, -13963, 0, -30338, 0, -22041, 0, -1565, 0, 8636, 0, 8581, 0, 11948, 0, 16785, 0, 10822, 0, -1573, 0, -2471, 0, 7029, 0, 6128, 0, -10707, 0, -21371, 0, -9542, 0, 8259, 0, 9440, 0, -1719, 0, -7107, 0, -5538, 0, -7761, 0, -10587, 0, 704, 0, 19832, 0, 24112, 0, 8516, 0, -4348, 0, 1086, 0, 8942, 0, 334, 0, -16169, 0, -20602, 0, -13347, 0, -8615, 0, -5706, 0, 6192, 0, 20946, 0, 18370, 0, -1301, 0, -12953, 0, -2276, 0, 12057, 0, 9649, 0, -1176, 0, -2588, 0, 2773, 0, 529, 0, -7181, 0, -6778, 0, -913, 0, -6202, 0, -20048, 0, -19600, 0, 3416, 0, 25143, 0, 24267, 0, 10766, 0, 4513, 0, 4815, 0, -1394, 0, -10437, 0, -8888, 0, -485, 0, -2965, 0, -14564, 0, -14101, 0, 4297, 0, 17082, 0, 7314, 0, -9572, 0, -11012, 0, -634, 0, 5263, 0, 6666, 0, 13488, 0, 20234, 0, 11005, 0, -9561, 0, -17734, 0, -7109, 0, 151, 0, -9824, 0, -20868, 0, -13483, 0, 4754, 0, 13504, 0, 12646, 0, 14139, 0, 15231, 0, 3776, 0, -13280, 0, -13770, 0, 3936, 0, 14438, 0, 4017, 0, -10817, 0, -9603, 0, -789, 0, -1440, 0, -8780, 0, -8129, 0, -1217, 0, -1672, 0, -5313, 0, 4700, 0, 25139, 0, 30712, 0, 11191, 0, -11183, 0, -14648, 0, -7456, 0, -7377, 0, -11835, 0, -7609, 0, 1232, 0, 450, 0, -5883, 0, -507, 0, 14234, 0, 15779, 0, -1388, 0, -14385, 0, -5372, 0, 11947, 0, 15812, 0, 8311, 0, 3691, 0, 988, 0, -10139, 0, -22774, 0, -18959, 0, -1898, 0, 5061, 0, -4204, 0, -8044, 0, 6709, 0, 23780, 0, 21745, 0, 7435, 0, -1100, 0, -3343, 0, -9742, 0, -15523, 0, -6580, 0, 10002, 0, 11153, 0, -6410, 0, -17802, 0, -8669, 0, 4647, 0, 3325, 0, -4571, 0, -2013, 0, 
    7848, 0, 10517, 0, 7893, 0, 12693, 0, 18642, 0, 7632, 0, -17799, 0, -30603, 0, -18007, 0, -501, 0, 2126, 0, -3445, 0, 92, 0, 8296, 0, 7439, 0, 1776, 0, 4855, 0, 12089, 0, 6444, 0, -9967, 0, -12888, 0, 4253, 0, 18251, 0, 9819, 0, -8271, 0, -15247, 0, -12824, 0, -13319, 0, -13725, 0, -1657, 0, 14227, 0, 15292, 0, 3846, 0, 1885, 0, 15134, 0, 21083, 0, 5418, 0, -15150, 0, -19012, 0, -10641, 0, -6183, 0, -4849, 0, 3154, 0, 10982, 0, 3648, 0, -12844, 0, -14500, 0, 2881, 0, 15450, 0, 8771, 0, -1853, 0, 2407, 0, 12669, 0, 11715, 0, 2223, 0, -2302, 0, -4344, 0, -15688, 0, -28371, 0, -21662, 0, 3408, 0, 19650, 0, 12861, 0, 959, 0, 2788, 0, 10518, 0, 7571, 0, -559, 0, -1054, 0, 2877, 0, -2801, 0, -11961, 0, -6917, 0, 9497, 0, 12303, 0, -6105, 0, -23128, 0, -18401, 0, -2491, 0, 5216, 0, 6979, 0, 13801, 0, 20137, 0, 12384, 0, -1600, 0, -3045, 0, 5781, 0, 3270, 0, -15605, 0, -27391, 0, -15532, 0, 3685, 0, 9384, 0, 5209, 0, 4883, 0, 5733, 0, -2121, 0, -10475, 0, -2923, 0, 14922, 0, 19002, 0, 3798, 0, -7669, 0, -1053, 0, 7625, 0, 594, 0, -13369, 0, -17278, 0, -12709, 0, -11741, 0, -9541, 0, 6257, 0, 25973, 0, 26348, 0, 6220, 0, -8081, 0, -2511, 0, 6736, 0, 1793, 0, -8661, 0, -8359, 0, -2096, 0, -3196, 0, -7308, 0, -1183, 0, 8595, 0, 2711, 0, -14991, 0, -19053, 0, 156, 0, 20492, 0, 21474, 0, 11282, 0, 7505, 0, 6297, 0, -3329, 0, -14394, 0, -12291, 0, -3514, 0, -6147, 0, -17864, 0, -15519, 0, 6188, 0, 22765, 0, 16348, 0, 1060, 0, -3457, 0, -159, 0, -1364, 0, -3038, 0, 5629, 0, 15713, 0, 0, 0,    
    /* xtpulse_t.dat */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 
    32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 32768, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
}; /* End of test values :  inpSignalROM[] */


#ifndef RANDOM_FUNCTION_GENERATOR
int main(void) {
    t_run_test(0, NULL);

    // stop the program
    asm("wfi");
    return 0;
}
#endif

/*  DECLARATIONS */    
static n_int   tableCount ;        /* Number of passes through table */

static varsize realData_1[NUM_POINTS] ;    /* Points to real part of data */
static varsize realData_2[NUM_POINTS] ;    /* Points to real part of data */
static varsize realData_3[NUM_POINTS] ;    /* Points to real part of data */
static varsize imagData_1[NUM_POINTS] ;    /* Points to imaginary part of data */
static varsize imagData_2[NUM_POINTS] ;    /* Points to imaginary part of data */
static varsize imagData_3[NUM_POINTS] ;    /* Points to imaginary part of data */

static int input_index;

static n_int GetInputValues( varsize *real, varsize *imag );


/* >> IMPORTANT NOTE << 
*
* Since benchmarks can be entered( run )multiple times, the benchmark
* MUST NOT depend on global data being initialized.  E.g. it must
* complelty initialize itself EVERY TIME its t_run_test()function
* is called.
* 
*/    

/*********************************************************************************
* FUNC	: int t_run_test( struct TCDef *tcdef,int argc, const char *argv[] )
* DESC  : This is the functions that carries out the algorithm. This function
*         is called from the main.  Same algorithm is called three times. Input
*         data for the algorithm is stored in 'algotst'c'.  All variables declared
*         and initialized in 'init.c'.  When 'BMDEBUG' and 'WINDOWS_EXAMPLE_CODE' 
*         defined in 'thcfg.h' it will write bunch of debug message in the RAM.  
*         All debug routines are in 'debug.c'. It calculates CRC using intermediate
*         values from the algorithms.  CRC is used to check if algorithm carried out
*         correctly.
* ARGUMENT: arc - not used
*           argv - not used
*           tcdef - structure defined in 'thlib.h'.  Following members of tcdef
*     			rec_iterations- recommend number of times algorithm should be carried out
*   			iterations  - actual number of times algorithm was carried
*                             most cases same as 'rec_iterations'
*               duration	- Total time to carry out the algorithm
*               CRC			- calculated CRC
*
* RETURNS : Success if the the CRC matches. 
*****************************************************************************************/    
static int t_run_test( int argc, const char *argv[] )
{    
    int iterations = 1;

    scanf("%d", &input_index);
    int inputs_count = sizeof(inpSignalROM) / sizeof(inpSignalROM[0]);
    if (input_index >= inputs_count) {
        printf("ERROR: input_index %d is out of range, max is %d\n", input_index, inputs_count-1);
        printf("stopping execution\n");
        asm volatile ("wfi");
    }

#if BMDEBUG
    char *szTitle = 
        "\nEEMBC Automotive/Industrial Subcommittee Benchmarks (c)1998-1999\n"
        "Algorithm 16 :  Fast Fourier Transform  Rev. 1.0A0 - aifftr01 \n" ; 
    char *szHeader = 
        "\n\nrealData, imagData, ( input signal, FFT, power & phase ), "
        "[pass count...], { loop counter }\n" ; 
    char szDebug[100] ; 
#endif /* BMDEBUG */
    n_char szDataSize[40] ; 
    /* Input test data table looped */
    n_int isTableLooped = false ;
    n_int bitRevInd[NUM_POINTS] ; 
    double trigArg ; 
    n_int index ;
    n_int brIndex ; 
    static varsize sineV[NUM_POINTS / 2] ;
    static varsize cosineV[NUM_POINTS / 2] ; 
    static varsize realBitRevData_1[NUM_POINTS] ;
    static varsize imagBitRevData_1[NUM_POINTS] ; 
    static varsize realBitRevData_2[NUM_POINTS] ;
    static varsize imagBitRevData_2[NUM_POINTS] ; 
    static varsize realBitRevData_3[NUM_POINTS] ;
    static varsize imagBitRevData_3[NUM_POINTS] ; 
    static varsize wReal_1 ;
    static varsize wImag_1 ;
    static varsize tRealData_1 ;
    static varsize tImagData_1 ; 
    static varsize wReal_2 ;
    static varsize wImag_2 ;
    static varsize tRealData_2 ;
    static varsize tImagData_2 ; 
    static varsize wReal_3 ;
    static varsize wImag_3 ;
    static varsize tRealData_3 ;
    static varsize tImagData_3 ; 
    static varsize *realLow_1 ;
    static varsize *imagLow_1 ;
    static varsize *realHi_1 ;
    static varsize *imagHi_1 ; 
    static varsize *realLow_2 ;
    static varsize *imagLow_2 ;
    static varsize *realHi_2 ;
    static varsize *imagHi_2 ; 
    static varsize *realLow_3 ;
    static varsize *imagLow_3 ;
    static varsize *realHi_3 ;
    static varsize *imagHi_3 ; 
    static n_long argIndex_1 ;
    static n_long deltaIndex_1 ; 
    static n_long argIndex_2 ;
    static n_long  deltaIndex_2 ; 
    static n_long argIndex_3 ;
    static n_long  deltaIndex_3 ; 
    static varsize n1_1 ;
    static varsize n2_1 ;
    static varsize l_1 ; 
    static varsize n1_2 ;
    static varsize n2_2 ;
    static varsize l_2 ; 
    static varsize n1_3 ;
    static varsize n2_3 ;
    static varsize l_3 ; 
    static varsize i_1 ;
    static varsize j_1 ;
    static varsize k_1 ;
    static varsize passCount_1 ; 
    static varsize i_2 ;
    static varsize j_2 ;
    static varsize k_2 ; 
    static varsize passCount_2 ; 
    static varsize i_3 ;
    static varsize j_3 ;
    static varsize k_3 ;
    static varsize passCount_3 ; 

    /* Unused */
    argc = argc ;
    argv = argv ;

    /*
     * First, initialize the data structures we need for the test
     * and allocate memory as needed.  Report an error if we can't.
     *
     */    

    /* Variable initializations at t=0 */    
		/* 
		 * BUG  145 in v1, realdata_2 is loaded, and unused. Pass 1 data is invalid
		 * If workloads unchanged, 
		 * Temporary fix should stabilize ports which do not initialize
		 * globals to 0 by default. VC performance results should be unchanged.
		 * for V2: isTableLooped = GetInputValues( realData_1, imagData_1 ) ;
		 * for V1.1: set realData_1 and imagData_1 to 0.
		 * 
		 */
	for( int loop_cnt = 0 ; loop_cnt < NUM_POINTS; loop_cnt++ )
	{
		realData_1[loop_cnt]=0;
		imagData_1[loop_cnt]=0;
	}
    tableCount = input_index ; 

#if BMDEBUG
    th_sprintf( szDataSize, "Data size = %s\n\n", 
        ( DATA_SIZE ? "LONG" : "SHORT" ) ) ; 
#else
    szDataSize[0] = (n_char)( '0' + DATA_SIZE ) ; 
    szDataSize[1] = '\0' ; 
#endif /* BMDEBUG */

    /* Populate the sine & cosine tables -- used by all instances */    
    for( i_1 = 0 ; i_1 < ( NUM_POINTS / 2 ) ; i_1++ )
    {        
        trigArg = (double)( i_1 * PI / ( NUM_POINTS / 2 ) ) ; 

        if( sin( trigArg ) == 1.0 )
        {
            sineV[i_1] = (n_long)( TRIG_SCALE_FACTOR - 1 ) ; 
        }
        else
        {

            sineV[i_1] = (n_long)( sin( trigArg ) * TRIG_SCALE_FACTOR ) ; 
        }

        if( cos( trigArg ) == 1.0 )
        {
            cosineV[i_1] = (n_long)( TRIG_SCALE_FACTOR - 1 ) ; 
        }
        else
        {
            cosineV[i_1] = (n_long)( cos( trigArg ) * TRIG_SCALE_FACTOR ) ; 
        }
    }

    /* Compute the bit reversal indicies  -- used by all the instances */    
    for( i_1 = 0 ; i_1 < NUM_POINTS ; i_1++ )
    {        
        index = i_1 ; 
        brIndex = 0 ; 
        for( j_1 = 0 ; j_1 < FFT_LENGTH ; j_1++ )
        {        
            brIndex <<= 1 ; 
            if( 0x01 &index )
            {
                brIndex |= 0x01 ; 
            }
            index >>= 1 ; 
        }
        bitRevInd[i_1] = brIndex ; 
    }

    // th_signal_start() ;

#if BMDEBUG
    DebugOut( szTitle ) ;     /* Print the title message in RAM file */
    DebugOut( szHeader ) ;    /* Print the test output file hdr in RAM file */
    DebugOut( szDataSize ) ;  /* ...and the data size */
#endif /* BMDEBUG */

    /*
     * This is the actual benchmark algorithm.
     *
     */    

    /* The FFT is a basic signal-processing function which can be applied to 
     * many test suites. Therefore, it is important to create an FFT benchmark
     * in the Automotive/Industrial suite which is representative of
     * "real-world" applications.  FFT processing in the automotive area 
     * might be applied in audio signal processing, specifically 
     * noise-cancellation processes, or speech recognition.  FFT in the 
     * automotive realm might also be used in signal processing for 
     * anti-collision systems.  FFT for industrial applications runs the 
     * gamut of resolution and frequency range.
     *
     * Compute the Radix 2 Decimation in Frequency iFFT on the complex input
     * values stored in the 'realData' and 'imagData' arrays.  Converts 
     * frequency-domain data to time-domain. Builds the sine and cosine
     * twiddle factors prior to execution of the iFFT loop. Also builds the
     * bit reversal indicies prior to execution of the loop. First, we do 
     * FFT, then compute power spectrum.
     *
     * This FFT is different than that used in the Telecom subcommittee.
     *
     */    

    for( int loop_cnt = 0 ; loop_cnt < iterations ; loop_cnt++ )
    {

#if BMDEBUG
        if( isTableLooped )
        {
            DebugOut( "END--END--END\n" ) ;
        }
#endif /* BMDEBUG */

        /***********************************************************************
            First Pass                                                          
        ***********************************************************************/

        /* Gets 'signal_in' value from test data*/
		/* 
		 * BUG  145 in v1, realdata_2 is loaded, and unused. Pass 1 data is invalid
		 * If workloads unchanged, 
		 * Temporary fix should stabilize ports which do not initialize
		 * globals to 0 by default. VC performance results should be unchanged.
		 * for V2: isTableLooped = GetInputValues( realData_1, imagData_1 ) ;
		 * for V1.1: set realData_1 and imagData_1 to 0.
		 * 
		 */
        isTableLooped = GetInputValues( realData_2, imagData_2 ) ;
        


#if BMDEBUG
        DebugOut( "Input signal : \n" ) ; 
        for( i_1 = 0 ; i_1 < NUM_POINTS ; i_1++ )
        { 
            th_sprintf( szDebug, "%8ld, %8ld\n", realData_1[i_1], 
                imagData_1[i_1] ) ; 
            DebugOut( szDebug ) ; 
        }
#endif /* BMDEBUG */

        /* Bit Reversal */    
        for( i_1 = 0 ; i_1 < NUM_POINTS ; i_1++ )
        {        
            realBitRevData_1[i_1] = realData_1[bitRevInd[i_1]] ; 
            imagBitRevData_1[i_1] = imagData_1[bitRevInd[i_1]] ; 
        }

        /* Return bit reversed data to input arrays */    
        for( i_1 = 0 ; i_1 < NUM_POINTS ; i_1++ )
        {        
            realData_1[i_1] = realBitRevData_1[i_1] ; 
            imagData_1[i_1] = imagBitRevData_1[i_1] ; 
        }

        /* FFT Computation */    

        /* Step through the stages */    
        for( passCount_1 = 0, k_1 = 1 ; 
        k_1 <= FFT_LENGTH ; 
        k_1++, passCount_1++ )
        {        
            n1_1 = 1 << k_1 ; 
            n2_1 = n1_1 >> 1 ; 
             
            /* Initialize twiddle factor lookup indicies */    
            argIndex_1 = 0 ; 
            deltaIndex_1 = ( NUM_POINTS / 2 ) / n2_1 ; 

            /* Step through the butterflies */    
            for( j_1 = 0 ; j_1 < n2_1 ; j_1++, passCount_1++ )
            {        
                /* Lookup twiddle factors */    
                wReal_1 = cosineV[argIndex_1] ; 
                wImag_1 = sineV[argIndex_1] ; 

                /* Process butterflies with the same twiddle factors */    
                for( i_1 = j_1 ; i_1 < NUM_POINTS ; i_1 += n1_1, passCount_1++ )
                {        
                    l_1 = i_1 + n2_1 ; 
                    realLow_1 = &realData_1[l_1] ; 
                    imagLow_1 = &imagData_1[l_1] ; 
                    realHi_1 = &realData_1[i_1] ; 
                    imagHi_1 = &imagData_1[i_1] ; 

                    /* Scale each stage to prevent overflow */    
                    *realLow_1 >>= STAGE_SCALE_FACTOR ; 
                    *imagLow_1 >>= STAGE_SCALE_FACTOR ; 
                    *realHi_1 >>= STAGE_SCALE_FACTOR ; 
                    *imagHi_1 >>= STAGE_SCALE_FACTOR ; 

                    tRealData_1 = *realLow_1 * wReal_1 - *imagLow_1 * wImag_1 ; 
                    tImagData_1 = *imagLow_1 * wReal_1 + *realLow_1 * wImag_1 ; 

                    /* Scale twiddle products to accomodate 
                     * 32bit accumulator */
                    tRealData_1 >>= BUTTERFLY_SCALE_FACTOR ; 
                    tImagData_1 >>= BUTTERFLY_SCALE_FACTOR ; 

                    realData_1[l_1] = *realHi_1 - tRealData_1 ; 
                    imagData_1[l_1] = *imagHi_1 - tImagData_1 ; 
                    realData_1[i_1] += tRealData_1 ; 
                    imagData_1[i_1] += tImagData_1 ; 
                }
                argIndex_1 += deltaIndex_1 ; 
            }

        } /* End of FFT loop */

#if BMDEBUG
        th_sprintf( szDebug, "[ %d ]\nFFT output : \n", passCount_1 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /* Display FFT spectrum */
        for( i_1 = 0 ; i_1 < NUM_POINTS ; i_1++ )
        {
#if BMDEBUG
            th_sprintf( szDebug, "%8ld, %8ld\n", (n_long)realData_1[i_1], 
                (n_long)imagData_1[i_1] ) ; 
            DebugOut( szDebug ) ; 
#else
            WriteOut( realData_1[i_1] ) ; 
            WriteOut( imagData_1[i_1] ) ; 
#endif
        }

        /* Compute power spectrum */    
        for( i_1 = 0 ; i_1 < NUM_POINTS ; i_1++ )
        {        
            /* This can all easily overflow! */
            realData_1[i_1] *= realData_1[i_1] ;
            realData_1[i_1] += imagData_1[i_1] * imagData_1[i_1] ; 
        }

#if BMDEBUG
        DebugOut( "Power spectrum : \n" ) ; 
#endif /* BMDEBUG */

        for( i_1 = 0 ; i_1 < NUM_POINTS ; i_1++ )
        {
            /* Display power spectrum and phase */
#if BMDEBUG
            th_sprintf( szDebug, "%8ld\n", (n_long)realData_1[i_1] ) ; 
            DebugOut( szDebug ) ; 
#else
            WriteOut( realData_1[i_1] ) ; 
#endif
        }

        /***********************************************************************
            Second Pass                                                         
        ***********************************************************************/

        isTableLooped += GetInputValues( realData_2, imagData_2 ) ;

#if BMDEBUG
        DebugOut( "Input signal : \n" ) ; 
        for( i_2 = 0 ; i_2 < NUM_POINTS ; i_2++ )
        { 
            th_sprintf( szDebug, "%8ld, %8ld\n", realData_2[i_2], 
                imagData_2[i_2] ) ; 
            DebugOut( szDebug ) ; 
        }
#endif /* BMDEBUG */

        /* Bit Reversal */    
        for( i_2 = 0 ; i_2 < NUM_POINTS ; i_2++ )
        {        
            realBitRevData_2[i_2] = realData_2[bitRevInd[i_2]] ; 
            imagBitRevData_2[i_2] = imagData_2[bitRevInd[i_2]] ; 
        }

        /* Return bit reversed data to input arrays */    
        for( i_2 = 0 ; i_2 < NUM_POINTS ; i_2++ )
        {        
            realData_2[i_2] = realBitRevData_2[i_2] ; 
            imagData_2[i_2] = imagBitRevData_2[i_2] ; 
        }

        /* FFT Computation */    

        /* Step through the stages */    
        for( passCount_2 = 0, k_2 = 1 ; 
        k_2 <= FFT_LENGTH ; 
        k_2++, passCount_2++ )
        {        
            n1_2 = 1 << k_2 ; 
            n2_2 = n1_2 >> 1 ; 
             
            /* Initialize twiddle factor lookup indicies */    
            argIndex_2 = 0 ; 
            deltaIndex_2 = ( NUM_POINTS / 2 ) / n2_2 ; 

            /* Step through the butterflies */    
            for( j_2 = 0 ; j_2 < n2_2 ; j_2++, passCount_2++ )
            {        
                /* Lookup twiddle factors */    
                wReal_2 = cosineV[argIndex_2] ; 
                wImag_2 = sineV[argIndex_2] ; 

                /* Process butterflies with the same twiddle factors */    
                for( i_2 = j_2 ; i_2 < NUM_POINTS ; i_2 += n1_2, passCount_2++ )
                {        
                    l_2 = i_2 + n2_2 ; 
                    realLow_2 = &realData_2[l_2] ; 
                    imagLow_2 = &imagData_2[l_2] ; 
                    realHi_2 = &realData_2[i_2] ; 
                    imagHi_2 = &imagData_2[i_2] ; 

                    /* Scale each stage to prevent overflow */    
                    *realLow_2 >>= STAGE_SCALE_FACTOR ; 
                    *imagLow_2 >>= STAGE_SCALE_FACTOR ; 
                    *realHi_2 >>= STAGE_SCALE_FACTOR ; 
                    *imagHi_2 >>= STAGE_SCALE_FACTOR ; 

                    tRealData_2 = *realLow_2 * wReal_2 - *imagLow_2 * wImag_2 ; 
                    tImagData_2 = *imagLow_2 * wReal_2 + *realLow_2 * wImag_2 ; 

                    /* Scale twiddle products to accomodate 
                     * 32bit accumulator */
                    tRealData_2 >>= BUTTERFLY_SCALE_FACTOR ; 
                    tImagData_2 >>= BUTTERFLY_SCALE_FACTOR ; 

                    realData_2[l_2] = *realHi_2 - tRealData_2 ; 
                    imagData_2[l_2] = *imagHi_2 - tImagData_2 ; 
                    realData_2[i_2] += tRealData_2 ; 
                    imagData_2[i_2] += tImagData_2 ; 
                }
                argIndex_2 += deltaIndex_2 ; 
            }

        } /* End of FFT loop */

#if BMDEBUG
        th_sprintf( szDebug, "[ %d ]\nFFT output : \n", passCount_2 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /* Display FFT spectrum */
        for( i_2 = 0 ; i_2 < NUM_POINTS ; i_2++ )
        {
#if BMDEBUG
            th_sprintf( szDebug, "%8ld, %8ld\n", (n_long)realData_2[i_2], 
                (n_long)imagData_2[i_2] ) ; 
            DebugOut( szDebug ) ; 
#else
            WriteOut( realData_2[i_2] ) ; 
            WriteOut( imagData_2[i_2] ) ; 
#endif
        }

        /* Compute power spectrum */    
        for( i_2 = 0 ; i_2 < NUM_POINTS ; i_2++ )
        {        
            /* This can all easily overflow! */
            realData_2[i_2] *= realData_2[i_2] ;
            realData_2[i_2] += imagData_2[i_2] * imagData_2[i_2] ; 
        }

#if BMDEBUG
        DebugOut( "Power spectrum : \n" ) ; 
#endif /* BMDEBUG */

        for( i_2 = 0 ; i_2 < NUM_POINTS ; i_2++ )
        {
            /* Display power spectrum and phase */
#if BMDEBUG
            th_sprintf( szDebug, "%8ld\n", (n_long)realData_2[i_2] ) ; 
            DebugOut( szDebug ) ; 
#else
            WriteOut( realData_2[i_2] ) ; 
#endif
        }

        /***********************************************************************
            Third Pass                                                          
        ***********************************************************************/

        isTableLooped += GetInputValues( realData_3, imagData_3 ) ;

#if BMDEBUG
        DebugOut( "Input signal : \n" ) ; 
        for( i_3 = 0 ; i_3 < NUM_POINTS ; i_3++ )
        { 
            th_sprintf( szDebug, "%8ld, %8ld\n", realData_3[i_3], 
                imagData_3[i_3] ) ; 
            DebugOut( szDebug ) ; 
        }
#endif /* BMDEBUG */

        /* Bit Reversal */    
        for( i_3 = 0 ; i_3 < NUM_POINTS ; i_3++ )
        {        
            realBitRevData_3[i_3] = realData_3[bitRevInd[i_3]] ; 
            imagBitRevData_3[i_3] = imagData_3[bitRevInd[i_3]] ; 
        }

        /* Return bit reversed data to input arrays */    
        for( i_3 = 0 ; i_3 < NUM_POINTS ; i_3++ )
        {        
            realData_3[i_3] = realBitRevData_3[i_3] ; 
            imagData_3[i_3] = imagBitRevData_3[i_3] ; 
        }

        /* FFT Computation */    

        /* Step through the stages */    
        for( passCount_3 = 0, k_3 = 1 ; 
        k_3 <= FFT_LENGTH ; 
        k_3++, passCount_3++ )
        {        
            n1_3 = 1 << k_3 ; 
            n2_3 = n1_3 >> 1 ; 
             
            /* Initialize twiddle factor lookup indicies */    
            argIndex_3 = 0 ; 
            deltaIndex_3 = ( NUM_POINTS / 2 ) / n2_3 ; 

            /* Step through the butterflies */    
            for( j_3 = 0 ; j_3 < n2_3 ; j_3++, passCount_3++ )
            {        
                /* Lookup twiddle factors */    
                wReal_3 = cosineV[argIndex_3] ; 
                wImag_3 = sineV[argIndex_3] ; 

                /* Process butterflies with the same twiddle factors */    
                for( i_3 = j_3 ; i_3 < NUM_POINTS ; i_3 += n1_3, passCount_3++ )
                {        
                    l_3 = i_3 + n2_3 ; 
                    realLow_3 = &realData_3[l_3] ; 
                    imagLow_3 = &imagData_3[l_3] ; 
                    realHi_3 = &realData_3[i_3] ; 
                    imagHi_3 = &imagData_3[i_3] ; 

                    /* Scale each stage to prevent overflow */    
                    *realLow_3 >>= STAGE_SCALE_FACTOR ; 
                    *imagLow_3 >>= STAGE_SCALE_FACTOR ; 
                    *realHi_3 >>= STAGE_SCALE_FACTOR ; 
                    *imagHi_3 >>= STAGE_SCALE_FACTOR ; 

                    tRealData_3 = *realLow_3 * wReal_3 - *imagLow_3 * wImag_3 ; 
                    tImagData_3 = *imagLow_3 * wReal_3 + *realLow_3 * wImag_3 ; 

                    /* Scale twiddle products to accomodate 
                     * 32bit accumulator */
                    tRealData_3 >>= BUTTERFLY_SCALE_FACTOR ; 
                    tImagData_3 >>= BUTTERFLY_SCALE_FACTOR ; 

                    realData_3[l_3] = *realHi_3 - tRealData_3 ; 
                    imagData_3[l_3] = *imagHi_3 - tImagData_3 ; 
                    realData_3[i_3] += tRealData_3 ; 
                    imagData_3[i_3] += tImagData_3 ; 
                }
                argIndex_3 += deltaIndex_3 ; 
            }

        } /* End of FFT loop */

#if BMDEBUG
        th_sprintf( szDebug, "[ %d ]\nFFT output : \n", passCount_3 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /* Display FFT spectrum */
        for( i_3 = 0 ; i_3 < NUM_POINTS ; i_3++ )
        {
#if BMDEBUG
            th_sprintf( szDebug, "%8ld, %8ld\n", (n_long)realData_3[i_3], 
                (n_long)imagData_3[i_3] ) ; 
            DebugOut( szDebug ) ; 
#else
            WriteOut( realData_3[i_3] ) ; 
            WriteOut( imagData_3[i_3] ) ; 
#endif
        }

        /* Compute power spectrum */    
        for( i_3 = 0 ; i_3 < NUM_POINTS ; i_3++ )
        {        
            /* This can all easily overflow! */
            realData_3[i_3] *= realData_3[i_3] ;
            realData_3[i_3] += imagData_3[i_3] * imagData_3[i_3] ; 
        }

#if BMDEBUG
        DebugOut( "Power spectrum : \n" ) ; 
#endif /* BMDEBUG */

        for( i_3 = 0 ; i_3 < NUM_POINTS ; i_3++ )
        {
            /* Display power spectrum and phase */
#if BMDEBUG
            th_sprintf( szDebug, "%8ld\n", (n_long)realData_3[i_3] ) ; 
            DebugOut( szDebug ) ; 
#else
            WriteOut( realData_3[i_3] ) ; 
#endif
        }

#if BMDEBUG
        th_sprintf( szDebug, "{ %08lX }\n", (n_ulong)loop_cnt ) ; 
        DebugOut( szDebug ) ; 
#else        
#if DATA_SIZE == 0        
        i_3 = (varsize)( loop_cnt &0x0000FFFF ) ; 
        WriteOut( i_3 ) ;
        i_3 = (varsize)( loop_cnt >> 16 ) ; 
        WriteOut( i_3 ) ; 
#else
        WriteOut( (varsize)loop_cnt ) ; 
#endif
        i_3 = (varsize)0xAAAA ; 
        WriteOut( i_3 ) ;
#endif /* BMDEBUG */

    }


	return 0;
} 


static n_int GetInputValues( varsize *real, varsize *imag )
{        
    n_int i ; 
    
/* Fill the real and imaginary sample with test data */    
    for( i = 0 ; i < NUM_POINTS ; i++ )
    {        
        *( real + i ) = inpSignalROM[tableCount++ ] ; 
        *( imag + i ) = inpSignalROM[tableCount++ ] ; 
    }
   
 /* If you run out of test data, then start table over */    
    if( tableCount < NUM_TESTS )
    {
        return 0 ; 
    }
   
 tableCount = input_index ; 
    
    return 1 ; 

} /* End of function 'GetInputValues' */
