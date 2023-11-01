#ifndef PUWMOD_H // TODO: change the name of the define to match the filename
#define PUWMOD_H
#include <eembc_adaptation.h>

/* Compilation switches to be defined( or not )based on application follow */

/* Define (=1) when compiling for DOUBLE variables */
#define DATA_SIZE   1       
/* ..or( =0 )when compiling for FLOAT variables */
#if DATA_SIZE == 0           /* SHORT variables */
typedef n_short varsize ;      /* Data and variables are 16 bits */
#define MAX_VARIABLE 0x7FFF  /* Must match sim. realtime ctr in test data */
#else                        /* Else, LONG variables */
typedef n_long varsize ;       /* Data and variables are 32 bits */
#define MAX_VARIABLE 0x7FFF  /* Must match sim. realtime ctr in test data */
#endif /* DATA_SIZE */

#define RAM_OUT 0   /* Define (=1)to direct debug text to RAM file */
                    /* or( =0 )to direct debug text to console */

/*******************************************************************************
    Defines                                                                     
*******************************************************************************/

#define false           0
#define true            !false

#if (BMDEBUG && RAM_OUT == 1)	/* Debug buffer size == 32K */ 
#define MAX_FILESIZE    8192    /* Maximum size of output file */
#else
#define MAX_FILESIZE    256     /* Maximum size of output file */
#endif 
#define NUM_TESTS       2420    /* Number of sets of input test data stimuli */
#define VAR_COUNT       1       /* Number of vars which must be allocated */
#define HEADER_SIZE     80      /* Approx size of text title and hdr messages */

#define MAX_PHASE       20          /* Maximum duty cycle, per 'pwmCounter' */
#define MAX_COUNT       MAX_PHASE+1 /* Overflow of duty cycle counter */
#define MID_PHASE       MAX_PHASE/2 /* 50% duty cycle */

/*******************************************************************************
    Global Variables                                                            
*******************************************************************************/

#ifndef ALGO_GLOBALS             /* Don't define these twice! */    

extern n_int   tableCount ;     /* Number of passes through table */
extern varsize commandPos ;      /* Current 'commanded position' pulled
                                  * from test data */
extern varsize *inpCmdPosition ; /* Array of 'cmdPosition' test data values */

#endif /* ALGO_GLOBALS */    


#endif