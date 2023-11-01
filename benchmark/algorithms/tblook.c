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
#include "tblook.h" // change to the name of the main file (eg. "a2time.h")

// forward declarations
static int t_run_test( int argc, const char *argv[] );
static n_int GetInputValues( n_void );

static const varsize engLoadROM[] = {
    0, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 
    150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 
    270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 
    390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500
} ;

static const varsize engSpeedROM[] = {
    0, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 
    1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200,
    2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200,
    3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200,
    4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000
} ;

static const varsize angleTableROM[] = {
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 70, 80, 90, 100, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 100, 90, 80, 70, 60, 50, 40, 39, 38, 37, 36, 35, 35, 35, 35, 35, 35, 60,
    45, 50, 50, 55, 55, 56, 56, 56, 56, 56, 56, 50, 50, 50, 50, 50, 50, 50, 60, 70, 80, 90, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 103, 90, 80, 72, 66, 60, 52, 44, 38, 32, 28, 27, 35, 35, 35, 35, 35, 60,
    30, 40, 40, 50, 54, 54, 54, 54, 54, 54, 54, 40, 40, 40, 40, 40, 40, 40, 50, 60, 70, 86, 90, 91, 92, 91, 90, 89, 89, 90, 92, 94, 94, 91, 74, 64, 63, 62, 54, 46, 40, 34, 29, 28, 35, 35, 35, 35, 35, 60,
    15, 30, 30, 40, 50, 51, 51, 51, 51, 51, 51, 30, 30, 40, 50, 60, 70, 76, 80, 86, 90, 94, 98, 99, 100, 99, 98, 97, 97, 98, 100, 102, 102, 92, 82, 72, 70, 64, 56, 48, 42, 36, 30, 29, 35, 35, 35, 35, 35, 60,
    10, 20, 20, 30, 40, 50, 50, 50, 50, 50, 50, 56, 62, 68, 74, 80, 84, 88, 92, 96, 100, 104, 108, 109, 110, 109, 108, 107, 107, 108, 110, 112, 112, 102, 92, 82, 72, 67, 59, 51, 47, 43, 41, 39, 37, 37, 37, 37, 37, 60,
    10, 20, 20, 30, 40, 50, 50, 50, 50, 50, 50, 56, 62, 68, 74, 80, 86, 92, 98, 102, 106, 110, 114, 115, 116, 115, 114, 113, 113, 114, 116, 118, 118, 108, 98, 88, 80, 70, 62, 54, 49, 45, 43, 41, 39, 39, 39, 39, 39, 100,
    12, 20, 20, 30, 40, 50, 50, 50, 50, 50, 50, 56, 62, 68, 74, 80, 86, 92, 98, 104, 108, 112, 116, 117, 118, 117, 116, 115, 115, 116, 118, 120, 120, 110, 100, 90, 80, 72, 64, 56, 52, 48, 46, 44, 42, 42, 42, 42, 42, 100,
    15, 20, 20, 30, 40, 50, 50, 50, 50, 50, 50, 56, 62, 68, 74, 80, 86, 92, 98, 104, 108, 112, 116, 117, 118, 117, 116, 115, 115, 116, 118, 120, 120, 110, 100, 90, 78, 75, 67, 59, 54, 49, 47, 45, 43, 43, 43, 43, 43, 100,
    20, 20, 20, 30, 40, 50, 50, 50, 50, 50, 50, 56, 62, 68, 74, 80, 86, 92, 98, 103, 107, 111, 115, 116, 117, 116, 115, 114, 114, 115, 117, 119, 119, 109, 99, 88, 78, 75, 67, 59, 54, 49, 47, 45, 43, 43, 43, 43, 43, 100,
    22, 22, 22, 32, 42, 52, 52, 52, 52, 52, 52, 58, 64, 70, 76, 84, 90, 96, 102, 102, 106, 110, 114, 115, 116, 115, 114, 113, 113, 114, 116, 118, 118, 108, 98, 88, 78, 75, 70, 59, 54, 49, 47, 45, 43, 43, 43, 43, 43, 100,
    24, 24, 24, 34, 44, 54, 54, 54, 54, 54, 54, 60, 66, 72, 78, 84, 90, 96, 102, 104, 108, 112, 116, 117, 118, 117, 116, 115, 115, 116, 118, 120, 120, 110, 100, 86, 86, 78, 75, 67, 59, 54, 49, 47, 44, 44, 44, 44, 44, 100,
    30, 30, 30, 40, 50, 60, 60, 60, 60, 60, 60, 66, 72, 78, 84, 90, 96, 102, 108, 112, 116, 120, 124, 125, 126, 125, 124, 123, 123, 124, 124, 122, 120, 118, 108, 88, 88, 78, 75, 67, 59, 54, 49, 47, 44, 44, 44, 44, 44, 100,
    30, 30, 30, 40, 50, 60, 60, 60, 60, 60, 60, 66, 72, 78, 84, 90, 96, 102, 116, 120, 124, 128, 132, 133, 134, 133, 132, 131, 131, 132, 130, 127, 124, 121, 116, 92, 92, 76, 68, 60, 56, 52, 50, 48, 46, 46, 46, 46, 46, 100,
    30, 30, 30, 40, 50, 60, 60, 60, 60, 60, 60, 66, 72, 78, 84, 90, 96, 104, 118, 122, 126, 130, 134, 135, 136, 135, 134, 133, 133, 133, 131, 128, 107, 106, 104, 92, 92, 76, 68, 60, 56, 52, 50, 48, 46, 46, 46, 46, 46, 100,
    31, 31, 31, 41, 51, 61, 61, 61, 61, 61, 61, 67, 73, 79, 85, 91, 97, 103, 120, 124, 128, 132, 136, 137, 138, 137, 136, 135, 134, 134, 132, 129, 107, 106, 104, 92, 92, 76, 68, 60, 56, 52, 50, 48, 46, 46, 46, 46, 46, 100,
    33, 33, 33, 43, 53, 63, 63, 63, 63, 63, 63, 69, 75, 81, 87, 93, 99, 105, 122, 126, 130, 134, 138, 139, 140, 139, 138, 137, 135, 135, 133, 130, 108, 107, 104, 94, 94, 77, 72, 66, 60, 72, 75, 70, 66, 66, 66, 66, 66, 100,
    35, 35, 35, 45, 55, 65, 65, 65, 65, 65, 65, 71, 77, 83, 88, 96, 100, 107, 124, 128, 132, 136, 140, 141, 142, 141, 140, 139, 136, 136, 134, 131, 108, 107, 105, 94, 94, 77, 76, 72, 70, 92, 100, 105, 86, 86, 86, 86, 86, 100,
    35, 35, 35, 45, 55, 65, 65, 65, 65, 65, 65, 67, 69, 71, 77, 83, 89, 95, 126, 130, 134, 138, 142, 143, 144, 143, 142, 141, 137, 137, 135, 132, 109, 108, 107, 106, 105, 105, 104, 108, 112, 116, 120, 120, 106, 106, 106, 106, 106, 100,
    35, 35, 35, 45, 55, 65, 65, 65, 65, 65, 65, 65, 65, 65, 71, 81, 91, 101, 127, 131, 135, 139, 143, 144, 145, 144, 144, 142, 138, 138, 136, 133, 131, 127, 125, 122, 120, 125, 130, 135, 140, 145, 140, 108, 116, 116, 116, 116, 116, 100,
    38, 38, 38, 48, 58, 68, 68, 68, 68, 68, 68, 68, 68, 68, 74, 84, 94, 104, 128, 132, 136, 140, 143, 145, 146, 145, 145, 143, 139, 139, 137, 134, 130, 126, 124, 126, 130, 135, 140, 145, 150, 155, 150, 140, 120, 120, 120, 120, 120, 100,
    41, 41, 41, 51, 61, 71, 71, 71, 71, 71, 71, 71, 71, 71, 77, 87, 97, 107, 129, 133, 137, 141, 143, 146, 147, 146, 146, 144, 140, 140, 138, 135, 129, 125, 123, 122, 124, 129, 134, 139, 144, 149, 139, 138, 122, 122, 122, 122, 122, 100,
    44, 44, 44, 54, 64, 74, 74, 74, 74, 74, 74, 74, 74, 74, 80, 90, 100, 110, 130, 134, 138, 142, 144, 147, 148, 147, 147, 145, 141, 141, 139, 134, 128, 124, 122, 122, 122, 127, 132, 137, 142, 147, 137, 136, 124, 124, 124, 124, 124, 100,
    47, 47, 47, 57, 67, 77, 77, 77, 77, 77, 77, 77, 77, 77, 87, 97, 107, 117, 131, 135, 139, 143, 144, 148, 149, 148, 148, 146, 142, 143, 140, 133, 127, 123, 121, 120, 121, 126, 131, 136, 141, 146, 136, 134, 126, 126, 126, 126, 126, 100,
    45, 45, 45, 55, 65, 75, 75, 75, 75, 75, 75, 75, 78, 88, 98, 108, 118, 128, 138, 142, 144, 144, 144, 149, 150, 149, 149, 145, 143, 145, 141, 132, 126, 122, 120, 119, 120, 125, 130, 135, 140, 145, 135, 132, 130, 130, 130, 130, 130, 100,
    43, 43, 43, 53, 63, 73, 73, 73, 73, 73, 73, 76, 79, 89, 109, 108, 118, 128, 138, 142, 144, 144, 144, 149, 150, 149, 149, 145, 143, 153, 142, 131, 125, 121, 120, 119, 118, 117, 116, 115, 114, 144, 139, 134, 136, 136, 136, 136, 136, 100,
    42, 42, 42, 52, 62, 72, 72, 72, 72, 72, 72, 75, 78, 88, 98, 108, 118, 128, 138, 142, 144, 144, 144, 142, 142, 138, 148, 143, 144, 142, 142, 130, 124, 124, 125, 125, 124, 123, 123, 123, 128, 143, 133, 128, 142, 142, 142, 142, 142, 100,
    41, 41, 41, 51, 61, 71, 71, 71, 71, 71, 71, 74, 77, 87, 97, 107, 117, 127, 137, 141, 143, 143, 143, 141, 141, 137, 146, 142, 133, 141, 141, 129, 123, 125, 130, 129, 128, 127, 127, 132, 137, 142, 132, 126, 148, 148, 148, 148, 148, 100,
    40, 40, 40, 50, 60, 70, 70, 70, 70, 70, 70, 73, 76, 86, 96, 106, 116, 126, 136, 140, 142, 142, 142, 140, 140, 136, 142, 141, 136, 140, 140, 139, 137, 135, 133, 131, 130, 129, 128, 132, 137, 142, 131, 125, 142, 142, 142, 142, 142, 100,
    46, 46, 46, 56, 66, 76, 76, 76, 76, 76, 76, 84, 92, 100, 108, 116, 124, 132, 140, 144, 146, 146, 146, 144, 144, 140, 140, 140, 140, 144, 140, 139, 137, 135, 133, 131, 130, 129, 128, 127, 126, 141, 131, 124, 136, 136, 136, 136, 136, 100,
    50, 50, 50, 60, 70, 80, 80, 80, 80, 80, 80, 88, 96, 104, 112, 120, 128, 136, 144, 148, 150, 150, 150, 148, 148, 144, 138, 139, 144, 148, 144, 143, 141, 139, 137, 135, 134, 133, 132, 131, 130, 141, 130, 123, 130, 130, 130, 130, 130, 100,
    54, 54, 54, 64, 74, 84, 84, 84, 84, 84, 84, 92, 100, 108, 116, 124, 132, 140, 148, 152, 154, 154, 154, 152, 152, 148, 136, 140, 148, 152, 148, 147, 145, 143, 141, 139, 138, 137, 136, 135, 134, 140, 130, 122, 126, 126, 126, 126, 126, 100,
    58, 58, 58, 68, 78, 88, 88, 88, 88, 88, 88, 96, 104, 112, 120, 128, 136, 144, 152, 156, 158, 158, 158, 156, 156, 152, 136, 144, 152, 156, 152, 151, 149, 147, 145, 143, 142, 141, 140, 139, 138, 134, 130, 121, 124, 124, 124, 124, 124, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 133, 129, 120, 122, 122, 122, 122, 122, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 132, 129, 128, 120, 120, 120, 120, 120, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 131, 130, 129, 123, 123, 123, 123, 123, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 130, 132, 131, 126, 126, 126, 126, 126, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 141, 134, 133, 130, 130, 130, 130, 130, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 141, 136, 135, 130, 130, 130, 130, 130, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 141, 138, 137, 130, 130, 130, 130, 130, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 141, 140, 139, 130, 130, 130, 130, 130, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 141, 140, 139, 130, 130, 130, 130, 130, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 141, 140, 139, 130, 130, 130, 130, 130, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 141, 140, 139, 130, 130, 130, 130, 130, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 141, 140, 139, 130, 130, 130, 130, 130, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 141, 140, 139, 130, 130, 130, 130, 130, 100,
    62, 62, 62, 72, 82, 92, 92, 92, 92, 92, 92, 100, 108, 116, 124, 132, 140, 148, 156, 160, 162, 162, 162, 160, 160, 156, 140, 148, 156, 160, 156, 155, 153, 151, 149, 147, 146, 145, 144, 143, 142, 141, 140, 139, 130, 130, 130, 130, 130, 100
} ; /* End of angleTableROM[] */

