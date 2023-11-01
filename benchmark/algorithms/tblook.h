#ifndef TBLOOK_H
#define TBLOOK_H
#include <eembc_adaptation.h>

/*******************************************************************************
    Defines                                                                     
*******************************************************************************/

#define false            0
#define true            !false

#if (BMDEBUG && RAM_OUT == 1)	/* Debug buffer size == 32K */ 
#define MAX_FILESIZE    8192    /* Maximum size of output file */
#else
#define MAX_FILESIZE    256     /* Maximum size of output file */
#endif 
#define NUM_TESTS       232     /* Number of sets of input test data stimuli */
#define VAR_COUNT       2       /* Number of vars which must be allocated */
#define HEADER_SIZE     100     /* Approx size of title and header messages */

#define NUM_X_ENTRIES   50      /* Number of entries on each axis of the */
#define NUM_Y_ENTRIES   50      /* ...2D table */

/* Compilation switches to be defined( or not )based on application follow */

/* Define (=1) when compiling for DOUBLE variables */
#define DATA_SIZE   1       
/* ..or( =0 )when compiling for FLOAT variables */
#if DATA_SIZE == 0              /* SHORT variables */
typedef n_short varsize ;         /* Data and variables are 16 bits */
#else                           /* Else, LONG variables */
typedef n_long varsize ;          /* Data and variables are 32 bits */
#endif /* DATA_SIZE */

#define RAM_OUT 0   /* Define (=1)to direct debug text to RAM file */
                    /* or( =0 )to direct debug text to console */


/*******************************************************************************
    Global Variables                                                            
*******************************************************************************/

#ifndef ALGO_GLOBALS            /* Don't define these twice! */


extern varsize numXEntries ;       /* The table 'x' size spec'd here */
extern varsize numYEntries ;       /* The table 'y' size spec'd here */
extern n_int   tableCount ;        /* Number of passes through table */
extern varsize loadValue ;         /* 'Load' pulled from test data */
extern varsize speedValue ;        /* 'Speed' pulled from test data */
extern varsize *inpLoadValue ;     /* Array of 'load' test data */
extern varsize *inpSpeedValue ;    /* Array of 'speed' test data */

extern const varsize *engSpeed ;   /* Linear array of 'y' axis */
extern const varsize *engLoad ;    /* Linear array of 'x' axis */
extern const varsize *angleTable ; /* 2D array of f( x, y )= 'angle' */

#endif /* ALGO_GLOBALS */

#endif