static const varsize inpValueROM[] = {
    0, 0, 999, 9999, 10, 100, 20, 200, 30, 300, 40, 400, 50, 500, 60, 600,
    70, 700, 80, 800, 90, 900, 100, 1000, 110, 1100, 120, 1200, 130, 1300,
    140, 1400, 150, 1500, 160, 1600, 170, 1700, 180, 1800, 190, 1900, 200, 2000,
    210, 2100, 220, 2200, 230, 2300, 240, 2400, 250, 2500, 260, 2600, 270, 2700,
    280, 2800, 290, 2900, 300, 3000,
    310, 3100, 320, 3200, 330, 3300, 340, 3400, 350, 3500, 360, 3600, 370, 3700,
    380, 3800, 390, 3900, 400, 4000,
    410, 4100, 420, 4200, 430, 4300, 440, 4400, 450, 4500, 460, 4600, 470, 4700,
    480, 4800, 490, 4900, 500, 5000,

/*  Read out the end column */

    500, 0, 500, 200, 500, 300,
    500, 400, 500, 500, 500, 600, 500, 700, 500, 800, 500, 900, 500, 1000,
    500, 1100, 500, 1200, 500, 1300,
    500, 1400, 500, 1500, 500, 1600, 500, 1700, 500, 1800, 500, 1900, 500, 2000,
    500, 2100, 500, 2200, 500, 2300, 500, 2400, 500, 2500, 500, 2600, 500, 2700,
    500, 2800, 500, 2900, 500, 3000,
    500, 3100, 500, 3200, 500, 3300, 500, 3400, 500, 3500, 500, 3600, 500, 3700,
    500, 3800, 500, 3900, 500, 4000,
    500, 4100, 500, 4200, 500, 4300, 500, 4400, 500, 4500, 500, 4600, 500, 4700,
    500, 4800, 500, 4900, 500, 5000,
    
/*  Read out a column in the middle */

    160, 0, 160, 200, 160, 300,
    160, 400, 160, 160, 160, 600, 160, 700, 160, 800, 160, 900, 160, 1000,
    160, 1100, 160, 1200, 160, 1300,
    160, 1400, 160, 1160, 160, 1600, 160, 1700, 160, 1800, 160, 1900, 160, 2000,
    160, 2100, 160, 2200, 160, 2300, 160, 2400, 160, 2160, 160, 2600, 160, 2700,
    160, 2800, 160, 2900, 160, 3000,
    160, 3100, 160, 3200, 160, 3300, 160, 3400, 160, 3160, 160, 3600, 160, 3700,
    160, 3800, 160, 3900, 160, 4000,
    160, 4100, 160, 4200, 160, 4300, 160, 4400, 160, 4160, 160, 4600, 160, 4700,
    160, 4800, 160, 4900, 160, 5000,
    
/*  Read out the end row */

    0, 5000, 20, 5000, 30, 5000, 40, 5000, 50, 5000, 60, 5000,
    70, 5000, 80, 5000, 90, 5000, 100, 5000, 110, 5000, 120, 5000, 130, 5000,
    140, 5000, 150, 5000, 160, 5000, 170, 5000, 180, 5000, 190, 5000, 200, 5000,
    210, 5000, 220, 5000, 230, 5000, 240, 5000, 250, 5000, 260, 5000, 270, 5000,
    280, 5000, 290, 5000, 300, 5000,
    310, 5000, 320, 5000, 330, 5000, 340, 5000, 350, 5000, 360, 5000, 370, 5000,
    380, 5000, 390, 5000, 400, 5000,
    410, 5000, 420, 5000, 430, 5000, 440, 5000, 450, 5000, 460, 5000, 470, 5000,
    480, 5000, 490, 5000, 500, 5000,

/* Reveal a range of values in a small "grid" via interpolation */

    160, 2100, 161, 2100, 162, 2100, 163, 2100,
    164, 2100, 165, 2100, 166, 2100, 167, 2100, 168, 2100, 169, 2100, 170, 2100,
    170, 2101, 170, 2102, 170, 2103, 170, 2104, 170, 2105, 170, 2106, 170, 2107,
    170, 2108, 170, 2109, 170, 2110, 170, 2120, 170, 2130, 170, 2140, 170, 2150,
    170, 2160, 170, 2170, 170, 2180, 170, 2190, 170, 2200
} ; /* End of test values:  inpValueROM[] */



#ifndef RANDOM_FUNCTION_GENERATOR
int main(void) {
    t_run_test(0, NULL);

    // stop the program
    asm("wfi");
    return 0;
}
#endif

static const varsize engLoadROM[] ;      /* Linear array of 'y' axis */
static const varsize engSpeedROM[] ;     /* Linear array of 'x' axis */
static const varsize angleTableROM[] ;   /* 2D array of f( x, y )= 'angle' */

static varsize numXEntries ;           /* The table 'x' size spec'd here */
static varsize numYEntries ;           /* The table 'y' size spec'd here */

static n_int   *RAMfile ;              /* Pointer to test output RAM file */
static n_int   *RAMfilePtr ;           /* Pointer to position in output RAM file */
static n_int   RAMfileSize ;       /* Size of the debug output RAM file */
static n_int   tableCount ;        /* Number of passes through table */
static n_int   *RAMfileEOF;        /* points to end of RAM file */
static n_int   RAMfile_increment;  /* difference between varsize and n_int */

static varsize loadValue ;             /* 'Load' pulled from test data */
static varsize speedValue ;            /* 'Speed' pulled from test data */

static const varsize *engSpeed ;       /* Linear array of 'y' axis */
static const varsize *engLoad ;        /* Linear array of 'x' axis */
static const varsize *angleTable ;     /* 2D array of f( x, y )= 'angle' */

static int input_index;

static int t_run_test(int argc, const char *argv[] ) {    
    int iterations = 1;
    scanf("%d", &input_index);
    int inputs_count = NUM_TESTS;
    if (input_index >= inputs_count) {
        printf("ERROR: input_index %d is out of range, max is %d\n", input_index, inputs_count-1);
        printf("stopping execution\n");
        asm volatile ("wfi");
    }

#if BMDEBUG
    char *szTitle = 
        "\nEEMBC Automotive/Industrial Subcommittee Benchmarks ( c )1998-1999\n"
        "Algorithm 1 :  Table Lookup & Interpolation Rev. 1.0\n" ; 
    char *szHeader = 
        "\n\nload, speed, engLoadLow, engLoadHi, engSpeedLow, engSpeedHi, "
        " engLoadDelta, engSpeedDelta, angle, counter\n" ; 
    char szDebug[100] ; 
#endif /* BMDEBUG */

    static n_char szDataSize[40] ; 
    varsize i1, i2, i3, j1, j2, j3 ;    /* Local indices and counters */
    static varsize outAngleValue1 ;     /* 'Angle' pulled from map */
    static n_float engSpeedDelta1 ;       /* Intermediate ratios from grid */
    static n_float engLoadDelta1 ;
    static varsize outAngleValue2 ;     /* 'Angle' pulled from map */
    static n_float engSpeedDelta2 ;       /* Intermediate ratios from grid */
    static n_float engLoadDelta2 ;
    static varsize outAngleValue3 ;     /* 'Angle' pulled from map */
    static n_float engSpeedDelta3 ;       /* Intermediate ratios from grid */
    static n_float engLoadDelta3 ;    
    n_int isTableLooped = false ;         /* Input test data table looped */
    tableCount = 0 ;    /* Start out at beginning of input test data */

    /* Unused */
    argc = argc ;
    argv = argv ;

    /*
     * First, initialize the data structures we need for the test
     * and allocate memory as needed.  Report an error if we can't.
     */

    /* Tell us the compiled data size */
#if BMDEBUG
    th_sprintf( szDataSize, "Data size = %s\n\n", 
        ( DATA_SIZE ? "LONG" : "SHORT" ) ) ; 
#else
    szDataSize[0] = (char)( '0' + DATA_SIZE ) ; 
    szDataSize[1] = '\0' ; 
#endif /* BMDEBUG */

    numXEntries = NUM_X_ENTRIES ;   /* ...specify the num of table entries */
    numYEntries = NUM_Y_ENTRIES ; 
    engSpeed = engSpeedROM ;        /* Point to the constant linear tables */
    engLoad = engLoadROM ; 
    angleTable = angleTableROM ;    /* ...and to the 3D table */


#if BMDEBUG
    DebugOut( szTitle ) ;           /* Print the title message in RAM file */
    DebugOut( szDataSize ) ;        /* ...and the data size */
    DebugOut( szHeader ) ;          /* Print the output file hdr in RAM file */
#endif /* BMDEBUG */

    /* This is the actual benchmark algorithm.
     *
     * Perform table interpolation using 'bilinear interpolation'
     * technique.  This technique determines the interpolated value by
     * using the four points in a grid which surrounds the desired point, 
     * essentially defining a planar surface, and presumes that the resulting 
     * linear approximation of the point in the plane is close enough to the 
     * actual value to be useful.  However, this technique does not account 
     * for the curvature of a surface and suffers from step-changes at grid 
     * boundaries. This technique is illustrated in 
     *      "Numerical Recipes in C", pp. 104-105.
     */
    for( int loop_cnt = 0 ; loop_cnt < iterations ; loop_cnt++ ) /* no stopping! */
    {
        /* 'engine load' and 'engine speed' index us into the 'angle' table */
#if BMDEBUG
        if( isTableLooped ) 
        {
            DebugOut( "END--END--END\n" ) ;        
        }
#endif /* BMDEBUG */

        /***********************************************************************
            First Pass                                                          
        ***********************************************************************/
        /* Gets 'loadValue' and 'speedValue' */
        isTableLooped = GetInputValues() ;  

#if BMDEBUG
        th_sprintf( szDebug, "%4ld, %4ld", (long)loadValue, (long)speedValue ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*  First, find the 'x' axis grid coefficients */
        for( i1 = 0 ; i1 < ( numXEntries - 1 ) ; i1++ )
        {
            if( ( loadValue < engLoad[i1+1] ) && ( loadValue >= engLoad[i1] ) ) 
            {
                break ; 
            }
        }

        if( i1 == ( numXEntries - 1 ) ) 
        {
            loadValue = engLoad[i1] ; 
        }

        /*  
         * Use the 'x' coefficients to calculate the proportional delta
         *  such that 'engLoadDelta' is in the range 0..1
         */
        engLoadDelta1 = (n_float)( loadValue - engLoad[i1] ) /
                        (n_float)( engLoad[i1 + 1] - engLoad[i1] ) ; 

#if BMDEBUG
        th_sprintf( szDebug, ", %3ld, %3ld", (n_long)i1, (n_long)i1+1 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*  Next, find the 'y' axis grid coefficients */
        for( j1 = 0 ; j1 < ( numYEntries-1 ) ; j1++ )
        {
            if( ( speedValue < engSpeed[j1+1] ) && 
                ( speedValue >= engSpeed[j1] ) ) 
            {
                break ; 
            }
        }

        if( j1 == ( numYEntries - 1 ) )
        {
            speedValue = engSpeed[j1] ; 
        }

        /*  
         * Use the 'y' coefficients to calculate the proportional grid delta
         * such that 'engSpeedDelta' is in the range 0..1.
         */
        engSpeedDelta1 = (n_float)( speedValue - engSpeed[j1] ) / 
                         (n_float)( engSpeed[j1 + 1] - engSpeed[j1] ) ; 

#if BMDEBUG
        th_sprintf( szDebug, ", %3ld, %3ld, %f, %f, ", (n_long)j1, (n_long)( j1 + 1 ),
            engLoadDelta1, engSpeedDelta1 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /* 
         * Now we can determine the interpolated output value for the input 
         * data from the table 'z' values.
         */
        outAngleValue1 = (varsize)( 
            ( ( 1.0 - engLoadDelta1 )*( 1.0 - engSpeedDelta1 ) *
              (n_float)angleTable[ i1 + ( j1 * numXEntries ) ] ) +
            ( engLoadDelta1 * ( 1.0 - engSpeedDelta1 ) * 
              (n_float)angleTable[ ( i1 + 1 ) + ( j1 * numXEntries ) ] ) +
            ( engLoadDelta1 * engSpeedDelta1 * 
              (n_float)angleTable[ ( i1 + 1 ) + ( numXEntries * ( j1 + 1 ) ) ] ) +
            ( ( 1.0 - engLoadDelta1 )* engSpeedDelta1 *
              (n_float)angleTable[ i1 + ( numXEntries * ( j1 + 1 ) ) ] ) ) ; 

        /* Display the 'outAngleValue', or just put it in RAM */
#if BMDEBUG
        th_sprintf( szDebug, " %6ld\n", (n_long)outAngleValue1 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( outAngleValue1 ) ; 
#endif /* BMDEBUG */

        /***********************************************************************
            Second Pass                                                         
        ***********************************************************************/
        /* Gets 'loadValue' and 'speedValue' */
        isTableLooped += GetInputValues() ;

#if BMDEBUG
        th_sprintf( szDebug, "%4ld, %4ld", (n_long)loadValue, (n_long)speedValue ) ;
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*  First, find the 'x' axis grid coefficients */
        for( i2=0 ; i2 < ( numXEntries - 1 ) ; i2++ )
        {
            if( ( loadValue < engLoad[i2 + 1] ) && 
                ( loadValue >= engLoad[i2] ) ) 
            {
                break ; 
            }
        }

        if( i2 == ( numXEntries-1 ) )
        {
            loadValue = engLoad[i2] ;
        }

        /*
         * Use the 'x' coefficients to calculate the proportional delta
         * such that 'engLoadDelta' is in the range 0..1 
         */
        engLoadDelta2 = (n_float)( loadValue - engLoad[i2] ) /
                        (n_float)( engLoad[i2 + 1] - engLoad[i2] ) ; 

#if BMDEBUG
        th_sprintf( szDebug, ", %3ld, %3ld", (n_long)i2, (n_long)(i2 + 1) ) ;
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*  Next, find the 'y' axis grid coefficients */
        for( j2=0 ; j2<( numYEntries-1 ) ; j2++ )
        {
            if( ( speedValue < engSpeed[j2 + 1] ) && 
                ( speedValue >= engSpeed[j2] ) )
            {
                break ; 
            }
        }

        if( j2 == ( numYEntries - 1 ) )
        {
            speedValue = engSpeed[j2] ; 
        }

        /*  
         * Use the 'y' coefficients to calculate the proportional grid delta
         * such that 'engSpeedDelta' is in the range 0..1.
         */
        engSpeedDelta2 = (n_float)( speedValue - engSpeed[j2] ) / 
                         (n_float)( engSpeed[j2 + 1] - engSpeed[j2] ) ; 

#if BMDEBUG
        th_sprintf( szDebug, ", %3ld, %3ld, %f, %f, ", (n_long)j2, (n_long)(j2 + 1),
            engLoadDelta2, engSpeedDelta2 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*
         * Now we can determine the interpolated output value for the input 
         * data from the table 'z' values.
         */
        outAngleValue2 = (varsize)( 
            ( ( 1.0 - engLoadDelta2 ) * ( 1.0 - engSpeedDelta2 ) *
              (n_float)angleTable[ i2 + ( j2 * numXEntries ) ] ) +
            ( engLoadDelta2 * ( 1.0 - engSpeedDelta2 ) * 
              (n_float)angleTable[ ( i2 + 1 ) + ( j2 * numXEntries ) ] ) +
            ( engLoadDelta2 * engSpeedDelta2 *
              (n_float)angleTable[ ( i2 + 1 )+( numXEntries * ( j2+1 ) ) ] ) +
            ( ( 1.0 - engLoadDelta2 ) * engSpeedDelta2 *
              (n_float)angleTable[ i2 + ( numXEntries * ( j2 + 1 ) ) ] ) ) ; 

        /* Display the 'outAngleValue', or just put it in RAM */
#if BMDEBUG
        th_sprintf( szDebug, " %6ld\n", (n_long)outAngleValue2 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( outAngleValue2 ) ; 
#endif /* BMDEBUG */

        /***********************************************************************
            Third Pass                                                          
        ***********************************************************************/
        /* Gets 'loadValue' and 'speedValue' */
        isTableLooped += GetInputValues() ;

#if BMDEBUG
        th_sprintf( szDebug, "%4ld, %4ld", (n_long)loadValue, (n_long)speedValue ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*  First, find the 'x' axis grid coefficients */
        for( i3=0 ; i3 < ( numXEntries - 1 ) ; i3++ )
        {
            if( ( loadValue < engLoad[i3 + 1] ) && 
                ( loadValue >= engLoad[i3] ) )
            {
                break ; 
            }
        }

        if( i3 == ( numXEntries-1 ) )
        {
            loadValue = engLoad[i3] ; 
        }

        /*
         * Use the 'x' coefficients to calculate the proportional delta
         * such that 'engLoadDelta' is in the range 0..1 
         */
        engLoadDelta3 = (n_float)( loadValue - engLoad[i3] ) / 
                        (n_float)( engLoad[i3 + 1] - engLoad[i3] ) ; 

#if BMDEBUG
        th_sprintf( szDebug, ", %3ld, %3ld", (n_long)i3, (n_long)i3+1 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*  Next, find the 'y' axis grid coefficients */
        for( j3=0 ; j3 < ( numYEntries - 1 ) ; j3++ )
        {
            if( ( speedValue < engSpeed[j3 + 1] ) && 
                ( speedValue >= engSpeed[j3] ) )
            {
                break ; 
            }
        }

        if( j3 == ( numYEntries - 1 ) )
        {
            speedValue = engSpeed[j3] ; 
        }

        /*
         * Use the 'y' coefficients to calculate the proportional grid delta
         * such that 'engSpeedDelta' is in the range 0..1
         */
        engSpeedDelta3 = (n_float)( speedValue - engSpeed[j3] ) / 
                         (n_float)( engSpeed[j3 + 1] - engSpeed[j3] ) ; 

#if BMDEBUG
        th_sprintf( szDebug, ", %3ld, %3ld, %f, %f, ", (n_long)j3, (n_long)( j3+1 ), 
            engLoadDelta3, engSpeedDelta3 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*
         * Now we can determine the interpolated output value for the input 
         * data from the table 'z' values 
         */
        outAngleValue3 = (varsize)( 
            ( ( 1.0 - engLoadDelta3 ) * ( 1.0 - engSpeedDelta3 ) *
              (n_float)angleTable[ i3 +( j3 * numXEntries ) ] ) +
            ( engLoadDelta3 * ( 1.0 - engSpeedDelta3 ) * 
              (n_float)angleTable[ ( i3+1 ) + ( j3 * numXEntries ) ] ) +
            ( engLoadDelta3 * engSpeedDelta3 *
              (n_float)angleTable[ ( i3 + 1 ) + ( numXEntries * ( j3 + 1 ) ) ] ) +
            ( ( 1.0 - engLoadDelta3 ) * engSpeedDelta3 * 
              (n_float)angleTable[ i3 + ( numXEntries * ( j3 + 1 ) ) ] ) ) ; 

        /* Display the 'outAngleValue', or just put it in RAM */
#if BMDEBUG
        th_sprintf( szDebug, " %6ld", (n_long)outAngleValue3 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %08lX\n", (n_long)loop_cnt ) ; 
        DebugOut( szDebug ) ; 
#else      
        WriteOut( outAngleValue3 ) ; 

#if DATA_SIZE == 0      /* Might break up the loop counter */
        i3 = (varsize)loop_cnt & 0x0000FFFF ; 
        WriteOut( i3 ) ;   /* ...in the output file */
        i3 = (varsize)loop_cnt >> 16 ; 
        WriteOut( i3 ) ; 
#else
        WriteOut( loop_cnt ) ; 
#endif
        i3 = (varsize)0xAAAA ; 
        WriteOut( i3 ) ;  /* Flag the end of data-block */
#endif /* BMDEBUG */

    }


    return 0;
} /* End of function 't_run_test' */



// n_int
// GetTestData( n_void ) 
// {
//     int i , j ;

//     inpLoadValue = (varsize*)th_malloc( ( NUM_TESTS + 1 ) * sizeof( varsize ) ) ;
//     inpSpeedValue = (varsize*)th_malloc( ( NUM_TESTS + 1 ) * sizeof( varsize ) ) ;

//     if( ( inpLoadValue == NULL ) || ( inpSpeedValue == NULL ) ) 
//     {
//         return( false ) ;
//     }
    
//     /* Copy the test values from ROM to RAM  */
//     for( i = 0, j = 0; i < NUM_TESTS ; i++, j++ )
//     {
//         inpLoadValue[i] = inpValueROM[j++] ;
//         inpSpeedValue[i] = inpValueROM[j] ;
//     }
//     return( true ) ;
// } /* End of function 'GetTestData' */

/*------------------------------------------------------------------------------
// FUNC    : GetInputValues
//
// DESC    : 
//      On each pass of the table lookup, two values must be input:  'loadValue' 
//      and 'speedValue'.  These values are the 'x' and 'y' indices for the 
//      'angle' table. Each time this function is called, the next two pairs 
//      of input values are pulled from the table in RAM.  The table wraps 
//      around, so that input data is continuous.  A flag is returned TRUE 
//      whenever the table wraps around.
//         
// RETURNS : 
//      true/false
// ---------------------------------------------------------------------------*/

static n_int GetInputValues( n_void ) {
    loadValue = inpValueROM[input_index++] ;
    if (input_index >= sizeof(inpValueROM) / sizeof(inpValueROM[0])) 
        input_index = 0;
    speedValue = inpValueROM[input_index++] ;
    if (input_index >= sizeof(inpValueROM) / sizeof(inpValueROM[0])) 
        input_index = 0;

    return false;


    // loadValue = inpLoadValue[tableCount] ;
    // speedValue = inpSpeedValue[tableCount] ;

    // if( ++tableCount < NUM_TESTS )
    // {
    //     return( false ) ;
    // }

    // tableCount = 0 ;
    // return( true ) ;

} /* End of function 'GetInputValues' */
